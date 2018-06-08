//------------------------------------------------------------------------------
//
//	Copyright (C) 2016 Nexell Co. All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//  AND	WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//  FOR A PARTICULAR PURPOSE.
//
//	Module		: Quick Rear Camera Library
//	File		: NX_CAVInMgr.cpp
//	Description	: Rearcam management module.
//	Author		: SeongO Park(ray@nexell.co.kr)
//	Export		:
//	History		:
//
//------------------------------------------------------------------------------
#include <stdint.h>
#include <drm/drm_fourcc.h>

#include "NX_CV4l2Camera.h"
#include "NX_CDrmDisplay.h"

#include <CNX_BaseClass.h>
#include <NX_AVIn.h>

#ifdef NX_DTAG
#undef NX_DTAG
#endif
#define NX_DTAG		"[NX_CAVInMgr]"
#include "NX_DbgMsg.h"

enum RENDER_MODULE {
	RENDER_DRM,
	RENDER_V4L2,
	RENDER_ANDROID
};

class NX_CAVInMgr : protected CNX_Thread
{
public:
	NX_CAVInMgr();
	~NX_CAVInMgr();

	int32_t StartService(CAMERA_INFO *pCamInfo, DISPLAY_INFO *pDspInfo);
	int32_t StopService();
	void RegisterRendererCB( void *pAppData, int32_t (callback)(void *, int32_t, void *, int32_t) );
	void RegisterControlCB( void *pAppData, int32_t (callback)(void *, int32_t, void *, int32_t) );

	int32_t Init();
	int32_t Deinit();
	int32_t SetPosition( int32_t x, int32_t y, int32_t width, int32_t height );


	//  Implementation Pure Virtual Function
	virtual void  ThreadProc();
private:
	enum { MAX_NUM_BUFFER=32, NUM_BUFFER=8 };

	int32_t			m_iDspModule;
	NX_CV4l2Camera	*m_hCam;
	NX_VIP_INFO		m_hVipInfo;
	NX_DISPLAY_INFO	m_hDspInfo;
	NX_CDrmDisplay	*m_hDisplay;
	int32_t 		(*m_hDspCallBack)(void *, int32_t, void *, int32_t);
	int32_t			(*m_hCtrlCallBack)(void *, int32_t, void *, int32_t);

	void			*m_pDspCbData;
	void			*m_pCtrlCbData;

	int32_t			m_bExitLoop;
	NX_VID_MEMORY_INFO *m_pCamBuffer[MAX_NUM_BUFFER];

	int32_t			m_bAllocated;

	//
	//	Video memory allocate & free
	//
	int32_t AllocateMemory( int32_t width, int32_t height, int32_t planes, int32_t buffers );
	void FreeMemory();

public:
	static void RearCamCallback( void *, int32_t );

public:
	static NX_CAVInMgr *GetInstance();
	static NX_CAVInMgr *m_pSingleTone;
};

//
//		Single Tone
//
NX_CAVInMgr *NX_CAVInMgr::m_pSingleTone = NULL;

NX_CAVInMgr *NX_CAVInMgr::GetInstance()
{
	if( NULL == m_pSingleTone )
	{
		m_pSingleTone = new NX_CAVInMgr();
	}
	return m_pSingleTone;
}

NX_CAVInMgr::NX_CAVInMgr()
	: m_hCam (NULL)
	, m_hDisplay (NULL)
	, m_hDspCallBack (NULL)
	, m_hCtrlCallBack (NULL)
	, m_pDspCbData (NULL)
	, m_pCtrlCbData (NULL)
	, m_bAllocated(0)
{
	for( int32_t i=0 ; i<MAX_NUM_BUFFER ; i++ )
	{
		m_pCamBuffer[i] = NULL;
	}
}


NX_CAVInMgr::~NX_CAVInMgr()
{
	if( m_bAllocated )
	{
		FreeMemory();
	}
}

#ifndef ALLINEDN
#define ALLINEDN(X,N)	( (X+N-1) & (~(N-1)) )
#endif
int32_t NX_CAVInMgr::AllocateMemory( int32_t width, int32_t height, int32_t planes, int32_t buffers )
{
	int32_t i;
	int32_t allocWidth = width;

	if( m_bAllocated )
		return 0;

	if( m_hVipInfo.bInterlace )
	{
		allocWidth  = ALLINEDN(width, 128);
	}
	else{
		allocWidth  = ALLINEDN(width, 64);
	}
	for( i=0 ; i<buffers ; i++ )
	{
		m_pCamBuffer[i] = NX_AllocateVideoMemory( allocWidth, height,
									planes, DRM_FORMAT_YUV420, 512 );
		if( NULL == m_pCamBuffer[i] )
			goto ERROR_EXIT;

		if( m_hVipInfo.bInterlace ){
			m_pCamBuffer[i]->width = width;
			m_pCamBuffer[i]->stride[0] = ALLINEDN(width, 128);
			m_pCamBuffer[i]->stride[1] = ALLINEDN(width, 64);
			m_pCamBuffer[i]->stride[2] = ALLINEDN(width, 64);
		}
	}
	m_bAllocated = 1;
	return 0;
ERROR_EXIT:
	for( i=0 ; i<buffers ; i++ )
	{
		if( m_pCamBuffer[i] )
		{
			NX_FreeVideoMemory( m_pCamBuffer[i] );
			m_pCamBuffer[i] = NULL;
		}
	}
	return -1;
}


void NX_CAVInMgr::FreeMemory()
{
	for( int32_t i=0 ; i<MAX_NUM_BUFFER ; i++ )
	{
		if( m_pCamBuffer[i] )
		{
			NX_FreeVideoMemory( m_pCamBuffer[i] );
			m_pCamBuffer[i] = NULL;
		}
	}
	m_bAllocated = 0;
}


int32_t NX_CAVInMgr::Init()
{
	if( 0 != AllocateMemory( m_hVipInfo.iWidth, m_hVipInfo.iHeight, m_hVipInfo.iNumPlane, NUM_BUFFER ) )
	{
		printf("AllocateMemory Failed!!!()\n");
		return -1;
	}

	//	Open Camera Module
	m_hCam = new NX_CV4l2Camera();
	//	Add Allocated Memory Buffer to Camera Module
	for( int32_t i=0; i<NUM_BUFFER ; i++ )
	{
		m_hCam->AddVideoMemory(m_pCamBuffer[i]);
	}
	//	Initialize V4L2 Camera
	if( 0 > m_hCam->Init( &m_hVipInfo ) )
	{
		goto ERROR_EXIT;
	}

	m_hDisplay = new NX_CDrmDisplay();

	if( 0 != m_hDisplay->DspInit( &m_hDspInfo ) )
	{
		goto ERROR_EXIT;
	}

	return 0;

ERROR_EXIT:
	if( m_hCam )
	{
		delete m_hCam;
		m_hCam = NULL;
	}
	if( m_hDisplay )
	{
		delete m_hDisplay;
		m_hDisplay = NULL;
	}
	FreeMemory();
	return -1;
}

int32_t NX_CAVInMgr::Deinit()
{
	if( m_hDisplay )
	{
		delete m_hDisplay;
		m_hDisplay = NULL;
	}
	if( m_hCam )
	{
		delete m_hCam;
		m_hCam = NULL;
	}
	return 0;
}

int32_t NX_CAVInMgr::SetPosition( int32_t x, int32_t y, int32_t width, int32_t height )
{
	if( m_hDisplay )
	{
		return m_hDisplay->SetPosition( x, y, width, height );
	}
	return -1;
}

void NX_CAVInMgr::ThreadProc()
{
	int32_t bufIdx = -1;
	while (!m_bExitLoop)
	{
		m_hCam->DequeueBuffer( &bufIdx );
		if( m_iDspModule == DSP_MODULE_QT && NULL != m_hDspCallBack )
		{
			m_hDspCallBack( m_pDspCbData, CB_TYPE_BUFFER, m_pCamBuffer[bufIdx], sizeof(NX_VID_MEMORY_INFO) );
		}
		else
		{
			if( m_hDisplay )
			{
				m_hDisplay->QueueBuffer( bufIdx, m_pCamBuffer[bufIdx] );
			}
		}
		m_hCam->QueueBuffer( bufIdx  );
	}
}


int32_t NX_CAVInMgr::StartService(CAMERA_INFO *pCamInfo, DISPLAY_INFO *pDspInfo)
{
	int32_t ret = 0;

	//	Setting Video Input Information
	memset( &m_hVipInfo, 0, sizeof(m_hVipInfo) );
	m_hVipInfo.iModule		= pCamInfo->iModule;
	m_hVipInfo.iSensorId	= pCamInfo->iSensorId;
	m_hVipInfo.bInterlace	= pCamInfo->bInterlace;
	m_hVipInfo.iWidth		= pCamInfo->iWidth;
	m_hVipInfo.iHeight		= pCamInfo->iHeight;
	m_hVipInfo.iFpsNum		= 30;
	m_hVipInfo.iFpsDen		= 1;
	m_hVipInfo.iNumPlane	= 1;
	m_hVipInfo.iCropX		= pCamInfo->iCropX;
	m_hVipInfo.iCropY		= pCamInfo->iCropY;
	m_hVipInfo.iCropWidth	= pCamInfo->iCropWidth;
	m_hVipInfo.iCropHeight	= pCamInfo->iCropHeight;
	m_hVipInfo.iOutWidth	= pCamInfo->iOutWidth;
	m_hVipInfo.iOutHeight	= pCamInfo->iOutHeight;

	//	Setting Display Information
	memset( &m_hDspInfo, 0, sizeof(m_hDspInfo) );
	m_hDspInfo.planeId				= pDspInfo->iPlaneId;
	m_hDspInfo.ctrlId				= pDspInfo->iCrtcId;
	m_hDspInfo.width				= pDspInfo->iSrcWidth;
	m_hDspInfo.height				= pDspInfo->iSrcHeight;
	m_hDspInfo.stride				= 0;
	m_hDspInfo.drmFormat			= pDspInfo->uDrmFormat;
	m_hDspInfo.numPlane				= 1;//MEM_NUM_PLANE;
	m_hDspInfo.dspSrcRect.left		= pDspInfo->iCropX;
	m_hDspInfo.dspSrcRect.top		= pDspInfo->iCropY;
	m_hDspInfo.dspSrcRect.right		= pDspInfo->iCropX + pDspInfo->iCropWidth;
	m_hDspInfo.dspSrcRect.bottom	= pDspInfo->iCropY + pDspInfo->iCropHeight;
	m_hDspInfo.dspDstRect.left		= pDspInfo->iDspX;
	m_hDspInfo.dspDstRect.top		= pDspInfo->iDspY;
	m_hDspInfo.dspDstRect.right		= pDspInfo->iDspX + pDspInfo->iDspWidth;
	m_hDspInfo.dspDstRect.bottom	= pDspInfo->iDspY + pDspInfo->iDspHeight;

	ret = Init();

	if( ret != 0 )
	{
		NxDbgMsg( NX_DBG_ERR, "Init() Failed !!!");
		return ret;
	}
	m_bExitLoop = 0;
	Start();
	return ret;
}

int32_t NX_CAVInMgr::StopService()
{
	m_bExitLoop = 1;
	Stop();
	Deinit();
	return 0;
}

void NX_CAVInMgr::RegisterRendererCB( void *pAppData, int32_t (callback)(void *, int32_t, void *, int32_t) )
{
	m_pDspCbData = pAppData;
	m_hDspCallBack = callback;
	m_iDspModule = DSP_MODULE_QT;
}

void NX_CAVInMgr::RegisterControlCB( void *pAppData, int32_t (callback)(void *, int32_t, void *, int32_t) )
{
	m_pCtrlCbData = pAppData;
	m_hCtrlCallBack = callback;
	m_iDspModule = DSP_MODULE_QT;
}


//////////////////////////////////////////////////////////////////////////////
//																			//
//									APIs									//
//																			//
//////////////////////////////////////////////////////////////////////////////
int32_t NXDA_StartAVInService(CAMERA_INFO *pCamInfo, DISPLAY_INFO *pDspInfo)
{
	NX_CAVInMgr *pMgr = NX_CAVInMgr::GetInstance();
	return pMgr->StartService(pCamInfo, pDspInfo);
}

void NXDA_StopAVInService()
{
	NX_CAVInMgr *pMgr = NX_CAVInMgr::GetInstance();
	pMgr->StopService();
}

int32_t NXDA_SetAVInVideoPosition( int32_t x, int32_t y, int32_t width, int32_t height )
{
	NX_CAVInMgr *pMgr = NX_CAVInMgr::GetInstance();
	return pMgr->SetPosition( x, y, width, height );
}

void NXDA_RegRenderCallback( void *pApp, int32_t (callback)(void *, int32_t, void *, int32_t) )
{
	NX_CAVInMgr *pMgr = NX_CAVInMgr::GetInstance();
	pMgr->RegisterRendererCB( pApp, callback );
}

void NXDA_RegControlCallback( void *pApp, int32_t (callback)(void *, int32_t, void *, int32_t) )
{
	NX_CAVInMgr *pMgr = NX_CAVInMgr::GetInstance();
	pMgr->RegisterControlCB( pApp, callback );
}

#include <NX_Type.h>

const char* NXDA_AVInGetVersion()
{
	return NX_VERSION_LIBAVIN;
}

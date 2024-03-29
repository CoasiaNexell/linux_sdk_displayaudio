//------------------------------------------------------------------------------
//
//	Copyright (C) 2015 Nexell Co. All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//  AND	WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//  FOR A PARTICULAR PURPOSE.
//
//	Module		:
//	File		:
//	Description	:
//	Author		:
//	Export		:
//	History		:
//
//------------------------------------------------------------------------------

#ifndef __NX_MOVIEPLAY_H__
#define __NX_MOVIEPLAY_H__

#ifdef ANDROID
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#endif

#include <stdint.h>

#define MP_RESULT			int32_t
#define PROGRAM_MAX			16
#define MAX_TRACK_NUM		10

typedef struct MOVIE_TYPE	*MP_HANDLE;

enum {
	MP_MSG_EOS								= 0x1000,
	MP_MSG_VIDEO_SAMPLE_ARRIVAL,
	MP_MSG_AUDIO_RENDER_UNDERRUN,
	MP_MSG_STREAM_READY,					// stream ready
                                            // reference NX_MPSetStreamType()

	MP_MSG_DEMUX_ERR						= 0x8000,
	MP_MSG_VIDEO_DECODER_ERR,
	MP_MSG_AUDIO_DECODER_ERR,
	MP_MSG_ERR_OPEN_AUDIO_DEVICE,
};

enum
{
	MP_STOP_STATE	= 0,
	MP_PLAY_STATE	= 1,
	MP_PAUSE_STATE	= 2,
	MP_READY_STATE	= 3,
};



//Error Code
enum {
	MP_ERR_NONE					= 0,
	MP_ERR	 					= -1,
	MP_ERR_INPUT_FILE			= -2,
	MP_NOT_SUPPORT_AUDIOCODEC	= -3,
	MP_NOT_SUPPORT_VIDEOCODEC	= -4,
	MP_NOT_SUPPORT_VIDEOWIDTH	= -5,
	MP_NOT_SUPPORT_VIDEOHEIGHT	= -6,
};

enum {
	MP_TRACK_VIDEO,
	MP_TRACK_AUDIO,
	MP_TRACK_DATA,
};

enum {
	MP_TYPE_URI,
	MP_TYPE_FILE,
};

typedef struct MP_TRACK_INFO {
	int32_t			iTrackIndex;	// Track Index
	int32_t			iTrackType;		// MP_TRACK_VIDEO, ...
	int32_t			iCodecId;
	int64_t			iDuration;		// Track Duration

	int32_t			iWidth;			// Only VideoTrack
	int32_t			iHeight;		// Only VideoTrack
	int32_t			iFrameRate;		// Only VideoTrack

	int32_t			iChannels;		// Only AudioTrack
	int32_t			iSampleRate;	// Only AudioTrack
	int32_t			iBitrate;		// Only AudioTrack
} MP_TRACK_INFO;

typedef struct MP_PROGRAM_INFO {
	int32_t			iAudioNum;
	int32_t			iVideoNum;
	int32_t			iSubTitleNum;
	int32_t			iDataNum;
	int64_t			iDuration;
	MP_TRACK_INFO 	TrackInfo[MAX_TRACK_NUM];
} MP_PROGRAM_INFO;

typedef struct MP_MEDIA_INFO{
	int32_t			iProgramNum;
	int32_t			iAudioTrackNum;
	int32_t			iVideoTrackNum;
	int32_t			iSubTitleTrackNum;
	int32_t			iDataTrackNum;
	MP_PROGRAM_INFO	ProgramInfo[PROGRAM_MAX];
} MP_MEDIA_INFO;

typedef struct MP_DSP_RECT {
	int32_t			iX;
	int32_t			iY;
	int32_t			iWidth;
	int32_t			iHeight;
} MP_DSP_RECT;

typedef struct MP_DSP_CONFIG {
	uint32_t		planeId;
	uint32_t		ctrlId;

	MP_DSP_RECT		srcRect;	// Source Crop Region
	MP_DSP_RECT		dstRect;	// Destination Position Region
} MP_DSP_CONFIG;


typedef struct MP_DRM_PLANE_INFO {
	int32_t		iConnectorID;		//  Dsp Connector ID
	int32_t		iPlaneId;			//  DRM Plane ID
	int32_t		iCrtcId;			//  DRM CRTC ID
} MP_DRM_PLANE_INFO;


#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
//
//	Configuration Function
//
MP_RESULT	NX_MPOpen( MP_HANDLE *phMp, void (*cbEvent)(void *pObj, uint32_t EventType, uint32_t EventData, uint32_t param), void *cbPrivate = NULL );
void		NX_MPClose( MP_HANDLE hMp );

MP_RESULT	NX_MPSetUri( MP_HANDLE hMp, const char *pUri );
MP_RESULT	NX_MPGetMediaInfo( MP_HANDLE hMp, MP_MEDIA_INFO *pInfo );

#ifdef ANDROID
MP_RESULT	NX_MPAddTrack( MP_HANDLE hMp, int32_t iTrack, ANativeWindow *pWindow, MP_DSP_CONFIG *pInfo );
#else
MP_RESULT	NX_MPAddVideoTrack( MP_HANDLE hMp, int32_t iTrack, MP_DSP_CONFIG *pInfo, int32_t bHdmiAudio = false );
MP_RESULT	NX_MPAddAudioTrack( MP_HANDLE hMp, int32_t iTrack, MP_DSP_CONFIG *pInfo, const char *pDeviceName );
#endif
MP_RESULT	NX_MPClearTrack( MP_HANDLE hMp );


//------------------------------------------------------------------------------
//
//	Control Function
//
MP_RESULT	NX_MPPlay( MP_HANDLE hMp );
MP_RESULT	NX_MPStop( MP_HANDLE hMp );
MP_RESULT	NX_MPPause( MP_HANDLE hMp );
MP_RESULT	NX_MPSeek( MP_HANDLE hMp, int64_t iSeekTime );			// mSec ( alsolute time )
MP_RESULT	NX_MPGetDuration( MP_HANDLE hMp, int64_t *duration );	// mSec
MP_RESULT	NX_MPGetPosition( MP_HANDLE hMp, int64_t *position );	// mSec

MP_RESULT	NX_MPAddSubDisplay( MP_HANDLE hMp, int32_t iTrack, MP_DSP_CONFIG *pInfo );
MP_RESULT	NX_MPClearSubDisplay ( MP_HANDLE hMp, int32_t iTrack );
MP_RESULT	NX_MPSetDspCrop( MP_HANDLE hMp, int32_t iTrack, MP_DSP_RECT *pRect );
MP_RESULT	NX_MPSetDspPosition( MP_HANDLE hMp, int32_t iTrack, MP_DSP_RECT *pRect );
MP_RESULT	NX_MPSetVideoLayerPriority( MP_HANDLE hMp, int32_t iTrack, int32_t iModule, int32_t iPriority );
MP_RESULT	NX_MPSetRenderCallBack( MP_HANDLE hMp, int32_t iTrack, void (*cbQtUpdateImg)(void *pImg) );

#ifndef ANDROID
void	NX_MPVideoMute( MP_HANDLE hMp, int32_t bOnoff, MP_DSP_CONFIG *pInfo);
//Video Speed
MP_RESULT	NX_MPSetVideoSpeed( MP_HANDLE hMp, float Speed  ); //support file:avi,mkv,mp4
MP_RESULT   NX_MPGetVideoSpeedSupport( MP_HANDLE hMp );		   // support file:avi,mkv,mp4
//A/V Sync
MP_RESULT	NX_MPSetAVSync( MP_HANDLE hMp, int64_t syncTimeMs ); //syncTime +,-, ms
//Fast Seek, Only .mp3 file
//Used for large files
//On:1, Off:0
MP_RESULT NX_MPSetFastSeek( MP_HANDLE hMp, int32_t  bOnOff);  //only .mp3
MP_RESULT NX_MPSetDeinterlace( MP_HANDLE hMp );  //only nxp4330, use before addvideotrack
#endif

//Dual Display
// iDspMode ==> 0:Default, (1:Only LCD, 2:Only Hdmi, 3:Only Tvout, 4:LCD+HDMI, 5:LCD+TVOUT)==>Dual Display
MP_RESULT	NX_MPSetDspMode(MP_HANDLE hMp, int32_t iTrack, MP_DSP_CONFIG *pInfo, int32_t iDspMode );

#ifdef ANDROID
MP_RESULT	NX_MPSetAudioStreamType( MP_HANDLE hMp, int32_t iStreamType );
#else
MP_RESULT 	NX_MPSetVolume( MP_HANDLE hMp, int32_t iLevel );		// 0% ~ 100%
#endif
MP_RESULT	NX_MPSetStreamType( MP_HANDLE hMp, int32_t  iStreamType); //0:normal 1:buffering ready

//------------------------------------------------------------------------------
//
//	Extra Function & Debug Function
//

int32_t		NX_MPCheckThumbnailInVideoFile( const char *pInFile, int32_t  *pThumbnailWidth, int32_t  *pThumbnailHeight ); //return 1:have thumbNail, 0:no thumbNail
int32_t		NX_MPGetThumbnail( const char *pInFile, const char *pOutFile ); //return 0:none, -1:error
int32_t 	NX_MPMakeThumbnail( const char *pInFile, const char *pOutFile, int32_t maxWidth, int32_t maxHeight, int32_t timeRatio );
char 		*NX_MPGetMimeType( const char *pInFile );

int32_t		NX_MPGetVersion( void );				// MSB|  Major( 8bit ) - Minor( 8bit ) - Revision( 8bit ) - Reserved( 8bit )  |LSB
void		NX_MPChgDebugLevel( int32_t iLevel );	// 0(Verbose), 1(Debug), 2(Info), 3(Warn), 4(Error), 5(Disable) :: Default(Info)
int32_t		NX_GetState( MP_HANDLE hMp );

void		NX_MPVideoMute( MP_HANDLE hMp, int32_t bOnoff, MP_DSP_CONFIG *pInfo);

int32_t 	NX_MPGetPlaneForDisplay( int crtcIdx, int layerIdx, int32_t findRgb, MP_DRM_PLANE_INFO *pDrmPlaneInfo );

#ifdef __cplusplus
}
#endif

#endif	// __NX_MOVIEPLAY_H__

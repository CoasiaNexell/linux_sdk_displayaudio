#ifndef NXBTSERVICE_H
#define NXBTSERVICE_H

// for pthread
#include <pthread.h>

// for ipc
#include "UDS_Server.h"

// for std
#include <vector>
#include <string>
using namespace std;

#include <time.h>

// NXBT header
#include <INX_BT.h>

// NX IPC header
#include <NX_Type.h>
#include <INX_IpcManager.h>
#include <NX_IpcPacket.h>
#include <NX_IpcUtils.h>

// NX DAUDIO UTILS header
#include <CNX_DAudioStatus.h>

#ifndef BUFFER_SIZE
#   define BUFFER_SIZE 1024
#endif /* BUFFER_SIZE */

#ifndef DEFAULT_SERVER_PATH
#   define DEFAULT_SERVER_PATH              "/tmp/nexell.daudio.bluetooth.service"
#endif /* DEFAULT_SERVER_PATH */

#ifndef DEFAULT_CLIENT_PATH_FOR_AUDIO
#   define DEFAULT_CLIENT_PATH_FOR_AUDIO    "/tmp/nexell.daudio.bluetooth.client.audio"
#endif

#ifndef DEFAULT_CLIENT_PATH_FOR_PHONE
#   define DEFAULT_CLIENT_PATH_FOR_PHONE    "/tmp/nexell.daudio.bluetooth.client.phone"
#endif

#ifndef DEFAULT_CLIENT_PATH_FOR_SETTINGS
#   define DEFAULT_CLIENT_PATH_FOR_SETTINGS "/tmp/nexell.daudio.bluetooth.client.settings"
#endif

#define PB_DATA_PATH	"/etc/bluetooth/pb_data.vcf"

#define DEVICE_NAME_SIZE    249
#define DEVICE_ADDRESS_SIZE 6

#define AVK "AVK"
#define MGT "MGT"
#define HS	"HS"
#define PBC "PBC"
#define MCE "MCE"

#define COMMAND_FORMAT_STX			'$'
#define COMMAND_FORMAT_REPLY_DONE	"OK"
#define COMMAND_FORMAT_REPLY_FAIL	"NG"
#define	COMMAND_FORMAT_SEPERATOR	'#'
#define COMMAND_FORMAT_ETX			'\n'

class NxBTService
{
public:
    enum CommandErrorType
    {
        CommandParseErrorType_None = 0,
        CommandParseErrorType_UnknownService = -1,
        CommandParseErrorType_UnknownCommand = -2,
        CommandParseErrorType_UnknownCommandArgument = -3,
        CommandParseErrorType_InvalidCommandFormat = -4,
        CommandParseErrorType_FailedFromInterface = -5
    };

    enum CommandType
    {
        CommandType_Service,
        CommandType_Command,
        CommandType_Parameter
    };

    enum CallStatus {
        UNKNOWN_CALL = 0,
        HANG_UP_CALL,
        INCOMMING_CALL,
        READY_OUTGOING_CALL,
        OUTGOING_CALL,
        PICK_UP_CALL,
        DISCONNECTED_CALL
    };

    enum GetPBStatus {
        PARAM_PROGRESS = 2,
        PARAM_PHONEBOOK,
        PARAM_FILE_TRANSFER_STATUS
    };

    enum PlayStatus {
        PlayStatus_Stopped = 0,
        PlayStatus_Playing,
        PlayStatus_Paused
    };

    enum DownloadType {
        DownloadType_None,
        DownloadType_PhoneBook,
		DownloadType_CallHistory
    };

    struct PlayInfo {
        char title[102];
        char artist[102];
        char album[102];
        char genre[102];
        int duration; // unit : milisecond
        int position; // unit : milisecond
    };

	struct connect_state_t
	{
		bool on;
		int index; // connected index
		char bd_addr[DEVICE_ADDRESS_SIZE];
		char name[DEVICE_NAME_SIZE];
	};

	struct AVKService
	{
		connect_state_t connection;
		PlayStatus status;
		PlayInfo info;
	};

	struct PBCService {
		connect_state_t connection;
		DownloadType download;
	};

	struct HSService
	{
		connect_state_t hs;
		connect_state_t mce;
		PBCService pbc;
	};

public:
    NxBTService();

    void start();

    void stop();

	bool initialize();

	bool isInitialized();

	void setInitialized(bool state);

private:
    void registerCallbackFunctions();

    static void* foo(void* args);

    static void* autoConnectThread(void* args);

	static void* testAudioFocusThread(void* args);

//    static void* checkALSADeviceIsClosedThread(void* args);

    void findClientType();

	bool runCommand(char* command);

    std::vector<std::string> createTokensFromCommand(char* command);

    std::string bdAddrToString(char* bd_addr, int len, char seperator);

    // callback functions
	static int32_t callbackFromIPCServer(int32_t sock, uint8_t* send, uint8_t* receive, int32_t max_buffer_size, void* obj);

	// callback functions
	static void sendMGTOpenSucceed_stub(void* pObj, int32_t result);

    static void sendMGTDisconnected_stub(void *pObj);

    static void sendPairingFailed_stub(void *pObj, int32_t fail_reason);

    static void updatePairedDevices_stub(void *pObj);

    static void updateUnpairedDevices_stub(void *pObj);

    static void sendPairingRequest_stub(void *pObj, bool auto_mode, char *name, char *bd_addr, int32_t pairing_code);

    static void callbackLinkDownEventManager(void* pObj, char* bd_addr, int32_t reason_code);

    static void sendAVKOpenFailed_stub(void *pObj);

    static void sendAVKConnectionStatus_stub(void *pObj, bool is_connected, char *name, char *bd_addr);

    static void sendAVKRCConnectionStatus_stub(void *pObj, bool is_connected);

    static void updatePlayStatusAVK_stub(void *pObj, int32_t play_status);

    static void updateMediaElementsAVK_stub(void *pObj, char *mediaTitle, char *mediaArtist, char *mediaAlbum, char *mediaGenre, int32_t playTime_msec);

    static void updatePlayPositionAVK_stub(void *pObj, int32_t play_pos_msec);

    static void sendAVKStreamingStarted_stub(void* pObj, bool is_opened);

    static void sendAVKStreamingStopped_stub(void *pObj);

    // HS
    static void sendHSOpenFailed_stub(void *pObj);

    static void sendHSConnectionStatus_stub(void *pObj_, bool is_connected_, char *name_, char *bd_addr_);

    static void sendHSCallStatus_stub(void *pObj, int32_t call_status);

    static void sendHSBatteryStatus_stub(void *pObj, int32_t batt_status);

    static void sendHSCallOperName_stub(void *pObj, char *name);

    static void sendHSAudioMuteStatus_stub(void *pObj, bool is_muted, bool is_opened);

    static void sendHSIncommingCallNumber_stub(void *pObj, char *number);

    // HS - PBC
    static void sendPBCOpenFailed_stub(void *pObj);

    static void sendPBCConnectionStatus_stub(void *pObj, bool is_connected);

    static void sendNotifyGetPhonebook_stub(void *pObj, int32_t type);

    // HS - MCE
    static void sendMCEOpenFailed_stub(void *pObj);

    static void sendMCEConnectionStatus_stub(void *pObj, bool is_connected);

	static void sendNotifyGetMessageMCE_stub(void *pObj);

	//-----------------------------------------------------------------------
	// MGT functions
	bool ping(std::string service, std::string command);

	bool setEnableAutoConnection(std::string service, std::string command);

	bool isEnabledAutoConnection(std::string service, std::string command);

	bool setEnableAutoPairing(std::string service, std::string command);

	bool isEnabledAutoPairing(std::string service, std::string command);

	bool acceptPairing(std::string service, std::string command);

	bool rejectPairing(std::string service, std::string command);

	bool unpairAll(std::string service, std::string command);

	bool unpair(std::string service, std::string command);

	bool localDeviceName(std::string service, std::string command);

	bool localDeviceAddress(std::string service, std::string command);

	bool renameLocalDevice(std::string service, std::string command);

	bool countOfPairedDevice(std::string service, std::string command);

	bool infoListOfPairedDeviceAll(std::string service, std::string command);

	bool infoOfPairedDeviceByIndex(std::string service, std::string command);

	bool nameOfPairedDeviceByIndex(std::string service, std::string command);

	bool addressOfPairedDeviceByIndex(std::string service, std::string command);
	//-----------------------------------------------------------------------
	// AVK functions
	bool connectToAVK(std::string service, std::string command);

	bool disconnectToAVK(std::string service, std::string command);

	bool indexOfConnectedDeviceToAVK(std::string service, std::string command);

	bool addressOfConnectedDeviceToAVK(std::string service, std::string command);

	bool isConnectedToAVK(std::string service, std::string command);

	bool playStartAVK(std::string service = "AVK", std::string command = "PLAY START");

	bool playStopAVK(std::string service = "AVK", std::string command = "PLAY STOP");

	bool playPauseAVK(std::string service = "AVK", std::string command = "PLAY PAUSE");

	bool playPrevAVK(std::string service = "AVK", std::string command = "PLAY PREV");

	bool playNextAVK(std::string service = "AVK", std::string command = "PLAY NEXT");

	bool playStatusAVK(std::string service = "AVK", std::string command = "PLAY STATUS");

	bool playInfoAVK(std::string service = "AVK", std::string command = "PLAT INFO");

	bool openAudioAVK(std::string service = "AVK", std::string command = "OPEN AUDIO");

	bool closeAudioAVK(std::string service = "AVK", std::string command = "CLOSE AUDIO");
	//-----------------------------------------------------------------------
	// HS functions
	bool connectToHS(std::string service, std::string command);

	bool disconnectFromHS(std::string service, std::string command);

	bool isConnectedToHS(std::string service, std::string command);

	bool indexOfConnectedDeviceToHS(std::string service, std::string command);

	bool dialPhoneNumber(std::string service, std::string command);

	bool reDialPhoneNumber(std::string service, std::string command);

	bool pickUpCall(std::string service, std::string command);

	bool hangUpCall(std::string service, std::string command);

	bool muteMicrophoneHS(std::string service, std::string command);

	bool openAudioHS(std::string service, std::string command);

	bool closeAudioHS(std::string service, std::string command);
	//-----------------------------------------------------------------------
	// MCE functions
	bool connectToMCE(std::string service, std::string command);

	bool disconnectFromMCE(std::string service, std::string command);

	bool isConnectedToMCE(std::string service, std::string command);

	bool downloadSMSMessage(std::string service, std::string command);

	//-----------------------------------------------------------------------
	// PBC functions
	bool connectToPBC(std::string service, std::string command);

	bool disconnectFromPBC(std::string service, std::string command);

	bool isConnectedToPBC(std::string service, std::string command);

	bool downloadPhoneBook(std::string service, std::string command);

	bool downloadCallHistory(std::string service, std::string command);

	//-----------------------------------------------------------------------
	// Utility functions
	bool findArgument(std::string* command, std::string target, std::string* argument);

	std::string findArgument(std::string* command);

	static bool isDigit(std::string text);

	std::string MakeReplyCommand(bool ok, std::vector<std::string> args);

	/**
	 * @brief findPID
	 * @param name		process name
	 * @param pids		list of pids
	 * @return true		find ok
	 *		   false	find failed
	 */
	static bool findPID(std::string name, std::vector<int32_t>& pids);

	static std::vector<std::string> createTokens(std::string text, char seperator, char stx = 0, char etx = 0);

private:
    static INX_BT *m_pModel;

    UDS_Server m_IPCServer;

    pthread_t h_pthread;

    pthread_t h_AutoConnectThread;
    pthread_cond_t h_AutoConnectCondition;
    pthread_mutex_t h_AutoConnectMutex;
    pthread_attr_t h_AutoConnectAttribution;
    bool m_bAutoConnect;

#if 1
	pthread_t h_TestAudioFocusThread;
	pthread_cond_t h_TestAudioFocusCondition;
	pthread_mutex_t h_TestAudioFocusMutex;
	pthread_attr_t h_AudioFocusAttribution;
#endif
    pthread_t h_CheckALSADeviceIsClosedThread;
    pthread_attr_t h_CheckALSADeviceIsClosedThreadAttr;

	void updateAVKConnectionState(struct connect_state_t target);

    AVKService m_AVK;

	HSService m_HS;

	CNX_DAudioStatus* m_pDAudioStatus;

	bool m_bInitialized;
};

extern NX_PROCESS_INFO	g_process_info;
//extern NX_APP_INFO		g_application_info;
extern INX_IpcManager*	g_ipc_manager_handle;

extern bool g_calling_mode_on;
extern bool g_has_audio_focus;
extern bool g_has_audio_focus_transient;

#endif // NXBTSERVICE_H

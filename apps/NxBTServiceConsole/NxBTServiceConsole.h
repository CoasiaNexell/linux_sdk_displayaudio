#ifndef __NXBTSERVICECONSOLE_H__
#define __NXBTSERVICECONSOLE_H__

#define NX_ALSA_DEV_NAME_P		"plughw:0,0"
#define NX_ALSA_DEV_NAME_C		"plughw:0,0"
#define NX_ALSA_BT_DEV_NAME_P	"plughw:0,2"
#define NX_ALSA_BT_DEV_NAME_C	"plughw:0,2"

enum {
    APP_MGT_MENU_GET_PAIRED_DEV_LIST,
	APP_MGT_MENU_ENABLE_AUTOCONN,
	APP_MGT_MENU_DISABLE_AUTOCONN,
    APP_MGT_MENU_ENABLE_AUTOPAIR,
    APP_MGT_MENU_DISABLE_AUTOPAIR,
    APP_MGT_MENU_PAIR_ACCEPT,
    APP_MGT_MENU_PAIR_REJECT,
    APP_MGT_MENU_UNPAIR,
    APP_MGT_MENU_ENABLE_DISCOVERABLE,
    APP_MGT_MENU_DISABLE_DISCOVERABLE,
#ifdef DISC
    APP_MGT_MENU_START_DISCOVERY,
    APP_MGT_MENU_STOP_DISCOVERY,
#endif
    APP_AVK_MENU_OPEN,
    APP_AVK_MENU_CLOSE,
    APP_AVK_MENU_GET_CONN_NUMBER,
    APP_AVK_MENU_GET_CONN_BD_ADDR,
	APP_AVK_MENU_GET_LAST_CONN_INFO,
    APP_AVK_MENU_PLAY_START,
    APP_AVK_MENU_PLAY_STOP,
    APP_AVK_MENU_PLAY_PAUSE,
    APP_AVK_MENU_PLAY_NEXT,
    APP_AVK_MENU_PLAY_PREV,
	APP_AVK_MENU_OPEN_AUDIO,
	APP_AVK_MENU_CLOSE_AUDIO,

    APP_HS_MENU_OPEN,
    APP_HS_MENU_CLOSE,
	APP_HS_MENU_GET_CONN_BD_ADDR,
	APP_HS_MENU_GET_LAST_CONN_INFO,
    APP_HS_MENU_PICKUP,
    APP_HS_MENU_HANGUP,
    APP_HS_MENU_OPEN_AUDIO,
    APP_HS_MENU_CLOSE_AUDIO,
    APP_HS_MENU_MUTE_MIC,
    APP_HS_MENU_UNMUTE_MIC,
    APP_HS_MENU_DIAL,
    APP_HS_MENU_REDIAL,
    APP_HS_MENU_SEND_DTMF,
    APP_HS_MENU_SEND_CIND,
    APP_HS_MENU_SEND_COPS,
	APP_HS_MENU_SEND_CLCC,
    APP_HS_MENU_GET_BATT,

    APP_PBC_MENU_OPEN,
    APP_PBC_MENU_CLOSE,
    APP_PBC_MENU_ABORT,
    APP_PBC_MENU_GET_CONTACT,
    APP_PBC_MENU_GET_CCH,

    APP_MCE_NENU_OPEN,
    APP_MCE_NENU_CLOSE,
    APP_MCE_NENU_ABORT,
    APP_MCE_MENU_START_MNS,
    APP_MCE_MENU_STOP_MNS,
	APP_MCE_MENU_GET_MESSAGE,

    APP_MENU_XML_PARSER,
    APP_MENU_QUIT = 99
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

typedef struct bt_remote_dev_info {
	char name[249];
	char bd_addr[6];
} nxbt_remote_dev_t;

typedef struct bt_paired_dev_info {
	nxbt_remote_dev_t pairedDevInfo[10];
} nxbt_paired_dev_t;

typedef struct bt_connected_dev_info {
	int connected_index;
	char bd_addr[6];
} nxbt_connected_dev_t;

typedef struct bt_avk_connected_dev_info {
        nxbt_connected_dev_t connectedDevInfoAVK[1];
} nxbt_avk_connected_dev_t;

typedef struct bt_avk_element_list_info {
	char title[102];
	char artist[102];
	char album[102];
	char genre[102];
	char playTime_msec[102];
} nxbt_avk_element_list_t;

int app_get_string(const char *querystring, char *str, int len);
int app_get_choice(const char *querystring);

#endif	// __NXBTSERVICECONSOLE_H__

#include<stdio.h>
#include<time.h>
#include<string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include "wpa_ctrl.h"

#define WIFI_INTF_NAME              "wlan0"
#define WIFI_AP_PROFILE             "/tmp/wpa_supplicant.conf"
#define WPA_MON_PATH                "/var/run/wpa_supplicant/" WIFI_INTF_NAME

static struct wpa_ctrl  *mon_conn = NULL;
static int              wpas = -1;     

static void wpa_close_monitoring(void){
    printf( "%s(): mon_conn=%p, wpas=%d\n", __FUNCTION__, mon_conn, wpas);
    //close out connection to wpa
    if(mon_conn){
        //close wpa connection
        wpa_ctrl_detach(mon_conn);
        wpa_ctrl_close(mon_conn);
        mon_conn = NULL;
    }
    if(wpas >= 0){
        //this close is unnecessary, wpa_ctrl_close does this
        //close(wpas);
        wpas = -1;
    }
}

//用于检测有网没网，突然断网又恢复
static void wpa_process_msg(const char *msg){
    const char msg1[] = "Trying to authenticate";
    const char msg2[] = "Trying to associate";
    const char msg3[] = "Associated with";
    int msg1_len = -1;
    int msg2_len;
    int msg3_len;
    int msg4_len;
    int msg5_len;
    int msg6_len;
    const char *pos = msg;
    static int network_not_found_cnt = 0;
    const int network_timeout = 4;
	
	
	/** Authentication completed successfully and data connection enabled */
	// #define WPA_EVENT_CONNECTED "CTRL-EVENT-CONNECTED "
	/** Disconnected, data connection is not available */
	// #define WPA_EVENT_DISCONNECTED "CTRL-EVENT-DISCONNECTED "
	/** Network block temporarily disabled (e.g., due to authentication failure) */
	// #define WPA_EVENT_TEMP_DISABLED "CTRL-EVENT-SSID-TEMP-DISABLED "
	

    if (msg1_len == -1) {
        msg1_len = strlen(msg1);
        msg2_len = strlen(msg2);
        msg3_len = strlen(msg3);
        msg4_len = strlen(WPA_EVENT_CONNECTED);
        msg5_len = strlen(WPA_EVENT_DISCONNECTED);
        msg6_len = strlen(WPA_EVENT_TEMP_DISABLED);
    }

    //skip priority
	//<3>CTRL-EVENT-SCAN-STARTED
    if(*pos == '<'){
        pos = strchr(pos, '>');
        pos = pos == NULL? msg : pos+1;
    }
    //messages of interest
    if(strncmp(pos, msg1, msg1_len) == 0){
        printf("WIFI_STATE_WPA_AUTHENTICATING\n");
    }
    else if(strncmp(pos, msg2, msg2_len) == 0){
        printf("WIFI_STATE_WPA_ASSOCIATING\n");
    }
    else if(strncmp(pos, msg3, msg3_len) == 0){
        printf("WIFI_STATE_WPA_ASSOCIATED\n");
    }
    else if(strncmp(pos, WPA_EVENT_CONNECTED, msg4_len) == 0){
        network_not_found_cnt = 0;
        printf("WIFI_STATE_WPA_CONNECTED\n");
    }
    else if(strncmp(pos, WPA_EVENT_DISCONNECTED, msg5_len) == 0){
        network_not_found_cnt = 0;
        //log when disconnected events occur
        printf( "%s(): %s\n",__FUNCTION__, msg);
        printf("WIFI_STATE_WPA_DISCONNECTED\n"); 
    }
    else if(strncmp(pos, "CTRL-EVENT-NETWORK-NOT-FOUND", strlen("CTRL-EVENT-NETWORK-NOT-FOUND")) == 0){
        //log when disconnected events occur
		//网口wlan 0 down？
        printf( "%s(): %s network_not_found_cnt: %d\n",__FUNCTION__, msg, network_not_found_cnt);
        // if(++network_not_found_cnt >= network_timeout && ipRcvd){
            // RESTART("network is down");
        // }
    }
    else if(strncmp(pos, WPA_EVENT_TEMP_DISABLED, msg6_len) == 0){
        //log if authentication fails
		//突然换密码？
        printf( "%s(): %s\n", __FUNCTION__, msg);
        printf("WIFI_STATE_WPA_AUTH_FAILED\n");
    }
}

int main(){
	// system("wpa_supplicant -B -D nl80211 -i wlan0 -c  %s",WIFI_AP_PROFILE);
	//open connection to wpa_supplicant
	mon_conn = wpa_ctrl_open(WPA_MON_PATH);
	if(mon_conn == NULL){
		printf( "wpa failed to open\n");
		return -1;
	}
	else{
		printf( "%s(): wpa sucessfully opened\n", __FUNCTION__);
	}
	if(wpa_ctrl_attach(mon_conn) == 0){
		printf( "%s(): wpa successfully attached\n", __FUNCTION__);
	}
	else{
		printf( "wpa failed to attach\n");
	}
	//wpa monitor setup
	wpas = wpa_ctrl_get_fd(mon_conn);
	printf( "WIFI/%s(): wpas: %d\n", __FUNCTION__, wpas);
	
	int result;
	int nfds = 0;
    fd_set rfds;
	char buf[256];
    size_t len = sizeof(buf) - 1;
	while(1){
		struct timeval tv = {.tv_sec = 3, .tv_usec = 0};
        nfds = wpas + 1;

        FD_ZERO(&rfds);
        if (wpas >= 0)        FD_SET(wpas, &rfds);
		result = select(nfds, &rfds, NULL, NULL, &tv);
        if(result < 0){
            if(errno == EINTR){
                continue;
            }
            printf( "select");
            break;
        }
        else if(result > 0){
			if(wpas > 0){
			//wpa event
				if(FD_ISSET(wpas, &rfds) && mon_conn != NULL){
					len = sizeof(buf) - 1;
					int res = wpa_ctrl_recv(mon_conn, buf, &len);
					if(res < 0){
						printf( "wpa_ctrl_recv: res=%d", res);
						break;
					}
					buf[len] = '\0';
					printf( "-------buf:%s\n", buf);
					//process received message
					wpa_process_msg(buf);
				}
			}
		}
	}
	wpa_close_monitoring();
	return 0;
}
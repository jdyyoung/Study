/* Copyright ?2015 HiTEM Engineering, Inc.  Skybell, Inc.
 * Proprietary information, NDA required to view or use this software.  
 * All rights reserved.
 */

//Name:				wifi.c
//Date:				2015-02-16
//Company:			HiTEM Engineering Inc.
//Description:                  wifi task, related functions, including sta/ap mode switching
//Version:			1.0

#include "lib.h"
#include "encrypt_file.h"
#include "iperf_api.h"
#include "iperf.h"
#include "iperf_queue.h"
#include "iperf_util.h"
#include "iperf_units.h"
#include "wifi_tx_pwr.h"
#include "video_adaptive.h"

#define Y_DBG 0 //0:enable debug print;1:disable debug print
#define SET_STATE(state) set_state(state, __FUNCTION__, __LINE__)
#define UP   1
#define DOWN 0

//wifi information, status, and fsm state
static wifi_info_t      wifi;
static wifi_mode_t      mode = WIFI_MODE_NONE;
static wifi_state_t     state = WIFI_STATE_COMATOSE;
//ap scanning
static struct list_head wifi_ap_list;                    // list of ap_list_t AP objects
static MUTEX            list_mutex;
static MUTEX            scan_mutex;

//wifi event monitoring
static THREAD           *wifi_thread;
static int              cmd_pipe[2] = {-1, -1};     //command pipe
static int              nlk = -1;                   //kernel monitoring
static int              nlr = -1;                   //route monitoring
static int              wpas = -1;                  //wpa monitoring
static struct wpa_ctrl  *mon_conn = NULL;
static bool             closingProcesses = FALSE;       //flag for when wpa or dhcp-server processes are being closed
static bool             shuttingDown = FALSE;           //hard disable, called by wifi_enable(FALSE)
static bool             modeTransition = FALSE;         //flag indicating if mode transition is going on
static drv_check_t      chk = {FALSE, FALSE, FALSE}; //, FALSE};       //used to check driver status flags
static bool             ipRcvd = FALSE;

//wifi routing to clients
typedef struct{
    struct list_head list;
    void             (*handler)(int button_event);
}CLIENT;

static MUTEX            client_mutex;
static struct list_head client_list;
static int              client_count;

wifi_state_t wifi_state(void){
    return state;
}

char* wifi_state_string(int state){
    switch (state){
    case WIFI_STATE_COMATOSE:           return "COMATOSE";
    case WIFI_STATE_IDLE:               return "IDLE";
    case WIFI_STATE_WAITING:            return "WAITING";
    case WIFI_STATE_DRIVER_READY:       return "DRIVER READY";
    case WIFI_STATE_STA_DRV_REMOVED:    return "STA DRIVER REMOVED";
    case WIFI_STATE_AP_DRV_REMOVED:     return "AP DRIVER REMOVED";
    case WIFI_STATE_INTF_UP:            return "INTERFACE UP";
    case WIFI_STATE_INTF_DOWN:          return "INTERFACE DOWN";
    case WIFI_STATE_WPA_AUTHENTICATING: return "AUTHENTICATING";
    case WIFI_STATE_WPA_ASSOCIATING:    return "ASSOCIATING";
    case WIFI_STATE_WPA_ASSOCIATED:     return "ASSOCIATED";
    case WIFI_STATE_WPA_CONNECTED:      return "CONNECTED";
    case WIFI_STATE_WPA_DISCONNECTED:   return "DISCONNECTED";
    case WIFI_STATE_WPA_AUTH_FAILED:    return "AUTH FAILED";
    case WIFI_STATE_IP_OBTAINED:        return "IP OBTAINED";
    case WIFI_STATE_READY:              return "READY";
    case WIFI_STATE_PAIRING:            return "PAIRING";
    case WIFI_STATE_AP_LIST_READY:      return "AP_LIST_READY";
    case WIFI_STATE_DEAD:               return "DEAD";
    default:                            return "UNKNOWN";
    }
}

char* wifi_mode_string(int mode){
    switch (mode) {
    case WIFI_MODE_NONE:   return "NONE";
    case WIFI_MODE_STA:    return "STA";
    case WIFI_MODE_AP:     return "AP";
    case WIFI_MODE_APSCAN: return "APSCAN";
    default:               return "UNKNWON";
    }
}

char* wifi_cmd_string(int cmd){
    switch(cmd){
    case WIFI_CMD_SET_MODE_STA:    return "SET_MODE_STA";
    case WIFI_CMD_SET_MODE_AP:     return "SET_MODE_AP";
    case WIFI_CMD_SET_MODE_APSCAN: return "SET_MODE_APSCAN";
    case WIFI_CMD_SET_MODE_NONE:   return "SET_MODE_NONE";
    case WIFI_CMD_ENABLE_PWR:      return "ENABLE_PWR";
    case WIFI_CMD_SHUTDOWN:        return "SHUTDOWN";
    case WIFI_CMD_GET_INFO:        return "GET_INFO";
    case WIFI_CMD_SCAN_FOR_APS:    return "SCAN_FOR_APS";
    case WIFI_CMD_DIE:             return "DIE";
    default:                       return "UNKNOWN";
    }
}

/**********************speed test related variables****************************/
static SPEED_TEST _test;
//static uint32_t speed_list[NUM_SPEEDS] = {SPEED_HIGH, SPEED_MED, SPEED_LOW};
static MUTEX speedtest_mutex;
static BOOL speedtest_running = FALSE;
static MUTEX iperf3_mutex;
static BOOL iperf3_running = FALSE;

static int wifi_run_udp_bitrate_test(SPEED_TEST* profile){
    struct iperf_test *test = NULL;
    struct iperf_stream *sp = NULL;
    float end_time;
    //float start_time;
    iperf_size_t bytes_sent;

    DBUG(1, "%s(): testing %u bps to %s:%d\n", __FUNCTION__, profile->target_bitrate, profile->host, profile->port);

    test = iperf_new_test();
    if(test == NULL){
        ALERT(0, "failed to create iperf test\n");
        goto iperf_exit;
    }
    iperf_defaults(test);
    //djp - specify output log file
    if(iperf_set_test_output_file(test, SPEED_TEST_JSON_OUTPUT_PATH) < 0){
        ALERT(0, "failed to open json output file!\n");
        goto iperf_cleanup;
    }
    iperf_set_test_role(test, 'c');
    iperf_set_test_server_hostname(test, profile->host);
    iperf_set_test_server_port(test, profile->port);
    iperf_set_test_duration(test, profile->duration_sec);
    iperf_set_test_reporter_interval(test, 1);
    iperf_set_test_stats_interval(test, 1);
    iperf_set_test_json_output(test, 1);

    //djp - set to UDP
    set_protocol(test, Pudp);
    iperf_set_test_blksize(test, profile->block_size);
    iperf_set_test_rate(test, profile->target_bitrate);

    if(iperf_run_client(test) < 0 ) {
        ALERT(0, "error - %s\n", iperf_strerror(i_errno));
        goto iperf_cleanup;
    }
    sp = SLIST_FIRST(&test->streams);
    if(sp){
        //start_time = 0.;
        end_time = timeval_diff(&sp->result->start_time, &sp->result->end_time);
        bytes_sent = sp->result->bytes_sent;
        profile->achieved_bitrate = 8*(bytes_sent / end_time);
        profile->total_packets = (sp->packet_count - sp->omitted_packet_count);
        profile->lost_packets = sp->cnt_error;
        profile->lost_percent = 100.0 * (profile->lost_packets)/(profile->total_packets);
    }
    iperf_free_test(test);
    return 0;

iperf_cleanup:
    iperf_free_test(test);
    unlink(SPEED_TEST_JSON_OUTPUT_PATH);
iperf_exit:
    return -1;
}

static int wifi_run_udp_bitrate_test_thread(THREAD *thread){
    SPEED_TEST test;
    int rc;

    //copy test parameters
    memcpy(&test, thread->private_data, sizeof(SPEED_TEST));

    //delete existing /skybell/misc/speedtest.json
    unlink(SPEED_TEST_JSON_OUTPUT_PATH);
    //disable firewall
    firewall_off();
    //clear out result parameters
    CLEAR_TEST_STATS(test);
    //run test
    ALERT(0, "starting udp bitrate test %u bps to %s:%u\n", test.target_bitrate, test.host, test.port);
    rc = wifi_run_udp_bitrate_test(&test);
    if(rc == 0){
        ALERT(0, "--test results--\n");
        ALERT(0, "achieved bitrate: %u\n", test.achieved_bitrate);
        ALERT(0, "total packets: %u\n", test.total_packets);
        ALERT(0, "lost packets: %u\n", test.lost_packets);
        ALERT(0, "lost percent: %f\n", test.lost_percent);
    }else{
        ALERT(0, "iperf3 test failed\n");
    }
    //reenable firewall
    firewall_on();
    speedtest_running = FALSE;
    MUTEX_UNLOCK(&speedtest_mutex);
    thread_cancel(thread);
    return 0;
}

int wifi_start_udp_bitrate_test(SPEED_TEST test){
    if(speedtest_running){
        return -1;
    }
    MUTEX_LOCK(&speedtest_mutex);   //this will be unlocked when test is over
    speedtest_running = TRUE;

    //copy data into _test
    _test.target_bitrate = test.target_bitrate;
    _test.port = test.port;
    local_strncpy(_test.host, test.host, sizeof(_test.host));
    _test.duration_sec = test.duration_sec;
    if(_test.duration_sec == 0)
        _test.duration_sec = 10;        //use default if none specified
    _test.block_size = test.block_size;
    if(_test.block_size == 0)
        _test.block_size = TEST_UDP_BLOCK_SIZE_MAX;  //use default if none specified

    //start thread
    if(thread_assign("wifi_run_udp_bitrate_test", wifi_run_udp_bitrate_test_thread, &_test) == NULL){
        speedtest_running = FALSE;
        MUTEX_UNLOCK(&speedtest_mutex);
        return -1;
    }
    return 0;
}

static int wifi_run_iperf3_server(THREAD *thread){
    int i;
    int durationSec = *(int*)(thread->private_data);

    //disable firewall
    firewall_off();
    DBUG(0,"starting iperf3 in server mode\n");
    //start iperf3 server
    do_system("/usr/bin/iperf3 -s &");
    for(i = 0; i < durationSec; ++i){
        usleep(1000000);
    }
    //kill iperf3 server
    do_system("killall iperf3 > /dev/null 2>&1");
    DBUG(0,"iperf3 server terminated\n");
    //reenable firewall
    firewall_on();
    iperf3_running = FALSE;
    MUTEX_UNLOCK(&iperf3_mutex);

    thread_cancel(thread);
    return 0;
}

int wifi_start_iperf3_server(int durationSec){
    static int _durationSec;

    if(iperf3_running){
        return -1;
    }
    MUTEX_LOCK(&iperf3_mutex);
    _durationSec = durationSec;
    iperf3_running = TRUE;
    //start thread
    if(thread_assign("wifi_run_iperf3_server", wifi_run_iperf3_server, &_durationSec) == NULL){
        speedtest_running = FALSE;
        MUTEX_UNLOCK(&iperf3_mutex);
        return -2;
    }
    return 0;
}

//we're using a mutex now so this would fail
#if 0
void wifi_determine_best_speed(char *host, int port){
    int selection;
    BOOL found;
    int rc;
    for(selection = 0, found = FALSE; !found && selection < NUM_SPEEDS; ++selection){
        CLEAR_TEST_STATS(speed_test);
        speed_test.target_bitrate = speed_list[selection];
        rc = wifi_run_udp_bitrate_test(host, port, &speed_test);
        if(rc < 0){
            ALERT(0, "failed to run bitrate test!\n");
            break;
        }
        DISP_SPEED_RESULTS(speed_test);
        if((speed_test.lost_percent <= ACCEPTABLE_LOSS) && (BITRATE_DIFF(speed_test) < BITRATE_MARGIN)){
            ALERT(0, "FOUND - speed profile has acceptable packet loss at target bitrate!\n");
            found = TRUE;
        }else if(selection < (NUM_SPEEDS-1)){
            ALERT(0, "testing again with lower speed...\n");
            sleep(1);
        }
    }
    if(!found)
        ALERT(0, "all speeds had unacceptable packet loss, going with lowest\n");
}
#endif

//2015-06-04, hitemeng - we are simply killing wifi related processes at startup instead
void wifi_signal_handler(int signo){
    #if 0
    ALERT(0, "wifi module received signal %d -- shutting down\n", signo);
    wifi_shutdown();
    //wifi_cmd(WIFI_CMD_SHUTDOWN);
    #endif
}

void* wifi_register_client(void(*handler)(int wifi_event)){
    CLIENT* client = (CLIENT*) HEAP_ALLOC(sizeof(CLIENT));

    if (client) {
        client->handler = handler;
        MUTEX_LOCK(&client_mutex);
        list_add_tail(&client->list, &client_list);
        client_count++;
        MUTEX_UNLOCK(&client_mutex);
    }
    return client;
}


int __wifi_unregister_client(void* this)
{
    CLIENT* client = (CLIENT*)this;

    if (client) {
        if (client_count == 0)
            return -1;
        list_del(&client->list);
        client_count--;
        heap_free(client);
    }
    return 0;
}


int wifi_unregister_client(void* this)
{
    int rc;

    MUTEX_LOCK(&client_mutex);
    rc = __wifi_unregister_client(this);
    MUTEX_UNLOCK(&client_mutex);
    return rc;
}


static void route_to_clients(int wifi_event){
    MUTEX_LOCK(&client_mutex);
    if (client_count) {
        struct list_head* this;
        struct list_head* next;
        list_for_each_safe(this, next, &client_list) {
            CLIENT* client = list_entry(this, CLIENT, list);
            if (client->handler) {
                client->handler(wifi_event);
            }
        }
    }
    MUTEX_UNLOCK(&client_mutex);
}

static void set_state(wifi_state_t new_state, const char* function, int lineno)
{
    if (new_state != state) {
        DBUG(1, "%s(): WIFI(%s --> %s) @ %s():%d\n", 
             __FUNCTION__, wifi_state_string(state), wifi_state_string(new_state), function, lineno); 
        state = new_state;
    }
}

#if 1
SCODE wifi_set_mac_addr(uint8_t* mac)
{
	char szCmd[64];
	int rc = 0;
	int status =0;
	
//cc	sprintf(szCmd,"/skybell/bin/macrw.arm %02X:%02X:%02X:%02X:%02X:%02X",
//cc		mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    sprintf(szCmd,"/skybell/bin/rtwpriv wlan0 efuse_set wmap,16A,%02X%02X%02X%02X%02X%02X",
    mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);//cc
	status = system(szCmd);
	rc = WEXITSTATUS(status);
	sleep(3);
	printf("!!!!!!!!!!!!!wifi_set_mac_addr rc [%d]!!!!!!!!!!1",rc);//cc
//	return rc==55?S_OK:S_FAIL;
	return rc==0?S_OK:S_FAIL;//cc
}

#else
SCODE wifi_set_mac_addr(uint8_t* mac, BOOL iwprivBypass){
    int rc1, rc2, rc3;
    if(!iwprivBypass){
        #if defined(SKYBELL_HD)
            rc1 = do_system("/usr/bin/iwpriv ra0 e2p 4=%02X%02X", mac[1], mac[0]);
            rc2 = do_system("/usr/bin/iwpriv ra0 e2p 6=%02X%02X", mac[3], mac[2]);
            rc3 = do_system("/usr/bin/iwpriv ra0 e2p 8=%02X%02X", mac[5], mac[4]);
        #else
            rc1 = do_system("/sbin/iwpriv ra0 e2p 4=%02X%02X", mac[1], mac[0]);
            rc2 = do_system("/sbin/iwpriv ra0 e2p 6=%02X%02X", mac[3], mac[2]);
            rc3 = do_system("/sbin/iwpriv ra0 e2p 8=%02X%02X", mac[5], mac[4]);
        #endif
    }else{
        rc1 = rc2 = rc3 = 0;
    }
    if(rc1 == 0 && rc2 == 0 && rc3 == 0) {
        char macstr[64];
        int rc;

        //need to do a reboot for the changes to take effect
        WARN(0, "CHANGED MAC TO [" WIFI_MACSTR "] !\n", WIFI_MAC(mac));
        snprintf(macstr, sizeof(macstr), "\x2 MAC SET " WIFI_MACSTR "\n\x3", WIFI_MAC(mac));
        rc = serial_sync_write(macstr, strlen(macstr), "MAC", 3000, NULL, NULL);
        if (rc == 0) {
            //delayed_reboot("WROTE MAC ADDRESS", 8); // v1087 2016/5/6 or later... removed this because dragan did not want it for factory
            return S_OK;
        }

        ERROR(0, "ERROR CHANGING MAC TO [" WIFI_MACSTR "] writing to ST-MICRO!\n", WIFI_MAC(mac));
        return S_FAIL;
    }

    ERROR(0, "ERROR CHANGING MAC TO [" WIFI_MACSTR "] rc1=%d, rc2=%d, rc3=%d\n",
          WIFI_MAC(mac), rc1, rc2, rc3);
    return S_FAIL;
}

#endif 

SCODE wifi_obtain_mac_info(uint8_t* return_mac){
    int fd;
    struct ifreq ifr;
    uint8_t *mac;

    fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
    if(fd < 0){
        WARN(0, "socket(AF_INET, SOCK_DGRAM)=%d [failed to open socket]\n", fd);
        return S_FAIL;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    //try wlan0 first
    strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
    int rc = ioctl(fd, SIOCGIFHWADDR, &ifr);
    if(rc != S_OK){
        //WARN(0, "ioctl(SIOCGIFHWADDR) failed trying another interface\n");
        //try ra0
        strncpy(ifr.ifr_name, "ra0", IFNAMSIZ-1);
        rc = ioctl(fd, SIOCGIFHWADDR, &ifr);
        if(rc != S_OK){
            WARN(0, "neither wlan0 or ra0 working\n");
            close(fd);
            return S_FAIL;
        }
    }

    close(fd);

    mac = (uint8_t *)ifr.ifr_hwaddr.sa_data;
    memcpy(wifi.mac, mac, sizeof(wifi.mac));
    if (return_mac != NULL) {
        memcpy(return_mac, mac, sizeof(wifi.mac));
        /*
        printf("%s.%d got mac from ra0 %02X:%02X:%02X:%02X:%02X:%02X!\n", __FUNCTION__, __LINE__,
                wifi.mac[0],
                wifi.mac[1],
                wifi.mac[2],
                wifi.mac[3],
                wifi.mac[4],
                wifi.mac[5]);
        */
    }
    return S_OK;
}


void wifi_macstr_to_mac(u8* uboot_mac_str, u8* uboot_mac){
    char* next;
    int i;

    // uboot MAC is of the format 11:22:33:AA:BB:CC
    for (next = (char*)uboot_mac_str, i = 0; i < 6; ++i, ++next /*get past ':'*/) {
        uboot_mac[i] = strtoul(next, &next, 16);
    }
}


#if 0
void wifi_mac_sanity_check(void)
{
    if (!factory()) {
        int i, maclen, mismatch; 
        u8 macstr[32];
        u8 stmicro_mac[6];
        u8 mac[6];

        int rc = wifi_obtain_mac_info(mac);
        if (rc != S_OK) {
            WARN(86, "MAC: wifi_obtain_mac_info()=%d FAILED!\n", rc);
            memset(mac, 0, sizeof(mac));
        } else {
            if (mac[0] != SKYBELL_MAC_0 ||
                mac[1] != SKYBELL_MAC_1 ||
                mac[2] != SKYBELL_MAC_2) {
                WARN(86, "MAC: mac[3]=%x:%x:%x FAILED!\n", mac[0], mac[1], mac[2]);
                memset(mac, 0, sizeof(mac));
            }
        }

        maclen = sizeof(macstr);
        rc = serial_sync_write("\x2 MAC GET\n\x3", 11, "MAC", 3000, macstr, &maclen);
        if (rc) {
            WARN(86, "MAC: serial_sync_write(MAC GET)=%d FAILED!\n", rc);
            memset(stmicro_mac, 0, sizeof(stmicro_mac));
        } else {
            wifi_macstr_to_mac(macstr+6, stmicro_mac);
            if (stmicro_mac[0] != SKYBELL_MAC_0 ||
                stmicro_mac[1] != SKYBELL_MAC_1 ||
                stmicro_mac[2] != SKYBELL_MAC_2) {
                WARN(86, "MAC: stmicro_mac[3]=%x:%x:%x FAILED!\n", stmicro_mac[0], stmicro_mac[1], stmicro_mac[2]);
                memset(stmicro_mac, 0, sizeof(stmicro_mac));
            }
        }

        for (mismatch = i = 0; i < sizeof(mac); ++i) {
            if (mac[i] != stmicro_mac[i]) {
                WARN(86, "MAC: mac[%d]=%x != stmicro_mac[%d]=%x FAILED!\n", i, mac[i], i, stmicro_mac[i]);
                mismatch += 1;
            }
        }

        if (mismatch) {
            #if 1
            ALERT(0, "MAC MISMATCH!!!\n");
            dump_memory("WIFI_MAC ", 0, mac, sizeof(mac));
            dump_memory("STM_MAC  ", 0, stmicro_mac, sizeof(stmicro_mac));
            #else
            bool wifi_sane = (mac[0] == SKYBELL_MAC_0 && mac[1] == SKYBELL_MAC_1 && mac[2] == SKYBELL_MAC_2);
            bool stmicro_sane = (stmicro_mac[0] == SKYBELL_MAC_0 && stmicro_mac[1] == SKYBELL_MAC_1 && stmicro_mac[2] == SKYBELL_MAC_2);

            if (!wifi_sane && !stmicro_sane) {
                ALERT(0, "NEITHER WIFI nor STMICRO MAC have the correct company code -- not changing!\n");
                return;
            }
            if (wifi_sane && stmicro_sane) {
                ALERT(0, "BOTH WIFI and STMICRO MAC have the correct company code -- not changing!\n");
                return;
            }

            if (wifi_sane) {
                ALERT(0, "WIFI MAC SANE, STMICRO MAC INVALID -- setting STMICRO MAC to WIFI MAC!\n"); 
            } else
            if (stmicro_sane) {
                ALERT(0, "STMICRO MAC SANE, WIFI MAC INVALID -- setting WIFI MAC to STMICRO MAC!\n"); 
            }
            #endif
        }
    }
}
#endif


static SCODE wifi_obtain_ip_info(void){
    struct ifaddrs *myaddrs, *ifa;
    int status;
    struct sockaddr_in *s4;
    #if WIFI_IP_V6_EN
    struct sockaddr_in6 *s6;
    #endif

    status = getifaddrs(&myaddrs);
    if(status < 0){
        WARN(errno, "getifaddrs failed!");
        //exit(1);
        return S_FAIL;
    }

    for(ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next){
        if(NULL == ifa->ifa_addr){
            continue;
        }
        if((ifa->ifa_flags & IFF_UP) == 0) {
            continue;
        }
        if(strcmp(ifa->ifa_name, WIFI_INTF_NAME) != 0){
            continue;
        }
        if(AF_INET == ifa->ifa_addr->sa_family){
            s4 = (struct sockaddr_in *)(ifa->ifa_addr);
            wifi.ipv4.addr = *(uint32_t *)&(s4->sin_addr);
        }
        #if WIFI_IP_V6_EN
        else if(AF_INET6 == ifa->ifa_addr->sa_family) {
            s6 = (struct sockaddr_in6 *)(ifa->ifa_addr);
            memcpy(wifi.ipv6.s6_addr, (uint8_t *)&(s6->sin6_addr), 16);
        }
        #endif
    }
    freeifaddrs(myaddrs);
    return S_OK;
}

static SCODE wifi_dns_resolve(const char *domain, uint8_t *dst){
    struct addrinfo *res, *ainfo;
    struct addrinfo hints;
    int error;
    uint8_t *ip;
    struct sockaddr_in *ipv4;
    #if WIFI_IP_V6_EN
    struct sockaddr_in6 *ipv6;
    #endif

    memset((char *)&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    #if WIFI_IP_V6_EN
    hints.ai_family = AF_UNSPEC;
    #else
    hints.ai_family = AF_INET;
    #endif

    error = getaddrinfo(domain, NULL, &hints, &res);

    if(error != 0){
        ERROR(errno, "getaddrinfo: %s\n", gai_strerror(error));
        return error;
    }

    for(ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next) {
        switch (ainfo->ai_family) {
            case AF_INET:
                ipv4 = (struct sockaddr_in *)ainfo->ai_addr;
                ip = (uint8_t *)&(ipv4->sin_addr);
                memcpy(dst, ip, 4);
                goto finish;
            #if WIFI_IP_V6_EN
            case AF_INET6:
                ipv6 = (struct sockaddr_in6 *)p->ai_addr;
                ip = (uint8_t *)&(ipv6->sin6_addr);
                memcpy(dst, ip, 16);
                break;
            #endif
            default:
                break;
        }
      }
    finish:
    freeaddrinfo(res);
    return S_OK;
}

SCODE wifi_get_info(wifi_info_t *wifi_data, bool sta_mode){
    uint8_t ip[16];
    
    //zero out wifi data
    memset(&wifi, 0, sizeof(wifi));

    //check if wifi is ready
    if(state != WIFI_STATE_IP_OBTAINED && state != WIFI_STATE_READY){
        ALERT(0, "wifi is not up, cannot obtain info (state %s)\n", wifi_state_string(state));
        return S_FAIL;
    }
    
    if(wifi_obtain_mac_info(NULL) != S_OK){
        ALERT(0, "failed to obtain mac info\n");
        return S_FAIL;
    }
    if(wifi_obtain_ip_info() != S_OK){
        ALERT(0, "failed to obtain ip info\n");
        return S_FAIL;
    }
    
    if (sta_mode && strlen(cfg.server_uri) == 0) {
        //we're only going to worry about IPV4 here    
        if(wifi_dns_resolve(cfg.dns_server_name, ip) != S_OK){
            WARN(0, "failed to resolve cmd server domain '%s'\n", cfg.dns_server_name);
            memset(&wifi.cmd_server.addr, 0, 4);
        }
        else
            memcpy(&wifi.cmd_server.addr, ip, 4);
    }
    
    *wifi_data = wifi;
    return S_OK;
}

static void wpa_close_monitoring(void){
    DBUG(1, "%s(): mon_conn=%p, wpas=%d\n", __FUNCTION__, mon_conn, wpas);
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

// kill off all processes created by wifi thread, shutdown
// all of the interfaces and remove the wifi drivers
static void kill_wifi(void)
{
    // kill any processes created by wifi thread
    // stop dhcp-server
	do_system("killall udhcpc wpa_supplicant hostapd udhcpd dnsmasq");//Charles
    do_system(WIFI_INTF_DOWN);//Charles
}

static void die(THREAD* thread){
    ALERT(0, "WIFI THREAD DEATH!!!\n");
    wifi_thread = NULL;
    thread_cancel(thread);
    SET_STATE(WIFI_STATE_DEAD);

    wpa_close_monitoring();
    if (nlr >= 0)  {close(nlr); nlr = -1;}
    if (nlk >= 0)  {close(nlk); nlk = -1;}
    close_pipe(cmd_pipe);

    kill_wifi();
}

void wifi_pwr_save(bool enable){
    #if 0
    MUTEX_LOCK(&pwr_mutex);
    if(enable)
        do_system("iwpriv ra0 set PSMode=Legacy_PSP");    //wifi driver goes to sleep and wakes up every 100ms
    else
        do_system("iwpriv ra0 set PSMode=CAM");    //normal wifi power mode
    MUTEX_UNLOCK(&pwr_mutex);
    #endif
}


#if 0
static void wifi_sync_power_setting_to_driver_config_file(void)
{
    char tempfilename[MAX_PATHNAME];
    FILE* ifile = fopen(WIFI_STA_CONFIG_FILENAME, "re");
    FILE* ofile;

    temp_filename(tempfilename, sizeof(tempfilename));
    ofile = fopen(tempfilename, "we");
    if (ofile != NULL && ifile != NULL) {
        char line[80];

        while (fgets(line, sizeof(line), ifile) != NULL) {
            if (strncmp(line, "PSMode=", 7) == 0)
                snprintf(line+7, 64, /*cfg.wifi_power_save? "Legacy_PSP\n" :*/ "CAM\n");
            fwrite(line, strlen(line), 1, ofile);
        }
    }
    if (ifile != NULL) fclose(ifile);
    if (ofile != NULL) fclose(ofile);
    copy_file(tempfilename, WIFI_STA_CONFIG_FILENAME);
}
#endif


static void wifi_start_STA(){
	DBUG(0,"wifi_start_STA() start\n");//Charles

    //we're just starting up STA mode, so can't have received an IP address
    //set this flag false so that as soon as we're connected WiFi, we try to obtain
    //an ip address
    ipRcvd = FALSE;

    SET_STATE(WIFI_STATE_WAITING);     //wait until driver properly loads
    wpa_close_monitoring();
#if 1//Charles
    do_system("killall hostapd dnsmasq");
    do_system("wpa_supplicant -B -D nl80211 -i wlan0 -c %s",WIFI_AP_PROFILE);
    do_system("udhcpc -i wlan0");
    printf("wifi_start_STA() end\n");//Charles
#else //Charles
    //wifi_sync_power_setting_to_driver_config_file();

#if WIFI_DRV_NEW_MT7601U == 0
    //make sure AP driver is not loaded
    if(module_installed(WIFI_AP_DRV_NAME)){
        module_unload(WIFI_AP_DRV_NAME, 5);
    }
#else
    do_system("sh /drivers/unload_AP_drv");
#endif
    do_system("sh /drivers/load_STA_drv");
#endif//Charles
}

static void wifi_stop_STA(){
    DBUG(1, "%s()...\n", __FUNCTION__);

    //wait for link to go down
    closingProcesses = TRUE;
    SET_STATE(WIFI_STATE_WAITING);

    //stop wpa monitoring
    wpa_close_monitoring();

    //stop dhcpc client
    //do_system("killall udhcpc");
    do_system("kill $(pidof udhcpc) > /dev/null 2>&1");
    ipRcvd = FALSE;
    //stop wpa_supplicant process
    //do_system("/etc/init.d/wpa_supplicant stop");
    //do_system("killall wpa_supplicant");
    do_system("kill $(pidof wpa_supplicant) > /dev/null 2>&1");

    //TODO - temporary fix so kernel panic doesn't occur but we need better method
    DBUG(1, "%s(): TOTAL KLUDGE, SLEEPING 3 seconds!!!\n", __FUNCTION__);
    sleep(3);
}

/**
 * @brief   retrieves stored mac address from cypress
 * @param   [out] mac pointer to output mac buffer
 */
int wifi_get_stored_mac(u8 *mac){
    int stored_mac[6];
    int length;
    u8 macstr[32];
    if(mac){
        length = sizeof(macstr);
        int rc = serial_sync_write("\x2 MAC GET\n\x3", 11, "MAC", 3000, macstr, &length);
        if (rc) {
            WARN(0, "Sorry, serial_sync_write(MAC GET)=%d FAILED!\n", rc);
            goto exit;
        }
        sscanf((char*)(macstr+6), WIFI_MACSTR, stored_mac+0, stored_mac+1, stored_mac+2, stored_mac+3, stored_mac+4, stored_mac+5);
        int i;
        for(i = 0; i < 6; ++i){
            mac[i] = (u8)stored_mac[i];
        }
        return 0;
    }
exit:
    return -1;
}

#ifdef YYY
/**
 * @brief   polls until STA driver is unloaded as indicated by lsmod
 * @return  -1 if failed to unload, 0 if success
 */
int wifi_wait_until_STA_drv_unloaded(void){
    FILE* file = NULL;
    char  filename[MAX_PATHNAME];
    char  line[64];
    int retry;
    char *sta_drv = NULL;

    for(retry = 0; retry < 5; ++retry){
        DBUG(1, "is STA drv unloaded? \n", __FUNCTION__, __LINE__);
        temp_filename(filename, sizeof(filename));
        unlink(filename);
        do_system("lsmod > %s", filename);
        file = fopen(filename, "re");
        if (file == NULL) {
            WARN(errno, "unable to open file '%s'", filename);
            goto error;
        }
        //search for STA driver
        while (fgets(line, sizeof(line), file) != NULL) {
            sta_drv = strstr(line, "mt7601u");
            if(sta_drv){
                break;
            }
        }
        unlink(filename);
        if(!sta_drv){
            break;
        }else{
            sleep(1);
            DBUG(1, "STA drv is NOT unloaded, trying again...\n", __FUNCTION__, __LINE__);
        }
    }
    return sta_drv ? -1 : 0;

error:
    unlink(filename);
    return -1;
}

/**
 * @brief   polls until AP driver is unloaded as indicated by lsmod
 * @return  -1 if failed to load, 0 if success
 */
int wifi_wait_until_AP_drv_loaded(void){
    FILE* file = NULL;
    char  filename[MAX_PATHNAME];
    char  line[64];
    int retry;
    char *ap_drv = NULL;

    for(retry = 0; retry < 5; ++retry){
        DBUG(1, "is AP drv loaded? \n", __FUNCTION__, __LINE__);
        temp_filename(filename, sizeof(filename));
        unlink(filename);
        do_system("lsmod > %s", filename);
        file = fopen(filename, "re");
        if (file == NULL) {
            WARN(errno, "unable to open file '%s'", filename);
            goto error;
        }
        //search for STA driver
        while (fgets(line, sizeof(line), file) != NULL) {
            ap_drv = strstr(line, "mt7601Uap");
            if(ap_drv){
                break;
            }
        }
        unlink(filename);
        if(ap_drv){
            break;
        }else{
            sleep(1);
            DBUG(1, "AP drv is NOT loaded, trying again...\n", __FUNCTION__, __LINE__);
        }
    }
    return ap_drv ? 0 : -1;

error:
    unlink(filename);
    return -1;
}

/**
 * @brief   polls until ra0 interface is up as indicated by ifconfig
 * @return  -1 if failed to come up, 0 if success
 */
int wifi_wait_until_ra0_up(void){
    FILE* file = NULL;
    char  filename[MAX_PATHNAME];
    char  line[64];
    int retry;
    char *ra0_intf = NULL;

    for(retry = 0; retry < 5; ++retry){
        DBUG(1, "is ra0 up? \n", __FUNCTION__, __LINE__);
        temp_filename(filename, sizeof(filename));
        unlink(filename);
        do_system("ifconfig > %s", filename);
        file = fopen(filename, "re");
        if (file == NULL) {
            WARN(errno, "unable to open file '%s'", filename);
            goto error;
        }
        //search for STA driver
        while (fgets(line, sizeof(line), file) != NULL) {
            ra0_intf = strstr(line, "ra0");
            if(ra0_intf){
                break;
            }
        }
        unlink(filename);
        if(ra0_intf){
            break;
        }else{
            sleep(1);
            DBUG(1, "ra0 is NOT up, trying again...\n", __FUNCTION__, __LINE__);
        }
    }
    return ra0_intf ? 0 : -1;

error:
    unlink(filename);
    return -1;
}

/**
 * @brief   polls until ra0 interface is down as indicated by ifconfig
 * @return  -1 if failed to come up, 0 if success
 */
int wifi_wait_until_ra0_down(void){
    FILE* file = NULL;
    char  filename[MAX_PATHNAME];
    char  line[64];
    int retry;
    char *ra0_intf = NULL;

    for(retry = 0; retry < 5; ++retry){
        DBUG(1, "is ra0 down? \n", __FUNCTION__, __LINE__);
        temp_filename(filename, sizeof(filename));
        unlink(filename);
        do_system("ifconfig > %s", filename);
        file = fopen(filename, "re");
        if (file == NULL) {
            WARN(errno, "unable to open file '%s'", filename);
            goto error;
        }
        //search for STA driver
        while (fgets(line, sizeof(line), file) != NULL) {
            ra0_intf = strstr(line, "ra0");
            if(ra0_intf){
                break;
            }
        }
        unlink(filename);
        if(!ra0_intf){
            break;
        }else{
            sleep(1);
            DBUG(1, "ra0 is NOT down, trying again...\n", __FUNCTION__, __LINE__);
        }
    }
    return ra0_intf ? -1 : 0;

error:
    unlink(filename);
    return -1;
}
#endif

static void wifi_start_AP(){
    DBUG(0, "%s()...\n", __FUNCTION__);

    SET_STATE(WIFI_STATE_WAITING);     //wait until driver properly loads
    wpa_close_monitoring();
	do_system("killall v5.arm udhcpc wpa_supplicant hostapd udhcpd dnsmasq");
	do_system("ifconfig wlan0 192.168.1.1");
	do_system("hostapd -B /skybell/config/net/hostapd.conf");
	do_system("/skybell/bin/dnsmasq -C /skybell/config/net/dnsmasq.conf");
}

static void wifi_stop_AP(){
    DBUG(0, "%s()...\n", __FUNCTION__);
    closingProcesses = TRUE;
    SET_STATE(WIFI_STATE_WAITING);
	do_system("killall hostapd dnsmasq");
}

static void wifi_shutdown(void){
    DBUG(1, "%s(): mode: %s(%d)\n", __FUNCTION__, wifi_mode_string(mode), mode);
    switch (mode) {
    case WIFI_MODE_STA:
        shuttingDown = TRUE;
        wifi_stop_STA();
        break;
    case WIFI_MODE_AP:
    case WIFI_MODE_APSCAN:
        shuttingDown = TRUE;
        wifi_stop_AP();
        break;
    default:
        break;
    }
}

static void set_mode(int newmode){
    int current_mode = mode;
    DBUG(0, "WIFI/%s(%s): currently '%s'\n", __FUNCTION__, wifi_mode_string(newmode), wifi_mode_string(mode));
    if (mode == newmode) {
        return;
    }

    modeTransition = TRUE;
    mode = newmode;

    // changing modes, first terminate current mode
    switch (current_mode) {
    case WIFI_MODE_STA:
        wifi_stop_STA();
        return;
    case WIFI_MODE_AP:
    case WIFI_MODE_APSCAN:
        wifi_stop_AP();
    default:
        break;
    }

    // if there was no active mode -- start up the new mode
    // (otherwise have to wait for previous mode to shut down)
    modeTransition = FALSE;

    switch (mode) {
    case WIFI_MODE_STA:
    case WIFI_MODE_APSCAN:
        wifi_start_STA();
        break;
    case WIFI_MODE_AP:
        wifi_start_AP();
        break;
    default:
        break;
    }
}

// return TRUE if the cmd_pipe[] is open and ready to 
// receive commands, FALSE otherwise
bool wifi_cmd_pipe_ready(void){
    return cmd_pipe[0] >= 0;
}

//put command request in pipe
static void wifi_cmd(wifi_cmd_t cmdReq){
    wifi_cmd_t c = cmdReq;
    DBUG(0, "%s(%s)...\n", __FUNCTION__, wifi_cmd_string(cmdReq));
    if(write(cmd_pipe[1], &c, sizeof(wifi_cmd_t)) != sizeof(wifi_cmd_t)){
        WARN(errno, "write(cmd_pipe[1]) failed, cmd = %d\n", c);
    }
}

void wifi_enable(bool enable){
    DBUG(1, "%s(enable=%d)...\n", __FUNCTION__, enable);
    
    if(!enable){
        //attempt shutdown
        wifi_cmd(WIFI_CMD_SHUTDOWN);
    }
    #if 0
    // 2015/5/21 -- turn wifi power on at beginning of wifi task and never turn if off!
    else{
        //attempt to turn on wifi power
        wifi_cmd(WIFI_CMD_ENABLE_PWR);
    }
    #endif
}

wifi_mode_t wifi_get_mode(void){
    return mode;
}

int wifi_set_mode(wifi_mode_t newmode, bool wait_for_idle){
    DBUG(0, "%s(newmode=%s(%d))...\n", __FUNCTION__, wifi_mode_string(newmode), newmode);
    if (wait_for_idle && (wifi_state() != WIFI_STATE_IDLE && wifi_state() != WIFI_STATE_READY)) {
        int seconds, timeout = 20;//Charles	newmode == WIFI_MODE_APSCAN? 20 : 60;
        DBUG(1, "%s(): waiting up to %d seconds for '%s' --> WIFI_STATE_IDLE\n", 
             __FUNCTION__, timeout, wifi_state_string(wifi_state()));
        for (seconds = 0; seconds < timeout && wifi_state() != WIFI_STATE_IDLE && wifi_state() != WIFI_STATE_READY; ++seconds) 
            sleep(1);
        if (wifi_state() == WIFI_STATE_IDLE || wifi_state() == WIFI_STATE_READY) {
            DBUG(1, "%s(): WIFI_STATE_IDLE/READY! (waited %d seconds)\n", __FUNCTION__, seconds); 
        } else {
            WARN(0, "%s(): TIMEOUT WAITING FOR '%s' --> WIFI_STATE_IDLE (transition_timeout!)\n", 
                  __FUNCTION__, wifi_state_string(wifi_state()));
        }
    }

    switch (newmode) {
    case WIFI_MODE_AP:
        wifi_cmd(WIFI_CMD_SET_MODE_AP);
        break;
    case WIFI_MODE_STA:
        wifi_cmd(WIFI_CMD_SET_MODE_STA);
        break;
    case WIFI_MODE_APSCAN:
        wifi_cmd(WIFI_CMD_SET_MODE_APSCAN);
        break;
    default:
        wifi_cmd(WIFI_CMD_SET_MODE_NONE);
        break;
    }

	DBUG(0,"wifi_set_mode end mode[%d]\n",newmode);//Charles
    return S_OK;
}

void wifi_scan_for_aps(void){
    wifi_cmd(WIFI_CMD_SCAN_FOR_APS);
}

// Return TRUE if the WIFI_AP_PROFILE file exists,
//        FALSE if the file does not exist.
//
// The wpa supplicant config file is used to determine
// if an AP profile has been configured.  If the file
// exists, an AP profile is configured (even if the file
// is zero length which denotes an open AP).  Note:
// WEP is configured in the wpa supplicant config file
// and handled by the wpa supplicant.
//
bool wifi_ap_profile_exists(void){
    return (file_exists(WIFI_AP_PROFILE, NULL) || encrypted_version_exists(WIFI_AP_PROFILE, FALSE));
}

void wifi_delete_ap_profile(void){
    unlink(WIFI_AP_PROFILE);                           //delete un-encrypted version if it exists
    unlink(WIFI_AP_PROFILE SKYBELL_ENCRYPTION_EXT);    //delete encrypted version if it exists
//Charles    cypress_send_config_status(0);
}

int wifi_get_alternate_ssid(char *filepath, char *ssid_out, size_t ssid_out_len, char *psk_out, size_t psk_out_len){
    FILE* fp = fopen(filepath, "re");
    char  line[128];

    if(!fp){
        return -1;
    }
    while(fgets(line, sizeof(line), fp) != NULL){
        char *ssid = strstr(line, "ssid=");
        if(ssid){
            snprintf(ssid_out, ssid_out_len, "%s", ssid+5);
            ssid_out[strlen(ssid_out)-1] = '\0';
        }
        char *psk = strstr(line, "psk=");
        if(psk){
            snprintf(psk_out, psk_out_len, "%s", psk+4);
            psk_out[strlen(psk_out)-1] = '\0';
        }
    }
    fclose(fp);
    return 0;
}

int wifi_create_factory_ap_profile(int which){
    FILE* file = fopen(WIFI_TMP_AP_PROFILE, "we");
    if (file == NULL) {
        WARN(0, "Unable to create file '%s'\n", WIFI_TMP_AP_PROFILE);
        return -1;
    }

    fputs("ctrl_interface=/var/run/wpa_supplicant\n", file);
    fputs("update_config=1\n", file);
    fputs("network={\n", file);
    if(file_exists("/skybell/misc/factory-ssid", NULL)){
#ifdef SKYBELL_V5	//Charles +
		char ssid_alt[64] = "SKA1";    /* default */
#elif defined(SKYBELL_SLIM2)	//Charles +
		char ssid_alt[64] = "SKB1";    /* default */
#else
        char ssid_alt[64] = "SKY1";    /* default */
#endif

        char psk_alt[64] = "01233210";    /* default */
        wifi_get_alternate_ssid("/skybell/misc/factory-ssid", ssid_alt, sizeof(ssid_alt), psk_alt, sizeof(psk_alt));
        fprintf(file, "  ssid=\"%s\"\n", ssid_alt);
        fputs(        "  proto=WPA RSN\n", file);
        fputs(        "  pairwise=CCMP TKIP\n", file);
        fprintf(file, "  psk=\"%s\"\n", psk_alt);
    }else{

#ifdef SKYBELL_V5	//Charles +
		fprintf(file, "  ssid=\"SKA%d\"\n", which+1);
#elif defined(SKYBELL_SLIM2)	//Charles +
        fprintf(file, "  ssid=\"SKB%d\"\n", which+1);
#else
		fprintf(file, "  ssid=\"SKY%d\"\n", which+1);
#endif
        fputs(        "  proto=WPA RSN\n", file);
        fputs(        "  pairwise=CCMP TKIP\n", file);
        fputs(        "  psk=\"01233210\"\n", file);
    }
    fputs("}\n", file);
    fclose(file);

    wifi_delete_ap_profile();
    return copy_file(WIFI_TMP_AP_PROFILE, WIFI_AP_PROFILE);
}

static int build_etc_network_interfaces(AP_SELECTION* ap)
{
    FILE* file;
    int i, rc[8] = {-1,-1,-1,1,1,1,1,1};

    file = fopen(WIFI_TMP_NETWORK_INTERFACES, "we");
    if (file == NULL) {
        ALERT(0, "Unable to create file '%s'\n", WIFI_TMP_NETWORK_INTERFACES);
        return -1;
    }

    rc[0] = fprintf(file, "auto lo\n");
    rc[1] = fprintf(file, "iface lo inet loopback\n");
    if (strcmp(ap->ipconfig_type, "manual") != 0) 
        rc[2] = fprintf(file, "iface eth0 inet dhcp\n");
    else {
		unsigned long static_ip, netmask, network, broadcast;

        if ((static_ip = inet_addr(ap->static_ip)) == INADDR_NONE) 
            static_ip = inet_addr("192.168.0.100"); 

		if ((netmask = inet_addr(ap->netmask)) == INADDR_NONE)              
            netmask   = inet_addr("255.255.255.0"); 

        network = ntohl(static_ip) & ntohl(netmask);
        broadcast = network | 255;

        rc[2] = fprintf(file, "iface eth0 inet static\n");
        rc[3] = fprintf(file, "    address %s\n", ap->static_ip);
        rc[4] = fprintf(file, "    netmask %s\n", ap->netmask);
        rc[5] = fprintf(file, "    network %lu.%lu.%lu.%lu\n",(network>>24)&0xFF,(network>>16)&0xFF,(network>>8)&0xFF,(network)&0xFF);
        rc[6] = fprintf(file, "    broadcast %lu.%lu.%lu.%lu\n",(broadcast>>24)&0xFF,(broadcast>>16)&0xFF,(broadcast>>8)&0xFF,(broadcast)&0xFF);
        rc[7] = fprintf(file, "    gateway %s\n", ap->gateway);
    }
    fclose(file);

    for (i = 0; i < ARRAY_SIZE(rc); ++i) {
        if (rc[i] <= 0) {
            ALERT(0, "Unable to write file '%s' (rc[%d,%d,%d,%d,%d,%d,%d,%d])\n", 
                  WIFI_TMP_NETWORK_INTERFACES, 
                  rc[0], rc[1], rc[2], rc[3], rc[4], rc[5], rc[6], rc[7]);
            unlink(WIFI_TMP_NETWORK_INTERFACES);
            return -1;
        }
    }

    copy_file(WIFI_TMP_NETWORK_INTERFACES, WIFI_ETC_NETWORK_INTERFACES); 
    return 0;
}


// create a new wpa_supplicant.conf file given the network body
int wifi_provision(AP_SELECTION* ap, char* network_body)
{
    FILE* file;
    int rc;

    rc = build_etc_network_interfaces(ap);
    if (rc < 0) {
        WARN(0, "build_etc_network_interfaces() failed with error %d\n", rc);
        return -1;
    }

    file = fopen(WIFI_TMP_AP_PROFILE, "we");
    if (file == NULL) {
        WARN(0, "Unable to create file '%s'\n", WIFI_TMP_AP_PROFILE);
        return -1;
    }

    fputs("ctrl_interface=/var/run/wpa_supplicant\n", file);
    fputs("update_config=1\n", file);
    fputs("network={\n", file);
    fputs(network_body, file);
    fputs("}\n", file);
    fclose(file);

    wifi_delete_ap_profile();
    return copy_file(WIFI_TMP_AP_PROFILE, WIFI_AP_PROFILE);
}

void wifi_clear_ap_list(void){
    struct list_head *this;
    struct list_head *next;
    ap_list_t *item;

    MUTEX_LOCK(&list_mutex);
    //check if list is empty first
    if(list_empty(&wifi_ap_list)){
        MUTEX_UNLOCK(&list_mutex);
        return;
    }
    //non-empty list
    list_for_each_safe(this, next, &wifi_ap_list){
        item = list_entry(this, ap_list_t, list);
        list_del(&item->list);
        heap_free(item);
    }
    MUTEX_UNLOCK(&list_mutex);
}

static void scan_for_aps(void){
	DBUG(0,"scan_for_aps() start mode=[%d]\n",mode);//Charles
	if (0) {//Charles   if(mode != WIFI_MODE_STA && mode != WIFI_MODE_APSCAN) {
        WARN(0, "can't scan for AP's in %s(%d) mode!\n", wifi_mode_string(mode), mode);
        return;
    }

    MUTEX_LOCK(&scan_mutex);
    wifi_clear_ap_list();
    wifi_scan(&wifi_ap_list);
    MUTEX_UNLOCK(&scan_mutex);
    DBUG(0, "%s(): SCANNING IS COMPLETE!\n", __FUNCTION__);

    if(list_empty(&wifi_ap_list)){
        DBUG(0, "%s(): No AP's found!\n", __FUNCTION__);
    } 
    else {
        if (system_debug_level >= 2) {
            DBUG(1, "%s(): ap_list_print()...\n", __FUNCTION__);
            extern void ap_list_print(char* list_name, struct list_head*);
            ap_list_print("WIFI", &wifi_ap_list); 
        }
    }

    route_to_clients(WIFI_STATE_AP_LIST_READY);
    DBUG(0, "%s(): ROUTING IS COMPLETE!route_to_clients mode=[%d]\n", __FUNCTION__,mode);
}


// get a pointer to the list of APs
// locks the list_mutex and returns success,
// **NOTE** must unlock the mutext with a 
// call to wifi_release_ap_list()!!!
//
// returns 0 on success
// 
//  Correct usage:
//    1: call wifi_scan_for_aps()
//    2: wait for the WIFI_STATE_AP_LIST_READY event
//    3. once that event has been received, call
//       wifi_get_ap_list() to process the list
//    4. and wifi_release_ap_list() once processing
//       is complete
//
int wifi_get_ap_list(struct list_head** ap_list_ptr)
{
    DBUG(5, "%s(): LOCKING MUTEX\n", __FUNCTION__);
    MUTEX_LOCK(&list_mutex);
    DBUG(5, "%s(): MUTEX LOCKED\n", __FUNCTION__);
    *ap_list_ptr = &wifi_ap_list;
    return 0;
}


// release the AP list pointer, unlocks the list_mutex
// and returns success.  **NOTE** this routine may only
// be called **AFTER** a successful call to wifi_get_ap_list()
//
// returns 0 on success
//
int wifi_release_ap_list(void)
{
    DBUG(5, "%s(): UNLOCKING MUTEX\n", __FUNCTION__);
    MUTEX_UNLOCK(&list_mutex);
    DBUG(5, "%s(): MUTEX UNLOCKED\n", __FUNCTION__);
    return 0;
}

static int nlk_read_event(){
    unsigned char payload[256];
    bool relevant = TRUE;      //flag for relevant messages
    static int drv = -1;     //0 = STA, 1 = AP, -1 = nothing yet
    struct sockaddr_nl dest_addr;
    struct iovec iov = {
        .iov_base = (void*)payload, 
        .iov_len = sizeof(payload)
    };
    struct msghdr msg = {
        .msg_name = (void*)&dest_addr, 
        .msg_namelen = sizeof(dest_addr), 
        .msg_iov = &iov,
        .msg_iovlen = 1
    };
    int nbytes;

    memset(payload, 0, sizeof(payload));  
    memset(&dest_addr, 0, sizeof(dest_addr)); 
    nbytes = recvmsg(nlk, &msg, 0);
    
    if(nbytes > 0) {
        DBUG(3, "WIFI/%s(): nlk[%s]\n", __FUNCTION__, (char*)payload);
        //add events
        if(strncmp((char*)payload, ADD_MODULE_STA, strlen(ADD_MODULE_STA)) == 0){
            chk.module = TRUE;
        }
        else if(strncmp((char*)payload, ADD_MODULE_AP, strlen(ADD_MODULE_AP)) == 0){
            chk.module = TRUE;
        }
        else if(strncmp((char*)payload, ADD_USB_DRV_STA, strlen(ADD_USB_DRV_STA)) == 0 || 
                strncmp((char*)payload, ADD_USB_DRV_AP, strlen(ADD_USB_DRV_AP)) == 0)
        {
            chk.usb_drv = TRUE;
        }
        else if(strncmp((char*)payload, ADD_CLASS_NET, strlen(ADD_CLASS_NET)) == 0){
            chk.class_net = TRUE;
        }
        //else if(strncmp((char*)payload, ADD_USB_DEV, strlen(ADD_USB_DEV)) == 0){
        //    chk.usb_dev = TRUE;
        //}
        //remove events
        else if(strncmp((char*)payload, REM_MODULE_STA, strlen(REM_MODULE_STA)) == 0){
            chk.module = FALSE;
            drv = 0;        //record that STA module was removed
        }
        else if(strncmp((char*)payload, REM_MODULE_AP, strlen(REM_MODULE_AP)) == 0){
            chk.module = FALSE;
            drv = 1;        //record that AP module was removed
        }
        else if(strncmp((char*)payload, REM_USB_DRV_STA, strlen(REM_USB_DRV_STA)) == 0 ||
                strncmp((char*)payload, REM_USB_DRV_AP, strlen(REM_USB_DRV_AP)) == 0)
        {
            chk.usb_drv = FALSE;
        }
        else if(strncmp((char*)payload, REM_CLASS_NET, strlen(REM_CLASS_NET)) == 0){
            chk.class_net = FALSE;
        }
        //else if(strncmp((char*)payload, REM_USB_DEV, strlen(REM_USB_DEV)) == 0){
        //    chk.usb_dev = FALSE;
        //}
        else
            relevant = FALSE;       //irrelevant message

        DBUG(0, "WIFI/%s(): module: %d | usb_drv: %d | class_net: %d,relevant = %d\n", __FUNCTION__, chk.module, chk.usb_drv, chk.class_net,relevant);
        //only act upon relevant messages
        if(relevant){
            //check if device is completely up, if so driver is ready
            #ifdef SKYBELL_HD
            if(chk.module && chk.usb_drv && chk.class_net){ // && chk.usb_dev){
            #else
            //AP driver doesn't spit out the same class net netlink message that the STA driver does
            if(chk.module && chk.usb_drv){ // && chk.usb_dev){
            #endif
                DBUG(3, "WIFI/%s(): driver up!\n", __FUNCTION__);
                SET_STATE(WIFI_STATE_DRIVER_READY);
            }
            //or device is completely down, so driver is fully removed
            else if(!chk.module && !chk.usb_drv && !chk.class_net){// && !chk.usb_dev){
                DBUG(3, "WIFI/%s(): driver down!\n", __FUNCTION__);
                if(drv == 1){
                    SET_STATE(WIFI_STATE_AP_DRV_REMOVED);
                }
                else if(!drv) {
                    SET_STATE(WIFI_STATE_STA_DRV_REMOVED);
                }
                memset(&chk, 0, sizeof(chk));
                drv = -1;       //reset
            }   
        }
    }
    else{
        WARN(errno, "recvmsg");
    }
    return 0;
}

static int nlr_read_event(int sockint, int (*msg_handler)(struct sockaddr_nl *,
                                               struct nlmsghdr *)){
    int status;
    int ret = 0;
    //2015-04-02, hitemeng - was 4096, reducing stack size
    //char buf[4096];
    char buf[1024];
    struct iovec iov = { buf, sizeof buf };
    struct sockaddr_nl snl;
    struct msghdr msg = { (void*)&snl, sizeof snl, &iov, 1, NULL, 0, 0};
    struct nlmsghdr *h;

    status = recvmsg(sockint, &msg, 0);

    if(status < 0){
        //Socket non-blocking so bail out once we have read everything
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return ret;

        //Anything else is an error
        ALERT(0, "read_netlink: Error recvmsg: %d\n", status);
        return status;
    }

    if(status == 0){
        DBUG(5, "read_netlink: EOF\n");
    }

    /* We need to handle more than one message per 'recvmsg' */
    for(h = (struct nlmsghdr *) buf; NLMSG_OK (h, (unsigned int)status); h = NLMSG_NEXT (h, status)){
        //Finish reading
        if (h->nlmsg_type == NLMSG_DONE)
            return ret;

        //Message is some kind of error
        if (h->nlmsg_type == NLMSG_ERROR){
            ALERT(0, "read_netlink: Message is an error - decode TBD\n");
            return -1; // Error
        }

        //Call message handler
        if(msg_handler){
            ret = (*msg_handler)(&snl, h);
            if(ret < 0){
                ALERT(0, "read_netlink: Message hander error %d\n", ret);
                return ret;
            }
        }
        else{
            ALERT(0, "read_netlink: Error NULL message handler\n");
            return -1;
        }
    }
    return ret;
}

static int msg_handler(struct sockaddr_nl *nl, struct nlmsghdr *msg){
    struct ifinfomsg *ifi;
    
    switch (msg->nlmsg_type)
    {
    case RTM_NEWADDR:
        DBUG(0, "WIFI/%s(): RTM_NEWADDR\n", __FUNCTION__);
        {
            struct ifaddrmsg *ifa = (struct ifaddrmsg *) NLMSG_DATA(msg);
            struct rtattr *rth = IFA_RTA(ifa);
            int rtl = IFA_PAYLOAD(msg);
#ifndef SKYBELL_HD
            BOOL ipObtained = FALSE;
#endif
            while(rtl && RTA_OK(rth, rtl)){
                if(rth->rta_type == IFA_LOCAL){
                    uint32_t ipaddr = htonl(*((uint32_t *)RTA_DATA(rth)));
                    char name[IFNAMSIZ];
                    if_indextoname(ifa->ifa_index, name);
                    DBUG(1, "WIFI/%s(): %s is now %d.%d.%d.%d\n",
                             __FUNCTION__, name,
                             (ipaddr >> 24) & 0xff,
                             (ipaddr >> 16) & 0xff,
                             (ipaddr >> 8) & 0xff,
                             ipaddr & 0xff);
#ifdef SKYBELL_HD
                    //obtained IP
                    SET_STATE(WIFI_STATE_IP_OBTAINED);
#else
                    //sit0 interface seems to be confusing things
                    if(strncmp(name, "ra0", 3) == 0 || strncmp(name, "wlan0", 5) == 0){
					    DBUG(0, "setting WIFI_STATE_IP_OBTAINED\n");
                        //obtained IP
                        SET_STATE(WIFI_STATE_IP_OBTAINED);
                        ipObtained = TRUE;
                    }
#endif
                }
                rth = RTA_NEXT(rth, rtl);
            }
#ifndef SKYBELL_HD
            if(!ipObtained){
                //djp - for 3.10 kernel, don't seem to get RTM_NEWLINK, only RTM_NEWADDR
                //when bringing up the ra0 interface through ifconfig or ip commands
                if (state == WIFI_STATE_WAITING && (mode == WIFI_MODE_APSCAN || mode == WIFI_MODE_AP)) {
                    DBUG(0,"SET_STATE(WIFI_STATE_READY)--route_to_clients(WIFI_STATE_INTF_UP)\n");
                    SET_STATE(WIFI_STATE_READY);
                    route_to_clients(WIFI_STATE_INTF_UP);
                }
            }
#endif
        }
        break;
    case RTM_DELADDR:
        DBUG(0, "WIFI/%s(): RTM_DELADDR\n", __FUNCTION__);
#ifndef SKYBELL_HD 
            //in 3.10 kernel we don't get a link down message when bringing down the interface, only a RTM_NEWADDR
            if(state != WIFI_STATE_IDLE){
                if(closingProcesses){
                    closingProcesses = FALSE;
                    SET_STATE(WIFI_STATE_INTF_DOWN);
                }
            }
#endif
        break;
    case RTM_NEWROUTE:
        DBUG(2, "WIFI/%s(): RTM_NEWROUTE\n", __FUNCTION__);
        break;
    case RTM_DELROUTE:
        DBUG(2, "WIFI/%s(): RTM_DELROUTE\n", __FUNCTION__);
        break;
    case RTM_NEWLINK:
        DBUG(0, "WIFI/%s(): RTM_NEWLINK\n", __FUNCTION__);
        ifi = NLMSG_DATA(msg);
        if(ifi->ifi_flags & IFF_RUNNING){
            DBUG(0, "WIFI/%s(): state '%s', mode '%s'\n", __FUNCTION__, wifi_state_string(state), wifi_mode_string(mode));
            if (state == WIFI_STATE_WAITING && mode == WIFI_MODE_APSCAN) {
                DBUG(0, "WIFI/%s(): RTM_NEWLINK | netlink_link_state: Up\n", __FUNCTION__);
                SET_STATE(WIFI_STATE_READY);
                route_to_clients(WIFI_STATE_INTF_UP);
            }
        }
        else{
            //link also has to be down, for it to be really down
            if(!(ifi->ifi_flags & IFF_UP)){
                //make sure we disregard these events in the idle state
                //where the link will be down and these messages will arrive continuously
                if(state != WIFI_STATE_IDLE){
                    //have to check if we intentionally have brought a link down
                    //because this can event can occur at undesired times and
                    //change the state
                    if(closingProcesses){
                        closingProcesses = FALSE;
                        DBUG(0, "WIFI/%s(): RTM_NEWLINK | netlink_link_state: Down\n", __FUNCTION__);
                        SET_STATE(WIFI_STATE_INTF_DOWN);
                    }
                    else {
                        DBUG(2, "WIFI/%s(): link down repeat\n", __FUNCTION__);
                    }
                }
                else {
                    DBUG(2, "WIFI/%s(): state == WIFI_STATE_IDLE!\n", __FUNCTION__);
                }
            }
        }
        
        break;
    case RTM_DELLINK:
        DBUG(2, "WIFI/%s(): RTM_DELLINK\n", __FUNCTION__);
        break;
    default:
        WARN(0, "WIFI/%s(): Unknown netlink nlmsg_type %d\n",
             __FUNCTION__,  msg->nlmsg_type);
        break;
    }
    return 0;
}

#if 0
static bool wifi_network_config_is_wep(char *filename){
    bool network_is_wep = FALSE;
    char  line[512];
    FILE *file = fopen(filename, "re");
    if (file == NULL) {
        WARN(errno, "unable to open file '%s'", filename);
        goto exit;
    }
    while (fgets(line, sizeof(line), file) != NULL) {
        if(strstr(line, "wep")){
            network_is_wep = TRUE;
            break;
        }
    }

    fclose(file);

exit:
    return network_is_wep;
}
#endif

static void driver_ready(){
    DBUG(0, "WIFI/%s(): mode=%s(%d)", __FUNCTION__, wifi_mode_string(mode), mode);
    SET_STATE(WIFI_STATE_WAITING);
    switch (mode) {
    case WIFI_MODE_AP:
        //start dhcp server
        do_system("/etc/init.d/dhcp-server start");
        break;
    case WIFI_MODE_APSCAN:
        // bring the interface up (scan only, no wpa supplicant)
        do_system(WIFI_INTF_UP);
		#if 0//Charles	WIFI_DRV_NEW_MT7601U    //not getting a newlink message saying intf is up
        SET_STATE(WIFI_STATE_READY);
		route_to_clients(WIFI_STATE_INTF_UP);
		#endif
        break;
    case WIFI_MODE_STA:
        // if factory mode, setup factory AP profile based on least signifcant 2 bits
        // of MAC address (semi random to select 1 of the 4 APs without building 4
        // custom images)
        if (factory()) {
            int which = which_factory_AP();
            int rc = wifi_create_factory_ap_profile(which);
            if (rc)
                ERROR(0, "Unable to create FACTORY AP profile!\n");
            //factory_ap_selection(which);   // setup static ip based on AP#
            factory_set_green_led(which);  // setup green LED to match the selected factory AP profile
            factory_set_server_uri(which); // setup server uri based on AP#
            cfg_write_if_dirty(&cfg);
        } 
        else {
            if (!wifi_ap_profile_exists()) {
                SET_STATE(WIFI_STATE_PAIRING);
                break;
            }
        }

//CC        decrypt_file(WIFI_AP_PROFILE);
        #if 0//Charles	WIFI_DRV_NEW_MT7601U
        do_system("mount -t debugfs debugfs /sys/kernel/debug/");
        do_system("echo 0x13B0 > /sys/kernel/debug/ieee80211/phy0/mt7601u/regidx");
        #endif

        //start wpa supplicant
//Finn,will remove it..    do_system(WIFI_START_WPA);  // TODO: if this call fails, need to go into AP mode!
		do_system("wpa_supplicant -B -D nl80211 -i wlan0 -c  %s",WIFI_AP_PROFILE);
        //open connection to wpa_supplicant
        mon_conn = wpa_ctrl_open(WPA_MON_PATH);
        if(mon_conn == NULL){
            WARN(0, "wpa failed to open\n");
            return;
        }
        else{
            DBUG(1, "%s(): wpa sucessfully opened\n", __FUNCTION__);
        }
        if(wpa_ctrl_attach(mon_conn) == 0){
            DBUG(1, "%s(): wpa successfully attached\n", __FUNCTION__);
        }
        else{
            WARN(0, "wpa failed to attach\n");
        }
        //wpa monitor setup
        wpas = wpa_ctrl_get_fd(mon_conn);
        DBUG(1, "WIFI/%s(): wpas: %d\n", __FUNCTION__, wpas);

        break;
    case WIFI_MODE_NONE:
        SET_STATE(WIFI_STATE_IDLE);
        break;
    }
}

static void driver_removed(){
    DBUG(0, "WIFI/%s(): shuttingDown=%d", __FUNCTION__, shuttingDown);
#if 0//Charles	WIFI_TX_PWR_ADJUST
        //this is a wifi shutdown so reset tx pwr here so that state machine can
        //start all over
        wifi_tx_pwr_request(REQUEST_RESET);
#endif

    //we're shutting down wifi completely so don't care what previous mode is
    if(shuttingDown){   
        //turn off gpio enable to wifi module
        mode = WIFI_MODE_NONE;
        shuttingDown = modeTransition = FALSE;
        SET_STATE(WIFI_STATE_IDLE);

    } 
    else {
        switch (mode) {
        case WIFI_MODE_AP:
            wifi_start_AP();
            break;
        case WIFI_MODE_STA:
        case WIFI_MODE_APSCAN:
            wifi_start_STA();
            break;
        case WIFI_MODE_NONE:
        default:
            DBUG(1, "WIFI/%s(): wifi mode set to NONE\n", __FUNCTION__);
            //reset flags
            shuttingDown = FALSE;
            modeTransition = FALSE;
            closingProcesses = FALSE;
            SET_STATE(WIFI_STATE_IDLE);
            break;
        }
    }
}

static void interface_down(){

    DBUG(1, "WIFI/%s()\n", __FUNCTION__);
    set_net_down();
    WARN(0, "no wifi module installed\n");
    SET_STATE(WIFI_STATE_IDLE);
    return;
}


static void configure_resolv_conf(AP_SELECTION* ap){
    BOOL googled = FALSE;
    FILE* file = fopen(WIFI_TMP_RESOLV_CONF, "we");
    if (file == NULL) {
        WARN(0, "Unable to create file '%s'\n", WIFI_TMP_RESOLV_CONF);
        return;
    }

    if (ap->dns[0][0]) {
        fprintf(file, "nameserver %s\n", ap->dns[0]);
        if (strcmp(ap->dns[0], WIFI_GOOGLE_DNS) == 0) googled = TRUE;
    }
    if (ap->dns[1][0]) {
        fprintf(file, "nameserver %s\n", ap->dns[1]);
        if (strcmp(ap->dns[1], WIFI_GOOGLE_DNS) == 0) googled = TRUE;
    }
    if (!googled) {
        fprintf(file, "nameserver %s\n", WIFI_GOOGLE_DNS);
    }
    fclose(file); 

    copy_file(WIFI_TMP_RESOLV_CONF, WIFI_RESOLV_CONF);
}


static void configure_static_ip(AP_SELECTION* ap){
    SET_STATE(WIFI_STATE_IP_OBTAINED);
    do_system("ifconfig " WIFI_INTF_NAME " %s netmask %s", ap->static_ip, ap->netmask); // static IP address and NETMASK
    do_system("route add default gw %s", ap->gateway); // GATEWAY address
    configure_resolv_conf(ap); // DNS1 & DNS2 addresses
}

int wifi_switch_config_to_dhcp(void){
    char payload[1024];
    FILE *file_orig, *file_new;

    /* encrypted version of provision file exists */
    if(file_exists(SKYBELL_PROVISION SKYBELL_ENCRYPTION_EXT, NULL)){
        decrypt_file(SKYBELL_PROVISION);
    }else if(!file_exists(SKYBELL_PROVISION, NULL)){
        /* shouldn't happen, but for some reason, neither encrypted or unencrypted
         * version exists */
        return -1;
    }

    /* open provision file as read-only just to get contents */
    file_orig = fopen(SKYBELL_PROVISION, "re");
    if(!file_orig){
        /* re-encrypt file and exit */
        encrypt_file(SKYBELL_PROVISION, TRUE);
        return -2;
    }

    size_t nbytes = fread(payload, 1, sizeof(payload), file_orig);
    fclose(file_orig);

    if (nbytes < sizeof(payload))
        payload[nbytes] = '\0';  // null terminate the data (assuming content is ASCII)

    /* read json file */
    cJSON *root = cJSON_Parse(payload);
    if(!root){
        /* re-encrypt file and exit */
        encrypt_file(SKYBELL_PROVISION, TRUE);
        return -3;
    }

    /* modify parameters in json to use dhcp */
    cJSON *provision = cJSON_GetObjectItem(root, "provision");
    if(!provision){
        cJSON_Delete(root);
        encrypt_file(SKYBELL_PROVISION, TRUE);
        return -4;
    }
    cJSON *ipconfig = cJSON_GetObjectItem(provision, "ipConfig");
    if(!ipconfig){
        cJSON_Delete(root);
        encrypt_file(SKYBELL_PROVISION, TRUE);
        return -5;
    }
    cJSON_ReplaceItemInObject(ipconfig, "dns1", cJSON_CreateString(""));
    cJSON_ReplaceItemInObject(ipconfig, "dns0", cJSON_CreateString(""));
    cJSON_ReplaceItemInObject(ipconfig, "mask", cJSON_CreateString(""));
    cJSON_ReplaceItemInObject(ipconfig, "type", cJSON_CreateString("dhcp"));
    cJSON_ReplaceItemInObject(ipconfig, "ip", cJSON_CreateString(""));
    cJSON_ReplaceItemInObject(ipconfig, "gateway", cJSON_CreateString(""));

    char *new_provision_json_str = cJSON_PrintUnformatted(root);
    if(!new_provision_json_str){
        cJSON_Delete(root);
        encrypt_file(SKYBELL_PROVISION, TRUE);
        return -6;
    }
    cJSON_Delete(root);

    /* open a temporary copy and write new provision information
     * into it
     */
    file_new = fopen(TMP_SKYBELL_PROVISION, "we");
    if(!file_new){
        free(new_provision_json_str);
        encrypt_file(SKYBELL_PROVISION, TRUE);
        return -7;
    }
    fprintf(file_new, "%s", new_provision_json_str);
    free(new_provision_json_str);
    fclose(file_new);

    /* overwrite existing provision one with new one */
    copy_file(TMP_SKYBELL_PROVISION, SKYBELL_PROVISION);
    unlink(TMP_SKYBELL_PROVISION);
    sync();

    /* re-encrypt provision file */
    encrypt_file(SKYBELL_PROVISION, TRUE);
    sync();

    return 0;
}

static void wifi_fsm(){
    static wifi_state_t previous_state = WIFI_STATE_DEAD;
    AP_SELECTION* ap;
    int bitrate, snr, rssi;

	DBUG(0,"wifi_fsm() start for\n");//Charles
    if (state != previous_state) {
        DBUG(1, "WIFI/%s() -- state: %s --> %s\n", 
             __FUNCTION__, wifi_state_string(previous_state), wifi_state_string(state));
#if 0//Charles
        if (cypress_wifi_substate(previous_state) != cypress_wifi_substate(state))
            cypress_send_config_status(0);
#endif//Charles
        previous_state = state;
    }

    switch(previous_state){
    case WIFI_STATE_IDLE:
        //default state
        break;
    case WIFI_STATE_WAITING:
        //do nothing
        //add timeout??
        break;
    case WIFI_STATE_DRIVER_READY:
        driver_ready();
        break;

    case WIFI_STATE_STA_DRV_REMOVED:
    case WIFI_STATE_AP_DRV_REMOVED:
        driver_removed();
        break;
    case WIFI_STATE_PAIRING:
        break;
    case WIFI_STATE_INTF_DOWN:
        interface_down();
        break;
    case WIFI_STATE_WPA_AUTHENTICATING:
        //notify clients
        SET_STATE(WIFI_STATE_WAITING);
        route_to_clients(previous_state);
        break;
    case WIFI_STATE_WPA_ASSOCIATING:
#if 0//Charles	WIFI_TX_PWR_ADJUST
        #if WIFI_DRV_NEW_MT7601U
        do_system("cat /sys/kernel/debug/ieee80211/phy0/mt7601u/regval");
        #endif
        wifi_tx_pwr_request(REQUEST_SET_PWR_MIN);
        #if WIFI_DRV_NEW_MT7601U
        //do_system("echo 0x2f2f0010 > /sys/kernel/debug/ieee80211/phy0/mt7601u/regval");
        do_system("cat /sys/kernel/debug/ieee80211/phy0/mt7601u/regval");
        #endif
#endif
        //notify clients
        SET_STATE(WIFI_STATE_WAITING);
        route_to_clients(previous_state);
        break;
    case WIFI_STATE_WPA_ASSOCIATED:
        //notify clients
        SET_STATE(WIFI_STATE_WAITING);
        route_to_clients(previous_state);
        break;
    case WIFI_STATE_WPA_CONNECTED:
        ap = ap_provision();
        if (/*factory()||*/ strcmp(ap->ipconfig_type, "manual") == 0) {
            // factory uses static network, static-ip relative to AP# (don't start udhcp client)
            configure_static_ip(ap);
        } else {
            //don't do anything unless IP address has not previously been issued
            //seems like wpa_supplicant sometimes issues this message even though
            //we've already been given an IP, and that ends up screwing up the network
            //interface when coap_send() is called to register
            if(!ipRcvd){
                //start dhcp client
                SET_STATE(WIFI_STATE_WAITING);
                do_system(WIFI_START_DHCPC);
            }else{
                //IP ADDRESS HAS ALREADY BEEN ISSUED HERE
                //if we hit WIFI_STATE_WPA_CONNECTED and coap stack is already up,
                //      this means we've reconnected due to temporary beacon loss
                //if we hit WIFI_STATE_WPA_CONNECTED and coap stack is down,
                //      this means this was a true disconnect, and fsm_disconnect() was
                //      called, which calls coap_start(), so we need to let sta.c
                //      know that it needs to resolve the server uri again and restart
                //      the coap stack
                //if(!coap_ready()){
                    //2017-02-16 djp - this is having issues...
                    //SET_STATE(WIFI_STATE_READY);
                    //route_to_clients(WIFI_STATE_WPA_RECONNECTED);
                //}
                SET_STATE(WIFI_STATE_READY);
            }
            /* TODO - get this working later... creates too many problems right now
            else{
                //IP ADDRESS HAS ALREADY BEEN ISSUED HERE SO THERE MUST HAVE BEEN A
                //PREVIOUS DISCONNECT OF ONE OF TWO TYPES:
                //1. if we hit WIFI_STATE_WPA_CONNECTED and coap stack is already up,
                //      this means we've reconnected due to temporary beacon loss
                //2. if we hit WIFI_STATE_WPA_CONNECTED and coap stack is down,
                //      this means this was a true disconnect, and fsm_disconnect() was
                //      called, which calls coap_start(), so we need to let sta.c
                //      know that it needs to resolve the server uri again and restart
                //      the coap stack
                if(!coap_ready()){
                    //2017-02-16 djp - this is having issues...
                    //SET_STATE(WIFI_STATE_READY);
                    //route_to_clients(WIFI_STATE_WPA_RECONNECTED);
                }
            }
            */
        }
        //adding this  here because sometimes initial boost seems to get overwritten
        //during association process... unclear why
        #if 0//Charles	WIFI_TX_PWR_ADJUST
        #if WIFI_DRV_NEW_MT7601U
        //do_system("cat /sys/kernel/debug/ieee80211/phy0/mt7601u/regval");
        #endif
        wifi_tx_pwr_request(REQUEST_SET_PWR_MIN);
        #if WIFI_DRV_NEW_MT7601U
        //do_system("echo 0x2f2f0010 > /sys/kernel/debug/ieee80211/phy0/mt7601u/regval");
        //do_system("cat /sys/kernel/debug/ieee80211/phy0/mt7601u/regval");
        #endif
        #endif
        //send wifi stats to adaptive video module
        wifi_read_stats(&bitrate, &snr, &rssi);
        video_adaptive_update_stats_wifi(bitrate, snr, rssi);
        break;
    case WIFI_STATE_WPA_DISCONNECTED:
        //notify clients
        SET_STATE(WIFI_STATE_WAITING);
        route_to_clients(previous_state);
        break;
    case WIFI_STATE_WPA_AUTH_FAILED:
        //TODO error recovery when authentication fails
        //notify clients
        route_to_clients(previous_state);
        break;
    case WIFI_STATE_IP_OBTAINED:
        //record that an ip address has been issued successfully so that wpa_supplicant will not send
        //another message saying it has been connected causing a WIFI_STATE_WPA_CONNECTED state
        //which starts up udhcpc again (and can hose up the network interface --> coap_send() fails and
        //leds go blue+green, may or may not recover)
        ipRcvd = TRUE;
        // encrypt_file(WIFI_AP_PROFILE, FALSE);
        DBUG(Y_DBG,"%s.%s.%d-----goto here -------------\n",__FILE__,__func__,__LINE__);
        set_net_start_time();
        //notify clients
        SET_STATE(WIFI_STATE_READY);
        route_to_clients(previous_state);
        //done with the mode transitioning
        modeTransition = FALSE;
        //wifi subsystem is ready to be used
        break;
    case WIFI_STATE_READY:
        //keep clients updated
        route_to_clients(state);
        //stay here until state is changed externally by a disconnection or a reset
        break;
    default:
        break;
    }
	DBUG(0,"wifi_fsm() end switch [%d]\n",previous_state);//Charles
}

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

    if (msg1_len == -1) {
        msg1_len = strlen(msg1);
        msg2_len = strlen(msg2);
        msg3_len = strlen(msg3);
        msg4_len = strlen(WPA_EVENT_CONNECTED);
        msg5_len = strlen(WPA_EVENT_DISCONNECTED);
        msg6_len = strlen(WPA_EVENT_TEMP_DISABLED);
    }

    //skip priority
    if(*pos == '<'){
        pos = strchr(pos, '>');
        pos = pos == NULL? msg : pos+1;
    }
    //messages of interest
    if(strncmp(pos, msg1, msg1_len) == 0){
        SET_STATE(WIFI_STATE_WPA_AUTHENTICATING);
    }
    else if(strncmp(pos, msg2, msg2_len) == 0){
        SET_STATE(WIFI_STATE_WPA_ASSOCIATING);
    }
    else if(strncmp(pos, msg3, msg3_len) == 0){
        SET_STATE(WIFI_STATE_WPA_ASSOCIATED);
    }
    else if(strncmp(pos, WPA_EVENT_CONNECTED, msg4_len) == 0){
        network_not_found_cnt = 0;
        SET_STATE(WIFI_STATE_WPA_CONNECTED);
    }
    else if(strncmp(pos, WPA_EVENT_DISCONNECTED, msg5_len) == 0){
        network_not_found_cnt = 0;
        //log when disconnected events occur
        DBUG(1, "WIFI/%s(): %s\n",__FUNCTION__, msg);
        //SET_STATE(WIFI_STATE_WPA_DISCONNECTED);    //2017-02-15 djp - we don't do anything in this state anymore
    }
    else if(strncmp(pos, "CTRL-EVENT-NETWORK-NOT-FOUND", strlen("CTRL-EVENT-NETWORK-NOT-FOUND")) == 0){
        //log when disconnected events occur
        DBUG(1, "WIFI/%s(): %s network_not_found_cnt: %d\n",__FUNCTION__, msg, network_not_found_cnt);
        if(++network_not_found_cnt >= network_timeout && ipRcvd){
            RESTART("network is down");
        }
    }
    else if(strncmp(pos, WPA_EVENT_TEMP_DISABLED, msg6_len) == 0){
        //log if authentication fails
        DBUG(1, "WIFI/%s(): %s\n", __FUNCTION__, msg);
        SET_STATE(WIFI_STATE_WPA_AUTH_FAILED);
    }
}

//gateway to wifi cmd requests
static void wifi_process_cmd(wifi_cmd_t cmd){
    DBUG(0, "WIFI/%s(): process cmd '%s'", __FUNCTION__, wifi_cmd_string(cmd));

    if (cmd == WIFI_CMD_DIE) {
        die(wifi_thread);
        return;
    }

    //make sure we're not doing anything else currently first
    // TODO: what about a checking for closingProcesses?
    if(modeTransition || shuttingDown){
        DBUG(0, "%s(): WIFI mode in transition(%d) or shutdown(%d) - ignoring request %s(%d)\n",
             __FUNCTION__, modeTransition, shuttingDown, wifi_cmd_string(cmd), cmd);
        return;
    }
	DBUG(0,"wifi_process_cmd() start switch cmd[%d]\n",cmd);//Charles

    switch(cmd){
    case WIFI_CMD_SET_MODE_APSCAN:
        set_mode(WIFI_MODE_APSCAN);
        break;
    case WIFI_CMD_SET_MODE_STA:
        set_mode(WIFI_MODE_STA);
        break;
    case WIFI_CMD_SET_MODE_AP:
        set_mode(WIFI_MODE_AP);
        break;
    case WIFI_CMD_SET_MODE_NONE:
        set_mode(WIFI_MODE_NONE);
        break;
    case WIFI_CMD_SHUTDOWN:
        //start shutdown process
        //can't rely on user here to shut things down properly so we have to do it
        wifi_shutdown();
        //gpio power off will be called later once driver is completely unloaded
        break;
    case WIFI_CMD_SCAN_FOR_APS:
        scan_for_aps();
        break;
    default:
        WARN(0, "invalid wifi_cmd(%d) received\n", cmd);
        break;
    }
	DBUG(0,"wifi_process_cmd() end cmd[%d]\n",cmd);//Charles
}


static int find_max4(int a, int b, int c, int d){
    int t1, t2;

    t1 = (a > b)? a : b;
    t2 = (c > d)? c : d;
    return ((t1 > t2)? t1 : t2);
}

static int wifi_task(THREAD* thread){
    //select
    int result;
    //wpa variables
    char buf[256];
    size_t len = sizeof(buf) - 1;
    int nfds = 0;
    fd_set rfds;
    wifi_cmd_t cmd;
    struct sockaddr_nl src_addr; //netlink kernel socket variable
    struct sockaddr_nl addr;     //netlink route socket variable
    bool start_wpa_supplicant = (bool)thread->private_data;
    bool wpa_running = FALSE;
    int retry;

    mode = WIFI_MODE_NONE;
    shuttingDown = FALSE;
    modeTransition = FALSE;
    closingProcesses = FALSE;
    memset(&chk, 0, sizeof(chk));
	DBUG(0,"wifi_task() start wifi_thread\n");//Charles

    //netlink kernel socket initialization
    nlk = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_KOBJECT_UEVENT);
    if(nlk < 0){
        REBOOT(errno, "nlk socket() failed");
        return -1;
    }

    for (retry = 0; retry < 5; ++retry) {
        memset(&src_addr, 0, sizeof(src_addr)); 
        src_addr.nl_family = AF_NETLINK;
        src_addr.nl_pid = getpid();
        src_addr.nl_groups = 1;
        if(bind(nlk, (struct sockaddr*)&src_addr, sizeof(src_addr)) == 0) 
            break;
        WARN(errno, "nlk bind(pid=%d): retry=%d", src_addr.nl_pid);
        sleep(3);
    }
    if (retry == 5) {
        REBOOT(0, "nlk bind(pid=%d) exhausted retries(%d)", src_addr.nl_pid, retry);
        return -2;
    }

    //netlink route socket initialization
    nlr = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_ROUTE);
    if(nlr < 0){
        REBOOT(errno, "nlr socket() failed");
        return -1;
    }

    for (retry = 0; retry < 5; ++retry) {
        memset((void *)&addr, 0, sizeof(addr)); 
        addr.nl_family = AF_NETLINK;
        addr.nl_pid = getpid();
        addr.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV6_IFADDR;
        if(bind(nlr, (struct sockaddr *)&addr, sizeof(addr)) == 0)
            break;
        WARN(errno, "nlr bind(pid=%d): retry=%d", addr.nl_pid, retry);
        sleep(3);
    }
    if (retry == 5) {
        REBOOT(0, "nlr bind(pid=%d) exhausted retries(%d)", src_addr.nl_pid, retry);
        return -2;
    }

    //2015-06-09, hitemeng - check if wpa is already up and wpa_supplicant.conf exists
    //                       if they both do, most likely skybell app was killed but was
    //                       previously running in station mode
    //first check if wpa_supplicant.conf exists
    if(start_wpa_supplicant && wifi_ap_profile_exists()){
        DBUG(1, "%s(): wifi ap profile exists\n", __FUNCTION__);
        //then check if wpa_supplicant is running, we check whether we can open it
        mon_conn = wpa_ctrl_open(WPA_MON_PATH);
        if(mon_conn != NULL){
            ALERT(0, "wpa ctrl opened\n");
            if(wpa_ctrl_attach(mon_conn) == 0){
                DBUG(1, "%s(): wpa is already running, skipping wifi shutdown\n", __FUNCTION__);
                //close down monitoring until we're running
                wpa_close_monitoring();
                wpa_running = TRUE;
            }else{
                wpa_close_monitoring();
            }
        }
    }
    if(!wpa_running){
        //2015-06-04, hitemeng - do a clean restart just in case
        kill_wifi();
    }

    SET_STATE(WIFI_STATE_IDLE);

    //create command pipe
    // moved to just before the 'for loop' 2016/2/23 for v1054 release (used to be before wifi_pwr_enable() above)
    if(pipe(cmd_pipe) == -1){
        REBOOT(errno, "pipe(wifi)\n");
        return -1;
    }
	DBUG(0,"wifi_task() for wifi_thread start_wpa_supplicant[%d]wpa_running[%d]\n",start_wpa_supplicant,wpa_running);//Charles

    int net_state = UP;
    for(wifi_thread = thread; wifi_thread != NULL;){
        struct timeval tv = {.tv_sec = 3, .tv_usec = 0};
        nfds = find_max4(cmd_pipe[0], nlk, nlr, wpas) + 1;

        FD_ZERO(&rfds);
        if (cmd_pipe[0] >= 0) FD_SET(cmd_pipe[0], &rfds); 
        if (nlk >= 0)         FD_SET(nlk, &rfds);
        if (nlr >= 0)         FD_SET(nlr, &rfds);
        if (wpas >= 0)        FD_SET(wpas, &rfds);

        if(ipRcvd){
            char statue[20] = {0};
            int net_fd = open("/sys/class/net/wlan0/operstate", O_RDONLY);
            if (net_fd < 0){
                ERROR(0,"open err\n");
            }

            read(net_fd, statue, 10);
            if (strstr(statue, "down") && net_state == UP){
                DBUG(Y_DBG,"%s.%s.%d-----goto here -------------\n",__FILE__,__func__,__LINE__);
                net_state = DOWN;
                SET_STATE(WIFI_STATE_WPA_DISCONNECTED);
            }
            else if(strstr(statue, "up" ) && net_state == DOWN){
                DBUG(Y_DBG,"%s.%s.%d-----goto here -------------\n",__FILE__,__func__,__LINE__);
                net_state = UP;
                SET_STATE(WIFI_STATE_IP_OBTAINED);
            }
            close(net_fd);
        }

        //block on file descriptor infinitely
        result = select(nfds, &rfds, NULL, NULL, &tv);
        if(result < 0){
            if(errno == EINTR){
                continue;
            }
            WARN(errno, "select");
            break;
        }
        else if(result > 0){
            //command pipe
            if(FD_ISSET(cmd_pipe[0], &rfds)){
                cmd = 0;
                //read single command
                if(read(cmd_pipe[0], &cmd, sizeof(cmd)) != sizeof(cmd)){
                    WARN(errno, "read(cmd_pipe[0]) failed\n");
                }
                else{
                    if (cmd == WIFI_CMD_DIE)
                        break;
                    wifi_process_cmd(cmd);
                }
            }
        
            //netlink kernel event
            if(FD_ISSET(nlk, &rfds)){
                nlk_read_event();
            }
        
            //netlink route event
            if(FD_ISSET(nlr, &rfds)){
                nlr_read_event(nlr, msg_handler);
            }

            //check if fd has been obtained for wpa
            if(wpas > 0){
                //wpa event
                if(FD_ISSET(wpas, &rfds) && mon_conn != NULL){
                    len = sizeof(buf) - 1;
                    int res = wpa_ctrl_recv(mon_conn, buf, &len);
                    if(res < 0){
                        WARN(errno, "wpa_ctrl_recv: res=%d", res);
                        break;
                    }
                    buf[len] = '\0';
                    DBUG(3, "%s\n", buf);
                    //process received message
                    wpa_process_msg(buf);
                }
            }
        }
        //update state machine
        wifi_fsm();
    }
	DBUG(0,"wifi_task() end modeTransition[%d] shuttingDown[%d]\n",modeTransition,shuttingDown);//Charles

    //will get here if wifi_thread becomes NULL or wifi_cmd(WIFI_CMD_DIE) is called (wifi_stop())
    die(thread);
    return 0;
}

// issue the WIFI_CMD_DIE command to terminate the wifi task and remove all of the
// resources (daemons, interfaces, kernel modules) -- this routine is called when
// transitioning between station and access-point modes
void wifi_stop(void){
    if (wifi_thread != NULL) {
        set_net_down();
        ALERT(0, "waiting for wifi thread to die!");
        while (wifi_thread != NULL) {
            wifi_cmd(WIFI_CMD_DIE);
            sleep(3);
			do_system("kill $(pidof udhcpc) > /dev/null 2>&1");//cc can't wifi stop when udhcpc still working
        }
        ALERT(0, "wifi thread now dead!");
    }
}

#define COUNTRY_PROFILE "/skybell/config/net/country_profile"


void set_wifi_chan(u8 set_value){
    char cmd[64] = {0};
    sprintf(cmd,"echo 0x%02x > /proc/net/rtl88x2cs/wlan0/chan_plan",set_value);
    do_system(cmd);
}

// called by either sta.c or ap.c to go into station mode or access-point mode respectively
// (note start_wpa_supplicant is true when called from sta.c and false when called from ap.c)
void wifi_start(bool start_wpa_supplicant){
#if 0//Charles
    set_net_down();
    encrypt_file(WIFI_AP_PROFILE, FALSE);
    wifi_stop();
#endif//Charles
    //start wifi thread
    ALERT(0, "waiting for wifi thread to start! (start_wpa_supplicant=%d)", start_wpa_supplicant);
    thread_assign("wifi-task", wifi_task, (void*)start_wpa_supplicant);
    while (wifi_thread == NULL)
        sleep(3);
    ALERT(0, "wifi thread started!");
}


//only called once at the beginning of life by main
void wifi_init(void){
    
    //disable firewall if we're in factory mode (this assumes that
    //iptables.skybell is the default set of firewall rules)
    if(factory()){
        firewall_off();
    }

    set_net_down();
    // encrypt_file(WIFI_AP_PROFILE, FALSE);

    //enable required mutexes
    MUTEX_INIT(&list_mutex, "wifi_list");
    MUTEX_INIT(&scan_mutex, "wifi_scan");
    MUTEX_INIT(&speedtest_mutex, "speedtest");
    MUTEX_INIT(&iperf3_mutex, "iperf3");

    //initialize list containing ap data
    INIT_LIST_HEAD(&wifi_ap_list);

    //for routing to clients
    MUTEX_INIT(&client_mutex, "wifi_client");
    INIT_LIST_HEAD(&client_list);
    client_count = 0;

}

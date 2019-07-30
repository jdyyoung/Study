#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <memory.h>

#include <netinet/ether.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "ak_thread.h"
#include "airkiss.h"
#include "rtw_cusdef.h"
#include "ak_ini.h"
#include "ak_config.h"
#include "yc_voice_ctrl.h"
#include "yc_qr_connect.h"

extern int g_configuring_wifi;
static int g_aks_run_flag = 0;

airkiss_context_t akcontex;
static const airkiss_config_t akconf =
    {
        (airkiss_memset_fn)&memset,
        (airkiss_memcpy_fn)&memcpy,
        (airkiss_memcmp_fn)&memcmp,
        (airkiss_printf_fn)&printf};

airkiss_status_t ak_status;
airkiss_result_t ak_result;

pthread_t tid[2];
int g_debuglv;
u8 g_stoprecv = FALSE, g_reinit;
char ifName[IFNAMSIZ];
char g_bssid[6] = {0, 0, 0, 0, 0, 0};
char g_wpafile_path[512];
int g_config_timeoute = -1; /*-1 is always parse, 0 stop parse, >0 parse all packets.*/
char g_ak_key[512];
struct timeval g_tv;
const unsigned char bc_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void wifi_do_scan(void);
void wifi_disable(void);
void wifi_enable(void);
static void *ufo_airkiss_connect(void *ak_result);
void exit_airkiss_connect(void);

//=====================	Custimize part====================================
#define MAX_LINE_LEN 1024
#define MAX_SCAN_TIMES 10

struct _chplan g_chPlan[] = {
    {
        .ch = 1,
        .bw_os = '+',
    },
    {
        .ch = 6,
        .bw_os = '-',
    },
    {
        .ch = 11,
        .bw_os = '-',
    },
    {
        .ch = 2,
        .bw_os = '+',
    },
    {
        .ch = 3,
        .bw_os = '+',
    },
    {
        .ch = 4,
        .bw_os = '+',
    },
    {
        .ch = 5,
        .bw_os = '-',
    },
    {
        .ch = 7,
        .bw_os = '-',
    },
    {
        .ch = 8,
        .bw_os = '-',
    },
    {
        .ch = 9,
        .bw_os = '-',
    },
    {
        .ch = 10,
        .bw_os = '-',
    },
    {
        .ch = 12,
        .bw_os = '-',
    },
    {
        .ch = 13,
        .bw_os = '-',
    },

    /*
		// 5G, Band1		
		{ .ch = 36,   	.bw_os = '+', },
		{ .ch = 40,   	.bw_os = '-', },
		{ .ch = 44,   	.bw_os = '+', },
		{ .ch = 48,   	.bw_os = '-', },
		
		// 5G, Band2
		{ .ch = 52,   	.bw_os = '+', },
		{ .ch = 56,   	.bw_os = '-', },
		{ .ch = 60,   	.bw_os = '+', },
		{ .ch = 64,   	.bw_os = '-', },
		
		// 5G, Band3
		{ .ch = 100,   	.bw_os = '+', },
		{ .ch = 104,   	.bw_os = '-', },
		{ .ch = 108,   	.bw_os = '+', },
		{ .ch = 112,   	.bw_os = '-', },
		{ .ch = 116,   	.bw_os = '+', },
		{ .ch = 132,   	.bw_os = '-', },
		{ .ch = 136,   	.bw_os = '+', },
		{ .ch = 140,   	.bw_os = '0', },

		// 5G, Band4
		{ .ch = 149,   	.bw_os = '+', },
		{ .ch = 153,   	.bw_os = '-', },
		{ .ch = 157,   	.bw_os = '+', },
		{ .ch = 161,   	.bw_os = '-', },
		{ .ch = 165,   	.bw_os = '0', },
*/
};

inline void RTK_SYSTEM(const char *cmd)
{
	DEBUG_PRINT("shell:  %s\n", cmd);
	system(cmd);
}

//=====================		Custimize part ending	====================================
char *survey_info_path()
{
	static char path[200];

	memset(path, 0, 200);
	sprintf(path, "/proc/net/%s/%s/survey_info", PROC_MODULE_PATH, ifName);
	return path;
}

/*
input:	@before		: string before unicode procedure.

description:	process unicode(ex chinese word)  SSID of AP,
*/

void wifi_do_scan(void)
{
	char cmdstr[250], fwstats[200], readline[MAX_LINE_LEN];
	FILE *fd = NULL;
	char *ch = NULL;
	int fwstatus = 0, i;

	sprintf(cmdstr, "echo 1 > /proc/net/%s/%s/survey_info\n",
	        PROC_MODULE_PATH, ifName);
	RTK_SYSTEM(cmdstr);

	//  poll /proc/../fwstatus and wait for scan done. wait 10sec at most.
	sprintf(fwstats, "/proc/net/%s/%s/fwstate", PROC_MODULE_PATH, ifName);
	for (i = 0; i < 10; i++)
	{
		sleep(1);
		fd = fopen(fwstats, "r");
		if (fd == NULL)
		{
			printf("file[%s] open can not create file !!! \n", fwstats);
			return;
		}
		memset(readline, 0, sizeof(char) * MAX_LINE_LEN);
		fgets(readline, MAX_LINE_LEN, fd);
		fclose(fd);
		if (strlen(readline) == 0)
			continue;
		if ((ch = strstr(readline, "=")) != NULL)
			fwstatus = (int)strtol(ch + 1, NULL, 0);
		if ((fwstatus & _FW_UNDER_SURVEY) == 0)
			break;
	}
}

void wifi_disable(void)
{
	//char cmdstr[250];
	RTK_SYSTEM("killall wpa_supplicant");
	//sprintf(cmdstr, "rmmod %s", WIFI_MODULE_NAME);
	//RTK_SYSTEM(cmdstr);
	RTK_SYSTEM("/usr/sbin/wifi_driver uninstall");
}

void wifi_enable(void)
{
	char cmdstr[250];
#if 0
	sprintf(cmdstr, "insmod /usr/modules/%s", WIFI_MODULE_NAME);
	RTK_SYSTEM(cmdstr);
	sleep(1);
#endif
	RTK_SYSTEM("/usr/sbin/wifi_driver station");
	sleep(1);
	//interface UP and enter monitor mode
	sprintf(cmdstr, "ifconfig %s up\n", ifName);
	RTK_SYSTEM(cmdstr);
	sleep(1);
}

/*
input:	@scan_res	: path of scan_result file
	@target_bssid	: bssid of target AP
output:	@		: AP's ssid, do nothing if strlen() is not zero.
	@flag		: indicate property of WPA, WPA2, WPS, IBSS, ESS, P2P.
	@channel		: AP's channel.
	
description:	parse scan_result to get the information of corresponding AP by BSSID.
*/
static int parse_scanres(char *scan_res, char *target_bssid, char *channel,
		     const char *ssid, u16 *flag)
{
	char readline[MAX_LINE_LEN];
	char *ch, *idx, *bssid, *flag_s;
	FILE *fp = NULL;
	int found = -EAGAIN;

	fp = fopen(scan_res, "r");
	if (!fp)
	{
		printf("%s:Unable to open file [%s] \n", __func__, scan_res);
		return -ENOENT;
	}

	while (!feof(fp))
	{
		*flag = 0;
		*channel = 0;
		if (!fgets(readline, MAX_LINE_LEN, fp))
			break;
		idx = strtok(readline, " \r\n");
		if (!idx)
			continue;
		bssid = strtok(NULL, " \r\n");
		if (!bssid || (strlen(bssid) < 17) || (strcmp(bssid, target_bssid) != 0))
			continue;
		found = TRUE;
		// Channel
		ch = strtok(NULL, " \r\n");
		if (!ch)
			continue;
		strcpy(channel, ch);
		// RSSI
		ch = strtok(NULL, " \r\n");
		if (!ch)
			continue;
		// SdBm
		ch = strtok(NULL, " \r\n");
		if (!ch)
			continue;
		// Noise
		ch = strtok(NULL, " \r\n");
		if (!ch)
			continue;
		// age
		ch = strtok(NULL, " \r\n");
		if (!ch)
			continue;
		// flag
		flag_s = strtok(NULL, " \r\n");
		if (!flag_s)
			continue;
		if ((ch = strstr(flag_s, PATTERN_WPA)) != NULL)
			*flag |= FLAG_WPA;
		if ((ch = strstr(flag_s, PATTERN_WPA2)) != NULL)
			*flag |= FLAG_WPA2;
		if ((ch = strstr(flag_s, PATTERN_WEP)) != NULL)
			*flag |= FLAG_WEP;
		if ((ch = strstr(flag_s, PATTERN_WPS)) != NULL)
			*flag |= FLAG_WPS;
		if ((ch = strstr(flag_s, PATTERN_IBSS)) != NULL)
			*flag |= FLAG_IBSS;
		if ((ch = strstr(flag_s, PATTERN_ESS)) != NULL)
			*flag |= FLAG_ESS;
		if ((ch = strstr(flag_s, PATTERN_P2P)) != NULL)
			*flag |= FLAG_P2P;

		DEBUG_PRINT("bssid = [%s],  channel=[%s] flag=%04X \n",
			  bssid, channel, *flag);
		break;
	}
	fclose(fp);
	return found;
}

void init_chplan(int chnum)
{
	int i;

	for (i = 0; i < chnum; i++)
	{
		if (g_chPlan[i].bw_os == '+')
			g_chPlan[i].bw_os = HAL_PRIME_CHNL_OFFSET_LOWER;
		else if (g_chPlan[i].bw_os == '-')
			g_chPlan[i].bw_os = HAL_PRIME_CHNL_OFFSET_UPPER;
		else
			g_chPlan[i].bw_os = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	}

	for (i = 0; i < chnum; i++)
		printf("probe_chplan[%d]:\tch=%d,\t bw_offset=%d \n",
		       i, g_chPlan[i].ch, g_chPlan[i].bw_os);
}

void wifi_monitor_mode_onoff(u8 onoff, const char *ifName)
{
	char cmdstr[200];

	memset(cmdstr, 0, sizeof(char) * 200);

	//iwconfig wlanX mode monitor
	//iw dev wlanX set type monitor
#ifdef CONFIG_IOCTL_CFG80211
	if (onoff)
		sprintf(cmdstr, "iw dev %s set type monitor\n", ifName);
	else
		sprintf(cmdstr, "iw dev %s set type managed\n", ifName);
#else
	if (onoff)
		sprintf(cmdstr, "iwconfig %s mode monitor\n", ifName);
	else
		sprintf(cmdstr, "iwconfig %s mode managed\n", ifName);
#endif
	RTK_SYSTEM(cmdstr);
}

static int get_rtheader_len(u8 *buf, size_t len)
{
	struct ieee80211_radiotap_header *rt_header;
	u16 rt_header_size;

	rt_header = (struct ieee80211_radiotap_header *)buf;
	/* check the radiotap header can actually be present */
	if (len < sizeof(struct ieee80211_radiotap_header))
		return -EILSEQ;
	/* Linux only supports version 0 radiotap format */
	if (rt_header->it_version)
		return -EILSEQ;
	rt_header_size = le16toh(rt_header->it_len);
	/* sanity check for allowed length and radiotap length field */
	if (len < rt_header_size)
		return -EILSEQ;
	return rt_header_size;
}

void wifi_set_channel(int chidx)
{
	char cmdstr[200];

	sprintf(cmdstr, "echo %d %d 1 > /proc/net/%s/%s/monitor \n",
	        g_chPlan[chidx].ch, g_chPlan[chidx].bw_os, PROC_MODULE_PATH, ifName);
	RTK_SYSTEM(cmdstr);
}

static int airkiss_finish()
{
	int8_t err;

	err = airkiss_get_result(&akcontex, &ak_result);
	if (err == 0)
	{
		printf("airkiss_get_result() ok!");
		printf("ssid = \"%s\", pwd = \"%s\", ssid_length = %d,"
		       "pwd_length = %d, random = 0x%02x\r\n",
		       ak_result.ssid,
		       ak_result.pwd, ak_result.ssid_length, ak_result.pwd_length, ak_result.random);
		exit_qr_code_connect();
		pthread_t connect_th;
		ak_thread_create(&connect_th, ufo_airkiss_connect, &ak_result, ANYKA_THREAD_MIN_STACK_SIZE, -1);
	}
	else
	{
		printf("airkiss_get_result() failed !\r\n");
	}
	return err;
}

void start_airkiss(char *key)
{
	char cmdstr[256];
	int8_t ret;
	printf("Start airkiss!\r\n");

	ret = airkiss_init(&akcontex, &akconf);

	if (ret < 0)
	{
		printf("Airkiss init failed!\r\n");
		return;
	}
#if AIRKISS_ENABLE_CRYPT
	airkiss_set_key(&akcontex, (const unsigned char *)key, strlen(key));
#endif
	printf("Finish init airkiss!\r\n");
	ak_status = AIRKISS_STATUS_CONTINUE;
	printf("===============\n%s\n===============\n", airkiss_version());

	ak_status = AIRKISS_STATUS_CONTINUE;
	memset(g_bssid, 0, 6);
	memset(cmdstr, '\0', sizeof(cmdstr));
	gettimeofday(&g_tv, NULL);
	sprintf(cmdstr, "rm -rf %s&", g_wpafile_path);
	RTK_SYSTEM(cmdstr);

	wifi_disable();
	wifi_enable();
	wifi_do_scan();
}

/*
	printf("-i = interface name\n\t");
	printf("-c = the path of wpa_supplicant config file\n\t");
	printf("-p = airkiss key\n\t");
	printf("-d = enable debug message, -D = more message.\n\t");
	printf("-t = timeout (seconds)\n\t");
	printf("-v = version\t\n\n");
*/
static int parse_argv()
{
	/*	initial variable by default valve	*/
	memset(ifName, 0, IFNAMSIZ);
	strcpy(ifName, DEFAULT_IF);
	memset(g_ak_key, 0, 512);
	strcpy(g_ak_key, DEFAULT_AK_KEY);
	//	rtk_sc_set_value(SC_DURATION_TIME, -1);
	strcpy(g_wpafile_path, DEFAULT_WPAFILE_PATH);
	g_debuglv = 1; //DEFAULT_DEBUG;

	printf("%s -- %s\n", __func__, PROGRAM_VERSION);
	if (g_debuglv)
	{

		DEBUG_PRINT("========option parse========\n");
		DEBUG_PRINT("g_ak_key = %s\n", g_ak_key);
		DEBUG_PRINT("timeout = %d\n", g_config_timeoute);
		DEBUG_PRINT("ifName = %s\n", ifName);
		DEBUG_PRINT("g_wpafile_path = %s\n", g_wpafile_path);
		DEBUG_PRINT("========================\n");
	}
	return 0;
}

/*
	description : check if execution time is exceeded?
	return : timeout 1, otherwise 0.
*/
static int check_config_timeout(void)
{
	struct timeval local_tv;

	if (g_config_timeoute == -1)
		return 0;
	gettimeofday(&local_tv, NULL);
	if ((local_tv.tv_sec - g_tv.tv_sec) > g_config_timeoute)
		return 1;
	else
	{
		//DEBUG_PRINT("[%d] seconds pass\n", g_config_timeoute);
		return 0;
	}
}

static void ProcessPacket(u8 *buffer, int size)
{
	u8 *da;
	//u8	*sa;
	u8 *bssid;
	u8 to_fr_ds;
	u8 type;
	airkiss_status_t ret;

	/*	80211 header format
		ver:	2bit
		type:	2bit
		subtype:	4bit
		tods:	1bit
		frds:	1bit
		other:	6bit		*/

	type = *buffer & TYPE_MASK;
	//	subtype = (*buffer & SUBTYPE_MASK) >> 4;
	if ((type != TYPE_DATA) || (size < 21))
		return;

	to_fr_ds = *(buffer + 1) & FRTODS_MASK;
	if (to_fr_ds == 1)
	{
		//sa = GetAddr2Ptr(buffer);
		bssid = GetAddr1Ptr(buffer);
		da = GetAddr3Ptr(buffer);
	}
	else if (to_fr_ds == 2)
	{
		//sa = GetAddr3Ptr(buffer);
		bssid = GetAddr2Ptr(buffer);
		da = GetAddr1Ptr(buffer);
	}
	else
	{
		//sa = NULL;
		da = NULL;
	}

	if ((da) && (memcmp(da, bc_mac, ETH_ALEN) == 0))
	{
		ret = airkiss_recv(&akcontex, buffer, size);
		if (ret == AIRKISS_STATUS_CHANNEL_LOCKED)
		{
			memcpy(g_bssid, bssid, 6);
			DEBUG_PRINT("g_bssid=[%02x:%02x:%02x:%02x:%02x:%02x]\n",
				  (u8)g_bssid[0], (u8)g_bssid[1], (u8)g_bssid[2],
				  (u8)g_bssid[3], (u8)g_bssid[4], (u8)g_bssid[5]);
		}
		printf("ak_sta=%d \n", ak_status);
		if (ret != AIRKISS_STATUS_CONTINUE)
			ak_status = ret;
	}
	return;
}

void doRecvfrom(void)
{
	int NumTotalPkts;
	int sockfd, sockopt;
	ssize_t numbytes;
	struct ifreq ifopts; /* set promiscuous mode */
	u8 buf[BUF_SIZ], *pkt;
	int rt_header_len = 0;

	printf("%s pthread id = %lu\n", __func__, pthread_self());
	//pthread_detach(pthread_self());
	//Create a raw socket that shall sniff
	/* Open PF_PACKET socket, listening for EtherType ETHER_TYPE */
	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1)
	{
		printf("listener: socket");
		return;
	}
	/* Set interface to promiscuous mode - do we need to do this every time? */
	strncpy(ifopts.ifr_name, ifName, IFNAMSIZ - 1);
	ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
	ifopts.ifr_flags |= IFF_PROMISC;
	ioctl(sockfd, SIOCSIFFLAGS, &ifopts);
	/* Allow the socket to be reused - incase connection is closed prematurely */
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1)
	{
		printf("setsockopt");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	/* Bind to device */
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ - 1) == -1)
	{
		printf("SO_BINDTODEVICE");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	NumTotalPkts = 0;
	while (1)
	{
		numbytes = recvfrom(sockfd, buf, BUF_SIZ, 0, NULL, NULL);
		if (numbytes < 0)
		{
			printf("Recvfrom error , failed to get packets\n");
			goto thr_exit;
		}

		if (g_stoprecv)
			goto thr_exit;

		// Get length of Radiotap header
		if ((rt_header_len = get_rtheader_len(buf, (size_t)numbytes)) < 1)
			continue;
		//Now process the packet
		pkt = (u8 *)buf + rt_header_len;
		// #ifdef CONFIG_NOFCS
		ProcessPacket(pkt, numbytes - rt_header_len + 4);
		// #else
		// 		ProcessPacket(pkt, numbytes - rt_header_len);
		// #endif
		NumTotalPkts++;
	}
thr_exit:
	close(sockfd);
	return;
}
#define NOTIFY_WIFI_OK 10000
static void notify_airkiss_cfg_wifi_ok()
{
	ak_print_normal_ex("airkiss configure wifi broadcast starting ...........\n ");
	int udp_fd; //sock fd
	int on = 1;

	socklen_t sockaddr_len;
	sockaddr_len = sizeof(struct sockaddr);

	struct sockaddr_in udp_addr;
	memset(&udp_addr, 0, sizeof(struct sockaddr));

	udp_addr.sin_family = AF_INET;	   /** IPV4 **/
	udp_addr.sin_port = htons(NOTIFY_WIFI_OK); /** port 10000 **/
	udp_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	if ((udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		return;
	}

	if (setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) != 0)
	{
		perror("setsockopt");
		close(udp_fd);
		return;
	}

	//发送至少20个random
	int cnt = 30;
	while (cnt--)
	{
		if (sendto(udp_fd, &(ak_result.random), 1, 0, (struct sockaddr *)&udp_addr, sockaddr_len) == -1)
		{
			perror("sendto");
			usleep(200 * 1000);
			continue;
		}
		usleep(200 * 1000);
	}
	close(udp_fd);
}

void doLogicHandle()
{
	int chnum = sizeof(g_chPlan) / sizeof(g_chPlan[0]);
	int chidx = 0;
	char cmdstr[256];
	memset(cmdstr, '\0', sizeof(cmdstr));
	pr("%s pthread id = %lu\n", __func__, pthread_self());
	//pthread_detach(pthread_self());
	while (g_aks_run_flag)
	{
		static u8 channel_lock = FALSE;
		usleep(TIME_CHSWITCH);
		if (check_config_timeout())
		{
			g_stoprecv = TRUE;
			g_aks_run_flag = 0;
			//超时提示音
			break;
		}
		if (ak_status == AIRKISS_STATUS_CHANNEL_LOCKED)
		{
			char bssid_str[64], ssid[64], ch[5];
			u16 flag = 0;

			if (channel_lock) // first AIRKISS_STATUS_CHANNEL_LOCKED
				continue;
			memset(bssid_str, 0, 64);
			memset(ch, 0, 5);
			sprintf(bssid_str, "%02x:%02x:%02x:%02x:%02x:%02x",
			        (u8)g_bssid[0], (u8)g_bssid[1], (u8)g_bssid[2],
			        (u8)g_bssid[3], (u8)g_bssid[4], (u8)g_bssid[5]);
		find_channel:
			if ((parse_scanres(survey_info_path(), bssid_str, ch, ssid, &flag) == TRUE) && (strlen(ch) != 0))
			{

				for (chidx = 0; chidx < chnum; chidx++)
				{
					if (atoi(ch) == g_chPlan[chidx].ch)
						break;
				}
				if (chidx == chnum)
				{
					printf("\n Unable to find corresponding channel in channel plan, ch=[%s] \n", ch);
					goto find_channel;
				}
				wifi_set_channel(chidx);
				RTK_SYSTEM(cmdstr);
				DEBUG_PRINT("Channel locked!! staying [ch:%s]\n", ch);
				channel_lock = TRUE;
			}
			else
			{
				DEBUG_PRINT("Channel locked!! but [%s] not found, scan again.\n", bssid_str);
				wifi_monitor_mode_onoff(FALSE, ifName);
				wifi_do_scan();
				wifi_monitor_mode_onoff(TRUE, ifName);
				goto find_channel;
			}
		}
		else if (ak_status == AIRKISS_STATUS_CONTINUE)
		{
			// switch channel & bandwidth
			wifi_set_channel(chidx);
			if (++chidx == chnum)
				chidx = 0; // start next scan round
		}
		else if (AIRKISS_STATUS_COMPLETE == ak_status)
		{
			//airkiss_finish()返回1，联网失败；返回0，联网成功
			/*
			if (airkiss_finish()) {
				// ak_key mismatch
				start_airkiss(g_ak_key);
				chidx = 0;
				channel_lock = FALSE;
			}else {
				exit_airkiss_connect();
				exit_qr_code_connect();
				break;
			}
			*/
			airkiss_finish();
			//exit_airkiss_connect();
			break;
		}
	}
	pr("结束airkiss接收线程\n");
	g_stoprecv = 1;
	pthread_join(tid[0], NULL);
}

int airkiss_get_ssid_pwd()
{
	int chnum = sizeof(g_chPlan) / sizeof(g_chPlan[0]);
	int err = 0;

	// system init
	if (parse_argv())
		return -EINVAL;
	init_chplan(chnum);

	start_airkiss(g_ak_key);

	wifi_monitor_mode_onoff(TRUE, ifName);

	g_stoprecv = 0;
	err = pthread_create(&(tid[0]), NULL, (void *)&doRecvfrom, NULL);
	if (err != 0)
	{
		g_stoprecv = 1;
		pr("\ncan't create thread :[%s]", strerror(err));
	}
	else
	{
		pr("\n doRecvfrom Thread created successfully\n");
	}

	g_aks_run_flag = 1;
	err = pthread_create(&(tid[1]), NULL, (void *)&doLogicHandle, NULL);
	if (err != 0)
	{
		g_aks_run_flag = 0;
		pr("\ncan't create thread :[%s]", strerror(err));
	}
	else
	{
		pr("\n doLogicHandle Thread created successfully\n");
	}

	return 0;
}

/**
 * check_wifi_connection - check ssid if connection ok.
 * return:  1, conneceted;  otherwise 0
 */
static int airkiss_wifi_connection(const char *ssid)
{
	char ip[16];
	// char result[100] = {0};
	// //int i = 0;
	// ak_cmd_exec("wpa_cli -iwlan0 status | grep wpa_state", result, 100);
	// ak_print_normal_ex("wpa_cli -iwlan0 status | grep wpa_state = %s\n", result);
	int ret = ak_net_get_ip("wlan0", ip);
	pr("联网结果：%d\n", ret);
	return !ret;
}

static void *ufo_airkiss_connect(void *param)
{
	airkiss_result_t *result = (airkiss_result_t *)param;
	char *ssid = result->ssid;
	char *pass = result->pwd;
	pthread_detach(pthread_self());
	pr("检测网络  -1 ssid:%s, pass:%s\n", ssid, pass);
	if (strlen(ssid))
	{
		struct sys_wifi_config *wifi_info = ak_config_get_sys_wifi();
		pr("检测网络  0\n");

		if (strcmp(wifi_info->ssid, ssid) || strcmp(wifi_info->passwd, pass))
		{
			pr("检测网络  1\n");

			//TODO 空密码修改加密方式
			strcpy(wifi_info->ssid, ssid);
			strcpy(wifi_info->passwd, pass);
			//strcpy(wifi_info->notify_connect_wifi, "WIFI_TRUE");
			if (strlen(wifi_info->ssid) && !strlen(wifi_info->passwd))
			{
				wifi_info->enc_type = WIFI_ENCTYPE_NONE;
			}
			else
			{
				wifi_info->enc_type = WIFI_ENCTYPE_WPA2_TKIP;
			}
			ak_config_set_sys_wifi(wifi_info, 1);
			change_connect_wifi_cfg("WIFI_TRUE");
			//usleep(1000 * 1000);

			int count_time = 0;
			while (count_time < 15)
			{
				pr("检测网络  2\n");
				int ret = airkiss_wifi_connection(ssid);
				ak_print_normal_ex("检查SSID:%d\n", ret);
				if (ret)
				{
					notify_airkiss_cfg_wifi_ok();
					return 1;
				}
				count_time++;
				sleep(1);
			}
		}
	}
	return 0;
}

int init_airkiss_connect()
{
	if (g_aks_run_flag)
	{
		pr("airkiss 配网已开启\n");
		return 1;
	}
	ak_status = AIRKISS_STATUS_CONTINUE;
	pr("开启airkiss 配网\n");
	airkiss_get_ssid_pwd();
	return 1;
}

int is_airkiss_connect_run()
{
	return g_aks_run_flag;
}

void exit_airkiss_connect(void)
{
	if (g_aks_run_flag)
	{
		//g_stoprecv = 1;
		g_aks_run_flag = 0;
		//直到两个线程结束join
		//pthread_join(tid[0],NULL);
		pthread_join(tid[1], NULL);
		into_no_net_story_mode();
	}
	else
	{
		pr("airkiss 未打开\n");
	}
}

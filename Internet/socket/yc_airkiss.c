#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ak_common.h"

#include "ak_thread.h"
#include "yc_http.h"
#include "airkiss.h"
#include "cJSON.h"
//#include "yc_airkiss.h"
extern const char *WECHAT_APPID;
extern const char *API_KEY;
extern const char *PRODUCT_ID;
extern char AUTH_MAC[16];
extern char DEVICE_ID[20];

static int is_authorize_success = 0;
static int airkiss_run_flag = 0;

#define DEFAULT_LAN_PORT 12476
//定义AirKiss库需要用到的一些标准函数，由对应的硬件平台提供，前三个为必要函数
static const airkiss_config_t akconf =
    {
        (airkiss_memset_fn)&memset,
        (airkiss_memcpy_fn)&memcpy,
        (airkiss_memcmp_fn)&memcmp,
        0};
void *lan_discover_server(void *parm)
{
	yc_airkiss_authorize();
	printf("%s thread id = %lu\n", __func__, pthread_self());
	pthread_detach(pthread_self());
	int udp_fd; //sock fd
	int on = 1;

	char API_KEY_DEVICE_ID[56];
	sprintf(API_KEY_DEVICE_ID, "%s_%s", API_KEY, DEVICE_ID);
	unsigned char g_lan_buf[200];
	unsigned short int g_lan_buf_len = sizeof(g_lan_buf);

	unsigned char g_rev_buf[200];
	unsigned short int g_rev_buf_len = sizeof(g_rev_buf);

	socklen_t sockaddr_len;
	sockaddr_len = sizeof(struct sockaddr);

	struct sockaddr_in udp_addr, server_addr;
	memset(&udp_addr, 0, sizeof(struct sockaddr));
	memset(&server_addr, 0, sizeof(struct sockaddr));

	/*** set socket attribute ***/
	udp_addr.sin_family = AF_INET;	     /** IPV4 **/
	udp_addr.sin_port = htons(DEFAULT_LAN_PORT); /** port 12476 **/
	udp_addr.sin_addr.s_addr = INADDR_ANY;       /** any ip  **/

	server_addr.sin_family = AF_INET;	     /** IPV4 **/
	server_addr.sin_port = htons(DEFAULT_LAN_PORT); /** port 12476 **/
	server_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

	if ((udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		return NULL;
	}

	if (setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &on, sizeof(on)) != 0)
	{
		perror("setsockopt");
		close(udp_fd);
		return NULL;
	}

	/** bind the socket  **/
	if (bind(udp_fd, (struct sockaddr *)&udp_addr, sizeof(udp_addr)) < 0)
	{
		perror("bind");
		close(udp_fd);
		return NULL;
	}

	struct sockaddr addr;
	while (airkiss_run_flag)
	{
		if (recvfrom(udp_fd, g_rev_buf, g_rev_buf_len, 0, (struct sockaddr *)&addr, &sockaddr_len) == -1)
		{
			perror("recvfrom");
			continue;
		}
		int ret = airkiss_lan_recv(g_rev_buf, g_rev_buf_len, &akconf);
		if (ret != AIRKISS_LAN_SSDP_REQ)
		{
			printf("airkiss_lan_recv error!\n");
			sleep(1);
			continue;
		}
		else
		{
			printf("g_rev_buf = %s\n", g_rev_buf);
			g_lan_buf_len = sizeof(g_lan_buf);
			int packret = airkiss_lan_pack(AIRKISS_LAN_SSDP_RESP_CMD, WECHAT_APPID, API_KEY_DEVICE_ID, 0, 0, g_lan_buf, &g_lan_buf_len, &akconf);
			if (packret != AIRKISS_LAN_PAKE_READY)
			{
				printf("Pack lan packet error!packret = %d--------------\n", packret);
				sleep(1);
				continue;
			}
			printf("sendto data is %s--------------------------------\n", g_lan_buf);
			if (sendto(udp_fd, g_lan_buf, g_lan_buf_len, 0, (struct sockaddr *)&server_addr, sockaddr_len) == -1)
			{
				perror("sendto");
				break;
			}
		}
		sleep(5);
	}
	close(udp_fd);
	return NULL;
}
int yc_airkiss_authorize_success()
{
	return is_authorize_success;
}
int yc_airkiss_authorize()
{
	char resp[1024] = {0};

	cJSON *root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "apiKey", API_KEY);
	//cJSON_AddStringToObject(root,"productId","51369");
	cJSON_AddStringToObject(root, "productId", PRODUCT_ID);
	//cJSON_AddStringToObject(root,"deviceId","ai10101010101001");

	cJSON *array = NULL;
	cJSON_AddItemToObject(root, "devices", array = cJSON_CreateArray());

	cJSON *obj = NULL;
	cJSON_AddItemToArray(array, obj = cJSON_CreateObject());
	cJSON_AddStringToObject(obj, "deviceId", DEVICE_ID);
	cJSON_AddStringToObject(obj, "mac", AUTH_MAC);

	char *str_js = cJSON_Print(root);
	cJSON_Delete(root);
	pr("%s json is : %s\n", __func__, str_js);
	sccPostHttp("iot-ai.tuling123.com", "/vendor/new/authorize", 80, str_js, strlen(str_js), resp, 5000);
	if (strlen(resp) == 0)
	{
		sccPostHttp("iot-ai.tuling123.com", "/vendor/new/authorize", 80, str_js, strlen(str_js), resp, 5000);
	}
	if (strlen(resp) == 0)
	{
		pr("Micro chat authorize fail!----------------------------------------\n");
		return 0;
	}
	pr("%s:resp\n%s\n", __func__, resp);
	is_authorize_success = 1;
	return 1;
}

int init_yc_airkiss()
{
	ak_pthread_t lan_discover_th;
	airkiss_run_flag = 1;
	ak_thread_create(&lan_discover_th, lan_discover_server, NULL, ANYKA_THREAD_MIN_STACK_SIZE, -1);
	return 1;
}

int yc_airkiss_reset_authorize()
{
	is_authorize_success = 0;
}

int exit_yc_airkiss()
{
	yc_airkiss_reset_authorize();
	airkiss_run_flag = 0;
	return 1;
}
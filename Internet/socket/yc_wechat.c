#include <stdio.h> /* printf, sprintf */
#include <dirent.h>
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <sys/time.h>
#include <ctype.h>
#include <time.h>

#include "ak_ai.h"
#include "ak_aenc.h"
#include "ak_aed.h"

#include "ak_common.h"
#include "ak_global.h"
#include "ak_config.h"

#include "mosquitto.h"
#include "aes.h"
#include "cJSON.h"
#include "ak_thread.h"
#include "yc_http.h"
#include "yc_wechat.h"
#include "yc_mqtt.h"
#include "yc_config.h"
#include "yc_interaction.h"
#include "yc_common.h"

#define WECHAT_UPLOAD_AMR "/tmp/wechat_upload.amr"
#ifdef YC_RTV
extern int yc_rtv_product_test_mode;
#endif
#if 0
static void *aenc_handle = NULL;
static void *ai_handle = NULL;
static void *stream_handle = NULL;
#endif
typedef struct
{
	char user_id[17];
	char aes_key[17];
	char api_key[33];
} turing_user_t;
static turing_user_t *tulingUser = NULL;

static int wechat_record_flag = AK_FALSE;
static char upload_head[] =
    "POST /resources/file/upload?apiKey=%s HTTP/1.1\r\n"
    "Host: %s\r\n"
    "Connection: keep-alive\r\n"
    "Content-Length: %d\r\n"
    "Cache-Control: no-cache\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36\r\n"
    "Content-Type: multipart/form-data; boundary=%s\r\n"
    "Accept: */*\r\n"
    "DNT: 1\r\n"
    "Accept-Encoding: gzip, deflate\r\n"
    "Accept-Language: en-US,en;q=0.8,zh-CN;q=0.6,zh;q=0.4,zh-TW;q=0.2,es;q=0.2\r\n"
    "\r\n";

static char upload_parameters[] =
    "Content-Disposition: form-data; name=\"speech\"\r\n\r\n%s";

static char upload_speech[] =
    "Content-Disposition: form-data; name=\"speech\"; filename=\"speech.wav\"\r\n"
    "Content-Type: application/octet-stream\r\n\r\n";

static void error(const char *msg)
{
	perror(msg);
	exit(0);
}

/*
 * 获取当前时间 单位:ms
 */
static long getCurrentTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/*
 * 获取随机字符串
 */
static void get_rand_str(char s[], int number)
{
	char *str = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	int i, lstr;
	char ss[2] = {0};
	lstr = strlen(str);			   //计算字符串长度
	srand((unsigned int)time((time_t *)NULL)); //使用系统时间来初始化随机数发生器
	for (i = 1; i <= number; i++)
	{					 //按指定大小返回相应的字符串
		sprintf(ss, "%c", str[(rand() % lstr)]); //rand()%lstr 可随机返回0-71之间的整数, str[0-71]可随机得到其中的字符
		strcat(s, ss);			 //将随机生成的字符串连接到指定数组后面
	}
}

struct resp_header
{
	int status_code;        //HTTP/1.1 '200' OK
	char content_type[128]; //Content-Type: application/gzip
	long content_length;    //Content-Length: 11683079
};

static void get_resp_header(const char *response, struct resp_header *resp)
{
	/*获取响应头的信息*/
	char *pos = strstr(response, "HTTP/");
	if (pos)
		sscanf(pos, "%*s %d", &resp->status_code); //返回状态码
	pos = strstr(response, "Content-Type:");	   //返回内容类型
	if (pos)
		sscanf(pos, "%*s %s", resp->content_type);
	pos = strstr(response, "Content-Length:"); //内容的长度(字节)
	if (pos)
		sscanf(pos, "%*s %ld", &resp->content_length);
}

/*
 * 构造请求，并发送请求
 */
static void buildRequestUpload(int socket_fd, char *file_data, int len, char *host)
{
	char *boundary_header = "------AiWiFiBoundary";
	char *end = "\r\n";      //结尾换行
	char *twoHyphens = "--"; //两个连字符
	char s[20] = {0};
	get_rand_str(s, 19);
	char *boundary = malloc(strlen(boundary_header) + strlen(s) + 1);
	//char boundary[strlen(boundary_header)+strlen(s) +1];
	memset(boundary, 0, strlen(boundary_header) + strlen(s) + 1);
	strcat(boundary, boundary_header);
	strcat(boundary, s);
	pr("boundary is : %s\n", boundary);
	char firstBoundary[128] = {0};
	char secondBoundary[128] = {0};
	char endBoundary[128] = {0};

	sprintf(firstBoundary, "%s%s%s", twoHyphens, boundary, end);
	sprintf(secondBoundary, "%s%s%s%s", end, twoHyphens, boundary, end);
	sprintf(endBoundary, "%s%s%s%s%s", end, twoHyphens, boundary, twoHyphens, end);
#if 0
	uint8_t in[17];
	uint8_t out[64] = {'0'};
	uint8_t aes_key_1[17];
	uint8_t iv[17]={0};
	memcpy(in, tulingUser->user_id, strlen(tulingUser->user_id));
	memcpy(aes_key_1, tulingUser->aes_key, strlen(tulingUser->aes_key));
	memcpy(iv, tulingUser->api_key, 16);
	AES128_CBC_encrypt_buffer(out, in, 16, aes_key_1, iv);
	uint8_t outStr[64] = {'0'};
	int i,aseLen=0;
	for(i=0;i < 16;i++){
		aseLen+=snprintf((char*)outStr+aseLen,64,"%.2x",out[i]);
	}
#endif
	char *parameter_data = malloc(strlen(upload_parameters) + strlen(boundary) + strlen(end) * 2 + strlen(twoHyphens) + 1);
	//pr("parameters is : %s\n", parameter_data);
	strcpy(parameter_data, upload_parameters);
	strcat(parameter_data, secondBoundary);
	int content_length = len + strlen(boundary) * 2 + strlen(parameter_data) + strlen(upload_speech) + strlen(end) * 3 + strlen(twoHyphens) * 3;
	//pr("content length is %d \n", content_length);

	char header_data[4096] = {0};
	int ret = snprintf(header_data, 4096, upload_head, tulingUser->api_key, host, content_length, boundary);

	pr("Request header is :\n %s\n", header_data);
	//printf("firstBoundary is :\n%s\n", firstBoundary);
	//printf("upload_speech is : \n%s\n",upload_speech);

	//header_data,boundary,parameter_data,boundary,upload_speech,fileData,end,boundary,boundary_end
	send(socket_fd, header_data, ret, 0);
	send(socket_fd, firstBoundary, strlen(firstBoundary), 0);
	send(socket_fd, parameter_data, strlen(parameter_data), 0);
	send(socket_fd, upload_speech, strlen(upload_speech), 0);

	int w_size = 0, pos = 0, all_Size = 0;
	while (1)
	{
		//pr("ret = %d\n",ret);
		pos = send(socket_fd, file_data + w_size, len - w_size, 0);
		w_size += pos;
		all_Size += len;
		if (w_size == len)
		{
			w_size = 0;
			break;
		}
	}

	send(socket_fd, endBoundary, strlen(endBoundary), 0);

	free(boundary);
	free(parameter_data);
}

/*
 * 获取响应结果
 */
static void getResponse(int socket_fd, char **text)
//static void getResponse(int socket_fd)
{
	/* receive the response */
	char response[4096];
	memset(response, 0, sizeof(response));
	int length = 0, mem_size = 4096;
#if 0
    while( length = recv(socket_fd, response, 1024, 0))
    {
        if(length < 0)
        {
            pr("Recieve Data From Server Failed!\n");
            break;
        }
        pr("RESPONSE:\n %s\n", response);
        memset(response, 0, sizeof(response));
    }
#else
	struct resp_header resp;
	int ret = 0;
	while (1)
	{
		ret = recv(socket_fd, response + length, 1, 0);
		if (ret <= 0)
			break;
		//找到响应头的头部信息, 两个"\r\n"为分割点
		int flag = 0;
		int i;
		for (i = strlen(response) - 1; response[i] == '\n' || response[i] == '\r'; i--, flag++)
			;
		if (flag == 4)
			break;
		length += ret;
		if (length >= mem_size - 1)
		{
			break;
		}
	}
	get_resp_header(response, &resp);
	pr("resp.content_length = %ld status_code = %d\n", resp.content_length, resp.status_code);
	if (resp.status_code != 200 || resp.content_length == 0)
	{
		return;
	}
	char *code = (char *)calloc(1, resp.content_length + 1);
	if (code == NULL)
	{
		return;
	}
	ret = 0;
	length = 0;
	while (1)
	{
		ret = recv(socket_fd, code + length, resp.content_length - length, 0);
		if (ret <= 0)
		{
			free(code);
			break;
		}
		length += ret;
		if (length == resp.content_length)
			break;
	}
	pr("response is %s\n", code);
	*text = code;
#endif
}

/*
 * 获取socket文件描述符
 */
static int get_socket_fd(char *host)
{
	int portno = 80;
	int sockfd;
	struct hostent *server;
	struct sockaddr_in serv_addr;

	/* create the socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
        return -1;
    }

    /* lookup the ip address */
    server = gethostbyname(host);
    if (server == NULL){
        error("ERROR, no such host");
        close(sockfd);
        return -1;
    }
	/* fill in the structure */
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

	/* connect the socket */
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR connecting");
        close(sockfd);
        return -1;
    }
	return sockfd;
}

/*******************************************
@函数功能:	json解析服务器数据
@参数:	pMsg	服务器数据
***********************************************/
static int parseJson_string(const char *pMsg, char *resp_mediaID)
{
	if (NULL == pMsg)
	{
		return -1;
	}
	cJSON *pJson = cJSON_Parse(pMsg);
	if (NULL == pJson)
	{
		return -1;
	}
	cJSON *pSub = NULL;
	if (cJSON_HasObjectItem(pJson, "desc"))
	{
		pSub = cJSON_GetObjectItem(pJson, "desc");
		pr("resp error desc is %s --------------------\n", pSub->valuestring);
	}
	pSub = cJSON_GetObjectItem(pJson, "code");
	if (NULL == pSub)
	{
		goto exit;
	}
	switch (pSub->valueint)
	{
	case 40001:
	case 40002:
	case 40003:
	case 40004:
	case 40005:
	case 40006:
	case 40007:
	case 305000:
	case 302000:
	case 200000:
		goto exit;
	}

	pSub = cJSON_GetObjectItem(pJson, "payload");
	if (NULL == pSub)
	{
		goto exit;
	}
	pr("resp_mediaID is %s---------------------\n", pSub->valuestring);
	strcpy(resp_mediaID, pSub->valuestring);
	return 1;

exit:
	if (pJson)
	{
		cJSON_Delete(pJson);
		pJson = NULL;
	}
	return -1;
}
/*
 * 读取文件
 */
static int readFile(const char *file_name, char **file_data, int *length)
{
	if (access(file_name, F_OK) != 0)
	{
		pr("file %s not exsit!\n", file_name);
		return -1;
	}
	FILE *fp = fopen(file_name, "rb");
	if (fp == NULL)
	{
		pr("open file %s error!\n", file_name);
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	int file_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *fileData = (char *)calloc(1, file_len + 1);
	if (fileData == NULL)
		return -1;
	int len = fread(fileData, 1, file_len, fp);
	*file_data = fileData;
	*length = len;
	fclose(fp);
	return 0;
}

/*
 * 发送请求，读取响应
 */
static int requestAndResponse(int socket_fd, char *file_data, int len, char *host)
{
	char *text = NULL;
	char resp[1024] = {0};
	char resp_mediaID[128] = {0};
	//第一步上传音频文件
	buildRequestUpload(socket_fd, file_data, len, host);
	long start_time = getCurrentTime();
	pr("Send finish at %ld.\r\n", start_time);
	//第二步获取上传文件回复的mediaID
	getResponse(socket_fd, &text);
	//getResponse(socket_fd);
	long end_time = getCurrentTime();
	pr("Response finish at %ld.\r\n", end_time);
	pr("耗时：%ld\n", end_time - start_time);
	if (text)
	{
		int ret = parseJson_string((const char *)text, resp_mediaID);
		free(text);
		text = NULL;
		if (ret < 0)
		{
			pr("get mediaID fail ------------------------\n");
			return -1;
		}
	}
	else
	{
		pr("req failed===================================\n");
		return -1;
	}

	cJSON *root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "apiKey", tulingUser->api_key);
	cJSON_AddNumberToObject(root, "type", 0); //type = 0 语音对讲
	cJSON_AddStringToObject(root, "deviceId", tulingUser->user_id);

#if 0
	cJSON *array = NULL;
	cJSON_AddItemToObject(root,"toUsers",array = cJSON_CreateArray());
	cJSON_AddItemToArray(array,cJSON_CreateString("father_id"));
	cJSON_AddItemToArray(array,cJSON_CreateString("mother_id"));
#endif
	cJSON *item = NULL;
	cJSON_AddItemToObject(root, "message", item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, "mediaId", resp_mediaID);

	char *str_js = cJSON_Print(root);
	cJSON_Delete(root);

	pr("respond media ID data : %s\n", str_js);
	//http POST
	sccPostHttp(host, "/iot/message/send", 80, str_js, strlen(str_js), resp, 5000);
	if (strlen(resp) == 0)
	{
		sccPostHttp(host, "/iot/message/send", 80, str_js, strlen(str_js), resp, 5000);
	}
	if (strlen(resp) == 0)
	{
		pr("http post mediaID  fail!\n");
		pr("resp_mediaID is %s---------------------\n", resp_mediaID);
		return -1;
	}

	pr("%s:resp %s\n", __func__, resp);
	cJSON *pJson = cJSON_Parse(resp);
	if (NULL == pJson)
	{
		return -1;
	}

	cJSON *pSub = cJSON_GetObjectItem(pJson, "code");
	if (NULL == pSub)
	{
		return -1;
	}
	else if (0 == pSub->valueint)
	{
		return 0;
	}
	else if (41004 == pSub->valueint)
	{
		return 41004;
	}
	else
	{
		return -1;
	}
}

static int InitTuling(const char *userId, const char *aes_key, const char *api_key)
{
	tulingUser = (turing_user_t *)calloc(1, sizeof(turing_user_t));
	if (tulingUser == NULL)
	{
		return -1;
	}
	memcpy(tulingUser->user_id, userId, strlen(userId));
	memcpy(tulingUser->aes_key, aes_key, strlen(aes_key));
	memcpy(tulingUser->api_key, api_key, strlen(api_key));
	return 0;
}
static void DestoryTuling(void)
{
	if (tulingUser)
	{
		free(tulingUser);
		tulingUser = NULL;
	}
}

int yc_wechat_upload_audio(const char *file_name)
{
	pr("%s start............\n", __func__);
	char *user_id = DEVICE_ID;
	char *aes_key = AES_KEY;
	//char *api_key = "78284dd1f87a414c9fef5dcd7e360c9e";
	char *api_key = API_KEY;
	InitTuling(user_id, aes_key, api_key);
	char *host = "iot-ai.tuling123.com";
	int ret = -1;
	//读取一个文件
	char *fileData = NULL;
	int len = 0;
	if (readFile(file_name, &fileData, &len) != 0){
		DestoryTuling();
		return ret;
	}
	int sockfd = get_socket_fd(host);
	if(sockfd < 0){
        pr("get sockfd error\n");
		free(fileData);
		DestoryTuling();
        return -1;
    }

	//非流式识别
	ret = requestAndResponse(sockfd, fileData, len, host);

	close(sockfd);
	free(fileData);
	DestoryTuling();
	pr("%s exit............\n", __func__);
	return ret;
}

#if 0 //单独录音方式
void *wechat_record_th_run(void *param)
{
	yc_print_time("开始录音");
	FILE *fp = fopen(WECHAT_UPLOAD_AMR, "wb+");
	if (fp == NULL)
	{
		ak_print_error_ex("创建微信聊天文件 failed\n");
		return;
	}
	const unsigned char amrheader[] = "#!AMR\n";
	fwrite(amrheader, sizeof(amrheader) - 1, 1, fp);
	struct list_head stream_head;
	INIT_LIST_HEAD(&stream_head);
	while (wechat_record_flag)
	{
		if (!ak_aenc_get_stream(stream_handle, &stream_head))
		{
			struct aenc_entry *entry = NULL;
			struct aenc_entry *ptr = NULL;

			list_for_each_entry_safe(entry, ptr, &stream_head, list)
			{
				if (entry)
				{
					// pr("写入录音文件：%d\n", entry->stream.len);
					yc_print_time("写录音文件");
					/* write data to file */
					if (fwrite(entry->stream.data, entry->stream.len, 1, fp) < 0)
					{
						ak_print_error_ex("write file err\n");
					}
				}
				/* after write to file,release stream */
				ak_aenc_release_stream(entry);
			}
		}
		ak_sleep_ms(10);
	}
	pr("录音结束，关闭文件\n");
	if (NULL != fp)
	{
		fclose(fp);
		fp = NULL;
	}
	yc_wechat_upload_audio(WECHAT_UPLOAD_AMR);
	pr("发送语音结束，删除临时文件\n");
	remove(WECHAT_UPLOAD_AMR);
	sync();
	return NULL;
}



ak_pthread_t wechat_record_th;
int wechat_record()
{
	if (wechat_record_flag)
	{
		pr("微信录音已打开\n");
		return 0;
	}
	yc_print_time("进入微信录音");

	exit_voice_record();
	//TODO 眼灯状态延迟500ms以上，所以需要改下状态方式
	//yc_interaction_func(STATUS_WECHAT_START_RECORD);
	yc_print_time("进入微信录音1");
	struct pcm_param ai_param = {0};
	static struct audio_param aenc_param = {0};
	ai_param.sample_bits = 16;
	/* channel number */
	ai_param.channel_num = 1;
	ai_param.sample_rate = 8000;

	ai_handle = ak_ai_open(&ai_param);

	if (NULL == ai_handle)
	{
		pr("打开ai失败\n");
		return 0;
	}

	ak_print_normal("\t 1. ak_ai_open OK\n");

	/* set source */
	if (ak_ai_set_source(ai_handle, AI_SOURCE_MIC))
	{
		ak_print_error_ex("set ai source mic fail\n");
		return 0;
	}
	else
	{
		ak_print_normal_ex("set ai source mic success\n");
	}

	/* set aec,aec only support 8K sample */
	ak_ai_set_aec(ai_handle, AUDIO_FUNC_DISABLE);

	/* set nr and agc,nr&agc only support 8K sample */
	ak_ai_set_nr_agc(ai_handle, AUDIO_FUNC_DISABLE);

	/* set resample */
	ak_ai_set_resample(ai_handle, AUDIO_FUNC_DISABLE);
	struct sys_user_config *globle_config = (struct sys_user_config *)ak_config_get_system_user();
	/* volume is from 0 to 12,volume 0 is mute */
	ak_ai_set_volume(ai_handle, globle_config->voice_encode_volume);

	/* clear ai buffer */
	ak_ai_clear_frame_buffer(ai_handle);

	if (AK_SUCCESS == ak_ai_set_frame_interval(ai_handle, AMR_FRAME_INTERVAL))
	{
		ak_print_normal_ex("frame interval=%d\n", AMR_FRAME_INTERVAL);
	}

	/* start capture will begin to get frame */
	ak_ai_start_capture(ai_handle);

	aenc_param.sample_bits = 16;
	aenc_param.channel_num = 1;
	aenc_param.sample_rate = 8000;
	aenc_param.type = AK_AUDIO_TYPE_AMR;
	aenc_handle = ak_aenc_open(&aenc_param);
	if (NULL == aenc_handle)
	{
		pr("打开aenc失败\n");
		return 0;
	}

	/* bind audio input & encode */
	stream_handle = ak_aenc_request_stream(ai_handle, aenc_handle);
	if (NULL == stream_handle)
	{
		ak_print_error_ex("ak_aenc_request_stream failed\n");
		return 0;
	}
	// ak_print_normal("ak_aenc_request_stream OK\n");

	wechat_record_flag = 1;
	yc_print_time("进入微信录音2");
	int ret = ak_thread_create(&wechat_record_th, wechat_record_th_run, NULL, ANYKA_THREAD_MIN_STACK_SIZE, -1);
	yc_interaction_func(STATUS_WECHAT_START_RECORD);
	if (ret)
	{
		pr("初始化微信录音线程失败\n");
		wechat_record_flag = 0;
		return 0;
	}

	return 1;
}
int wechat_send()
{
	pr("发送微信录音\n");

	if (wechat_record_flag == 0)
	{
		pr("微信录音未开启\n");
		yc_interaction_func(STATUS_WECHAT_SEND_FAILED);
		init_voice_record();
		return 0;
	}
	yc_interaction_func(STATUS_WECHAT_SEND_SUCCESS);
	wechat_record_flag = 0;
	ak_thread_join(wechat_record_th);

	pr("取消录音媒体流\n");
	if (NULL != stream_handle)
	{
		ak_aenc_cancel_stream(stream_handle);
		stream_handle = NULL;
	}

	pr("录音结束，关闭编码器\n");
	if (NULL != aenc_handle)
	{
		/* aenc close */
		ak_aenc_close(aenc_handle);
		aenc_handle = NULL;
	}

	pr("录音结束，关闭AI\n");
	if (NULL != ai_handle)
	{
		ak_ai_stop_capture(ai_handle);
		/* ai close */
		ak_ai_close(ai_handle);
		ai_handle = NULL;
	}

	sleep(1);
	init_voice_record();
	return 1;
}
#endif
void *wechat_voice_cb(char *file_name)
{
	int ret = -1;

#ifdef YC_RTV
	if( 1 == yc_rtv_product_test_mode){
		pr("开始播报测试录音。。。。。。。。。。。。。。。\n");
		voice_stop_play();
		voice_add_play(file_name);
		voice_record_set_mode(_NONE_MODE, NULL);
		return NULL;
	}
	else{
		ret = yc_wechat_upload_audio(file_name);
	}
#else
	ret = yc_wechat_upload_audio(file_name);
#endif

	if (0 == ret)
	{
		pr("发送微信录音成功\n");
		yc_interaction_func(STATUS_WECHAT_SEND_SUCCESS);
	}
	else if (41004 == ret)
	{
		pr("设备未绑定\n");
		yc_interaction_func(STATUS_WECHAT_NO_BOUND);
	}
	else
	{
		pr("发送微信录音失败\n");
		yc_interaction_func(STATUS_WECHAT_SEND_FAILED);
	}

	voice_record_set_mode(_NONE_MODE, NULL);
}

int wechat_record()
{
	if (wechat_record_flag)
	{
		pr("微信录音已打开\n");
		return 0;
	}
	// yc_interaction_func(STATUS_WECHAT_START_RECORD);
	yc_print_time("进入微信录音");
	clear_aiwifi_file();
	voice_record_set_mode(_WECHAT_MODE, wechat_voice_cb);
	wechat_record_flag = 1;
}
int wechat_send()
{
	vr_stop_record();
	wechat_record_flag = 0;
}
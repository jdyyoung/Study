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

#include "yc_common.h"
#include "aes.h"
#include "cJSON.h"
#include "yc_http.h"
#include "yc_ai_voice.h"
#include "ufo_init.h"
#include "yc_ai_voice.h"
#include "yc_interaction.h"
#include "yc_voice_record.h"

#define FILE_SEND
extern char *API_KEY;
extern char DEVICE_ID[20];
extern char *AES_KEY;

#ifdef YC_DEBUG
static int save_sound_count = 0;
#endif

static int aiwifi_is_story_sony = 0;

typedef struct
{
    char token[64];
    char user_id[17];
    char aes_key[17];
    char api_key[33];
} TulingUser_t;
char aiwifi_token[64] = {0};

static TulingUser_t *tulingUser = NULL;
static int g_aiwifi_play = 0;

static char upload_head[] =
    "POST /speech/chat HTTP/1.1\r\n"
    "Host: %s\r\n"
    "Connection: keep-alive\r\n"
    "Content-Length: %d\r\n"
    "Cache-Control: no-cache\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36\r\n"
    "Content-Type: multipart/form-data; boundary=%s\r\n"
    "Accept: */*\r\n"
    "Accept-Encoding: gzip, deflate\r\n"
    "Accept-Language: en-US,en;q=0.8,zh-CN;q=0.6,zh;q=0.4,zh-TW;q=0.2,es;q=0.2\r\n"
    "\r\n";

static char upload_parameters[] =
    "Content-Disposition: form-data; name=\"parameters\"\r\n\r\n%s";

static char upload_speech[] =
    "Content-Disposition: form-data; name=\"speech\"; filename=\"speech.pcm\"\r\n"
    "Content-Type: application/octet-stream\r\n\r\n";

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

/*
 * 获取当前时间
 */
long getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/*
 * 获取随机字符串
 */
void get_rand_str(char s[], int number)
{
    char *str = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int i, lstr;
    char ss[2] = {0};
    lstr = strlen(str);                        //计算字符串长度
    srand((unsigned int)time((time_t *)NULL)); //使用系统时间来初始化随机数发生器
    for (i = 1; i <= number; i++)
    {                                            //按指定大小返回相应的字符串
        sprintf(ss, "%c", str[(rand() % lstr)]); //rand()%lstr 可随机返回0-71之间的整数, str[0-71]可随机得到其中的字符
        strcat(s, ss);                           //将随机生成的字符串连接到指定数组后面
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
    pos = strstr(response, "Content-Type:");       //返回内容类型
    if (pos)
        sscanf(pos, "%*s %s", resp->content_type);
    pos = strstr(response, "Content-Length:"); //内容的长度(字节)
    if (pos)
        sscanf(pos, "%*s %ld", &resp->content_length);
}

/*
 * 构造请求，并发送请求
 */
void buildRequest(int socket_fd, char *file_data, int len, int asr_type, int realtime, int index, char *identify, char *host)
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
    printf("boundary is : %s\n", boundary);
    char firstBoundary[128] = {0};
    char secondBoundary[128] = {0};
    char endBoundary[128] = {0};

    sprintf(firstBoundary, "%s%s%s", twoHyphens, boundary, end);
    sprintf(secondBoundary, "%s%s%s%s", end, twoHyphens, boundary, end);
    sprintf(endBoundary, "%s%s%s%s%s", end, twoHyphens, boundary, twoHyphens, end);

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "ak", tulingUser->api_key);
    cJSON_AddNumberToObject(root, "asr", asr_type);
    cJSON_AddNumberToObject(root, "tts", 3);
    cJSON_AddNumberToObject(root, "flag", 3);
    cJSON_AddNumberToObject(root, "tone", 21);
    if (identify)
    {
        cJSON_AddStringToObject(root, "identify", identify);
    }
    if (realtime)
    {
        cJSON_AddNumberToObject(root, "realTime", 1);
    }
    if (index)
    {
        cJSON_AddNumberToObject(root, "index", index);
    }

    uint8_t in[17];
    uint8_t out[64] = {'0'};
    uint8_t aes_key_1[17];
    uint8_t iv[17] = {0};
    memcpy(in, tulingUser->user_id, strlen(tulingUser->user_id));
    memcpy(aes_key_1, tulingUser->aes_key, strlen(tulingUser->aes_key));
    memcpy(iv, tulingUser->api_key, 16);
    AES128_CBC_encrypt_buffer(out, in, 16, aes_key_1, iv);
    uint8_t outStr[64] = {'0'};
    int i, aseLen = 0;
    for (i = 0; i < 16; i++)
    {
        aseLen += snprintf((char *)outStr + aseLen, 64, "%.2x", out[i]);
    }

    cJSON_AddStringToObject(root, "uid", outStr);
    cJSON_AddStringToObject(root, "token", tulingUser->token);
    char *str_js = cJSON_Print(root);
    cJSON_Delete(root);
    printf("parameters is : %s\n", str_js);
    char *parameter_data = malloc(strlen(str_js) + strlen(upload_parameters) + strlen(boundary) + strlen(end) * 2 + strlen(twoHyphens) + 1);
    sprintf(parameter_data, upload_parameters, str_js);
    strcat(parameter_data, secondBoundary);

    int content_length = len + strlen(boundary) * 2 + strlen(parameter_data) + strlen(upload_speech) + strlen(end) * 3 + strlen(twoHyphens) * 3;
    //printf("content length is %d \n", content_length);

    char header_data[4096] = {0};
    int ret = snprintf(header_data, 4096, upload_head, host, content_length, boundary);

    //printf("Request header is : %s\n", header_data);

    //header_data,boundary,parameter_data,boundary,upload_speech,fileData,end,boundary,boundary_end
    send(socket_fd, header_data, ret, 0);
    send(socket_fd, firstBoundary, strlen(firstBoundary), 0);
    send(socket_fd, parameter_data, strlen(parameter_data), 0);
    send(socket_fd, upload_speech, strlen(upload_speech), 0);

    int w_size = 0, pos = 0, all_Size = 0;
    while (1)
    {
        //printf("ret = %d\n",ret);
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
    free(str_js);
}

/*
 * 获取响应结果
 */
void getResponse(int socket_fd, char **text)
{
    /* receive the response */
    char response[4096];
    memset(response, 0, sizeof(response));
    int length = 0, mem_size = 4096;
    /*
    while( length = recv(socket_fd, response, 1024, 0))
    {
        if(length < 0)
        {
            printf("Recieve Data From Server Failed!\n");
            break;
        }
        
        printf("RESPONSE:\n %s\n", response);
        
        memset(response, 0, sizeof(response));
    }
    */

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
    printf("resp.content_length = %ld status_code = %d\n", resp.content_length, resp.status_code);
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
        //printf("result = %s len=%d\n",code,len);
        if (length == resp.content_length)
            break;
    }
#ifdef YC_DEBUG
    char save_name[64] = {0};
    sprintf(save_name, "sound_rsp_%d.txt", save_sound_count);
    yc_save_log(save_name, code, strlen(code));
    save_sound_count++;
#endif

    printf("response is %s\n", code);

    *text = code;
}

/*
 * 获取socket文件描述符
 */
int get_socket_fd(char *host)
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

    // STRUCT TIMEVAL TV;
    // TV.TV_SEC = 0;
    // TV.TV_USEC = 500000;

    // STRUCT TIMEVAL SEND_TV;
    // SEND_TV.TV_SEC = 0;
    // SEND_TV.TV_USEC = 500000;

    // setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    // setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &send_tv, sizeof(send_tv));

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR connecting");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

/*
 * 更新token
 */
void updateTokenValue(const char *token)
{
    int size = sizeof(tulingUser->token);
    memset(tulingUser->token, 0, size);
    snprintf(tulingUser->token, size, "%s", token);
}
static int aiwifi_download_and_play_mp3(const char *url, FUNC cb)
{
    if (g_aiwifi_play == 0)
    {
        pr("停止播放音频，不需要播放\n");
        cb(NULL);
        return 1;
    }
    //播放下载文件
    voice_add_play_ai(url, cb);
    return 1;
}
#if 0
static int aiwifi_download_and_play_mp3(const char *url, FUNC cb)
{
    char ai_download_file[128] = {0};
    char *p1 = NULL;
    int ret = 1;
    int mode = ufo_check_work_mode();
    p1 = strrchr(url, '/');
    if (p1 == NULL)
    {
        pr("get download file name error!\n");
        return -1;
    }
    if (_AI_VOICE_MODE != mode && _AI_VOICE_KEY_MODE != mode)
    {
        pr(" not ai_voice_mode return:%d!\n", mode);
        return -1;
    }
    pr("p1=%s\n", p1);

    memset(ai_download_file, 0, sizeof(ai_download_file));
    sprintf(ai_download_file, "/tmp%s", p1);
    pr("download file name is %s\n", ai_download_file);

    //下载mp3到设备
    ret = sccGetHttp((char *)url, ai_download_file, 200);
    if (0 == ret)
    {
        ret = sccGetHttp((char *)url, ai_download_file, 200);
    }

    if (0 == ret)
    {
        //20.mp3 提示音
        pr("net error!\n");
        return -1;
    }
    mode = ufo_check_work_mode();
    if (_AI_VOICE_MODE != mode && _AI_VOICE_KEY_MODE != mode)
    {
        pr(" not ai_voice_mode return:%d!\n", mode);
        remove(ai_download_file);
        return -1;
    }
    yc_interaction_func(STATU_DOWNLOAD_MP3_FINISHED);
    //播放下载文件
    if(_AI_VOICE_MODE == mode){
        voice_add_play_ai(ai_download_file, cb);
    }
    else{
        voice_add_play_ai(ai_download_file, NULL);
    }
    
    return 1;
}
#endif
void stop_aiwifi_download_and_play()
{
    if (g_aiwifi_play)
    {
        g_aiwifi_play = 0;
    }
}

/*******************************************
@函数功能:	json解析服务器数据
@参数:	pMsg	服务器数据
***********************************************/
static void parseJson_string(const char *pMsg, char *aiwifi_token, FUNC cb)
{
    char aiwifi_tip[512] = {0};
    char aiwifi_url[512] = {0};
    char aiwifi_nlp[512] = {0};
    int array_size, iCnt;
    // pthread_t aiwifi_download_and_play_tid;
    if (NULL == pMsg)
    {
        return;
    }
    cJSON *pJson = cJSON_Parse(pMsg);
    if (NULL == pJson)
    {
        return;
    }
    cJSON *pSub = cJSON_GetObjectItem(pJson, "token"); //获取token的值，用于下一次请求时上传的校验值
    if (pSub != NULL)
    {
        printf("TokenValue = %s\n", pSub->valuestring);
        //updateTokenValue(pSub->valuestring);
        int size = sizeof(aiwifi_token);
        memset(aiwifi_token, 0, size);
        snprintf(aiwifi_token, "%s", pSub->valuestring);
    }
    pSub = cJSON_GetObjectItem(pJson, "code");
    if (NULL == pSub)
    {
        goto exit;
    }

    if(20007 == pSub->valueint || 20008 == pSub->valueint)
    {
       aiwifi_is_story_sony = 1;
    }
    else
    {
         aiwifi_is_story_sony = 0;
    }
    
    switch (pSub->valueint)
    {
    case 20001:
        //standby control
        if (cJSON_HasObjectItem(pJson, "nlp"))
        {
            cJSON *item = cJSON_GetObjectItem(pJson, "nlp");
            cJSON *subitem = cJSON_GetArrayItem(item, 0);
            printf("%s:nlp is   %s\n", __func__, subitem->valuestring);
            strcpy(aiwifi_nlp, subitem->valuestring);
            //TODO:进入待机模式下的接口：go_into_standby_status
            aiwifi_download_and_play_mp3(aiwifi_nlp, go_into_ufo_mode(_NONE_MODE));
        }
        break;
    case 20002:
        if (cJSON_HasObjectItem(pJson, "nlp"))
        {
            cJSON *item = cJSON_GetObjectItem(pJson, "nlp");
            cJSON *subitem = cJSON_GetArrayItem(item, 0);
            printf("%s:nlp is   %s\n", __func__, subitem->valuestring);
            strcpy(aiwifi_nlp, subitem->valuestring);
            //TODO:进入待机模式下的接口：go_into_standby_status
            aiwifi_download_and_play_mp3(aiwifi_nlp, cb);
        }
        if (cJSON_HasObjectItem(pJson, "func"))
        {
            cJSON *func = cJSON_GetObjectItem(pJson, "func");
            if (cJSON_HasObjectItem(func, "operate"))
            {
                cJSON *operate = cJSON_GetObjectItem(func, "operate");
                if (cJSON_HasObjectItem(func, "volumn"))
                {
                    if (operate != NULL)
                    {
                        if (operate->valueint == 1)
                        {
                            voice_set(12);
                        }
                        else
                        {
                            voice_set(1);
                        }
                    }
                }
                else if (operate != NULL)
                {
                    if (operate->valueint == 1)
                    {
                        voice_up();
                    }
                    else
                    {
                        voice_down();
                    }
                }
            }
        }
        //volume contro;
        break;
    case 20011: //animals sound
    case 20019: //musical instrument sound
    case 20020: //natural sound

        if (cJSON_HasObjectItem(pJson, "func"))
        {
            cJSON *func = cJSON_GetObjectItem(pJson, "func");
            if (cJSON_HasObjectItem(func, "url"))
            {
                cJSON *url = cJSON_GetObjectItem(func, "url");
                printf("%s:url is   %s\n", __func__, url->valuestring);
                strcpy(aiwifi_url, url->valuestring);
                aiwifi_download_and_play_mp3(aiwifi_url, cb);
            }
        }
        else if (cJSON_HasObjectItem(pJson, "nlp"))
        {
            cJSON *item = cJSON_GetObjectItem(pJson, "nlp");
            cJSON *subitem = cJSON_GetArrayItem(item, 0);
            printf("%s:nlp is   %s\n", __func__, subitem->valuestring);
            strcpy(aiwifi_nlp, subitem->valuestring);
            aiwifi_download_and_play_mp3(aiwifi_nlp, NULL);
        }
        break;
    case 20030:
        //itoem study
        if (cJSON_HasObjectItem(pJson, "func"))
        {
            cJSON *func = cJSON_GetObjectItem(pJson, "func");
            if (func == NULL)
            {
                break;
            }
            cJSON *results_array = cJSON_GetObjectItem(func, "results");
            if (results_array)
            {
                cJSON *results_list = results_array->child;
                int results_list_cnt = 0;

                while (results_list != NULL)
                {
                    cJSON *values = cJSON_GetObjectItem(results_list, "values");
                    if (values)
                    {
                        printf("%s:%d valule = %s\n", __func__, __LINE__, values->valuestring);
                        cJSON *ttsUrl_array = cJSON_GetObjectItem(values, "ttsUrl");
                        if (ttsUrl_array)
                        {
                            array_size = cJSON_GetArraySize(ttsUrl_array);
                            for (iCnt = 0; iCnt < array_size; iCnt++)
                            {
                                cJSON *ttsUrl_array_sub = cJSON_GetArrayItem(ttsUrl_array, iCnt);
                                if (NULL == ttsUrl_array_sub)
                                {
                                    continue;
                                }
                                printf("%s:ttsUrl[%d] is   %s\n", __func__, iCnt, ttsUrl_array_sub->valuestring);
                                strcpy(aiwifi_url, ttsUrl_array_sub->valuestring);
                                if (results_list_cnt == 2)
                                {
                                    aiwifi_download_and_play_mp3(aiwifi_url, cb);
                                }
                                else
                                {
                                    aiwifi_download_and_play_mp3(aiwifi_url, NULL);
                                }
                            }
                        }
                    }
                    results_list = results_list->next;
                    results_list_cnt++;
                    if (results_list_cnt == 3)
                    {
                        break;
                    }
                }
            }
        }
        break;
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
    case 20034:
        //English word study
        if (cJSON_HasObjectItem(pJson, "nlp"))
        {
            cJSON *item = cJSON_GetObjectItem(pJson, "nlp");
            if (item != NULL)
            {
                array_size = cJSON_GetArraySize(item);
                for (iCnt = 0; iCnt < array_size; iCnt++)
                {
                    cJSON *subitem = cJSON_GetArrayItem(item, iCnt);
                    if (NULL == subitem)
                    {
                        continue;
                    }
                    printf("%s:nlp[%d] is   %s\n", __func__, iCnt, subitem->valuestring);
                    strcpy(aiwifi_nlp, subitem->valuestring);
                    if (iCnt == (array_size - 1))
                    {
                        aiwifi_download_and_play_mp3(aiwifi_nlp, cb);
                    }
                    else
                    {
                        aiwifi_download_and_play_mp3(aiwifi_nlp, NULL);
                    }
                }
            }
        }
        goto exit;
    default:
        //normal AI talking
        pSub = cJSON_GetObjectItem(pJson, "asr");
        if (NULL == pSub)
        {
            cb();
            break;
        }
        printf("info: %s \n", pSub->valuestring);
        if (strstr(pSub->valuestring, "打开绘本") != NULL || strstr(pSub->valuestring, "看绘本") != NULL || strstr(pSub->valuestring, "读绘本") != NULL)
        {
            pr("go into recognize mode\n");
            go_into_ufo_mode(_RECOGNIZE_MODE);
            break;
        }
        if (strstr(pSub->valuestring, "开台灯") != NULL)
        {
            pr("即将打开台灯......\n");
#ifdef YC_ALD
            aladdin_ctrl_led(1);
#endif
            break;
        }
        pSub = cJSON_GetObjectItem(pJson, "tts");
        if (NULL != pSub)
        {
            printf("tts: %s \n", pSub->valuestring);
        }
        if (!strcmp(pSub->valuestring, "90001"))
        {
            //go into recognize mode
            go_into_ufo_mode(_RECOGNIZE_MODE);
            break;
        }
        if (cJSON_HasObjectItem(pJson, "func"))
        {
            cJSON *func = cJSON_GetObjectItem(pJson, "func");
            if (cJSON_HasObjectItem(func, "url") && cJSON_HasObjectItem(func, "tip"))
            {
                cJSON *url = cJSON_GetObjectItem(func, "url");
                cJSON *tip = cJSON_GetObjectItem(func, "tip");
                //printf("%s:tip is %s =============url is   %s\n", __func__, tip->valuestring, url->valuestring);
                pr("s =============url is   %s\n", url->valuestring);
                strcpy(aiwifi_url, url->valuestring);
                strcpy(aiwifi_tip, tip->valuestring);
            }
        }
        if (cJSON_HasObjectItem(pJson, "nlp") && strlen(aiwifi_url) == 0)
        {
            cJSON *item = cJSON_GetObjectItem(pJson, "nlp");
            cJSON *subitem = cJSON_GetArrayItem(item, 0);
            printf("%s:nlp is   %s\n", __func__, subitem->valuestring);
            strcpy(aiwifi_nlp, subitem->valuestring);
        }

        if (!strlen(aiwifi_nlp) /* && !strlen(aiwifi_tip) */ && !strlen(aiwifi_url))
        {
            printf("三个url都为空，继续语音侦测\n");
            cb();
            break;
        }
        if (strlen(aiwifi_nlp))
        {
            if (!strlen(aiwifi_url))
            {
                aiwifi_download_and_play_mp3(aiwifi_nlp, cb);
            }
            else
            {
                aiwifi_download_and_play_mp3(aiwifi_nlp, NULL);
            }
        }
        // if (strlen(aiwifi_tip))
        // {
        //     aiwifi_download_and_play_mp3(aiwifi_tip, NULL);
        // }
        if (strlen(aiwifi_url))
        {
            aiwifi_download_and_play_mp3(aiwifi_url, cb);
        }
        break;
    }
exit:
    if (pJson)
    {
        cJSON_Delete(pJson);
        pJson = NULL;
    }
}
/*
 * 读取文件
 */
int readFile(const char *file_name, char **file_data, int *length)
{
    if (access(file_name, F_OK) != 0)
    {
        printf("file %s not exsit!\n", file_name);
        return -1;
    }
    FILE *fp = fopen(file_name, "rb");
    if (fp == NULL)
    {
        printf("open file %s error!\n", file_name);
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
void requestAndResponse(int socket_fd, char *file_data, int len, int asr_type, int realtime, int index, char *identify, char *host, char *aiwifi_token, FUNC cb)
{
    char *text = NULL;
    buildRequest(socket_fd, file_data, len, asr_type, realtime, index, identify, host);
    long start_time = getCurrentTime();
    printf("Send finish at %ld.\r\n", start_time);
    getResponse(socket_fd, &text);
    long end_time = getCurrentTime();
    printf("Response finish at %ld.\r\n", end_time);
    printf("耗时：%ld\n", end_time - start_time);
    if (text)
    {
        parseJson_string((const char *)text, aiwifi_token, cb);
        free(text);
    }
    else
    {
        printf("req failed\n");
        cb();
    }
}

int InitTuling(const char *userId, const char *aes_key, const char *api_key, const char *token)
{
    tulingUser = (TulingUser_t *)calloc(1, sizeof(TulingUser_t));
    if (tulingUser == NULL)
    {
        return -1;
    }
    memcpy(tulingUser->user_id, userId, strlen(userId));
    memcpy(tulingUser->aes_key, aes_key, strlen(aes_key));
    memcpy(tulingUser->api_key, api_key, strlen(api_key));
    memcpy(tulingUser->token, token, strlen(token));
    return 0;
}
void DestoryTuling(void)
{
    if (tulingUser)
    {
        free(tulingUser);
        tulingUser = NULL;
    }
}
int aiwifi(const char *file_name, int asr)
{
    aiwifi_callback(file_name, asr, notify_ai_voice);
}
int aiwifi_callback(const char *file_name, int asr, FUNC cb)
{
    printf("%s start............\n", __func__);
    char *user_id = DEVICE_ID;
    char *aes_key = AES_KEY;
    char *api_key = API_KEY;
    InitTuling(user_id, aes_key, api_key, aiwifi_token);

    //asr_type 0:pcm_16K_16bit; 1:pcm_8K_16bit; 2:amr_8K_16bit; 3:amr_16K_16bit; 4:opus
    //int asr_type = atoi(argv[2]);
    int asr_type = asr;

    char *host = "smartdevice.ai.tuling123.com";
    g_aiwifi_play = 1;
#ifndef FILE_SEND
    //遍历目录，读取所有文件

    const char *dir_name = file_name;
    DIR *dirp;
    struct dirent *dp;
run:
    dirp = opendir(dir_name); //打开目录指针
    while ((dp = readdir(dirp)) != NULL)
    { //通过目录指针读目录
        printf("filename = %s\n", dp->d_name);
        printf("filetype = %d\n", dp->d_type);
        if (dp->d_type == 4)
            continue;
        int sockfd = get_socket_fd(host);
        char *filename = malloc(strlen(dir_name) + strlen(dp->d_name) + 1);
        memset(filename, 0, strlen(dir_name) + strlen(dp->d_name) + 1);
        sprintf(filename, "%s%s", dir_name, dp->d_name);
        printf("final filename = %s\n", filename);
        char *fileData = NULL;
        int len = 0;
        readFile(filename, &fileData, &len);
        requestAndResponse(sockfd, fileData, len, asr_type, 0, 0, NULL, host, aiwifi_token, cb);
        free(fileData);
        free(filename);
        close(sockfd);
        sleep(1);
    }
    (void)closedir(dirp); //关闭目录
    goto run;
#else
    //读取一个文件
    //char *file_name= argv[1];
    char *fileData = NULL;
    int len = 0;
    if (readFile(file_name, &fileData, &len) != 0)
    {
        DestoryTuling();
        cb();
        return -1;
    }

#ifdef YC_DEBUG
    char save_name[64] = {0};
    system("cp /tmp/aiwifi/yc_aiwifi_upload.amr /tmp/aiwifi/yc_aiwifi.amr");
    sprintf(save_name, "/tmp/aiwifi/sound_send_%d.amr", save_sound_count);
    rename("/tmp/aiwifi/yc_aiwifi.amr", save_name);
    sync();
#endif

    int sockfd = get_socket_fd(host);
    if(sockfd < 0){
        pr("get sockfd error\n");
        free(fileData);
        DestoryTuling();
        cb();
        return -1;
    }

    //单个包的大小
    int size = 320 * 1024;
    if (len > size)
    {
        //流式识别
        int piece = (len % size != 0) ? (len / size + 1) : (len / size);
        char identify[20] = {0};
        get_rand_str(identify, 19);

        int i = 0;
        for (i = 0; i < piece; i++)
        {
            requestAndResponse(sockfd, fileData + size * i, (i == (piece - 1)) ? (len - size * i) : size, asr_type, 1, (i == (piece - 1)) ? (-i - 1) : (i + 1), identify, host, aiwifi_token, cb);
        }
    }
    else
    {
        //非流式识别
        requestAndResponse(sockfd, fileData, len, asr_type, 0, 0, NULL, host, aiwifi_token, cb);
    }

#endif
    
    close(sockfd);
    free(fileData);
    DestoryTuling();
    printf("%s exit............\n", __func__);
    return 0;
}

int ufo_check_aiwifi_is_story_sony()
{
    pr("aiwifi_is_story_sony : %d\n", aiwifi_is_story_sony);
    if (aiwifi_is_story_sony)
    {
        return 1;
    }
    return 0;
}

void next_story_sony()
{
    if (0 == ufo_check_aiwifi_is_story_sony())
        return;

    voice_stop_play();

    if (!access(VR_UPLOAD_FILE, F_OK))
    {
        aiwifi(VR_UPLOAD_FILE, 2);
    }
    else
    {
        pr("VR_UPLOAD_FILE is NULL!\n");
    }
}

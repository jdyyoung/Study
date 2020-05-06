//#include "cmd_util.h"
#include "mbedtls/sha256.h"
#include "mbedtls/base64.h"
#include "cjson/cJSON.h"
#include "net/HTTPClient/HTTPCUsr_api.h"

#include "yc_xr872_rec.h"

// #define TEST_SERVER
// #define PRINT_LONG_BUF
#define SAVE_READING_DATA
#define VERSION "YC_REC_LIB_V19122500"
enum YC_LOG_LEVEL{
	L_ERROR   = 0,
	L_WARN    = 1,
	L_INFO    = 2,
	L_DEBUG   = 3,
	L_VERBOSE = 4,
};

enum _REC_SELECT{
	JUDGE_MODE = 0,
	SEARCH_MODE = 1,
};
#define LOG_LEVEL 1
#define LOGCOM(level,x,y...)		\
do{									\
	if(level <= LOG_LEVEL) printf(x,##y);  \
}while(0)

#define LOGE(x, y...)   LOGCOM( L_ERROR,  x, ##y)
#define LOGW(x, y...)   LOGCOM( L_WARN,   x, ##y)
#define LOGI(x, y...)   LOGCOM( L_INFO,   x, ##y)
#define LOGD(x, y...)   LOGCOM( L_DEBUG,  x, ##y)
#define LOGV(x, y...)   LOGCOM( L_VERBOSE,x, ##y)

#define pr(x, y...)    LOGCOM( L_DEBUG, "\n[%s:%d] " x,__func__,__LINE__,##y)
#define LOG(x, y...)    LOGI(x, ##y)

#define ONLY_UPDATE_YC_TOKEN 1
#define UPDATE_ALL_YC_PARM   0

#define CANDIDATES_SIZE 10240
#define YC_PLAY_INDEX_MAX 256
#define META_SIZE_MAX     512

const char *deviceName = "ycmz0011";

#ifdef TEST_SERVER
char* server_domain = "http://pgc-test.mocn.cc/m-server/api/user/thirdAuth";
char* get_yc_mp3_domain = "http://pgc-test.mocn.cc/m-server/api/tools/getMusicDataUrl";
char* reading_data_domain = "http://bm-test.mocn.cc/bookMall-h5/api/common/addBookReadData";
#else
char* server_domain = "http://pgc.mocn.cc/m-server/api/user/thirdAuth";
char* get_yc_mp3_domain = "http://pgc.mocn.cc/m-server/api/tools/getMusicDataUrl";
char* reading_data_domain = "http://bm.mocn.cc/bookMall-h5/api/common/addBookReadData";
#endif

typedef struct g_rec_parm_t{
	unsigned long update_token_timestamp;
	auth_parm_t auth_parm;
	char yc_token[9];
	char app_key[33];
	char recognize_domain[128];
    char SHA_key[129];
	char candidates[CANDIDATES_SIZE];
	char sign_candidates[CANDIDATES_SIZE];
	char last_isbn[16];
	char play_list[YC_PLAY_INDEX_MAX][64];
	char play_pre[128];
	int play_page;

	char isbn_page[20];
	char music_url[256];
	char meta[META_SIZE_MAX];

#ifdef SAVE_READING_DATA
	uint8_t recognize_start_flag;
	char last_book_num[16]; //保存上次识别到书号
	char uploda_info[1024];    //保存需要上传的书号
	long recognize_start_time;
#endif
}g_rec_parm_t;

static g_rec_parm_t *g_rec = NULL;
static yc_malloc_t yc_malloc;
static yc_free_t yc_free;

#ifdef PRINT_LONG_BUF
static void print_long_buf(char* long_buf){
	char* p1 = NULL;
	char pr_buf[513] = {0};
	int size = (strlen(long_buf) / 512);
	printf("long_buf is:");
	uint8_t i;
	for (i=0;i< size;i++){
		p1 = long_buf+ i*512;
		memset(pr_buf,0,513);
		strncpy(pr_buf,p1,512);
		printf("%s\n",pr_buf);
	}
	printf("%s\n",long_buf+i*512);
}
#endif

static void* yc_http_header(){
	const char* yc_http_header = "Content-Type: application/json; charset=UTF-8\r\nConnection: Keep-Alive";
	return (void*)yc_http_header;
}

static void* yc_http_get_token_header(){
	const char* yc_http_header = "Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\nConnection: Keep-Alive";
	return (void*)yc_http_header;
}

static int HTTPC_post_test(HTTPParameters *clientParams, char *credentials,uint32_t credentials_size,HTTP_CLIENT_GET_HEADER http_user_header_cb)
{
	int ret = -1;
	int total_len  =0;
	unsigned int toReadLength = 10240;//4096;
	unsigned int Received = 0;
	char *buf = yc_malloc(toReadLength);
	if (buf == NULL){
		LOGE("yc_malloc pbuffer failed..\n");
		return ret;
	}
		
	HTTP_CLIENT httpClient;
	memset((void *)&httpClient, 0, sizeof(httpClient));

	// memset(buf, 0, toReadLength);
	clientParams->HttpVerb = VerbPost;
	clientParams->pData = credentials;
	// memcpy(buf, credentials, strlen(credentials));
	clientParams->pLength = strlen(credentials);
	clientParams->nTimeout = 3; //1s timeout
request:
	if ((ret = HTTPC_open(clientParams)) != 0) {
		LOGE("http open err..\n");
		goto relese;
	}
	if ((ret = HTTPC_request(clientParams, http_user_header_cb)) != 0) {
		LOGE("http request err..,ret = %d\n",ret);
		goto relese;
	}
	if ((ret = HTTPC_get_request_info(clientParams, &httpClient)) != 0) {
		LOGE("http get request info err..,ret = %d\n",ret);
		goto relese;
	}
	if (httpClient.HTTPStatusCode != HTTP_STATUS_OK) {
		if((httpClient.HTTPStatusCode == HTTP_STATUS_OBJECT_MOVED) ||
				(httpClient.HTTPStatusCode == HTTP_STATUS_OBJECT_MOVED_PERMANENTLY)) {
			LOGD("Redirect url..\n");
			HTTPC_close(clientParams);
			memset(clientParams, 0, sizeof(*clientParams));
			clientParams->HttpVerb = VerbGet;
			if (httpClient.RedirectUrl->nLength < sizeof(clientParams->Uri))
				strncpy(clientParams->Uri, httpClient.RedirectUrl->pParam,
						httpClient.RedirectUrl->nLength);
			else
				goto relese;
			LOGD("go to request.\n");
			goto request;

		} else {
			LOGE("httpClient.HTTPStatusCode = %d\n",(int)httpClient.HTTPStatusCode);
			ret = httpClient.HTTPStatusCode;
			LOGD("get result not correct.\n");
			goto relese;
		}
	}
	if (httpClient.TotalResponseBodyLength != 0 || (httpClient.HttpFlags & HTTP_CLIENT_FLAG_CHUNKED )) {
		memset(credentials, 0, strlen(credentials));
		do {
			memset(buf, 0, toReadLength);
			if ((ret = HTTPC_read(clientParams, buf, toReadLength, (void *)&Received)) != 0) {
				//LOGD("get data,Received:%d\n",Received);
				if (ret == 1000) {
					ret = 0;
					// TODO:打印buf只有1K
					if(strlen(buf) < 1000){
						LOGD("%s  --- The end..\n",buf);
					}
					if(0 != strlen(buf)){
						//防止内存访问越界，比较长度大小
						// memset(credentials, 0, strlen(credentials));
						if(strlen(buf) > credentials_size ){
							LOGE("memcpy dest no enough\n");
							goto relese;
						}
						memcpy(credentials+total_len,buf,Received);
						total_len+=Received;
					}
				} else{
					LOGE("Transfer err...ret:%d\n",ret);
				}
				// printf("-------total_len= %d---------------\n",total_len);
				break;
			} else {
				LOGD("get data,Received:%d,ret = %d,strlen(buf)=%d\n",Received,ret,strlen(buf));
				LOGD("get data buf = %s\n",buf);
				// memset(credentials, 0, strlen(credentials));
				if(strlen(buf) > credentials_size ){
					LOGE("memcpy dest no enough\n");
					goto relese;
				}
				memcpy(credentials+total_len,buf,Received);
				total_len+=Received;
			}
		} while (1);
	}
relese:
	yc_free(buf);
	buf = NULL;
	HTTPC_close(clientParams);
	return ret;
}

static int httpc_exec_post(char *url,char* request_data,uint32_t post_buf_size,HTTP_CLIENT_GET_HEADER http_user_header_cb)
{
	int ret = 1;
	HTTPParameters *clientParams;
	clientParams = yc_malloc(sizeof(*clientParams));
	if (!clientParams) {
		LOGE(" yc_malloc clientParams is NULL\n");
		return ret;
	} else {
		memset(clientParams, 0, sizeof(HTTPParameters));
	}

	strcpy(clientParams->Uri, url);
	if ((ret = HTTPC_post_test(clientParams, request_data,post_buf_size,http_user_header_cb)) != 0){
		LOGE("HTTPC_post_test error\n");
		goto releaseParams;
	}
	ret = 0;
releaseParams:
	if (clientParams != NULL) {
		yc_free(clientParams);
		clientParams = NULL;
	}
	return ret;
}

static void yc_str_remove(char *s1, char *s2,char* result){
    char *p1, *p2;
    p2 = s1;
    while ((p1 = strstr(s1, s2)) != NULL)
    {
    strncat(result, s1, p1 - p2);
    strcpy(s1, p1 + strlen(s2));
    p2 = s1;
    }
    strcat(result, s1);
}

static uint8_t get_yc_recognize_mp3(){
	uint8_t ret = 1;
	//10k 约等于50页（11329）,设置最多页：150页，约等于36K
	uint32_t post_buf_size = 36 * 1024;

	char* post_buf = NULL;
    int page = 0, index = 0;
    char isbn[16] = {0}, list[256] = {0};
    char *p1 = NULL,*p2 = NULL,*p3 = NULL,*p4 = NULL,*ptmp = NULL;

	p1 = g_rec->isbn_page;
	p2 = strchr(p1, '_');
	if (p2 == NULL){
		LOGE("%d:strchr ret NULL\n",__LINE__);
		return YC_STATUS_STR_PARSE_NULL_126;
	}
	strncpy(isbn, p1, p2 - p1);
    p2++;
    page = strtol(p2, NULL, 10);
    LOGD("查到--- isbn:%s page:%d\n", isbn, page);

	//isbn当前识别到的书号，download_file上次识别到的书号:同本书，同一页
    if (0 == strcmp(isbn, g_rec->last_isbn) && page == g_rec->play_page){
        LOGD("文件名一样，播放页一样，不播音频,下载名:%s----当前页：%d---上一页：%d\n", g_rec->last_isbn, page, g_rec->play_page);
		memset(g_rec->music_url,0,256);
		sprintf(g_rec->music_url, "%s%s", g_rec->play_pre, g_rec->play_list[page]);
		// *music_url = g_rec->music_url;
        return YC_STATUS_OK;
    }

	//不同图书，需要重新拉取书号
	if (strcmp(isbn, g_rec->last_isbn)){
        if (strlen(g_rec->last_isbn)){
            memset(g_rec->last_isbn, 0, sizeof(g_rec->last_isbn));
            memset(g_rec->play_list, 0, sizeof(g_rec->play_list));
            memset(g_rec->play_pre, 0, sizeof(g_rec->play_pre));
        }

		//10K buf:response len > 8k
		post_buf = yc_malloc(post_buf_size);
		if(NULL == post_buf){
			LOGE("get music url malloc post_buf = NULL\n");
			return YC_STATUS_MEM_ALLOC_FAIL_123;
		}
		else{
			memset(post_buf,0,post_buf_size);
		}

		//isbn=9787539780856C&token=xecng4ep
		sprintf(post_buf, "isbn=%s&token=%s", isbn,g_rec->yc_token);
		if ((ret = httpc_exec_post(get_yc_mp3_domain, post_buf,post_buf_size,yc_http_get_token_header)) != 0){
			ret = YC_STATUS_HTTP_POST_ERR_124;
			goto releaseParams;
		}
        // if (!recognize_run_flag){
		// 	ret = 1;//TODO:主动退出识别码
		// 	goto releaseParams;
        // }

		p1 = strstr(post_buf, "\"targetIdList\":[\"");
		if(p1 == NULL){
			memset(g_rec->candidates, 0, 10240);
		}
		else{
			p1 += strlen("\"targetIdList\":");
			p2 = strchr(p1, ']');
			p2++;
			memset(g_rec->candidates, 0, 10240);
			strncpy(g_rec->candidates, p1, p2 - p1);

			char* tmp_candidates  = yc_malloc(CANDIDATES_SIZE);
			if(NULL == tmp_candidates){
				LOGE(" malloc tmp_candidates = NULL\n");
				ret = YC_STATUS_MEM_ALLOC_FAIL_123;
				goto releaseParams;
			}
			else{
				memset(tmp_candidates, 0, CANDIDATES_SIZE);
			}
			strcpy(tmp_candidates, g_rec->candidates);
			memset(g_rec->sign_candidates, 0, CANDIDATES_SIZE);
			yc_str_remove(tmp_candidates, "\"",g_rec->sign_candidates);

			yc_free(tmp_candidates);
			tmp_candidates = NULL;
		}

        p1 = strstr(post_buf, "\"splitMusicUrl\":\"");
        if (p1 != NULL){
            p1 += strlen("\"splitMusicUrl\":\"");
            p2 = strchr(p1, '\"');
            if (p2 != NULL){
                strncpy(g_rec->play_pre, p1, p2 - p1);
				LOGD("流媒体前缀：%s\n", g_rec->play_pre);
            }
        }
		else{
			LOGE("get mp3 url err,response data no splitMusicUrl string\n");
			ret = YC_STATUS_STR_PARSE_NULL_126;
			goto releaseParams;
		}

		//填充play_list
        ptmp = post_buf;
        while (1){
            p1 = strstr((char *)ptmp, "\"endTime\":");
            p2 = strstr((char *)ptmp, "\"startTime\":");
            if (p1 == NULL || p2 == NULL){
				break;
			}
            p1 += strlen("\"endTime\":");
            p2 += strlen("\"startTime\":");

            p3 = strstr(p1, "\"subMusicUrl\":\"");
            char subMusicUrl[128] = {0};
            if (p3 != NULL){
                p3 += strlen("\"subMusicUrl\":\"");
                p4 = strchr(p3, '\"');
                if (p4 != NULL){
                    strncpy(subMusicUrl, p3, p4 - p3);
                    LOGD("截取到url:%s\n", subMusicUrl);
                }
            }
            memset(list, 0, sizeof(list));
            if (p2 > p1){
                strncpy(list, p1, p2 - p1);
                ptmp = p2;
            }
            else{
                strncpy(list, p2, p1 - p2);
                ptmp = p1;
            }

            p1 = list;
            while (1){
                p2 = strchr(p1, '_');
                if (p2 == NULL){
					 break;
				}

                p2++;
                index = strtol(p2, NULL, 10);
                // LOGD("page index:%d\n", index);
                if (index < YC_PLAY_INDEX_MAX){
                    if (strlen(subMusicUrl)){
                        strcpy((char *)g_rec->play_list[index], subMusicUrl);
                    }
                }
                p1 = p2;
            }
        }

		//重新拉书的音频列表，置-1
        g_rec->play_page = -1;
        strcpy(g_rec->last_isbn, isbn);
        LOGD("g_rec->last_isbn:%s\n", g_rec->last_isbn);
    }

	if (strlen(g_rec->last_isbn)){
        g_rec->play_page = page;
        if (strlen(g_rec->play_pre)){
            if (strlen(g_rec->play_list[page])){
				memset(g_rec->music_url,0,256);
				sprintf(g_rec->music_url, "%s%s", g_rec->play_pre, g_rec->play_list[page]);
				// *music_url = g_rec->music_url;
				LOGD("play music url is %s\n",g_rec->music_url);
			}
        }
    }

	ret = YC_STATUS_OK;
releaseParams:
	if(post_buf){
		yc_free(post_buf);
		post_buf = NULL;
	}
	return ret;
}

/*
 * 获取随机字符串
 */
static void get_rand_str(char s[], int number)
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

//yc token 30min 有效，进入绘本识别模式更新一次
//account=yangcheng&pwd=yangcheng10160406&devicemode=YC01&deviceserial=BY02304A26AF109F&version=19092600
static uint8_t get_yc_token(uint8_t only_update_token){
	uint8_t ret = 1;
	uint32_t post_buf_size = 1024;
	//1K buf
	char* post_buf = yc_malloc(post_buf_size);
	if(NULL == post_buf){
		LOGE("yc_token 申请内存post_buf = NULL\n");
		return YC_STATUS_MEM_ALLOC_FAIL_123;
	}
	else{
		memset(post_buf,0,post_buf_size);
		// pr("post_buf is %p\n",post_buf);
	}
	char rand_num[9] = {0};
	char signature[65] = {0};
	uint8_t sha256_out[32];
	get_rand_str(rand_num,sizeof(rand_num)-1);
	mbedtls_sha256_context ctx2;
	mbedtls_sha256_init(&ctx2);
    mbedtls_sha256_starts(&ctx2, 0); /* SHA-256, not 224 */
	/* Simulating multiple fragments */
    mbedtls_sha256_update(&ctx2, (unsigned char*)g_rec->auth_parm.api_key, strlen(g_rec->auth_parm.api_key));
	mbedtls_sha256_update(&ctx2, (unsigned char*)rand_num, strlen(rand_num));
	mbedtls_sha256_update(&ctx2, (unsigned char*)g_rec->auth_parm.api_secret, strlen(g_rec->auth_parm.api_secret));
    mbedtls_sha256_finish(&ctx2, sha256_out);
	mbedtls_sha256_free(&ctx2);
	for (size_t i = 0; i < sizeof(sha256_out); i++){
		sprintf(signature+i*2,"%02x", sha256_out[i]);
	}
	sprintf(post_buf, "apiKey=%s&nonceStr=%s&sign=%s&deviceMode=%s&mac=%s&version=%s&sn=%s", g_rec->auth_parm.api_key,rand_num, signature,g_rec->auth_parm.device_mode,g_rec->auth_parm.mac,g_rec->auth_parm.version,g_rec->auth_parm.sn);
	LOGD("post_buf =%s\n",post_buf);
	if ((ret=httpc_exec_post(server_domain,post_buf,post_buf_size,yc_http_get_token_header)) != 0){
		ret = YC_STATUS_HTTP_POST_ERR_124;
		goto releaseParams;
	}
	// printf("&deviceMode=%s&mac=%s&sn=%s\n", g_rec->auth_parm.device_mode,g_rec->auth_parm.mac,g_rec->auth_parm.sn);
	//更新token
	cJSON* pJson = cJSON_Parse(post_buf);
	if(NULL == pJson){
		LOGE("[%s]\n", cJSON_GetErrorPtr());
		ret = YC_STATUS_JSON_PARSE_ERR_125;
		goto releaseParams;
	}
	cJSON *code = cJSON_GetObjectItem(pJson, "code");
	if (NULL == code || code->valueint != 1000){
		LOGE("update token error code != 1000\n");
		ret = YC_STATUS_JSON_PARSE_ERR_125;
		goto releasseJson;
    }
	if (!cJSON_HasObjectItem(pJson, "data")){
        LOGE("data key no exist\n");
		ret = YC_STATUS_JSON_PARSE_ERR_125;
        goto releasseJson;
    }
    cJSON *data = cJSON_GetObjectItem(pJson, "data");
	if (!cJSON_HasObjectItem(data, "token")){
        LOGE("token key no exist\n");
		ret = YC_STATUS_JSON_PARSE_ERR_125;
        goto releasseJson;
    }
	else{
		cJSON *token = cJSON_GetObjectItem(data, "token");
		// LOGD("token is %s\n",token->valuestring);
		memset(g_rec->yc_token,0,strlen(token->valuestring));
		strncpy(g_rec->yc_token,token->valuestring,strlen(token->valuestring));

		char* p1 = strstr(post_buf, "\"timestamp\":");
		if (p1 == NULL){
			LOGE("timestamp key no exit !\n");
			ret = YC_STATUS_STR_PARSE_NULL_126;
			goto releasseJson;
		}

		p1 += strlen("\"timestamp\":");
		char* p2 = strchr(p1, '}');
		if (p2 == NULL){
			LOGE("No timestamp !\n");
			ret = YC_STATUS_STR_PARSE_NULL_126;
			goto releasseJson;
		}
		char tm[16] = {0};
		//-3:去掉usec
		strncpy(tm, p1, p2 - p1 - 3);
		g_rec->update_token_timestamp = strtol(tm, NULL, 0);
		struct timeval set_time;
		set_time.tv_sec = g_rec ->update_token_timestamp;
		set_time.tv_usec = 0;
		settimeofday(&set_time,NULL);
	}
	//更新各项参数
	if(!only_update_token){
		cJSON *rserver = cJSON_GetObjectItem(data, "rserver");
		unsigned char base64_out[512] = {0};
		size_t base64_outlen;
		mbedtls_base64_decode( base64_out, 512, &base64_outlen,(unsigned char*)rserver->valuestring,strlen(rserver->valuestring));
		// LOGD("rserver = %s ,base64_outlen = %d\n",base64_out,base64_outlen);
		if(0 == base64_outlen){
			LOGE("yc_token base64_decode fial:base64_outlen = 0\n");
			ret = YC_STATUS_BASE64_ERR_127;
			goto releasseJson;
		}
		char* p1 = (char*)base64_out;
		char* p2 = strchr(p1, '@');
		if(NULL == p2){
			LOGE("strchr return NULL %d\n",__LINE__);
			ret = YC_STATUS_STR_PARSE_NULL_126;
			goto releasseJson;
		}
		strncpy(g_rec->app_key, p1, p2 - p1);
		p1 = p2 + 1;
		p2 = strchr(p1, '@');
		if(NULL == p2){
			LOGE("strchr return NULL %d\n",__LINE__);
			ret = YC_STATUS_STR_PARSE_NULL_126;
			goto releasseJson;
		}
		strncpy(g_rec->recognize_domain, p1, p2 - p1);
		p1 = p2 + 1;
		strcpy(g_rec->SHA_key, p1);
		if (strlen(g_rec->app_key) == 0 || strlen(g_rec->recognize_domain) == 0 || strlen(g_rec->SHA_key) == 0)
		{
			LOGE("strchr return NULL %d\n",__LINE__);
			ret = YC_STATUS_STR_PARSE_NULL_126;
			goto releasseJson;
		}
		LOGD("app_key = %s;recognize_domain = %s;SHA_key = %s\n",g_rec->app_key,g_rec->recognize_domain,g_rec->SHA_key);
	}
	ret = YC_STATUS_OK ;
releasseJson:
	if (pJson){
		cJSON_Delete(pJson);
		pJson = NULL;
	}
releaseParams:
	if(post_buf){
		yc_free(post_buf);
		post_buf =NULL;
	}
	return ret;
}

// static void *yc_memcpy(void *dest, const void *src, size_t count)
// {
// 	char *tmp = dest;
// 	const char *s = src;

// 	while (count--)
// 	*tmp++ = *s++ ;

// 	return dest;
// }

static int yc_recognize(uint8_t* pic_data,uint32_t data_len,enum _REC_SELECT mode){
	int ret = 1;
	uint8_t sha256_out[32];
	char signature[65] = {0};
	char timestamp[16] = {0};
	char* p1 = NULL;

	//判断时间戳差大于30 min，去更新token
	struct timeval cur_time;
	gettimeofday(&cur_time,NULL);
	if((cur_time.tv_sec - g_rec->update_token_timestamp) > 1800){
		ret = get_yc_token(ONLY_UPDATE_YC_TOKEN);
		if(ret != YC_STATUS_OK){
			return ret;
		}
	}
	sprintf(timestamp, "%lu000", cur_time.tv_sec);

	//buf 40K:picture 20K ,base64_encode => 20K*2
	uint8_t* base64_dst = yc_malloc(data_len*2);
	if(NULL == base64_dst){
		LOGE("yc_recognize  malloc buf fail\n");
		return YC_STATUS_MEM_ALLOC_FAIL_123;
	}
	memset(base64_dst,0,data_len*2);

	size_t base64_outlen;
	mbedtls_base64_encode( base64_dst, data_len *2, &base64_outlen, pic_data, data_len);
	if(0 == base64_outlen){
			LOGE("yc_recognize base64_encode fial:base64_outlen = 0\n");
			yc_free(base64_dst);
			base64_dst = NULL;
			return YC_STATUS_BASE64_ERR_127;
	}
	else{
		LOGD("----data_len = %d------pic_data base64_outlen=%d------------\n",data_len,base64_outlen);
	}
	mbedtls_sha256_context ctx2;
	mbedtls_sha256_init(&ctx2);
    mbedtls_sha256_starts(&ctx2, 0); /* SHA-256, not 224 */
	/* Simulating multiple fragments */
	mbedtls_sha256_update(&ctx2, (unsigned char*)"appKey", strlen("appKey"));
    mbedtls_sha256_update(&ctx2, (unsigned char*)g_rec->app_key, strlen(g_rec->app_key));
	if(mode == JUDGE_MODE){
		mbedtls_sha256_update(&ctx2, (unsigned char*)"candidates", strlen("candidates"));
    	mbedtls_sha256_update(&ctx2, (unsigned char*)g_rec->sign_candidates, strlen(g_rec->sign_candidates));
	}
	mbedtls_sha256_update(&ctx2, (unsigned char*)"deviceName", strlen("deviceName"));
	mbedtls_sha256_update(&ctx2, (unsigned char*)deviceName, strlen(deviceName));
	mbedtls_sha256_update(&ctx2, (unsigned char*)"image", strlen("image"));
	mbedtls_sha256_update(&ctx2, base64_dst, base64_outlen);
	mbedtls_sha256_update(&ctx2, (unsigned char*)"notrackingtrue", strlen("notrackingtrue"));
	mbedtls_sha256_update(&ctx2, (unsigned char*)"timestamp", strlen("timestamp"));
	mbedtls_sha256_update(&ctx2, (unsigned char*)timestamp, strlen(timestamp));
	mbedtls_sha256_update(&ctx2, (unsigned char*)g_rec->SHA_key, strlen(g_rec->SHA_key));
    mbedtls_sha256_finish(&ctx2, sha256_out);
	mbedtls_sha256_free(&ctx2);
	
	for (size_t i = 0; i < sizeof(sha256_out); i++){
		sprintf(signature+i*2,"%02x", sha256_out[i]);
	}
	// CMD_LOG("signature = %s\r\n",(char*)signature);

	size_t size = data_len *2 + 512;
	char* send_buf = yc_malloc(size);
	if(NULL == send_buf){
		LOGE("yc_malloc send_buf fail\n");
		yc_free(base64_dst);
		base64_dst = NULL;
		return YC_STATUS_MEM_ALLOC_FAIL_123;
	}

	memset(send_buf, 0, size);
    strcpy(send_buf, "{\"appKey\":\"");
    strcat(send_buf, g_rec->app_key);

	if(mode == JUDGE_MODE){
		strcat(send_buf, "\",\"candidates\":");
    	strcat(send_buf, g_rec->candidates);
		strcat(send_buf, ",\"image\":\"");
	}
	else{
		strcat(send_buf, "\",\"image\":\"");
	}
    size = strlen(send_buf);
	// uint8_t i;
	// for(i=0;i<(base64_outlen/1024);i++){
	// 	memcpy(send_buf+size+i*1024,base64_dst+i*1024,1024);
	// }
	// memcpy(send_buf+size+i*1024,base64_dst+i*1024,base64_outlen%1024);
    // memmove(send_buf + size, base64_dst, base64_outlen);
	// strcat(send_buf,(char*)base64_dst);
	// sprintf(send_buf + size,"%s",base64_dst);
	// strcpy(send_buf + size,(char*)base64_dst);
	if(base64_outlen >(data_len *2)){
		LOGE("memory error\n");
		ret = YC_STATUS_MEM_ALLOC_FAIL_123;
		yc_free(base64_dst);
		base64_dst = NULL;
		goto releaseParams;
	}
	memcpy(send_buf+size,base64_dst,base64_outlen);

    strcat(send_buf, "\",\"notracking\":\"true");
    strcat(send_buf, "\",\"signature\":\"");
    strcat(send_buf, signature);
    strcat(send_buf, "\",\"timestamp\":\"");
    strcat(send_buf, timestamp);
    strcat(send_buf, "\",\"deviceName\":\"");
    strcat(send_buf, deviceName);
    strcat(send_buf, "\"}");

#ifdef PRINT_LONG_BUF
	print_long_buf(send_buf);
#endif

	if(base64_dst){
		yc_free(base64_dst);
		base64_dst = NULL;
	}
	
	if(mode == JUDGE_MODE){
		p1 = strstr(g_rec->recognize_domain,"search");
		if(p1 == NULL){
			LOGD("recognize_domain no search string\n");
		}
		else{
			strcpy(p1,"judge");
		}
	}
	else{
		p1 = strstr(g_rec->recognize_domain,"judge");
		if(p1 == NULL){
			LOGD("recognize_domain no judge string\n");
		}
		else{
			strcpy(p1,"search");
		}
	}
	LOGD("g_rec->recognize_domain is %s\n",g_rec->recognize_domain);
	ret = httpc_exec_post(g_rec->recognize_domain,send_buf,(data_len *2 + 512),yc_http_header);
	if( ret != 0){
		LOGE("httpc_exec_post error,ret = %d\n",ret);
		if( 404 == ret){
			ret = YC_STATUS_404_NO_REC_PIC_128;
		}else{
			ret = YC_STATUS_HTTP_POST_ERR_124;
		}
		goto releaseParams;
	}
	
	cJSON* pJson = cJSON_Parse((char*)send_buf);
	if(NULL == pJson){
		LOGE("[%s]\n", cJSON_GetErrorPtr());
		ret = YC_STATUS_JSON_PARSE_ERR_125;
		goto releaseParams;
	}

	cJSON *cjson_code = cJSON_GetObjectItem(pJson, "statusCode");
	if (NULL == cjson_code || cjson_code->valueint != 0){
		if(NULL == cjson_code){
			LOGE("JSON:statusCode_key no exist\n");
		}
		else{
			LOGE("statusCode = %d\n",cjson_code->valueint);
		}
		if(strlen(send_buf) < 1024){
			printf("recognize_domain response is:%s\n",send_buf);
		}
		else{
			LOGE("strlen(send_buf) > 1024\n");
		}
		ret = YC_STATUS_JSON_PARSE_ERR_125;
		goto releasseJson;
    }
	if (!cJSON_HasObjectItem(pJson, "result")){
        LOGE("JSON:result_key no exist\n");
		if(strlen(send_buf) <1024){
			printf("recognize_domain response is:%s\n",send_buf);
		}
		else{
			LOGE("strlen(send_buf) > 1024\n");
		}
		ret = YC_STATUS_JSON_PARSE_ERR_125;
        goto releasseJson;
    }
    cJSON *result = cJSON_GetObjectItem(pJson, "result");
	if (!cJSON_HasObjectItem(result, "target")){
        LOGE("JSON:target_key no exist\n");
		if(strlen(send_buf) < 1024){
			printf("recognize_domain response is:%s\n",send_buf);
		}
		else{
			LOGE("strlen(send_buf) > 1024\n");
		}
		ret = YC_STATUS_JSON_PARSE_ERR_125;
        goto releasseJson; 
    }
	else{
		cJSON *target = cJSON_GetObjectItem(result, "target");
		cJSON *name = cJSON_GetObjectItem(target, "name");
		strcpy(g_rec->isbn_page,name->valuestring);
		cJSON *meta = cJSON_GetObjectItem(target, "meta");
		strcpy(g_rec->meta,meta->valuestring);
		LOGD("查到:isbn_page:%s,meta:%s\n",name->valuestring,meta->valuestring);
	}
	ret = YC_STATUS_OK;
releasseJson:
	if (pJson){
		cJSON_Delete(pJson);
		pJson = NULL;
	}
releaseParams:
	if(send_buf){
		yc_free(send_buf);
		send_buf = NULL;
	}
	return ret;
}

int yc_recognize_init(auth_parm_t* yc_auth,yc_ops_t* ops_parm){
	int ret = 1;
	printf("\n---------%s:build@%s_%s-------\n",VERSION, __DATE__, __TIME__);
	if(yc_auth == NULL ||ops_parm == NULL){
		LOGE("传入参数为NULL,yc_rec初始化失败\n");
		return YC_STATUS_IN_PARM_ERR_120;
	}
	else{
		if(ops_parm->yc_plat_malloc == NULL || ops_parm->yc_plat_free == NULL){
			LOGE("传入参数ops_parm的malloc或free为NULL,yc_rec初始化失败\n");
			return YC_STATUS_IN_PARM_ERR_120;
		}
	}
	if(g_rec != NULL){
		LOGE("yc_rec 已经初始化过了，请勿重复初始化\n");
		return YC_STATUS_REPEAT_INIT_121;
	}
	yc_malloc = ops_parm->yc_plat_malloc;
	yc_free = ops_parm->yc_plat_free;

	g_rec = (g_rec_parm_t*)yc_malloc(sizeof(g_rec_parm_t));
	if(NULL == g_rec){
		LOGE("yc_rec 初始化申请内存失败\n");
		return YC_STATUS_MEM_ALLOC_FAIL_123;
	}
	else{
		// printf("g_rec address is %p\n",g_rec);
		memset(g_rec,0,sizeof(g_rec_parm_t));
		memcpy(&g_rec->auth_parm,(void*)yc_auth,sizeof(auth_parm_t));
		//认证并更新各项参数
		ret = get_yc_token(UPDATE_ALL_YC_PARM);
		if(ret != 0){
			yc_free(g_rec);
			g_rec = NULL;
		}
	}
	return ret;
}

#ifdef SAVE_READING_DATA
static int yc_recognize_upload_data(uint8_t is_exit){
    int ret = 1;
	uint32_t reading_time;
	struct timeval cur_time;
	gettimeofday(&cur_time, NULL);

	if(is_exit){
		if( strlen(g_rec->uploda_info) == 0 || (cur_time.tv_sec - g_rec->recognize_start_time) < 120){
			LOGD("阅读时间不到2min，不上传数据\n");
			return 1;
		}
	}
	else{
		if(strlen(g_rec->uploda_info) < 1000){
			LOGD("上传数据buf不达标，不上传数据\n");
			return 1;
		}
	}

	uint32_t post_buf_size = 1088;
	char* post_buf = yc_malloc(post_buf_size);
	if(NULL == post_buf){
		LOGE("yc_token 申请内存post_buf = NULL\n");
		return YC_STATUS_MEM_ALLOC_FAIL_123;
	}
	else{
		
		memset(post_buf,0,post_buf_size);
	}
	reading_time = (cur_time.tv_sec - g_rec->recognize_start_time) % 60 ? (cur_time.tv_sec - g_rec->recognize_start_time) / 60 +1:(cur_time.tv_sec - g_rec->recognize_start_time) / 6;
	LOGD("reading time is %d mins\n",reading_time);
    sprintf(post_buf, "openId=%s&readMinute=%d&bookId=%s", g_rec->auth_parm.sn, reading_time,g_rec->uploda_info);
    LOGD("上传数据内容 is : %s\n", post_buf);
	if ((ret=httpc_exec_post(reading_data_domain,post_buf,post_buf_size,yc_http_get_token_header)) != 0){
		LOGD("回传阅读数据失败\n");
		ret = YC_STATUS_HTTP_POST_ERR_124;
		goto exit;
	}
	else{
		LOGD("上传阅读数据成功\n");
		ret = YC_STATUS_OK;
	}
exit:
	//上传完清空相应数据
	memset(g_rec->uploda_info,0,sizeof(g_rec->uploda_info));
	memset(g_rec->last_book_num,0,sizeof(g_rec->last_book_num));
	g_rec->recognize_start_flag = 0;
	g_rec->recognize_start_time = 0;
	if(post_buf){
		yc_free(post_buf);
		post_buf = NULL;
	}
    return ret;
}

static void yc_recognize_save_data(){
    char *p1 = NULL, *p2 = NULL;
    char book_num[16] = {0}; //保存当前识别到书号
	struct timeval cur_time;

    p1 = g_rec->isbn_page;
    p2 = strchr(p1, '_');
    if (p2 == NULL){
        pr("get isbn error !\n");
        return;
    }
    strncpy(book_num, p1, p2 - p1);

	if(!strcmp(g_rec->last_book_num, book_num)){
		LOGD("识别的书本号与上次相同\n");
		return;
	}

    if (g_rec->recognize_start_flag == 0){
        g_rec->recognize_start_flag = 1;
		gettimeofday(&cur_time, NULL);
        g_rec->recognize_start_time = cur_time.tv_sec;
        LOGD("数据上传：识别开始计时时间：%ld\n", g_rec->recognize_start_time);
    }


    if (!strlen(g_rec->last_book_num)){
        sprintf(g_rec->uploda_info, "%s", book_num);
    }
    else{
        sprintf(g_rec->uploda_info, "%s:%s", g_rec->uploda_info, book_num);
    }
	
    strcpy(g_rec->last_book_num, book_num);
    LOGD("uploda_info：%s\n", g_rec->uploda_info);
}
#endif

// uint8_t yc_recognize_run(yc_rec_handle_t* rec_handler,uint8_t* jpg_data,uint32_t data_len,char** music_url){
uint8_t yc_recognize_run(uint8_t* jpg_data,uint32_t data_len,out_info_t* out_data){
	uint32_t ret = 1;
	if(NULL == jpg_data || data_len <= 0){
		LOGE("jpg_data is NULL or data_len <=0\n");
		return YC_STATUS_IN_PARM_ERR_120;
	}
	if(out_data == NULL){
		LOGE("out_data is NULL \n");
		return YC_STATUS_IN_PARM_ERR_120;
	}
	if(NULL == g_rec){
		LOGE("yc_rec 未初始化,请先调用yc_recognize_init()\n");
		return YC_STATUS_NO_INIT_122;
	}
	memset(g_rec->music_url,0,256);
	out_data->music_url = g_rec->music_url;
	memset(g_rec->meta,0,META_SIZE_MAX);
	out_data->meta = g_rec->meta;
	memset(g_rec->isbn_page,0,20);
	out_data->isbn_page = g_rec->isbn_page;

	if (strlen(g_rec->candidates))
    {
		ret = yc_recognize(jpg_data,data_len,JUDGE_MODE);
		if(ret != 0){
			ret = yc_recognize(jpg_data,data_len,SEARCH_MODE);
		}
    }
    else
    {
		ret = yc_recognize(jpg_data,data_len,SEARCH_MODE);
    }
	if(ret != 0){
		goto exit;
	}

#ifdef SAVE_READING_DATA
    yc_recognize_save_data();
	yc_recognize_upload_data(0);
#endif

	ret = get_yc_recognize_mp3();
	if(ret != 0){
		memset(g_rec->music_url,0,256);
	}
exit:
	return ret;
}

uint8_t yc_get_recognize_run_status(){
	return 0;
}

void yc_recognize_deinit(){
#ifdef SAVE_READING_DATA
	yc_recognize_upload_data(1);
#endif
	if(g_rec){
		yc_free(g_rec);
		g_rec = NULL;
	}
}


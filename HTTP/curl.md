

```C
/*
Camera Event Report
POST URL: /v1/camera/event/report
Content-Type : multipart/form-data
Authorization : accessTokenRedbee Company Confidential 2020
Request JSON example:
{
    "eventImage":"image file",
    "eventType":1,
    "eventTime":1591953011313,
}
eventImage: The first frame file.
eventType: 1---Ring button, 2---Motion detected, 3---Liveview, 4---Alarm
eventTime : Timestamp
Response JSON example:
{
    "result": true,
    "error": "",
    "data": {
        "reportId":"33c1568d80904e9daa5e230b852cff99AC"
    }
}
*/

/*
	curl --location --request POST 'http://120.78.224.26:8080/DBC05/v1/camera/event/report' 
	--header 'Authorization: eyJhbGciOiJIUzI1NiIsIlR5cGUiOiJKd3QiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJyZWRiZWUiLCJleHAiOjE2MDgyNjkwMDQsInR5cGUiOiJBY2Nlc3NUb2tlbiIsImRldmljZUlkIjoiZmE3M2ZjZjY0YzQzNDE1NTlkYjEwYzYyMGY5OTljMzUifQ.kR8A7PKO9P8Oz39W8lKySH4rjp2-5iUQX01ylyouCJY' 
	--form 'eventImage=@"snap.jpg"' --form 'eventType="1"' --form 'eventTime=1591953011313'
*/
int event_push_report(int type)
{
	int ret = 0;
	int seconds = time((time_t*)NULL);

	CURL *curl;
	CURLcode res;
	struct curl_slist *hs=NULL;
	struct MemoryStruct chunk;
	char* authString = NULL;
	cJSON* cjson_test = NULL;
	cJSON* cjson_item = NULL;
	cJSON* cjson_temp = NULL;
	char* json_str = NULL;


	DBUG(0,"event_push_report type = %d !\n",type);

	curl = curl_easy_init();
	if(!curl) {
		WARN(0,"curl == NULL\n");
		ret = -1;
		goto _exit;
	}

	chunk.memory = malloc(1);
	chunk.size = 0;

	char url[256] = {0};
	sprintf(url, "%s%s", CURRENT_BACKEND, "v1/camera/event/report");
	curl_easy_setopt(curl, CURLOPT_URL, url);
	
	authString = (char*)malloc(strlen("Authorization: ") +1 +strlen(d3_kvs.access_token));
	sprintf(authString, "Authorization: %s", d3_kvs.access_token);

	struct curl_httppost* formpost = NULL;
	struct curl_httppost* formlast = NULL;

	char type_str[128] ={0};
	sprintf(type_str,"%d",type);
	char seconds_str[128] ={0};
	sprintf(seconds_str,"%d",seconds);

	curl_formadd(&formpost, &formlast, CURLFORM_COPYNAME, "eventImage",CURLFORM_FILE, "/mnt/diskc/Event.jpg", CURLFORM_END);
	// curl_formadd(&formpost, &formlast, CURLFORM_COPYNAME, "msg",CURLFORM_COPYCONTENTS, json_str,CURLFORM_END);
	curl_formadd(&formpost, &formlast, CURLFORM_COPYNAME, "eventType",CURLFORM_PTRCONTENTS, type_str,CURLFORM_END);
	curl_formadd(&formpost, &formlast, CURLFORM_COPYNAME, "eventTime",CURLFORM_PTRCONTENTS, seconds_str,CURLFORM_END);

	/* Set the form info */
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
	//curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
	// hs = curl_slist_append(hs, "Content-Type : multipart/form-data");
	hs = curl_slist_append(hs, authString);
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	curl_easy_setopt(curl,CURLOPT_TIMEOUT, 60);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);
	// curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

	res = curl_easy_perform(curl);
	ret = res;
	event_report_result = false;
	if(res != CURLE_OK){
		WARN(0, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
		goto _exit;	
	}
	else {
		DBUG(1,"%lu bytes retrieved\n", (unsigned long)chunk.size);
		ALERT(0,"chunk.memory:\n%s\n\n", chunk.memory);	
	}
	
	cjson_test=cJSON_Parse( chunk.memory);
	if (!cjson_test){
		WARN(0,"cjson_test == NULL! \n");
		ret = -1;
		goto _exit;
	}
		
	cjson_item = cJSON_GetObjectItem(cjson_test, "data");
	if(!cjson_item){
		WARN(0,"response JSON no data item\n");
		ret = -1;
		goto _exit;
	}
	cjson_item = cJSON_GetObjectItem(cjson_item, "reportId");
	if(!cjson_item){
		WARN(0,"response JSON no reportId item\n");
		ret = -1;
		goto _exit;
	}
	// DBUG(1,"reportId = %s\n", cjson_item->valuestring);
	strncpy(g_report_id,  cjson_item->valuestring,sizeof(g_report_id));
	event_report_result = true;
	
	_exit:
		if(chunk.memory){
			free(chunk.memory);
			chunk.memory = NULL;
		}		
		if(cjson_test){
			cJSON_Delete(cjson_test);
			cjson_test = NULL;
		}
		if(authString){
			free(authString);
			authString = NULL;
		}
		if(formpost){
			curl_formfree(formpost);
		}
		if(curl){
			curl_easy_cleanup(curl);
		}
		if(json_str){
			free(json_str);
			json_str = NULL;
		}

	return ret;
}
```



```C
/*
Camera Avatar
POST URL: /v1/camera/avatar
Content-Type : multipart/form-data
Authorization : accessToken
Request JSON example:
{
	"avatarImage":"image file",
} 
avatarImage: The camera image file.
Response JSON example:
{
    "result": true,
    "error": "",
    "data": { 
    	"reportId":"33c1568d80904e9daa5e230b852cff99AC"
	}
}
*/
static int avatar_image_upload(const char* image_file)
{
	CURL *curl;
	CURLcode res;
	struct curl_slist *hs=NULL;
	struct MemoryStruct chunk;
	// char* str = NULL;
	char* authString = NULL;

	curl = curl_easy_init();
	if( ! curl) {
		WARN(0,"curl == NULL\n");
		return -1;
	}
	
	chunk.memory = (char*)malloc(1);
	chunk.size = 0;
	
	char url[256] ={0};
	sprintf(url, "%s%s", CURRENT_BACKEND, "v1/camera/avatar");
	curl_easy_setopt(curl, CURLOPT_URL, url);
		
	authString = malloc(strlen("Authorization: ") +1 +strlen(d3_kvs.access_token));
	sprintf(authString, "Authorization: %s", d3_kvs.access_token);

	struct curl_httppost* formpost = NULL;
	struct curl_httppost* formlast = NULL;

	/* Add simple file section */
	curl_formadd(&formpost, &formlast, CURLFORM_COPYNAME, "avatarImage",CURLFORM_FILE, image_file, CURLFORM_END);

	/* Set the form info */
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
	hs = curl_slist_append(hs, authString);
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	curl_easy_setopt(curl,CURLOPT_TIMEOUT, 60);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

	res = curl_easy_perform(curl);
	if(res != CURLE_OK){
		WARN(0, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));	
	}
	else {
		DBUG(0,"%lu bytes retrieved\n", (unsigned long)chunk.size);			
		DBUG(0,"\n%s\n\n", chunk.memory);
	}
	free(chunk.memory);
	curl_formfree(formpost);
	curl_easy_cleanup(curl);
	free(authString);

	return res;
}

```



---

20210913：

Libcurl实现断点续传 - chang290 - 博客园 - https://www.cnblogs.com/chang290/archive/2012/08/12/2634858.html

HTTP多线程下载+断点续传（libcurl库） - 立超的专栏 - 博客园 - https://www.cnblogs.com/zlcxbb/p/6006861.html

curl_easy_setopt-curl库的关键函数之一 - DoubleLi - 博客园 - https://www.cnblogs.com/lidabo/p/4583067.html

使用libcurl遇到的坑_Swallow_he的博客-CSDN博客 - https://blog.csdn.net/Swallow_he/article/details/86648555

libcurl使用easy模式阻塞卡死等问题的完美解决 - Bigben - 博客园 - https://www.cnblogs.com/bigben0123/p/3192978.html

```
设置curl_easy_setopt 的第二个参数为CURLOPT_RANGE，第三个参数为“X-Y”格式的字串，其中X为开始下载的字节，Y为结束下载的字节。
例如，如果一个文件你已经下载了1000个字节，总长度为105200，
如果你想接着下载，你就应该设置第三个参数为“1000-105200”，就可以实现从1000字节开始下载
```



---

```C
//多文件变成，使用一个全局变量curl_ready
BOOL curl_ready = FALSE; 
static int curl_system_post_event(char* payload,char *client_id,char *hmac_signature){
  //  char url[MAX_PATHNAME*2];
    CURLcode res;
    CURL *curl = NULL;
    struct curl_slist *headers = NULL;
    char buf[520];
    cJSON *response = NULL;
    int update_flag=0;
    //    int active = 0;
    HTTPResponse_t chunk = {NULL, 0};


    if(!curl_ready){
        WARN(0, "libcurl has not been initialized!, doing that now");
		curl_ready = curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK ? TRUE : FALSE;
        if (!curl_ready){
        	goto error;
        }
     }

    DBUG(2, "%s() -- POSTING EVENT with payload '%s'\n", __FUNCTION__, payload);

    chunk.data = malloc(1);
    if(!chunk.data){
        ERROR(0, "failed to alloc mem for response");
        goto error;
    }
    chunk.size = 0;

    /* get a curl handle */
    curl = curl_easy_init();
    if(!curl){
        ERROR(0, "failed to obtain curl handle");
        goto error;
    }
    
    if(cfg.stage){
        if(clientid_alarm_dot_com() && !strncmp(cfg.event_uri,CFG_DEFAULT_EVENT_URI_PROD,strlen(CFG_DEFAULT_EVENT_URI_PROD))){
            memset(cfg.event_uri,0,sizeof(cfg.event_uri));
            memcpy(cfg.event_uri,CFG_DEFAULT_EVENT_URI_STAGE,sizeof(CFG_DEFAULT_EVENT_URI_STAGE));    
            
        }else if(clientid_alarm_dot_com() && !strncmp(cfg.event_uri,CFG_DEFAULT_EVENT_URI_PROD,strlen(CFG_DEFAULT_EVENT_URI_PROD))){
            memset(cfg.event_uri,0,sizeof(cfg.event_uri));
            memcpy(cfg.event_uri,CFG_DEFAULT_EVENT_URI_RESIDEO,sizeof(CFG_DEFAULT_EVENT_URI_RESIDEO));
            
        }
    }

    if(update_flag){
        int rc = cfg_write_if_dirty(&cfg); 
        if(rc == 0)
            DBUG(1, "event_uri updated in system settings\n");
    }

    snprintf(buf, sizeof(buf), cfg.event_uri);
	/*
        snprintf(curl_cmd, sizeof(curl_cmd), 
                 "curl -X POST -L -o %s --upload-file %s --cacert /firmware/ca-bundle.crt -v"
                 " -H \"Authorization: Bearer %s\""
                 " -H \"x-skybell-client-id %s\""
                 " -H \"x-skybell-signature %s\""
                 " -H \"Content-Type: application/json\""
                 " -H \"Cache-Control:no-cache\" \"%s\" > /dev/null 2>&1", 
                 response_filename, json_filename, skybell_access_token(), client_id, hmac_signature, url);
	*/

    DBUG(1, "%s: url=[%s]\n", __FUNCTION__, buf);

    /* complete within 60 seconds */
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
    curl_easy_setopt(curl, CURLOPT_URL, buf);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen((char *)payload));
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "/firmware/ca-bundle.crt");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    headers = curl_slist_append(headers, "Content-Type: application/json");
    sprintf(buf,"x-skybell-client-id: %s",client_id);
    headers = curl_slist_append(headers, buf);
    if ((hmac_signature != NULL) && (memcmp(hmac_signature,"null",4)) ) {
      sprintf(buf,"x-skybell-signature: %s",hmac_signature);
      headers = curl_slist_append(headers, buf);
    };
    headers = curl_slist_append(headers, "charsets: utf-8");
    headers = curl_slist_append(headers, "cache-control: no-cache");
    snprintf(buf, sizeof(buf), "Authorization: Bearer %s", skybell_access_token());

    headers = curl_slist_append(headers, buf);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    /* set callback on receiving data */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_response_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

    /* perform the request (this is a blocking call) */
    res = curl_easy_perform(curl);
    if(res != CURLE_OK){
        ERROR(0, "failed to perform request: %s", curl_easy_strerror(res));
        goto error;
    }
    else{
        //printf("response[%lu]: \n%s\n", (long)chunk.size, chunk.data);
        response = cJSON_Parse((const char*)chunk.data);
        if(!response){
            ERROR(0, "could not parse response");
            goto error;
        }
    }

    /* cleanup curl */
    free(chunk.data);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    /* cleanup cJSON */
    cJSON_Delete(response);
    return 0;

error:
    if(response){
        cJSON_Delete(response);
    }
    if(chunk.data){
        free(chunk.data);
    }
    if(headers){
        curl_slist_free_all(headers);
    }
    if(curl){
        curl_easy_cleanup(curl);
    }
    return -1;
}
```



```C
#include <curl/curl.h>

CURLcode res;
res = curl_global_init(CURL_GLOBAL_DEFAULT);
/* Check for errors */
if(res != CURLE_OK) {
    ERROR(0, "curl_global_init() failed: %s\n",
    curl_easy_strerror(res));
    return 1;
}
while(1){
    sleep(1);
}

curl_global_cleanup();
```

20210817:

CURLOPT_ERRORBUFFER - https://curl.se/libcurl/c/CURLOPT_ERRORBUFFER.html

```C
curl = curl_easy_init();
if(curl) {
  CURLcode res;
  char errbuf[CURL_ERROR_SIZE];
 
  curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
 
  /* provide a buffer to store errors in */
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
 
  /* set the error buffer as empty before performing a request */
  errbuf[0] = 0;
 
  /* perform the request */
  res = curl_easy_perform(curl);
 
  /* if the request did not complete correctly, show the error
  information. if no detailed error information was written to errbuf
  show the more generic information from curl_easy_strerror instead.
  */
  if(res != CURLE_OK) {
    size_t len = strlen(errbuf);
    fprintf(stderr, "\nlibcurl: (%d) ", res);
    if(len)
      fprintf(stderr, "%s%s", errbuf,
              ((errbuf[len - 1] != '\n') ? "\n" : ""));
    else
      fprintf(stderr, "%s\n", curl_easy_strerror(res));
  }
}
```


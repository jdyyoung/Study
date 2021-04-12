

表单提交的多表提交：

```C
/*curl --location --request POST 'http://120.78.224.26:8080/DBC05/v1/camera/event/report' 
	--header 'Authorization: eyJhbGciOiJIUzI1NiIsIlR5cGUiOiJKd3QiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJyZWRiZWUiLCJleHAiOjE2MDgyNjkwMDQsInR5cGUiOiJBY2Nlc3NUb2tlbiIsImRldmljZUlkIjoiZmE3M2ZjZjY0YzQzNDE1NTlkYjEwYzYyMGY5OTljMzUifQ.kR8A7PKO9P8Oz39W8lKySH4rjp2-5iUQX01ylyouCJY' 
	--form 'eventImage=@"snap.jpg"' --form 'eventType="1"' --form 'eventTime="1591953011313"'
*/
	
	char type_str[128] ={0};
	sprintf(type_str,"%d",type);
	char seconds_str[128] ={0};
	sprintf(seconds_str,"%d",seconds);

	curl_formadd(&formpost, &formlast, CURLFORM_COPYNAME, "eventImage",CURLFORM_FILE, "/mnt/diskc/Event.jpg", CURLFORM_END);
	curl_formadd(&formpost, &formlast, CURLFORM_COPYNAME, "eventType",CURLFORM_PTRCONTENTS, type_str,CURLFORM_END);
	curl_formadd(&formpost, &formlast, CURLFORM_COPYNAME, "eventTime",CURLFORM_PTRCONTENTS, seconds_str,CURLFORM_END);
```



---

libcurl 的curl_formadd 应用：

用“Content-Type : multipart/form-data”  post 文件

libcurl - curl_formadd() - https://curl.se/libcurl/c/curl_formadd.html

(1条消息) libcurl第七课 multipart/formdata表单使用_bingzhang8449的博客-CSDN博客 - https://blog.csdn.net/bingzhang8449/article/details/100964556

(1条消息) 使用CURL库，发送HTTP的POST的文件传输_我是小超斌-CSDN博客 - https://blog.csdn.net/qq_39436605/article/details/85159568

使用libcurl POST数据和上传文件 - DoubleLi - 博客园 - https://www.cnblogs.com/lidabo/p/4159592.html

libcurl  调试打印：

CURLOPT_DEBUGFUNCTION - https://curl.se/libcurl/c/CURLOPT_DEBUGFUNCTION.html



```c
CURL *curl;
	CURLcode code;
	CURLFORMcode formCode;
	long retcode = 0;
	long flen;
	char *slen;
	struct curl_httppost *post=NULL;
	struct curl_httppost *last=NULL;

	FILE *fp = fopen(file_name, "rb"); 
	if (fp == NULL)
	{
		printf("Open file fail!!!\n");
		return -1;
	}

	fseek(fp,0,SEEK_END);
	flen=ftell(fp);
	slen = string_add("%ld",flen);

	curl=curl_easy_init();

	curl_easy_setopt(curl,CURLOPT_URL,"http://www.acloud.com/rest/smart/Upload");

	curl_formadd(&post, &last, CURLFORM_COPYNAME, "key",CURLFORM_COPYCONTENTS, key,CURLFORM_END);
	curl_formadd(&post, &last, CURLFORM_COPYNAME, "parentId", CURLFORM_COPYCONTENTS, "-1",CURLFORM_END);
	curl_formadd(&post, &last, CURLFORM_COPYNAME, "Filename", CURLFORM_FILE, file_name , CURLFORM_END);   
	curl_formadd(&post, &last, CURLFORM_COPYNAME, "fileLength",CURLFORM_COPYCONTENTS, slen,CURLFORM_END);
	curl_formadd(&post, &last, CURLFORM_COPYNAME, "file",CURLFORM_COPYCONTENTS, "file",CURLFORM_END);
	curl_formadd(&post, &last, CURLFORM_COPYNAME, "submit",CURLFORM_COPYCONTENTS, "submit",CURLFORM_END);
			
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
		
	code=curl_easy_perform(curl);
        
```



---


```
curl命令：
-L/--location ：模拟登录

-X/--request POST url

-H/--header

-F/--form

--data-raw
```

curl 的移植，应用，测试

(1条消息) libcurl的API详解__风-CSDN博客 - https://blog.csdn.net/linux1500176279/article/details/78702470

(1条消息) libcurl API介绍及简单编程_哲学天空的博客-CSDN博客 - https://blog.csdn.net/u012278016/article/details/79929952



```C
res = curl_global_init(CURL_GLOBAL_DEFAULT);
	/* Check for errors */
	if(res != CURLE_OK) {
		ERROR(0, "curl_global_init() failed: %s\n",
		curl_easy_strerror(res));
		return 1;
	}

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	char *ptr = realloc(mem->memory, mem->size + realsize + 1); 
	if(ptr == NULL) {
		/* out of memory! */ 
		DBUG(0,"not enough memory (realloc returned NULL)\n");
		return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

int device_online_report(int online)
{
	CURL *curl;
	CURLcode res;
	struct curl_slist *hs=NULL;
	struct MemoryStruct chunk;
	char* str = NULL;
	char* authString = NULL;
	wifi_link_info_t info;
	
	cJSON* cjson_online = NULL;
	cJSON* cjson_online_data = NULL;
	cJSON* cjson_ota_data = NULL;
	
	// TUTK online
	curl = curl_easy_init();
	if(curl) {
		// curl --location --request POST 'http://120.78.224.26:8080/DBC05/v1/camera/online' \
		--header 'Authorization: eyJhbGciOiJIUzI1NiIsIlR5cGUiOiJKd3QiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJyZWRiZWUiLCJleHAiOjE2MDc1ODY1MDgsInR5cGUiOiJBY2Nlc3NUb2tlbiIsImRldmljZUlkIjoiMmU2MWM1YmM0ODM5NDg2MGJlNmZiNmU1ODI3NGUxN2QifQ.bhjtNHmtCQtV-EF1UybEhq0dEDykzJHOKSBdkrtp8ZI' \
		--header 'Content-Type: application/json' \
		--data-raw '{"online":true}'

		chunk.memory = malloc(1);
		chunk.size = 0;
		
		cjson_test = cJSON_CreateObject();
		if(online)
			cJSON_AddTrueToObject(cjson_test, "status");
		else
			cJSON_AddFalseToObject(cjson_test, "status");
		
		if (wifi_get_link_info(&info) == 0) {
			cJSON_AddNumberToObject(cjson_test, "wifiSignalLevel", info.level);
			cJSON_AddNumberToObject(cjson_test, "wifiLinkQuality", info.link);

			appSystemParamterSet(XDC01_SYS_WIFI_LINK_QUALITY, info.link);
			appSystemParamterSet(XDC01_SYS_WIFI_SIGANLE_LEVEL, info.signal);
		}

		str = cJSON_PrintUnformatted(cjson_test);
		DBUG(0,"%s\n", str);
		
		authString = malloc(strlen("Authorization: ") +1 +strlen(d3_kvs.access_token));
		sprintf(authString, "Authorization: %s", d3_kvs.access_token);

		{
			char url[256];
			memset(url, 0x0, sizeof(url));

			sprintf(url, "%s%s", CURRENT_BACKEND, "v1/camera/online");
			curl_easy_setopt(curl, CURLOPT_URL, url);
		}
//		curl_easy_setopt(curl, CURLOPT_URL,
//			"http://47.113.198.190:8080/RedbeeBackend/v1/camera/online");

		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		hs = curl_slist_append(hs, "Content-Type: application/json");
		hs = curl_slist_append(hs, authString);
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
		{
			ERROR(0, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
		}
		else {
			DBUG(0,"%lu bytes retrieved\n", (unsigned long)chunk.size);
			
			DBUG(0,"\n%s\n\n", chunk.memory);
			if(cjson_online = cJSON_Parse(chunk.memory))
			{
				if(cjson_online_data = cJSON_GetObjectItem(cjson_online, "error"))
				{
					if(strlen(cjson_online_data->valuestring) == 0)
					{
						if(0x0 == gDeviceState)
							appSystemParamterSet(XDC01_SYS_SYSTEM_STATU,STATUS_READY);
						service_up = true;
						DBUG(0,"device_online_report sent, service_up.\n");
					}
				}

				if(cjson_online_data = cJSON_GetObjectItem(cjson_online, "data"))
				{
					if(cjson_ota_data = cJSON_GetObjectItem(cjson_online, "version"))
					{
						if(strlen(cjson_online_data->valuestring) > 0)
							strcpy(d3_fw_ota.version, cjson_online_data->valuestring);
					}

					if(cjson_ota_data = cJSON_GetObjectItem(cjson_online, "sha256sum"))
					{
						if(strlen(cjson_online_data->valuestring) > 0)
							strcpy(d3_fw_ota.checksum, cjson_online_data->valuestring);
					}

					if(cjson_ota_data = cJSON_GetObjectItem(cjson_online, "url"))
					{
						if(strlen(cjson_online_data->valuestring) > 0)
						{
							strcpy(d3_fw_ota.url, cjson_online_data->valuestring);
							gProcessOtaBackend = true;

							DBUG(0,"device_online_report OTA request by Redbee backend version=%s checksum=%s url=%s inprogress=%d\n", 
								d3_fw_ota.version, d3_fw_ota.checksum, d3_fw_ota.url, d3_fw_ota.inprogress);
						}
					}
				}
			}
		}
		free(chunk.memory);
		free(authString);
		cJSON_Delete(cjson_online);
		
		curl_easy_cleanup(curl);
		free(str);
	}

	return res;
}

int camera_verify()
{
	CURL *curl;
	CURLcode res;
	struct curl_slist *hs=NULL;
	struct MemoryStruct chunk;
	char* str = NULL;
	char* authString = NULL;
	
	// Camera verify
	curl = curl_easy_init();
	if(curl) {
		// curl --location --request POST 'http://120.78.224.26:8080/DBC05/v1/camera/verify' \
		// --header 'Content-Type: application/json' \
		// --data-raw '{"cameraUid":"JT9WV9A56C1KYZRK111A","cameraSn":"A1AAB00001"}'

		chunk.memory = malloc(1);
		chunk.size = 0;
		
		cjson_test = cJSON_CreateObject();
		cJSON_AddStringToObject(cjson_test, "cameraUid", d3_camera.tutk_uid);
		cJSON_AddStringToObject(cjson_test, "cameraSn", d3_camera.device_sn);

		str = cJSON_PrintUnformatted(cjson_test);
		DBUG(0,"%s\n", str);

		{
			char url[256];
			memset(url, 0x0, sizeof(url));

			sprintf(url, "%s%s", CURRENT_BACKEND, "v1/camera/verify");
			curl_easy_setopt(curl, CURLOPT_URL, url);
		}
//		curl_easy_setopt(curl, CURLOPT_URL,
//				"http://47.113.198.190:8080/RedbeeBackend/v1/camera/verify");

		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		hs = curl_slist_append(hs, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);
		curl_easy_setopt(curl,CURLOPT_TIMEOUT, 60);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
		{
			ERROR(0, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			free(str);
			
			return res;
		}
		else {
			DBUG(0,"%lu bytes retrieved\n", (unsigned long)chunk.size);
			
			DBUG(0,"\n%s\n\n", chunk.memory);
			if(cjson_test = cJSON_Parse(chunk.memory))
			{
				if(cjson_data = cJSON_GetObjectItem(cjson_test, "error"))
				{
					if(strlen(cjson_data->valuestring) > 0)
					{
						free(chunk.memory);
						curl_easy_cleanup(curl);
						free(str);
						
						return -1;
					}
				}

				if(cjson_data = cJSON_GetObjectItem(cjson_test, "data"))
				{
					if(cjson_token = cJSON_GetObjectItem(cjson_data, "registerToken"))
					{
						DBUG(0,"\n%s\n\n", cjson_token->valuestring);
						strcpy(d3_camera.reg_token, cjson_token->valuestring);

						{
							int i=0;
							srand((unsigned int)time(0));
							static char valid[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
							memset(d3_camera.tutk_password, 0x0, sizeof(d3_camera.tutk_password));
							for (i = 0; i < 16; ++i)
							    d3_camera.tutk_password[i] = valid[random() % (ARRAY_SIZE(valid)-1)];

							sprintf(d3_camera.hw_version, "V0%d", SystemParam.HwVersion );
							sprintf(d3_camera.fw_version, "V%d", D3_FWVER );
							strcpy(d3_camera.device_mac, get_mac_addr());
							DBUG(0,"New tutk_password generated: %s\n%s %s %s\n", d3_camera.tutk_password,
								d3_camera.hw_version, d3_camera.fw_version, d3_camera.device_mac);
							
							update_wifi_setting();
						}
						
						write_camera_setting();
					}
				}
			}
		}
		free(chunk.memory);
		curl_easy_cleanup(curl);
		free(str);
		
		return CURLE_OK;
	}
	else
	{
		return -1;
	}
}
```


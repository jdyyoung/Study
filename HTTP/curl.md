

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


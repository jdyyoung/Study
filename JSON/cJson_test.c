#include<stdio.h>
#include<stdlib.h>
#include "cJSON.h"

/*
{
    "uuid":"0123456789",
    "aplist":[
        {
            "ssid":"0"
        },
        {
            "ssid":"1"
        },
        {
            "ssid":"2"
        }
    ]
}
*/

int main(){
	char *msg = NULL;	
    cJSON * root_json=cJSON_CreateObject();   //创建根数据对象	

    cJSON_AddItemToObject(root_json, "uuid", cJSON_CreateString("0123456789"));
	cJSON *jsonArray = cJSON_CreateArray();
    cJSON_AddItemToObject(root_json,"aplist",jsonArray);	

    int i = 0;
	char string[32];
    
    for(i=0;i<3;i++){
        cJSON *ArrayItem0 = cJSON_CreateObject();
		sprintf(string,"%d",i);
	    cJSON_AddStringToObject(ArrayItem0,"ssid",string);
		cJSON_AddItemToArray(jsonArray,ArrayItem0);
    }

	// msg = cJSON_Print(root_json);
	msg = cJSON_PrintUnformatted(root_json);
	

	printf("生成的JSON0:\n%s\n",msg);
    free(msg);
	return 0;
}

#include <stdio.h>
#include "cJSON.h"

#define API_KEY "1d465391260749458b65c27b020207f1"
int main(){
	cJSON* root = cJSON_CreateObject();
	cJSON_AddStringToObject(root,"apiKey",API_KEY);
	cJSON_AddStringToObject(root,"productId","YC_RTV_01");

	cJSON* array = NULL;
	cJSON_AddItemToObject(root,"devices",array=cJSON_CreateArray());

	cJSON* obj = NULL;
	cJSON_AddItemToArray(array,obj=cJSON_CreateObject());
	cJSON_AddStringToObject(obj,"deviceId","aiAA001122334481");
	cJSON_AddStringToObject(obj,"mac","001122334481");

	cJSON_AddItemToArray(array,obj=cJSON_CreateObject());
	cJSON_AddStringToObject(obj,"deviceId","aiAA001122334482");
	cJSON_AddStringToObject(obj,"mac","001122334482");

	char* str_js = cJSON_Print(root);
	cJSON_Delete(root);

	printf("json=\n%s\n",str_js);
	return 1;
}

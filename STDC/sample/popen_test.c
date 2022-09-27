/*
	popen + pidof 判断进程是否存在
*/
#include<stdbool.h>
#include<stdio.h>
#include <errno.h>
#include <string.h>

int main(){
	const char* cmd = "pidof streamer";
	FILE* fp =NULL;
    char buffer[32] = {0};
    int rv = -1;

    fp=popen(cmd,"r");
    if(NULL == fp){
		printf("popen pidof streamer error\n");
        return -1;
    }
	
	
    if( (rv=fread(buffer,1,sizeof(buffer),fp))<0 )
    {
        printf("Read from fp failure:%s\n",strerror(errno));
		pclose(fp); 
        return -2;
    }
    printf("result_buf=%s,strlen =%d\n",buffer,strlen(buffer));
 
    pclose(fp);  
	return 0;
}
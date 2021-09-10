#include<stdio.h>

int main(){
	char* url = "rtsp://192.168.31.115:8554/live";
	char localIp[100] = {0};
	//匹配"rtsp://"开始到“:”结束的字符串，存储到指针中
	sscanf(url, "rtsp://%[^:]", localIp);
	//localIp:192.168.31.115
	printf("localIp:%s\n",localIp);
	return 1;
}
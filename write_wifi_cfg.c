#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/fcntl.h>

#define SSID "~ !@#$%^&*()_+-={}|:;<>?,."
#define PASS "~ !@#$%^&*()_+-={}|:;<>?,."

int main(){
	char buf[144] = {0};
	int fd = open("/tmp/wifi_cfg",O_WRONLY | O_CREAT | O_TRUNC);
	if(fd < 0){
		perror("open");
	}
	sprintf(buf,"%s%s%s%s%s%s","ssid:",SSID,"\n","pass:",PASS,"\n");

	int ret = write(fd,buf,strlen(buf));
	close(fd);
	return 1;
}

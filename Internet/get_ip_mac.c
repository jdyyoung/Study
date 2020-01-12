#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>

/*
ioctl()函数获取本机IP、MAC - gpengtao的专栏 - CSDN博客 - https://blog.csdn.net/gpengtao/article/details/7926403
(1条消息)ioctl获取本地IP和MAC地址 - fengfengdiandia的专栏 - CSDN博客 - https://blog.csdn.net/fengfengdiandia/article/details/9301849
ioctl函数详细说明（网络） - evenness的专栏 - CSDN博客 - https://blog.csdn.net/evenness/article/details/7665970
*/
 
#define DEFAULT_IF			"ens33"
int main()
{
    int sock;
    int res;
    struct ifreq ifr;
 
    sock = socket(AF_INET, SOCK_STREAM, 0);
    strcpy(ifr.ifr_name, DEFAULT_IF);
    res = ioctl(sock, SIOCGIFADDR, &ifr);
 
    printf("IP: %s\n",inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
 
    strcpy(ifr.ifr_name, DEFAULT_IF);
    res = ioctl(sock, SIOCGIFHWADDR, &ifr);
 
    int i;
    char mac[32];
    for(i = 0; i < 6; ++i)
    {
        sprintf(mac + 3*i, "%02x:", (unsigned char)ifr.ifr_hwaddr.sa_data[i]);
    }
    printf("MAC: %s\n",mac);
 
	close(sock);
    return 0;
}

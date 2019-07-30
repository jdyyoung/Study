#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>

#if 0
linux c 实现网络状态检测 - zdg_c的博客 - CSDN博客 - https://blog.csdn.net/zdg_c/article/details/80594095
int net_check()
{

	int net_fd;
	char statue[20] = {0};

	/*
	operstate 当前网络状态。                
	IF_OPER_UNKNOWN,  未知
	IF_OPER_NOTPRESENT,网卡不存在 
	IF_OPER_DOWN,      断线 
	IF_OPER_LOWERLAYERDOWN, 我不知道
	IF_OPER_TESTING,   测试 
	IF_OPER_DORMANT,休眠
	IF_OPER_UP, 在线
	*/
	net_fd = open("/sys/class/net/wlan0/operstate", O_RDONLY); //以只读的方式打开/sys/class/net/wlan0/operstate
	if (net_fd < 0)
	{
		pr("open err\n");
		return -1;
	}

	//pr("open success\n");
	//memset(statue,0,sizeof(statue));
	read(net_fd, statue, 10);
	//pr("statue is %s",statue);
	if (NULL != strstr(statue, "up"))
	{
		//pr("on line\n");
		close(net_fd);
		return 0;
	}
	else if (NULL != strstr(statue, "down"))
	{
		pr("off line\n");
		close(net_fd);
		return -1;
	}
	else
	{
		pr("unknown err\n");
		close(net_fd);
		return -1;
	}
}

int net_check(){
	int net_fd;
	char statue[20] = {0};

	net_fd = open("/sys/class/net/wlan0/wireless/link", O_RDONLY);
	if (net_fd < 0)
	{
		pr("open err\n");
		return -1;
	}
	if(read(net_fd, statue, 10) < 0){
		pr("read err\n");
		close(net_fd);
		return -1;
	}
	pr("statue is %s",statue);
	close(net_fd);
	return atoi(statue);
}
#endif

//#define NET_PORT 53
//#define NET_IP "8.8.8.8" //谷歌DNS
#define NET_PORT 80
//#define NET_IP "www.baidu.com" //百度80端口
#define NET_IP "www.qq.com" //百度80端口

//获取联网状态
int check_net(void)
{
    int fd;
    int in_len=0;
    struct sockaddr_in servaddr;
    struct hostent *host;
    unsigned int inaddr=0l;
    struct in_addr *ipaddr;
    in_len = sizeof(struct sockaddr_in);
    fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd < 0)
    {
        perror("socket");
        return -1;
    }
 
    /*设置默认服务器的信息*/
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(NET_PORT);
    //servaddr.sin_addr.s_addr = inet_addr(NET_IP);
    memset(servaddr.sin_zero,0,sizeof(servaddr.sin_zero));
 
    /*判断是主机名还是ip地址*/
    if( inaddr=inet_addr(NET_IP)==INADDR_NONE)
    {
        if((host=gethostbyname(NET_IP) )==NULL) /*是主机名*/
        {
            perror("gethostbyname error");
            return (-1);
        }
		#ifdef DEBUG
        printf("0IP Address : %s\n",inet_ntoa(*((struct in_addr *)host->h_addr)));
        printf("1IP Address : %s\n",inet_ntoa(*((struct in_addr *)host->h_addr_list[1])));
        printf("h_name : %s\n",host->h_name);
        printf("h_aliases : %s\n",host->h_aliases);
        printf("h_addrtype,h_length : %d,%d,AF_INET=%d\n",host->h_addrtype,host->h_length,AF_INET);
		#endif
		//方法1：
        ipaddr = (struct in_addr *)host->h_addr;
		servaddr.sin_addr.s_addr = (ipaddr->s_addr);
		//方法2：这个算是多此一举了！多了一句无用功
//      inaddr = inet_addr(inet_ntoa(*ipaddr));
//		servaddr.sin_addr.s_addr = inaddr;
	}
    else    /*是ip地址*/
    {
        servaddr.sin_addr.s_addr = inaddr;
    }
    /*connect 函数*/
    if(connect(fd,(struct sockaddr* )&servaddr,in_len) < 0 )
    {
        printf("not connect to internet!\n ");
        close(fd);
        return 0; //没有联网成功
    }
    else
    {
        printf("=====connect ok!=====\n");
        close(fd);
        return 1;
    }
}

int main(){
	check_net();
	return 1;
}


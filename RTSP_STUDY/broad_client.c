/*
*broadcast_client.c - 多播的客户端
*/
#include <sys/types.h>    
#include <sys/socket.h>    
#include <netinet/in.h>    
#include <arpa/inet.h>    
#include <time.h>    
#include <string.h>    
#include <stdio.h>    
#include <unistd.h>    
#include <stdlib.h>

#define MCAST_PORT 8888
#define MCAST_ADDR "239.255.255.11"
#define MCAST_INTERVAL 1             /*发送间隔时间*/
#define BUFF_SIZE 256                /*接收缓冲区大小*/

int main(int argc, char*argv[])
{  
    struct sockaddr_in local_addr;              /*本地地址*/
   
    int fd = socket(AF_INET, SOCK_DGRAM, 0);     /*建立套接字*/
    if (fd == -1)
    {
        perror("socket()");
        exit(1);
    }  
    
    int yes = 1;
    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0)     
    {    
        perror("Reusing ADDR failed");    
        exit(1);    
    }
   
    /*初始化本地地址*/
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(MCAST_PORT);

    /*绑定socket*/
    int err = bind(fd,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;
    if(err < 0)
    {
        perror("bind()");
        exit(1);
    }
   
    /*设置回环许可*/
    int loop = 1;
    err = setsockopt(fd,IPPROTO_IP, IP_MULTICAST_LOOP,&loop, sizeof(loop));
    if(err < 0)
    {
        perror("setsockopt():IP_MULTICAST_LOOP");
        exit(1);
    }
   
    /*加入多播组*/
    struct ip_mreq mreq;                                    
    mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR); /*多播地址*/
    mreq.imr_interface.s_addr = htonl(INADDR_ANY); /*本地网络接口为默认*/
    
    /*将本机加入多播组*/
    err = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq));
    if (err < 0)
    {
        perror("setsockopt():IP_ADD_MEMBERSHIP");
        exit(1);
    }
   
    int times = 0;
    int addr_len = sizeof(local_addr);
    char buff[BUFF_SIZE];
    int n = 0;
    
    /*循环接收多播组的消息，5次后退出*/
    for(times = 0;times < 5;times++)
    {
        memset(buff, 0, BUFF_SIZE);                 /*清空接收缓冲区*/
        
        /*接收数据*/
        n = recvfrom(fd, buff, BUFF_SIZE, 0,(struct sockaddr*)&local_addr,&addr_len);
        if( n== -1)
        {
            perror("recvfrom()");
        }
                                                    /*打印信息*/
        printf("Recv %dst message from server:%s\n", times, buff);
        sleep(MCAST_INTERVAL);
    }
   
    /*退出多播组*/
    err = setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq, sizeof(mreq));
       
    close(fd);
    return 0;
}

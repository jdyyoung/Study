/*
*broadcast_server.c - 多播服务程序
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
#define MCAST_DATA "BROADCAST TEST DATA"            /*多播发送的数据*/
#define MCAST_INTERVAL 1                            /*发送间隔时间*/

int main(int argc, char*argv)
{
    struct sockaddr_in mcast_addr;     
    int fd = socket(AF_INET, SOCK_DGRAM, 0);         /*建立套接字*/
    if (fd == -1)
    {
        perror("socket()");
        exit(1);
    }
   
    memset(&mcast_addr, 0, sizeof(mcast_addr));/*初始化IP多播地址为0*/
    mcast_addr.sin_family = AF_INET;                /*设置协议族类行为AF*/
    mcast_addr.sin_addr.s_addr = inet_addr(MCAST_ADDR);/*设置多播IP地址*/
    mcast_addr.sin_port = htons(MCAST_PORT);        /*设置多播端口*/
   
    /*向多播地址发送数据*/
    while(1) 
    {
        int n = sendto(fd,MCAST_DATA,sizeof(MCAST_DATA),0,(struct sockaddr*)&mcast_addr,sizeof(mcast_addr)) ;
        if( n < 0)
        {
            perror("sendto()");
            exit(1);
        }      
        sleep(MCAST_INTERVAL);                          /*等待一段时间*/
    }
   
    return 0;
}

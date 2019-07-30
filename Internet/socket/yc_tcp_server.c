#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <netdb.h>
#include <ctype.h>

#include <net/if.h>
#include <signal.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>

#include "ak_thread.h"
#include "ak_common.h"
#include "ak_global.h"
#include "ak_vi.h"
#include "ak_ini.h"
#include "ak_config.h"
#include "ufo_init.h"

#include "yc_voice_ctrl.h"
#include "yc_tcp_server.h"
#include "yc_video_ctrl.h"
#include "yc_queue.h"
#include "yc_interaction.h"
#include "yc_common.h"

pthread_t ufo_tcp_th;
UFODATA ufodata;

int ufo_parse_cmd(unsigned char *cmd, CLIENT *pclient)
{
    unsigned int hbtime = 0;
    char res[16] = {0};

    if (cmd == NULL || pclient == NULL)
        return -1;

    if (cmd[0] == 0x01)
    {
        if (cmd[1] == 0x01)
        {
            pclient->hb = 20;
            hbtime |= (unsigned int)cmd[4];
            hbtime |= ((unsigned int)cmd[5] << 8);
            hbtime |= ((unsigned int)cmd[6] << 16);
            hbtime |= ((unsigned int)cmd[7] << 24);
            //settime(hbtime);

            memset(res, 0, sizeof(res));
            res[0] = 0x0f;
            res[1] = 0x01;
            res[2] = 0x01;
            send(pclient->send_sock, res, 16, 0);

            if (pclient->type == 0)
            {
                //TODO 开始抓帧往TCP发
                // anyka_start_video_withbps(FRAMES_ENCODE_PICTURE, 320, pclient, ufo_send_net_video);
                pclient->type = 1;
            }
        }
        else if (cmd[1] == 0x00)
        {
            pclient->hb = 20;
            hbtime |= (unsigned int)cmd[4];
            hbtime |= ((unsigned int)cmd[5] << 8);
            hbtime |= ((unsigned int)cmd[6] << 16);
            hbtime |= ((unsigned int)cmd[7] << 24);
            //settime(hbtime);

            if (pclient->type == 1)
            {
                // anyka_stop_video(pclient);
                pclient->type = 0;
            }

            memset(res, 0, sizeof(res));
            res[0] = 0x0f;
            res[1] = 0x01;
            res[2] = 0x01;
            send(pclient->send_sock, res, 16, 0);
        }
        else
        {
            memset(res, 0, sizeof(res));
            res[0] = 0x0f;
            res[1] = 0x01;
            res[2] = 0x00;
            send(pclient->send_sock, res, 16, 0);
        }
    }
    else if (cmd[0] == 0x03)
    {
        if (cmd[1] == 0x01)
        {
            pclient->hb = 20;
            //SetCamerFlip(0);
            ak_vi_set_flip_mirror(pclient->handle, 0, 0);

            memset(res, 0, sizeof(res));
            res[0] = 0x0f;
            res[1] = 0x03;
            res[2] = 0x01;
            send(pclient->send_sock, res, 16, 0);
        }
        else if (cmd[1] == 0x02)
        {
            pclient->hb = 20;
            //SetCamerFlip(3);
            ak_vi_set_flip_mirror(pclient->handle, 1, 1);

            memset(res, 0, sizeof(res));
            res[0] = 0x0f;
            res[1] = 0x03;
            res[2] = 0x01;
            send(pclient->send_sock, res, 16, 0);
        }
        else
        {
            memset(res, 0, sizeof(res));
            res[0] = 0x0f;
            res[1] = 0x03;
            res[2] = 0x00;
            send(pclient->send_sock, res, 16, 0);
        }
    }
    else if (cmd[0] == 0x04)
    {
        if (cmd[1] == 0x01)
        {
            unsigned int len = 0;
            char *pstart = NULL, *pend = NULL;
            char ssid[128] = {0}, pass[128] = {0};

            pclient->hb = 20;
            len |= (unsigned int)cmd[8];
            len |= ((unsigned int)cmd[9] << 8);
            len |= ((unsigned int)cmd[10] << 16);
            len |= ((unsigned int)cmd[11] << 24);

            if (len >= 12)
            {
                pstart = strstr((char *)cmd + 16, "ssid:");
                if (pstart)
                {
                    pstart += strlen("ssid:");
                    pend = strchr(pstart, ';');
                    if (pend)
                    {
                        strncpy(ssid, pstart, pend - pstart);
                        pend++;
                        pstart = strstr(pend, "pass:");
                        if (pstart)
                        {
                            pstart += strlen("pass:");
                            strcpy(pass, pstart);
                        }
                    }
                }
            }

            if (strlen(ssid) && strlen(pass))
            {
                memset(res, 0, sizeof(res));
                res[0] = 0x0f;
                res[1] = 0x04;
                res[2] = 0x01;
                send(pclient->send_sock, res, 16, 0);

                struct sys_wifi_config *wifi_info = ak_config_get_sys_wifi();
                if (strcmp(wifi_info->ssid, ssid) || strcmp(wifi_info->passwd, pass))
                {
                    strcpy(wifi_info->ssid, ssid);
                    strcpy(wifi_info->passwd, pass);
                    //anyka_set_sys_wifi2_setting(wifi_info);
                }
            }
            else
            {
                memset(res, 0, sizeof(res));
                res[0] = 0x0f;
                res[1] = 0x04;
                res[2] = 0x00;
                send(pclient->send_sock, res, 16, 0);
            }
        }
        else if (cmd[1] == 0x02)
        {
            memset(res, 0, sizeof(res));
            res[0] = 0x0f;
            res[1] = 0x04;
            res[2] = 0x01;
            send(pclient->send_sock, res, 16, 0);

            //system("/usr/sbin/lrled.sh off");
            //system("/usr/sbin/lbled.sh off");
            system("/usr/sbin/recover_cfg.sh");
            system("/usr/sbin/service.sh restart");
        }
        else
        {
            memset(res, 0, sizeof(res));
            res[0] = 0x0f;
            res[1] = 0x04;
            res[2] = 0x00;
            send(pclient->send_sock, res, 16, 0);
        }
    }
    else if (cmd[0] == 0x05)
    {
        //update from http
        memset(res, 0, sizeof(res));
        res[0] = 0x0f;
        res[1] = 0x05;
        res[2] = 0x00;
        send(pclient->send_sock, res, 16, 0);
    }
    else
    {
        memset(res, 0, sizeof(res));
        res[0] = 0x0f;
        res[1] = cmd[0];
        res[2] = 0x00;
        send(pclient->send_sock, res, 16, 0);

        return -1;
    }

    return 0;
}
extern int ufo_rtv_recognize_type = -1;
int ufo_tcp_receive_handler(unsigned char *cmd, CLIENT *pclient)
{
    if (cmd == NULL || pclient == NULL)
        return -1;
    //解析帧头（0XFF）
    if (DATA_PROTOCOL_HEAD == cmd[0])
    {
        //解析协议类型(POST:0X01)
        if (DATA_PROTOCOL_TYPE_POST == cmd[1])
        {
            //解析协议版本
            if (DATA_PROTOCOL_VERSION == cmd[2])
            {
                //解析帧编号
                if (0x01 == cmd[3] && 0x00 == cmd[4])
                {
                    //解析设备编号
                    if (DATA_PROTOCOL_DEVICE_NO == cmd[5])
                    {
                        //解析消息是否心跳包
                        if (DATA_PROTOCOL_MSG_HEART == cmd[6])
                        {
                            //解析消息长度
                            if (DATA_PROTOCOL_LENGTH_NULL == cmd[7] && DATA_PROTOCOL_LENGTH_NULL == cmd[8])
                            {
                                if (DATA_PROTOCOL_REAR_0 == cmd[9] && DATA_PROTOCOL_REAR_1 == cmd[10] &&
                                    DATA_PROTOCOL_REAR_2 == cmd[11] && DATA_PROTOCOL_REAR_3 == cmd[12])
                                {
                                    //收到心跳包返回
                                    pr("ufo_tcp_receive_handler send heart\n");
                                    pclient->hb = 20;
                                    tcp_send_cmd(DATA_PROTOCOL_TYPE_ACK, DATA_PROTOCOL_MSG_HEART);
                                    return 0;
                                }
                            }
                        } //解析消息是否重识别
                        else if (DATA_PROTOCOL_MSG_RE_IDENTIFICATION == cmd[6])
                        {
                            //解析消息长度
                            if (DATA_PROTOCOL_LENGTH_NULL == cmd[7] && DATA_PROTOCOL_LENGTH_NULL == cmd[8])
                            {
                                if (DATA_PROTOCOL_REAR_0 == cmd[9] && DATA_PROTOCOL_REAR_1 == cmd[10] &&
                                    DATA_PROTOCOL_REAR_2 == cmd[11] && DATA_PROTOCOL_REAR_3 == cmd[12])
                                {
                                    //收到重识别
                                    pr("ufo_tcp_receive_handler receive re-identification\n");
                                    video_ctrl_same_count_reset();
                                    return 0;
                                }
                            }
                        }
                    }
                }
            }
        } //解析协议类型(POST:0X03)
        else if (DATA_PROTOCOL_TYPE_REQUEST == cmd[1])
        {
            //解析协议版本
            if (DATA_PROTOCOL_VERSION == cmd[2])
            {
                //解析帧编号
                if (0x01 == cmd[3] && 0x00 == cmd[4])
                {
                    //解析设备编号
                    if (DATA_PROTOCOL_DEVICE_NO == cmd[5])
                    {
                        //解析消息是否识别类型
                        if (DATA_PROTOCOL_MSG_RECOGNIZE_TYPE == cmd[6])
                        {
                            //解析消息长度:1 cmd7 低位 cmd8 高位
                            if (1 == cmd[7] && 0 == cmd[8])
                            {
                                if (DATA_PROTOCOL_REAR_0 == cmd[10] && DATA_PROTOCOL_REAR_1 == cmd[11] &&
                                    DATA_PROTOCOL_REAR_2 == cmd[12] && DATA_PROTOCOL_REAR_3 == cmd[13])
                                {
                                    //判断识别类型
                                    pr("ufo_tcp_receive_handler receive recognize_type:%d\n", cmd[9]);
                                    ufo_rtv_recognize_type = cmd[9];
                                    return 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return -1;
}
static int active_disconnect = 0;
void *ufo_server_receive(void *param)
{
    int i = 0, num = 0;
    char req[2048] = {0};

    CLIENT *pclient = (CLIENT *)param;
    printf("start send server:%s:%d\n", pclient->ip, pclient->tcp_port);
    pthread_detach(pthread_self());
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(pclient->send_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(pclient->send_sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    while (ufodata.run_flag)
    {
        pclient->hb--;
        if (pclient->hb == 0)
        {
            for (i = 0; i < 16; i++)
            {
                if (ufodata.client[i] == pclient)
                {
                    pclient = ufodata.client[i];
                    ufodata.client[i] = NULL;
                    ufodata.client_num--;
                    printf("del client[%d] num:%d client ip:%s\n", i, ufodata.client_num, pclient->ip);
                    if (ufodata.client_num == 0)
                    {
                        //主动断开与电视端互联，不做音效与灯效
                        if (!active_disconnect)
                        {
                            yc_interaction_func(STATUS_SOUND_REMINDER_1);
                            yc_interaction_func(STATUS_LOSE_TV);
                        }
                        else
                        {
                            active_disconnect = 0;
                        }
                        ufo_rtv_recognize_type = -1;
                    }

                    // anyka_stop_video(pclient);

                    printf("stop send server:%s:%d\n", pclient->ip, pclient->tcp_port);
                    close(pclient->send_sock);
                    free(pclient);
                    pclient = NULL;
                    pthread_exit(0);
                    return NULL;
                }
            }
        }

        memset(req, 0, sizeof(req));
        //num = recv(pclient->send_sock, req, 511, 0);
        num = recv(pclient->send_sock, req, 2047, 0); //最大接收2KBdata
        if (num < 0)
        {
            usleep(100);
            continue;
        }

        printf("tcp recvdata:\n"
               "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
               req[0], req[1], req[2], req[3], req[4], req[5], req[6], req[7],
               req[8], req[9], req[10], req[11], req[12], req[13], req[14], req[15]);

        //ufo_parse_cmd((unsigned char *)req, pclient);
        ufo_tcp_receive_handler((unsigned char *)req, pclient);
    }

    printf("stop send server:%s:%d\n", pclient->ip, pclient->tcp_port);
    close(pclient->send_sock);
    return NULL;
}

int tcp_sock = 0;
void *ufo_tcp_server(void *param)
{
    int client_sock = 0;
    struct sockaddr_in server, client;
    socklen_t sin_size = 0;

    int i = 0;
    CLIENT *pclient = NULL;

    printf("start tcp server:port:%d   ip:%s\n", ufodata.TCP_port, ufodata.dev_ip);

    pthread_detach(pthread_self());

    if ((tcp_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket error\n");
        return NULL;
    }

    int val = 1;
    setsockopt(tcp_sock, SOL_SOCKET, SO_REUSEADDR, (void *)&val, sizeof(int));

    memset(&client, 0, sizeof(struct sockaddr_in));
    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(ufodata.TCP_port);
    server.sin_addr.s_addr = inet_addr(ufodata.dev_ip);

    if (bind(tcp_sock, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        close(tcp_sock);
        printf("bind error\n");
        return NULL;
    }

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(tcp_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(tcp_sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    if (listen(tcp_sock, 100) < 0)
    {
        close(tcp_sock);
        printf("listen error\n");
        return NULL;
    }

    printf("start tcp listen\n");

    sin_size = sizeof(struct sockaddr_in);
    while (ufodata.run_flag)
    {
        client_sock = accept(tcp_sock, (struct sockaddr *)&client, &sin_size);
        if (client_sock < 0)
        {
            usleep(100);
            continue;
        }

        printf("tcp accept:%s:%d\n", inet_ntoa(client.sin_addr), htons(client.sin_port));

        for (i = 0; i < 16; i++)
        {
            if (ufodata.client[i] == NULL)
            {
                pclient = (CLIENT *)malloc(sizeof(CLIENT));
                if (pclient == NULL)
                    break;

                strcpy(pclient->ip, inet_ntoa(client.sin_addr));
                pclient->tcp_port = htons(client.sin_port);
                pclient->send_sock = client_sock;
                pclient->hb = 20;
                pclient->type = 0;

                if (ufodata.client_num == 0)
                {
                    //连上电视线停止音频bug:299
                    voice_stop_play();
                    into_recognize_mode(0);
                    yc_interaction_func(STATUS_SOUND_REMINDER_0);
                    yc_interaction_func(STATUS_LOGIN_TV);
                }

                ufodata.client[i] = pclient;
                ufodata.client_num++;
                ak_thread_create(&pclient->send_th, ufo_server_receive, pclient, ANYKA_THREAD_MIN_STACK_SIZE, -1);
                printf("add client[%d] num:%d client ip:%s\n", i, ufodata.client_num, ufodata.client[i]->ip);
                break;
            }
        }
    }

    for (i = 0; i < 16; i++)
    {
        if (ufodata.client[i] != NULL)
        {
            pclient = ufodata.client[i];
            ufodata.client[i] = NULL;
            ufodata.client_num--;
            printf("del client[%d] num:%d client ip:%s\n", i, ufodata.client_num, pclient->ip);

            exit_video_ctrl();
            pthread_join(pclient->send_th, NULL);
            free(pclient);
            pclient = NULL;
        }
    }

    //printf("stop tcp server\n");
    //close(tcp_sock);
    return NULL;
}

int tcp_is_connect()
{
    return ufodata.client_num;
}

#if 0
int tcp_send_cmd(char cmd)
{
    if (!tcp_is_connect())
        return 0;
    char res[32] = {0};
    memset(res, 0, sizeof(res));
    res[0] = 0x06;
    res[1] = cmd;
    tcp_send(res,32);
    return 1;
}
#else
int tcp_send_cmd(char protocol_type, char cmd)
{
    if (!tcp_is_connect())
        return 0;

    char res[2048] = {0}; //CMD固定数据14字节
    memset(res, 0, sizeof(res));
    res[0] = DATA_PROTOCOL_HEAD;
    res[1] = protocol_type;
    res[2] = DATA_PROTOCOL_VERSION;
    res[3] = 0X01; //只有一帧
    res[4] = 0X00; //第一帧
    res[5] = DATA_PROTOCOL_DEVICE_NO;
    res[6] = cmd;
    res[7] = DATA_PROTOCOL_LENGTH_NULL;
    res[8] = DATA_PROTOCOL_LENGTH_NULL;
    res[9] = DATA_PROTOCOL_REAR_0;
    res[10] = DATA_PROTOCOL_REAR_1;
    res[11] = DATA_PROTOCOL_REAR_2;
    res[12] = DATA_PROTOCOL_REAR_3;
    res[13] = '\0';
    ak_print_normal("发送指令：%s\n", res);
    tcp_send(res, 14);

    return 1;
}
#endif

void ufo_tcp_send_recognize(unsigned char *resp, int len)
{
    char res[2048] = {0}; //识别结果最大2KB
    memset(res, 0, sizeof(res));
    res[0] = DATA_PROTOCOL_HEAD;
    res[1] = DATA_PROTOCOL_TYPE_REQUEST;
    res[2] = DATA_PROTOCOL_VERSION;
    res[3] = 0X01; //只有一帧
    res[4] = 0X00; //第一帧
    res[5] = DATA_PROTOCOL_DEVICE_NO;
    res[6] = DATA_PROTOCOL_MSG_RECOGNIZE; //识别结果
    res[7] = len & 0XFF;                  //数据长度低位
    res[8] = (len >> 8) & 0XFF;           //数据长度高位
    memcpy(res + 9, resp, len);
    res[9 + len] = DATA_PROTOCOL_REAR_0;
    res[9 + len + 1] = DATA_PROTOCOL_REAR_1;
    res[9 + len + 2] = DATA_PROTOCOL_REAR_2;
    res[9 + len + 3] = DATA_PROTOCOL_REAR_3;
    res[9 + len + 4] = '\0';
    ak_print_normal("识别结果发送的数据：%s\n", res);
    tcp_send(res, 9 + len + 5);
    return;
}

//yys queue link 2019-03-05 16:16:08
LinkQueue send_queue;
pthread_t ufo_tcp_send_th;
struct queue_args
{
    ak_sem_t send_sem;
    ak_mutex_t send_mutex;
    int send_flag;
};
struct queue_args queue_arg = {0};
//yys out queue in thread 2019-03-05 16:16:08
void *ufo_tcp_send(void *param)
{
    CLIENT *pclient = NULL;
    int i = 0;
    struct EleType data_out = {0};
    while (1)
    {
        ak_thread_sem_wait(&queue_arg.send_sem);
        while (LengthLinkQueue(&send_queue) >= 1)
        {
            DelQueue(&send_queue, &data_out);
            ak_print_normal("[yc_tcp_server] <%s> queue_quantity:%d\n", __func__, LengthLinkQueue(&send_queue));
            ak_print_normal("tcp send 连接数：%d,数据：%s,len=%d\n", ufodata.client_num, data_out.res, data_out.len);
#if 0
            ak_print_normal("tcp send out queue data:\n");
            for(i = 0; i < data_out.len; i++)
            {
                printf("%02x ", data_out.res[i]);
            }
            ak_print_normal("\n");
            ak_print_normal("tcp send out queue data end.\n");
#endif
            for (int i = 0; i < 16; i++)
            {
                if (ufodata.client[i] != NULL)
                {
                    pclient = ufodata.client[i];
                    send(pclient->send_sock, data_out.res, data_out.len, 0);
                }
            }
        }
    }
}
#if 1
int tcp_send(char *res, int len)
{
    if (!tcp_is_connect())
        return 0;

    //yys in queue 2019-03-05 16:16:08
    struct EleType data_in = {0};
    int i = 0;
    ak_thread_mutex_lock(&queue_arg.send_mutex);
    memcpy(data_in.res, res, sizeof(data_in.res));
    data_in.len = len;
#if 0
    ak_print_normal("tcp send in queue data:\n");
    for(i = 0; i < data_in.len; i++)
    {
        printf("%02x ", data_in.res[i]);
    }
    ak_print_normal("\n");
    ak_print_normal("tcp send in queue data end.\n");
#endif
    AddQueue(&send_queue, data_in);
    ak_thread_mutex_unlock(&queue_arg.send_mutex);
    ak_thread_sem_post(&queue_arg.send_sem);
    return 1;
}
#else
int tcp_send(char *res, int len)
{
    if (!tcp_is_connect())
        return 0;
    CLIENT *pclient = NULL;
    ak_print_normal("连接数：%d,数据：%s,len=%d\n", ufodata.client_num, res + 16, len);
    for (int i = 0; i < 16; i++)
    {
        if (ufodata.client[i] != NULL)
        {
            pclient = ufodata.client[i];
            send(pclient->send_sock, res, len, 0);
        }
    }
    return 1;
}
#endif
int init_tcp_server(char *ip)
{
    if (ufodata.run_flag)
    {
        ak_print_normal("TCP 已经启动\n");
        return 0;
    }
    ak_print_normal("TCP 服务启动\n");
    memset(&ufodata, 0, sizeof(UFODATA));
    ufodata.TCP_port = 6668;
    memcpy(ufodata.dev_ip, ip, strlen(ip));
    ufodata.run_flag = 1;
    //yys init queue 2019-03-05 16:16:08
    InitLinkQueue(&send_queue);
    ak_thread_sem_init(&queue_arg.send_sem, 0);
    ak_thread_mutex_init(&queue_arg.send_mutex);
    ak_thread_create(&ufo_tcp_send_th, ufo_tcp_send, NULL, ANYKA_THREAD_MIN_STACK_SIZE, -1);
    ak_thread_create(&ufo_tcp_th, ufo_tcp_server, NULL, ANYKA_THREAD_MIN_STACK_SIZE, -1);
    return 1;
}

int exit_tcp_server()
{
    if (!ufodata.run_flag)
    {
        ak_print_normal("TCP 未启动\n");
        return 0;
    }
    ak_print_normal("TCP 服务停止\n");

    //yys delete queue 2019-03-05 16:16:08
    ak_thread_cancel(ufo_tcp_send_th);
    ak_thread_sem_destroy(&queue_arg.send_sem);
    ak_thread_mutex_destroy(&queue_arg.send_mutex);
    ClearLinkQueue(&send_queue);

    //pthread_join(ufo_udp_th, NULL);
    // pthread_join(ufo_tcp_th, NULL);
    ak_thread_cancel(ufo_tcp_th);
    if (NULL != tcp_sock)
    {
        pr("stop tcp server\n");
        close(tcp_sock);
    }
    ufodata.run_flag = 0;
    return 1;
}

void yc_tcp_server_stop_connecting(void)
{
    if (!tcp_is_connect())
        return;
    ak_print_normal("发送断开与电视连接\n");
    active_disconnect = 1;
    tcp_send_cmd(DATA_PROTOCOL_TYPE_POST, DATA_PROTOCOL_MSG_STOP_CONNECTING);
}
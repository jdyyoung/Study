#include "yc_udp_server.h"
#include "ak_config.h"

pthread_t broadcast_sk_id = 0;
int udp_server_run_flag;
int UDP_PORT = 8192;
extern char AUTH_MAC[16];

static void ufo_get_machine_ip(char *send_buf)
{
    char *p = send_buf;
    /********* get ip  ********/
    if( 0 == ak_net_get_ip("wlan0",send_buf)){
        pr("ip info:%s\n",send_buf);
        strcat(send_buf, "@");
        send_buf += strlen(send_buf);
    }
    else{
        send_buf = p;
        memset(send_buf,0,strlen(send_buf));
        strcat(send_buf, "0.0.0.0@0.0.0.0@0.0.0.0@");
        pr("no ip/netmask/gateway:%s\n",send_buf);
        return;
    }

    /******* get net mask *******/
    if( 0 == ak_net_get_netmask("wlan0",send_buf)){
        pr("mask info:%s\n",send_buf);
        strcat(send_buf, "@");
        send_buf += strlen(send_buf);
    }
    else{
        send_buf = p;
        memset(send_buf,0,strlen(send_buf));
        strcat(send_buf, "0.0.0.0@0.0.0.0@0.0.0.0@");
        pr("no ip/netmask/gateway:%s\n",send_buf);
        return;
    }
    /******* get net gateway *******/
    if( 0 == ak_net_get_route("wlan0",send_buf)){
        pr("gateway info:%s\n",send_buf);
        strcat(send_buf, "@");
        send_buf += strlen(send_buf);
    }
    else{
        send_buf = p;
        memset(send_buf,0,strlen(send_buf));
        strcat(send_buf, "0.0.0.0@0.0.0.0@0.0.0.0@");
        pr("no ip/netmask/gateway:%s\n",send_buf);
        return;
    }
}

static void ufo_get_net_dns(char *dns_buf)
{
    char* p = dns_buf;
    //first nameserver
    if(0 == ak_net_get_dns(0, dns_buf)){
        strcat(dns_buf, "@");
        dns_buf += strlen(dns_buf);
        //second nameserver
        if(0 == ak_net_get_dns(1, dns_buf)){
            strcat(dns_buf, "@");
            pr("dns_buf:%s\n",p);
            return;
        }
        else{
            strncpy(dns_buf,p, strlen(p));
            pr("dns_buf:%s\n",p);
            return;
        }
    }
    else{
        dns_buf = p;
        memset(dns_buf,0,strlen(dns_buf));
        sprintf(dns_buf, "0.0.0.0@0.0.0.0@");
        pr("It dose not get DNS.\n");
    }
}

static void udp_get_local_info(char *send_buf)
{
    char *p = send_buf;
    char ch_name[50] = {0};
    char dhcp[4] = {0};
    char dev_name[50] = {0};
    char dev_mac[64] = {0};
    char soft_version[10] = {0};
    int i = 0;
    char mac[64] = {0};

	unsigned char result[16] = {0};

    /*** open the config file to get the handle ***/
    void *camera_info = ak_ini_init("/etc/jffs2/anyka_cfg.ini");

    /** get camera osd name message and ethernet dhcp status respectively **/
    //ak_ini_get_item_value(camera_info, "camera", "osd_name", ch_name, "");
    ak_ini_get_item_value(camera_info, "ethernet", "dhcp", dhcp);
    ak_ini_get_item_value(camera_info, "global", "dev_name", dev_name);
    ak_ini_get_item_value(camera_info, "global", "mac", dev_mac);
    ak_ini_get_item_value(camera_info, "global", "soft_version", soft_version);

    /** After store the message, we close the config file  **/
    ak_ini_destroy(camera_info);

    camera_info = ak_ini_init("/etc/jffs2/scc.conf");
    ak_ini_get_item_value(camera_info, "scc", "user", ch_name);

    /*********	get mac addr ***********/
    ak_net_get_mac("wlan0", mac, 64);

    if (!strlen(ch_name))
        sprintf(ch_name, "%s", AUTH_MAC);

    ak_ini_destroy(camera_info);

    /*************** get channel name *****************/
    strncpy(p, ch_name, strlen(ch_name));
    strcat(p, "@"); //add segmentation
    p += strlen(p);

    /*********  get net ip & netmask & gateway ***********/
    ufo_get_machine_ip(p);
    p += strlen(p);

    /*********  get net  dns ***********/
    ufo_get_net_dns(p);
    p += strlen(p);

    strncpy(p, mac, strlen(mac));

    strcat(p, "@"); //add segmentation
    p += strlen(p);

    /*********	get soft version ***********/
    strncpy(p, soft_version, strlen(soft_version));

    strcat(p, "@"); //add segmentation
    p += strlen(p);

    /*********	get status ***********/
    strncpy(p, "1", strlen("1"));

    strcat(p, "@"); //add segmentation
    p += strlen(p);

    /*********	get bat ***********/
    strncpy(p, "100", strlen("100"));

    return;
}

void ufo_broadcast(void)
{
    char snd_buf[256] = {0};

    int brd_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (brd_sock != -1)
    {
        int optval = 1;
        setsockopt(brd_sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int));

        struct sockaddr_in dest;
        memset(&dest, 0, sizeof(struct sockaddr_in));
        dest.sin_family = AF_INET;
        dest.sin_port = htons(8192);
        dest.sin_addr.s_addr = inet_addr("224.0.0.1");

        udp_get_local_info(snd_buf);

        sendto(brd_sock, snd_buf, strlen(snd_buf), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));
        //usleep(100*1000);
        //sendto(brd_sock, snd_buf, strlen(snd_buf), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));
        printf("[%s:%d] Send Broadcast Success, info: [%s]\n", __func__, __LINE__, snd_buf);

        close(brd_sock);
    }
}

int udp_fd; //sock fd
static void *udp_broadcast_pth(void *args)
{

    int on = 1;
    char rec_buf[128] = {0};
    char snd_buf[256] = {0};
    // char *flgs1 = "Anyka IPC ,Get IP Address!";
    char *flgs2 = "RTV,Get IP Address!";

    // ak_print_normal("[%s:%d] This thread id : %ld\n", __func__, __LINE__, (long int)gettid());

    socklen_t sockaddr_len;
    sockaddr_len = sizeof(struct sockaddr);

    struct sockaddr_in udp_addr, server_addr;
    memset(&udp_addr, 0, sizeof(struct sockaddr));
    memset(&server_addr, 0, sizeof(struct sockaddr));

    /*** set socket attribute ***/
    udp_addr.sin_family = AF_INET;         /** IPV4 **/
    udp_addr.sin_port = htons(UDP_PORT);   /** port 8192 **/
    udp_addr.sin_addr.s_addr = INADDR_ANY; /** any ip  **/
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500000;

    pthread_detach(pthread_self());

    while (udp_server_run_flag)
    {
        /** create socket  **/
        if ((udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            perror("udp socket");
            ak_print_normal("[%s:%d] Create broadcast socket failed.\n", __func__, __LINE__);
            usleep(500);
            continue;
        }
        else
        {
            ak_print_normal("[%s:%d] create udp socket success, socket fd:%d.\n", __func__, __LINE__, udp_fd);
        }
        // setsockopt(socket，SOL_SOCKET, SO_RCVTIMEO，(char *) & tv, sizeof(struct tv_usec));
        /** set socket options **/
        if (setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &on, sizeof(on)) != 0)
        {
            perror("setsockopt");
            close(udp_fd);
            usleep(500);
            continue;
        }

        /*** set close-on-exec flag ***/
        if (fcntl(udp_fd, F_SETFD, FD_CLOEXEC) == -1)
        {
            ak_print_normal("[%s:%d] error:%s\n", __func__, __LINE__, strerror(errno));
            close(udp_fd);
            usleep(500);
            continue;
        }

        /** bind the socket  **/
        if (bind(udp_fd, (struct sockaddr *)&udp_addr, sizeof(udp_addr)) < 0)
        {
            perror("bind udp");
            close(udp_fd);
            sleep(3);
            continue;
        }
        /*
		struct ip_mreq mreq;
		mreq.imr_multiaddr.s_addr = inet_addr("224.0.1.8");
		mreq.imr_interface.s_addr = INADDR_ANY;
		setsockopt(udp_fd ,IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
*/
        ak_print_normal("[%s:%d] ready to receive broadcast，run_flag:%d\n", __func__, __LINE__, udp_server_run_flag);

        while (udp_server_run_flag)
        {
            /*** receive the broadcast package blocking ***/
            if (recvfrom(udp_fd, rec_buf, sizeof(rec_buf), 0,
                         (struct sockaddr *)&server_addr, &sockaddr_len) == -1)
            {
                ak_print_normal("[%s:%d] recv broadcast failed\n", __func__, __LINE__);
                perror("receive date failed\n");
                continue;
            }
            else
            {
                rec_buf[127] = 0;
                ak_print_normal("[%s:%d] recv PC broadcast: %s\n", __func__, __LINE__, rec_buf);

                /** compare the flag to make sure that the broadcast package was sent from our software **/
                int ret = strncmp(rec_buf, flgs2, strlen(flgs2));
                ak_print_normal("对比结果：%d\n", ret);
                if (ret)
                {
                    ak_print_normal("[%s:%d] recv data is not match\n", __func__, __LINE__);
                    bzero(rec_buf, sizeof(rec_buf));
                    continue;
                }
                /*** get the machine info fill into the snd_buf ***/
                udp_get_local_info(snd_buf);

                /** send the message back to the PC **/
                if (sendto(udp_fd, snd_buf, strlen(snd_buf), 0,
                           (struct sockaddr *)&server_addr, sockaddr_len) == -1)
                {
                    ak_print_normal("[%s:%d] send broadcast failed, %s\n", __func__, __LINE__, strerror(errno));
                }
                ak_print_normal("[%s:%d] Send Broadcast Success, info: [%s]\n", __func__, __LINE__, snd_buf);
                bzero(snd_buf, sizeof(snd_buf));
                bzero(rec_buf, sizeof(rec_buf));
            }
        }

        break;
    }

    // ak_print_normal("[%s:%d] Exit thread, id : %ld\n", __func__, __LINE__, (long int)gettid());
    return NULL;
}

int init_udp_server()
{
    if (udp_server_run_flag)
    {
        ak_print_normal("UDP服务已启动\n");
        return 0;
    }
    ak_print_normal("开启UDP服务\n");
    udp_server_run_flag = 1;
    int ret = ak_thread_create(&broadcast_sk_id, udp_broadcast_pth, NULL, ANYKA_THREAD_MIN_STACK_SIZE, -1);
    if (ret != 0)
    {
        udp_server_run_flag = 0;
        pr("开启UDP线程失败\n");
    }
    return 1;
}
int exit_udp_server()
{
    if (!udp_server_run_flag)
    {
        return 0;
    }
    ak_print_normal("关闭UDP服务,pid:%lu\n", broadcast_sk_id);

    if (broadcast_sk_id != 0)
    {
        //ak_thread_cancel(broadcast_sk_id);
        udp_server_run_flag = 0;
        shutdown(udp_fd, SHUT_RDWR);
        ak_thread_join(broadcast_sk_id);
    }

    /** finish, close the socket **/
    if (NULL != udp_fd)
    {
        pr("stop udp server\n");
        close(udp_fd);
    }
    //udp_server_run_flag = 0;
    return 1;
}
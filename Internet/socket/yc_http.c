
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

#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ak_common.h"
#include "yc_http.h"
#include "base64.h"
#include "ak_config.h"

#define ZDBG 0
#define SHFR(x, n) (((x) >> (n)))
#define ROTR(x, n) (((x) >> (n)) | ((x) << ((sizeof(x) << 3) - (n))))
#define ROTL(x, n) (((x) << (n)) | ((x) >> ((sizeof(x) << 3) - (n))))

#define CHX(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define BSIG0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define BSIG1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SSIG0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ SHFR(x, 3))
#define SSIG1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))

#define MAX_CONTENT_LENGTH (10 * 1024 * 1024)

// static const char _b64chr[] =
//     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//     "abcdefghijklmnopqrstuvwxyz"
//     "0123456789+/";

static uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

#if ZDBG
static int zdump_hex(const uint8_t *data, int size)
{
    int i;
    int l = 32;

    if (data[0] == 0x21)
        return 0;

    for (i = 0; i < size; i++)
    {
        if ((i % l) == 0)
        {
            pr("[%02x] ", i / l);
        }
        pr("%02x", data[i]);
        if (((i + 1) % l) == 0)
        {
            pr("\n");
        }
    }

    pr("\n");
    return 0;
}
#else
#define zdump_hex(a, b)
#endif

static int ztransform(const uint8_t *msg, uint32_t *h)
{
    uint32_t w[64];
    uint32_t a0, b1, c2, d3, e4, f5, g6, h7;
    uint32_t t1, t2;

    int i = 0;
    int j = 0;

    for (i = 0; i < 16; i++)
    {
        w[i] = msg[j] << 24 | msg[j + 1] << 16 | msg[j + 2] << 8 | msg[j + 3];
        j += 4;
    }

    for (i = 16; i < 64; i++)
    {
        w[i] = SSIG1(w[i - 2]) + w[i - 7] + SSIG0(w[i - 15]) + w[i - 16];
    }

    zdump_hex((uint8_t *)w, 64 * 4);

    a0 = h[0];
    b1 = h[1];
    c2 = h[2];
    d3 = h[3];
    e4 = h[4];
    f5 = h[5];
    g6 = h[6];
    h7 = h[7];

    for (i = 0; i < 64; i++)
    {
        t1 = h7 + BSIG1(e4) + CHX(e4, f5, g6) + k[i] + w[i];
        t2 = BSIG0(a0) + MAJ(a0, b1, c2);

        h7 = g6;
        g6 = f5;
        f5 = e4;
        e4 = d3 + t1;
        d3 = c2;
        c2 = b1;
        b1 = a0;
        a0 = t1 + t2;
    }

    h[0] += a0;
    h[1] += b1;
    h[2] += c2;
    h[3] += d3;
    h[4] += e4;
    h[5] += f5;
    h[6] += g6;
    h[7] += h7;

    return 0;
}

int zsha256(const uint8_t *src, uint32_t len, uint32_t *hash)
{
    uint8_t *tmp = (uint8_t *)src;
    uint8_t cover_data[SHA256_COVER_SIZE];
    uint32_t cover_size = 0;

    uint32_t i = 0;
    uint32_t n = 0;
    uint32_t m = 0;
    uint32_t h[8];

    h[0] = 0x6a09e667;
    h[1] = 0xbb67ae85;
    h[2] = 0x3c6ef372;
    h[3] = 0xa54ff53a;
    h[4] = 0x510e527f;
    h[5] = 0x9b05688c;
    h[6] = 0x1f83d9ab;
    h[7] = 0x5be0cd19;

    memset(cover_data, 0x00, sizeof(uint8_t) * SHA256_COVER_SIZE);

    n = len / SHA256_BLOCK_SIZE;
    m = len % SHA256_BLOCK_SIZE;

    if (m < 56)
    {
        cover_size = SHA256_BLOCK_SIZE;
    }
    else
    {
        cover_size = SHA256_BLOCK_SIZE * 2;
    }

    if (m != 0)
    {
        memcpy(cover_data, tmp + (n * SHA256_BLOCK_SIZE), m);
    }
    cover_data[m] = 0x80;
    cover_data[cover_size - 4] = ((len * 8) & 0xff000000) >> 24;
    cover_data[cover_size - 3] = ((len * 8) & 0x00ff0000) >> 16;
    cover_data[cover_size - 2] = ((len * 8) & 0x0000ff00) >> 8;
    cover_data[cover_size - 1] = ((len * 8) & 0x000000ff);

    zdump_hex(tmp, len - m);
    zdump_hex(cover_data, cover_size);

    for (i = 0; i < n; i++)
    {
        ztransform(tmp, h);
        tmp += SHA256_BLOCK_SIZE;
    }

    tmp = cover_data;
    n = cover_size / SHA256_BLOCK_SIZE;
    for (i = 0; i < n; i++)
    {
        ztransform(tmp, h);
        tmp += SHA256_BLOCK_SIZE;
    }

    if (NULL != hash)
    {
        memcpy(hash, h, sizeof(uint32_t) * 8);
    }
    return 0;
}

int sccPostHttp(char *host, char *url, int port, char *data, int size, char *resp, int timeout_ms)
{
    int ret = 0, mlen = size + 10240, https_flag = 0;
    int sock_fd = -1;
    struct sockaddr_in my_addr;
    struct hostent *host_info;
    char *buf = NULL;
    char host_ip[16] = {0};
    if (port == 443)
        https_flag = 1;
    if (host == NULL || url == NULL || data == NULL)
        return 0;

    pr("sccPostHttp url:%s size:%d timeout:%d port:%d\n", url, size, timeout_ms, port);

    host_info = gethostbyname(host);
    if (host_info == NULL)
        return 0;

    strcpy(host_ip, inet_ntoa(*((struct in_addr *)host_info->h_addr)));
    pr("sccPostHttp host:%s ip:%s size:%d\n", host_info->h_name, host_ip, size);

    buf = (char *)malloc(mlen);
    if (buf == NULL)
        return 0;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        pr("sccPostHttp err:create sock fail\n");
        free(buf);
        return 0;
    }

    struct timeval tv;

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = timeout_ms % 1000 * 1000;

    struct timeval send_tv;
    send_tv.tv_sec = 0;
    send_tv.tv_usec = 300000;
    pr("设置时间戳秒：%lu   微秒：%lu  buf:%p\n", tv.tv_sec, tv.tv_usec, buf);

    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, &send_tv, sizeof(send_tv));

    //clear the struct
    memset(&my_addr, 0, sizeof(struct sockaddr));

    //init tcp struct
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = inet_addr(host_ip);

    ret = connect(sock_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
    if (ret == -1)
    {
        pr("sccPostHttp err:connect server ip fail   buf:%p\n", buf);
        close(sock_fd);
        free(buf);
        return 0;
    }
    SSL *pSSL = NULL;
    SSL_CTX *pCTX = NULL;

    if (https_flag)
    {

        pCTX = SSL_CTX_new(SSLv23_client_method());
        if (pCTX == NULL)
        {
            printf("SSL_CTX_new error !\n");
            close(sock_fd);
            free(buf);
            return 0;
        }

        pSSL = SSL_new(pCTX);
        if (pSSL == NULL)
        {
            printf("SSL_new error !\n");
            close(sock_fd);
            free(buf);
            return 0;
        }

        ret = SSL_set_fd(pSSL, sock_fd);
        if (ret == 0)
        {
            printf("SSL_set_fd fail\n");
            close(sock_fd);
            free(buf);
            return 0;
        }

        ret = SSL_connect(pSSL);
        if (ret != 1)
        {
            printf("SSL_connect fail\n");
            close(sock_fd);
            free(buf);
            return 0;
        }
    }
    memset(buf, 0, mlen);
    if (strstr(host, "mocn.cc") || strstr(host, "pkdd.net") || strstr(host, "52reading.cc"))
    {
        sprintf(buf,
                "POST %s HTTP/1.1\r\n"
                "Host: %s\r\n"
                // "Content-Type: application/json; charset=UTF-8\r\n"
                "Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n"
                "Content-Length: %d\r\n"
                "Connection: Keep-Alive\r\n"
                "\r\n"
                "%s",
                url, host, size, data);
        pr("send buf:\n%s\n", buf);
    }
    else
    {
        sprintf(buf,
                "POST %s HTTP/1.1\r\n"
                "Host: %s\r\n"
                "Content-Type: application/json; charset=UTF-8\r\n"
                // "Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n"
                "Content-Length: %d\r\n"
                "Connection: Keep-Alive\r\n"
                "\r\n"
                "%s",
                url, host, size, data);
    }
    if (!https_flag)
        send(sock_fd, buf, strlen(buf), 0);
    else
        SSL_write(pSSL, buf, strlen(buf));

    // pr("%s", buf);

    int len = 0;
    int i = 0;
    int total_len = 0;
    int progress = 0;
    ret = 0;
    char *p1 = NULL;
    size = 0;
    // printf("recv buf:\n");
    while (1)
    {
        memset(buf, 0, mlen);
        if (!https_flag)
            ret = recv(sock_fd, buf, mlen, 0);
        else
            ret = SSL_read(pSSL, buf, mlen);

        // pr("收到数据长度：%d,总长度：%d\n", ret, total_len);
        // pr("收到数据：%s\n", buf);
        if (ret <= 0)
            break;
        if (i == 0)
        {

            if (total_len == 0)
            {
                p1 = strstr(buf, "Content-Length: ");
                if (p1 != NULL)
                {
                    p1 += strlen("Content-Length: ");
                    total_len = strtol(p1, NULL, 10);
                }
                p1 = NULL;
                p1 = strstr(buf, "\r\n\r\n");
                if (p1 == NULL)
                {
                    p1 = strstr(buf, "\n\n");
                    if (p1)
                        p1 += 2;
                }
                else
                {
                    p1 += 4;
                }
                char http_head[4096] = {0};
                strncpy(http_head, buf, p1 - buf);
                // pr("http头：%s\n", http_head);
                ret -= (p1 - buf);
            }
            pr("解析到长度:%d,截取http头后长度：%d\n", total_len, ret);
            if (ret == 0)
            {
                i++;
                continue;
            }
            memcpy(resp, p1, ret);
        }
        else
        {
            memcpy(resp + size, buf, ret);
        }

        i++;
        size += ret;
        if (total_len != 0)
        {
            progress = (size * 100) / total_len;
            pr("进度：%d,size:%d\n", progress, size);
        }

        if (progress >= 100)
        {
            break;
        }
        // printf("%s", buf + len);
        len += ret;
    }

    pr("posthttp:\n%s\n", resp);

    close(sock_fd);

    free(buf);
    if (https_flag)
    {
        SSL_shutdown(pSSL);
        SSL_free(pSSL);
        SSL_CTX_free(pCTX);
    }
    return ret;
}

int en_post(char *host, char *url, int port, char *data, int size, char *resp, int timeout_ms)
{
    int ret = 0, mlen = size + 10240;
    int sock_fd = -1;
    struct sockaddr_in my_addr;
    struct hostent *host_info;
    char *buf = NULL, *ptmp = NULL;
    char host_ip[16] = {0};

    if (host == NULL || url == NULL || data == NULL)
        return 0;

    pr("sccPostHttp url:%s size:%d timeout:%d\n", url, size, timeout_ms);

    host_info = gethostbyname(host);
    if (host_info == NULL)
        return 0;

    strcpy(host_ip, inet_ntoa(*((struct in_addr *)host_info->h_addr)));
    pr("sccPostHttp host:%s ip:%s size:%d\n", host_info->h_name, host_ip, size);

    buf = (char *)malloc(mlen);
    if (buf == NULL)
        return 0;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        pr("sccPostHttp err:create sock fail\n");
        free(buf);
        return 0;
    }

    struct timeval tv;

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = timeout_ms % 1000 * 1000;

    struct timeval send_tv;
    send_tv.tv_sec = 0;
    send_tv.tv_usec = 200000;

    pr("设置时间戳秒：%lu   微秒：%lu  buf:%p\n", tv.tv_sec, tv.tv_usec, buf);

    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, &send_tv, sizeof(send_tv));

    //clear the struct
    memset(&my_addr, 0, sizeof(struct sockaddr));

    //init tcp struct
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = inet_addr(host_ip);

    ret = connect(sock_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
    if (ret == -1)
    {
        pr("sccPostHttp err:connect server ip fail   buf:%p\n", buf);
        close(sock_fd);
        free(buf);
        return 0;
    }

    memset(buf, 0, mlen);

    sprintf(buf,
            "POST %s HTTP/1.1\r\n"
            "Host: %s:%d\r\n"
            "Content-Type: application/octet-stream\r\n"
            // "Content-Type: application/json; charset=UTF-8\r\n"
            // "Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n"
            "Content-Length: %d\r\n"
            "Connection: Keep-Alive\r\n"
            "\r\n",
            url, host, port, size);
    int head_size = strlen(buf);
    memcpy(buf + head_size, data, size);

    pr("头长度：%d,  数据长度:%d\n", head_size, size);

    send(sock_fd, buf, size + head_size, 0);

    // pr("%s", buf);

    int len = 0;
    memset(buf, 0, mlen);
    // pr("recv buf:\n");

    ret = recv(sock_fd, buf + len, mlen - len, 0);

    // pr("%s", buf + len);
    len += ret;
    pr("recv buf:\n%s\n", buf);

    ptmp = strstr(buf, "\r\n\r\n");
    if (ptmp == NULL)
    {
        ptmp = strstr(buf, "\n\n");
        if (ptmp)
            ptmp += 2;
    }
    else
    {
        ptmp += 4;
    }

    if (ptmp)
    {
        strcpy(resp, ptmp);
    }

    close(sock_fd);

    free(buf);

    return ret;
}

int post_recognize(char *host, char *url, int port, char *data, int size, char *resp, int timeout_ms)
{
    int ret = 0, mlen = size + 10240;
    int sock_fd = -1;
    struct sockaddr_in my_addr;
    struct hostent *host_info;
    char *buf = NULL, *ptmp = NULL;
    char host_ip[16] = {0};

    if (host == NULL || url == NULL || data == NULL)
        return 0;

    pr("sccPostHttp url:%s size:%d timeout:%d\n", url, size, timeout_ms);

    host_info = gethostbyname(host);
    if (host_info == NULL)
        return 0;

    strcpy(host_ip, inet_ntoa(*((struct in_addr *)host_info->h_addr)));

    buf = (char *)malloc(mlen);
    if (buf == NULL)
        return 0;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        pr("sccPostHttp err:create sock fail\n");
        free(buf);
        return 0;
    }

    struct timeval tv;

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = timeout_ms % 1000 * 1000;
    struct timeval send_tv;
    send_tv.tv_sec = 0;
    send_tv.tv_usec = 200000;
    pr("设置时间戳秒：%lu   微秒：%lu  buf:%p\n", tv.tv_sec, tv.tv_usec, buf);

    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, &send_tv, sizeof(send_tv));

    //clear the struct
    memset(&my_addr, 0, sizeof(struct sockaddr));

    // struct sys_user_config *net_config = ak_config_get_system_user();

    //init tcp struct
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    // my_addr.sin_addr.s_addr = inet_addr("10.10.32.229");
    my_addr.sin_addr.s_addr = inet_addr(host_ip);
    pr("sccPostHttp host:%s ip:%s size:%d\n", host_info->h_name, host_ip, size);
    ret = connect(sock_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
    if (ret == -1)
    {
        pr("sccPostHttp err:connect server ip fail   buf:%p\n", buf);
        close(sock_fd);
        free(buf);
        return 0;
    }

    memset(buf, 0, mlen);

    sprintf(buf,
            "POST %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Content-Type: application/json; charset=UTF-8\r\n"
            // "Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n"
            "Content-Length: %d\r\n"
            "Connection: Keep-Alive\r\n"
            "\r\n"
            "%s",
            url, host, size, data);

    send(sock_fd, buf, strlen(buf), 0);
    // pr("%s", buf);

    int len = 0;
    memset(buf, 0, mlen);
    // pr("recv buf:\n");

    ret = recv(sock_fd, buf + len, mlen - len, 0);

    // pr("%s", buf + len);
    len += ret;
    pr("recv buf:\n%s\n", buf);

    ptmp = strstr(buf, "\r\n\r\n");
    if (ptmp == NULL)
    {
        ptmp = strstr(buf, "\n\n");
        if (ptmp)
            ptmp += 2;
    }
    else
    {
        ptmp += 4;
    }

    if (ptmp)
    {
        strcpy(resp, ptmp);
    }

    close(sock_fd);

    free(buf);

    return 1;
}

int sccGetHttpCallback(char *get_url, char *name, int timeout_ms, DOWN_LOAD_CB *cb)
{
    int buf_len = 10240, https_flag = 0;
    int ret = 0;
    int sock_fd = -1, size = 0;
    struct sockaddr_in my_addr;
    struct hostent *host_info;
    char *buf = NULL, *ptmp = NULL, *p1 = NULL, *p2 = NULL;
    char host[128] = {0}, url[128] = {0};
    char host_ip[16] = {0};
    char redict_url[256] = {0};
    pr("请求url:%s\n", get_url);
    ptmp = strstr(get_url, "https://");
    if (ptmp)
    {
        https_flag = 1;
        ptmp += strlen("https://");
    }
    else
    {
        ptmp = strstr(get_url, "http://");
        if (ptmp)
        {
            https_flag = 0;
            ptmp += strlen("http://");
        }
        else
        {
            printf("sccGetHttp url:%s error\n", get_url);
            return 0;
        }
    }

    p1 = strchr(ptmp, '/');
    if (p1 == NULL)
    {
        printf("sccGetHttp url:%s error\n", get_url);
        return 0;
    }

    strncpy(host, ptmp, p1 - ptmp);
    strcpy(url, p1);

    pr("sccGetHttp host:%s url:%s\n", host, url);
    if (host == NULL || url == NULL)
        return 0;

    host_info = gethostbyname(host);
    if (host_info == NULL)
        return 0;

    strcpy(host_ip, inet_ntoa(*((struct in_addr *)host_info->h_addr)));

    pr("sccGetHttp ip:%s\n", host_ip);
    buf = (char *)malloc(buf_len);
    if (buf == NULL)
        return 0;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        pr("sccGetHttp err:create sock fail\n");
        free(buf);
        return 0;
    }

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    //clear the struct
    memset(&my_addr, 0, sizeof(struct sockaddr));

    //init tcp struct
    my_addr.sin_family = AF_INET;

    if (https_flag)
        my_addr.sin_port = htons(443);
    else
        my_addr.sin_port = htons(80);
    my_addr.sin_addr.s_addr = inet_addr(host_ip);

    ret = connect(sock_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
    if (ret == -1)
    {
        pr("sccGetHttp err:connect server ip fail\n");
        close(sock_fd);
        free(buf);
        return 0;
    }
    SSL *pSSL = NULL;
    SSL_CTX *pCTX = NULL;

    if (https_flag)
    {
        pCTX = SSL_CTX_new(SSLv23_client_method());
        if (pCTX == NULL)
        {
            printf("SSL_CTX_new error !\n");
            close(sock_fd);
            free(buf);
            return 0;
        }

        pSSL = SSL_new(pCTX);
        if (pSSL == NULL)
        {
            printf("SSL_new error !\n");
            close(sock_fd);
            free(buf);
            return 0;
        }

        ret = SSL_set_fd(pSSL, sock_fd);
        if (ret == 0)
        {
            printf("SSL_set_fd fail\n");
            close(sock_fd);
            free(buf);
            return 0;
        }

        ret = SSL_connect(pSSL);
        if (ret != 1)
        {
            printf("SSL_connect fail\n");
            close(sock_fd);
            free(buf);
            return 0;
        }
    }

    memset(buf, 0, buf_len);
    sprintf(buf,
            "GET %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko\r\n"
            "cache-control: no-cache\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "Accept-Language: zh-CN,zh;q=0.9\r\n"
            "Connection: keep-alive\r\n"
            "\r\n",
            url, host);

    if (!https_flag)
        ret = send(sock_fd, buf, strlen(buf), 0);
    else
        ret = SSL_write(pSSL, buf, strlen(buf));

    //pr("send buf:\n%s\n", buf);

    if (ret == -1)
    {
        pr("sccGetHttp send error !\n");
        close(sock_fd);
        if (https_flag)
        {
            SSL_shutdown(pSSL);
            SSL_free(pSSL);
            SSL_CTX_free(pCTX);
        }
        free(buf);
        return 0;
    }
    if (access(name, F_OK) == 0)
    {
        remove(name);
        sync();
    }
    FILE *fp = fopen(name, "wb+");
    if (fp == NULL)
    {
        pr("sccGetHttp open file error !\n");
        close(sock_fd);
        if (https_flag)
        {
            SSL_shutdown(pSSL);
            SSL_free(pSSL);
            SSL_CTX_free(pCTX);
        }
        free(buf);
        return 0;
    }

    memset(buf, 0, buf_len);
    int total_len = 0;
    int progress = 0;
    int i = 0;
    if (cb != NULL)
    {
        cb(progress);
    }
    while (1)
    {
        if (!https_flag)
            ret = recv(sock_fd, buf, buf_len, 0);
        //ret = read(sock_fd, buf, 1);
        else
            ret = SSL_read(pSSL, buf, buf_len);
        if (i == 0 && ret > 0)
        {
            pr("收到数据长度：%d\n", ret);

            if (total_len == 0)
            {
                p1 = strstr(buf, "Content-Length: ");
                if (p1 != NULL)
                {
                    p1 += strlen("Content-Length: ");
                    total_len = strtol(p1, NULL, 10);
                }
                p1 = NULL;
                p1 = strstr(buf, "\r\n\r\n");
                if (p1 == NULL)
                {
                    p1 = strstr(buf, "\n\n");
                    if (p1)
                        p1 += 2;
                }
                else
                {
                    p1 += 4;
                }
                char http_head[1024] = {0};
                strncpy(http_head, buf, p1 - buf);
                pr("http头：%s\n", http_head);
                ret -= (p1 - buf);
            }
            if (total_len == 0)
            {
                p1 = NULL;
                //有可能是302重定向
                p1 = strstr(buf, "HTTP/1.1 302 Found");
                if (p1 == NULL)
                {
                    p1 = strstr(buf, "HTTP/1.1 301 Found");
                }
                if (p1 != NULL)
                {
                    p1 = strstr(buf, "Location: ");
                    if (p1 != NULL)
                    {

                        p1 += strlen("Location: ");
                        p2 = strstr(p1, ".mp3");
                        if (p2 != NULL)
                        {
                            p2 += 4;
                            strncpy(redict_url, p1, p2 - p1);
                            pr("截取到的redict_url:%s\n", redict_url);
                            break;
                        }
                    }
                }
            }
            pr("解析到长度:%d,截取http头后长度：%d\n", total_len, ret);
            if (MAX_CONTENT_LENGTH < total_len)
            {
                pr("需要下载文件大于10M\n");
                free(buf);
                fclose(fp);
                if (https_flag)
                {
                    SSL_shutdown(pSSL);
                    SSL_free(pSSL);
                    SSL_CTX_free(pCTX);
                }
                close(sock_fd);
                return -100;
            }
            if (ret == 0)
            {
                i++;
                continue;
            }
            fwrite(p1, ret, 1, fp);
        }
        else
        {
            if (ret <= 0)
            {
                break;
            }
            fwrite(buf, ret, 1, fp);
        }
        i++;
        size += ret;
        if (i % 5 == 0)
        {
            fflush(fp);
        }
        // }
        if (total_len != 0)
        {
            progress = (size * 100) / total_len;
            // pr("下载进度：%d,总长度：%d,下载长度%d\n", progress, total_len, size);
            if (cb != NULL)
            {
                cb(progress);
            }
            if (progress == 100)
            {
                pr("下载完成\n");
                break;
            }
        }
    }
    free(buf);
    fclose(fp);
    close(sock_fd);
    if (https_flag)
    {
        SSL_shutdown(pSSL);
        SSL_free(pSSL);
        SSL_CTX_free(pCTX);
    }
    if (strlen(redict_url))
    {
        return sccGetHttpCallback((char *)redict_url, name, timeout_ms, cb);
    }

    if (progress < 100)
    {
        if (access(name, F_OK) == 0)
        {
            remove(name);
            sync();
        }
        pr("下载失败，移除缓存文件\n");
        return 0;
    }

    return 1;
}
int sccGetHttp(char *get_url, char *name, int timeout_ms)
{
    sccGetHttpCallback(get_url, name, timeout_ms, NULL);
}
int init_http()
{
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    return 1;
}
int exit_http()
{
    return 1;
}
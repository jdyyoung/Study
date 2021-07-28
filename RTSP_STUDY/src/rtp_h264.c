/*
 * 作者：_JT_
 * 博客：https://blog.csdn.net/weixin_42462202
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "rtp.h"

#define H264_FILE_NAME  "test.h264"
#define CLIENT_IP       "127.0.0.1"
#define CLIENT_PORT     9832

#define FPS             25

static inline int startCode3(char* buf)
{
    if(buf[0] == 0 && buf[1] == 0 && buf[2] == 1)
        return 1;
    else
        return 0;
}

static inline int startCode4(char* buf)
{
    if(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1)
        return 1;
    else
        return 0;
}

static char* findNextStartCode(char* buf, int len)
{
    int i;

    if(len < 3)
        return NULL;

    for(i = 0; i < len-3; ++i)
    {
        if(startCode3(buf) || startCode4(buf))
            return buf;
        
        ++buf;
    }

    if(startCode3(buf))
        return buf;

    return NULL;
}

static int getFrameFromH264File(int fd, char* frame, int size)
{
    int rSize, frameSize;
    char* nextStartCode;

    if(fd < 0)
        return fd;

    rSize = read(fd, frame, size);
    if(!startCode3(frame) && !startCode4(frame))
        return -1;
    
    nextStartCode = findNextStartCode(frame+3, rSize-3);
    if(!nextStartCode)
    {
        lseek(fd, 0, SEEK_SET);
        frameSize = rSize;
    }
    else
    {
        frameSize = (nextStartCode-frame);
        lseek(fd, frameSize-rSize, SEEK_CUR);
    }

    return frameSize;
}

static int createUdpSocket()
{
    int fd;
    int on = 1;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
        return -1;

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));

    return fd;
}

static int rtpSendH264Frame(int socket, char* ip, int16_t port,
                            struct RtpPacket* rtpPacket, uint8_t* frame, uint32_t frameSize)
{
    uint8_t naluType; // nalu第一个字节
    int sendBytes = 0;
    int ret;

    naluType = frame[0];

    if (frameSize <= RTP_MAX_PKT_SIZE) // nalu长度小于最大包场：单一NALU单元模式
    {
        /*
         *   0 1 2 3 4 5 6 7 8 9
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         *  |F|NRI|  Type   | a single NAL unit ... |
         *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */
        memcpy(rtpPacket->payload, frame, frameSize);
        ret = rtpSendPacket(socket, ip, port, rtpPacket, frameSize);
        if(ret < 0)
            return -1;

        rtpPacket->rtpHeader.seq++;
        sendBytes += ret;
        //???,这里需要理解，以下语句多余，没起什么作用？？？
        if ((naluType & 0x1F) == 7 || (naluType & 0x1F) == 8) // 如果是SPS、PPS就不需要加时间戳
            goto out;
    }
    else // nalu长度小于最大包场：分片模式
    {
        /*
         *  0                   1                   2
         *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | FU indicator  |   FU header   |   FU payload   ...  |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        /*   MSB           LSB
         *     FU Indicator
         *    0 1 2 3 4 5 6 7
         *   +-+-+-+-+-+-+-+-+
         *   |F|NRI|  Type   |
         *   +---------------+
         */

        /*   MSB           LSB
         *      FU Header
         *    0 1 2 3 4 5 6 7
         *   +-+-+-+-+-+-+-+-+
         *   |S|E|R|  Type   |
         *   +---------------+
         */

        int pktNum = frameSize / RTP_MAX_PKT_SIZE;       // 有几个完整的包
        int remainPktSize = frameSize % RTP_MAX_PKT_SIZE; // 剩余不完整包的大小
        int i, pos = 1;

        /* 发送完整的包 */
        for (i = 0; i < pktNum; i++)
        {
            //0x60:0110 0000
            rtpPacket->payload[0] = (naluType & 0x60) | 28;
            //0x1F:0001 1111
            rtpPacket->payload[1] = naluType & 0x1F;
            
            if (i == 0) //第一包数据
                rtpPacket->payload[1] |= 0x80; // start //0x80:1000 0000
            else if (remainPktSize == 0 && i == pktNum - 1) //最后一包数据
                rtpPacket->payload[1] |= 0x40; // end //0x40:0100 0000

            memcpy(rtpPacket->payload+2, frame+pos, RTP_MAX_PKT_SIZE);
            ret = rtpSendPacket(socket, ip, port, rtpPacket, RTP_MAX_PKT_SIZE+2);
            if(ret < 0)
                return -1;

            rtpPacket->rtpHeader.seq++;
            sendBytes += ret;
            pos += RTP_MAX_PKT_SIZE;
        }

        /* 发送剩余的数据 */
        if (remainPktSize > 0)
        {
            rtpPacket->payload[0] = (naluType & 0x60) | 28;
            rtpPacket->payload[1] = naluType & 0x1F;
            rtpPacket->payload[1] |= 0x40; //end

            memcpy(rtpPacket->payload+2, frame+pos, remainPktSize+2);
            ret = rtpSendPacket(socket, ip, port, rtpPacket, remainPktSize+2);
            if(ret < 0)
                return -1;

            rtpPacket->rtpHeader.seq++;
            sendBytes += ret;
        }
    }

out:

    return sendBytes;
}

int main(int argc, char* argv[])
{
    int socket;
    int fd;
    int fps = 25;
    int startCode;
    struct RtpPacket* rtpPacket;
    uint8_t* frame;
    uint32_t frameSize;

    fd = open(H264_FILE_NAME, O_RDONLY);
    if(fd < 0)
    {
        printf("failed to open %s\n", H264_FILE_NAME);
        return -1;
    }

    socket = createUdpSocket();
    if(socket < 0)
    {
        printf("failed to create socket\n");
        return -1;
    }

    rtpPacket = (struct RtpPacket*)malloc(500000);
    frame = (uint8_t*)malloc(500000);

    //SSRC可以是自定义随机一个数，所以随便填了一个数：0x88923423
    rtpHeaderInit(rtpPacket, 0, 0, 0, RTP_VESION, RTP_PAYLOAD_TYPE_H264, 0,
                    0, 0, 0x88923423);

    while(1)
    {
        frameSize = getFrameFromH264File(fd, frame, 500000);
        if(frameSize < 0)
        {
            printf("read err\n");
            continue;
        }

        //判断H264的NALU是用00 00 00 01还是00 00 01分隔的
        if(startCode3(frame))
            startCode = 3;
        else
            startCode = 4;

        //减去分隔字节数开始真正NAUL
        frameSize -= startCode;
        rtpSendH264Frame(socket, CLIENT_IP, CLIENT_PORT,
                            rtpPacket, frame+startCode, frameSize);

        //时间戳计算有疑问 TODO:这里90000是假设？
        /*根据多媒体类型和帧率计算得出时间戳增量*/
        /*RTP 载荷类型和时间戳详解 - little_star - 博客园 - https://www.cnblogs.com/wyqfighting/archive/2013/03/05/2943992.html
        RTP有效负载(载荷)类型 (RTP Payload Type)_C#_一个嵌入式小白-CSDN博客 - https://blog.csdn.net/qq_40732350/article/details/88374707
        */
        rtpPacket->rtpHeader.timestamp += 90000/FPS;

        //时间间隔
        usleep(1000*1000/fps);
    }

    free(rtpPacket);
    free(frame);

    return 0;
}


:<<comment
从零开始写一个RTSP服务器（二）RTSP协议的实现_JT同学的博客-CSDN博客
 https://blog.csdn.net/weixin_42462202/article/details/99068041
file:
src/rtsp_server.c

test:
编译运行源码，打开vlc，输入rtsp://127.0.0.1:8554，点击开始播放，
可以看到控制台会打印出交互过程，或是用wireshak抓包
comment

#compile-1

:<<comment
从零开始写一个RTSP服务器（三）RTP传输H.264_JT同学的博客-CSDN博客 
- https://blog.csdn.net/weixin_42462202/article/details/99089020
file:
inc/rtp.h
src/rtp.c
src/rtp_h264.c
comment

#compile-2
gcc src/rtp.c src/rtp_h264.c -o rtp_h264_test

:<<comment
从零开始写一个RTSP服务器（四）一个传输H.264的RTSP服务器_JT同学的博客-CSDN博客 
- https://blog.csdn.net/weixin_42462202/article/details/99111635
file:
inc/rtp.h
src/rtp.c
src/h264_rtsp_server.c
comment

#compile-3
gcc src/rtp.c src/h264_rtsp_server.c -o h264_rtsp_server_test

:<<comment
从零开始写一个RTSP服务器（五）RTP传输AAC_JT同学的博客-CSDN博客 
- https://blog.csdn.net/weixin_42462202/article/details/99200935
file:
inc/rtp.h
src/rtp.c
src/rtp_h264.c
comment

#compile-4
gcc src/rtp.c src/rtp_aac.c -o rtp_aac_test

:<<comment
从零开始写一个RTSP服务器（六）一个传输AAC的RTSP服务器_JT同学的博客-CSDN博客 
- https://blog.csdn.net/weixin_42462202/article/details/99311193
file:
inc/rtp.h
src/rtp.c
src/aac_rtsp_server.c
comment

#compile-5
gcc src/rtp.c src/aac_rtsp_server.c -o aac_rtsp_server_test

:<<comment
从零开始写一个RTSP服务器（七）多播传输RTP包_JT同学的博客-CSDN博客 
- https://blog.csdn.net/weixin_42462202/article/details/99329048
file:
inc/rtp.h
src/rtp.c
src/multicast.c.c
src/

sdp:
tst/multicast.sdp
comment

#compile-6
gcc src/rtp.c src/multicast.c -o multicast_test


:<<comment
从零开始写一个RTSP服务器（八）一个多播的RTSP服务器_JT同学的博客-CSDN博客_rtsp 多播 
- https://blog.csdn.net/weixin_42462202/article/details/99413781
file:
inc/rtp.h
src/rtp.c
src/multicast_rtsp_server.c
comment

#compile-7
gcc src/multicast_rtsp_server.c src/rtp.c -lpthread -o multicast_rtsp_server_test

:<<comment
从零开始写一个RTSP服务器（九）一个RTP OVER RTSP/TCP的RTSP服务器_JT同学的博客-CSDN博客_rtsp tcp 
- https://blog.csdn.net/weixin_42462202/article/details/99442338
file:
inc/tcp_rtp.h
src/tcp_rtp.c
src/rtsp_tcp_server.c
comment

#compile-8
gcc src/rtp.c src/rtsp_tcp_server.c -o rtsp_tcp_server_test
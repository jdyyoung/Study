
:<<comment
���㿪ʼдһ��RTSP������������RTSPЭ���ʵ��_JTͬѧ�Ĳ���-CSDN����
 https://blog.csdn.net/weixin_42462202/article/details/99068041
file:
src/rtsp_server.c

test:
��������Դ�룬��vlc������rtsp://127.0.0.1:8554�������ʼ���ţ�
���Կ�������̨���ӡ���������̣�������wireshakץ��
comment

#compile-1

:<<comment
���㿪ʼдһ��RTSP������������RTP����H.264_JTͬѧ�Ĳ���-CSDN���� 
- https://blog.csdn.net/weixin_42462202/article/details/99089020
file:
inc/rtp.h
src/rtp.c
src/rtp_h264.c
comment

#compile-2
gcc src/rtp.c src/rtp_h264.c -o rtp_h264_test

:<<comment
���㿪ʼдһ��RTSP���������ģ�һ������H.264��RTSP������_JTͬѧ�Ĳ���-CSDN���� 
- https://blog.csdn.net/weixin_42462202/article/details/99111635
file:
inc/rtp.h
src/rtp.c
src/h264_rtsp_server.c
comment

#compile-3
gcc src/rtp.c src/h264_rtsp_server.c -o h264_rtsp_server_test

:<<comment
���㿪ʼдһ��RTSP���������壩RTP����AAC_JTͬѧ�Ĳ���-CSDN���� 
- https://blog.csdn.net/weixin_42462202/article/details/99200935
file:
inc/rtp.h
src/rtp.c
src/rtp_h264.c
comment

#compile-4
gcc src/rtp.c src/rtp_aac.c -o rtp_aac_test

:<<comment
���㿪ʼдһ��RTSP������������һ������AAC��RTSP������_JTͬѧ�Ĳ���-CSDN���� 
- https://blog.csdn.net/weixin_42462202/article/details/99311193
file:
inc/rtp.h
src/rtp.c
src/aac_rtsp_server.c
comment

#compile-5
gcc src/rtp.c src/aac_rtsp_server.c -o aac_rtsp_server_test

:<<comment
���㿪ʼдһ��RTSP���������ߣ��ಥ����RTP��_JTͬѧ�Ĳ���-CSDN���� 
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
���㿪ʼдһ��RTSP���������ˣ�һ���ಥ��RTSP������_JTͬѧ�Ĳ���-CSDN����_rtsp �ಥ 
- https://blog.csdn.net/weixin_42462202/article/details/99413781
file:
inc/rtp.h
src/rtp.c
src/multicast_rtsp_server.c
comment

#compile-7
gcc src/multicast_rtsp_server.c src/rtp.c -lpthread -o multicast_rtsp_server_test

:<<comment
���㿪ʼдһ��RTSP���������ţ�һ��RTP OVER RTSP/TCP��RTSP������_JTͬѧ�Ĳ���-CSDN����_rtsp tcp 
- https://blog.csdn.net/weixin_42462202/article/details/99442338
file:
inc/tcp_rtp.h
src/tcp_rtp.c
src/rtsp_tcp_server.c
comment

#compile-8
gcc src/rtp.c src/rtsp_tcp_server.c -o rtsp_tcp_server_test
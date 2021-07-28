:<<comment
���㿪ʼдһ��RTSP������������RTSPЭ���ʵ��_JTͬѧ�Ĳ���-CSDN����
 https://blog.csdn.net/weixin_42462202/article/details/99068041
file:
src/rtsp_server.c

test:
��������Դ�룬��vlc������rtsp://127.0.0.1:8554�������ʼ���ţ�
���Կ�������̨���ӡ���������̣�������wireshakץ��
comment

compile_1(){
	echo "compile out rtsp_server_test......"
	gcc -Iinc/ src/rtsp_server.c -o rtsp_server_test
}

:<<comment
���㿪ʼдһ��RTSP������������RTP����H.264_JTͬѧ�Ĳ���-CSDN���� 
- https://blog.csdn.net/weixin_42462202/article/details/99089020
file:
inc/rtp.h
src/rtp.c
src/rtp_h264.c
comment

compile_2(){
	echo "compile out rtp_h264_test......"
	gcc -Iinc/ src/rtp.c src/rtp_h264.c -o rtp_h264_test
}

:<<comment
���㿪ʼдһ��RTSP���������ģ�һ������H.264��RTSP������_JTͬѧ�Ĳ���-CSDN���� 
- https://blog.csdn.net/weixin_42462202/article/details/99111635
file:
inc/rtp.h
src/rtp.c
src/h264_rtsp_server.c
comment

compile_3(){
	echo "compile out h264_rtsp_server_test......"
	gcc -Iinc/ src/rtp.c src/h264_rtsp_server.c -o h264_rtsp_server_test
}

:<<comment
���㿪ʼдһ��RTSP���������壩RTP����AAC_JTͬѧ�Ĳ���-CSDN���� 
- https://blog.csdn.net/weixin_42462202/article/details/99200935
file:
inc/rtp.h
src/rtp.c
src/rtp_h264.c
comment

compile_4(){
	echo "compile out rtp_aac_test......"
	gcc -Iinc/ src/rtp.c src/rtp_aac.c -o rtp_aac_test
}

:<<comment
���㿪ʼдһ��RTSP������������һ������AAC��RTSP������_JTͬѧ�Ĳ���-CSDN���� 
- https://blog.csdn.net/weixin_42462202/article/details/99311193
file:
inc/rtp.h
src/rtp.c
src/aac_rtsp_server.c
comment

compile_5(){
	echo "compile out aac_rtsp_server_test......"
	gcc -Iinc/ src/rtp.c src/aac_rtsp_server.c -o aac_rtsp_server_test
}

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

compile_6(){
	echo "compile out multicast_test......"
	gcc -Iinc/ src/rtp.c src/multicast.c -o multicast_test
}

:<<comment
���㿪ʼдһ��RTSP���������ˣ�һ���ಥ��RTSP������_JTͬѧ�Ĳ���-CSDN����_rtsp �ಥ 
- https://blog.csdn.net/weixin_42462202/article/details/99413781
file:
inc/rtp.h
src/rtp.c
src/multicast_rtsp_server.c
comment
compile_7(){
	echo "compile out multicast_rtsp_server_test......"
	gcc -Iinc/ src/multicast_rtsp_server.c src/rtp.c -lpthread -o multicast_rtsp_server_test
}

:<<comment
���㿪ʼдһ��RTSP���������ţ�һ��RTP OVER RTSP/TCP��RTSP������_JTͬѧ�Ĳ���-CSDN����_rtsp tcp 
- https://blog.csdn.net/weixin_42462202/article/details/99442338
file:
inc/tcp_rtp.h
src/tcp_rtp.c
src/rtsp_tcp_server.c
comment

compile_8(){
	echo "compile out rtsp_tcp_server_test......"
	gcc -Iinc/ src/rtp.c src/rtsp_tcp_server.c -o rtsp_tcp_server_test
}

usage_show(){
	echo "./build.sh test_1 | test_2 | test_3 | test_4 | test_5 | test_6 | test_7 | test_8"
}
case "$1" in
	test_1)
	compile_1
	;;
	test_2)
	compile_2
	;;
	test_3)
	compile_3
	;;
	test_4)
	compile_4
	;;
	test_5)
	compile_5
	;;
	test_6)
	compile_6
	;;
	test_7)
	compile_7
	;;
	test_8)
	compile_8
	;;
	clean)
	rm *_test
	;;
	*)
		usage_show
esac


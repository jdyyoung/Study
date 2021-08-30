

WebRTC音视频同步详解_sonysuqin的专栏-CSDN博客_webrtc音视频同步 - https://blog.csdn.net/sonysuqin/article/details/107297157

硬货专栏 ｜深入浅出 WebRTC AEC（声学回声消除） - 知乎 - https://zhuanlan.zhihu.com/p/335570792

2021-07-21：

测试 aws-webRTC需要访问:

```
https://awslabs.github.io/amazon-kinesis-video-streams-webrtc-sdk-js/examples/index.html
```



```
github.io无法访问问题及解决（2020.8）：
现在无法打开 *.github.io 原因是电信运营商 DNS 污染（域名指往不正确的IP地址）
这次应该是运营商行为。像封某歌是国家行为。
可以通过修改 hosts文件 / 修改DNS服务器 / 代理 的方式访问。
可将 DNS 修改为 114.114.114.114，一个良心 DNS 服务商。
但大多数访客是不会进行上方操作的。作为站长，还是使用域名为好，服务器不稳定可以方便地更换服务商
github.io无法访问问题及解决（2020.8） - 知乎 - https://zhuanlan.zhihu.com/p/168760260
github_解决github.io无法访问的问题 - 咸月 - 博客园 - https://www.cnblogs.com/isXianYue/p/14083628.html
```

---

1.git clone 下载aws-webrtc 仓库代码，读readme.md

2.准备依赖库源码：

下载，并交叉编译好依赖库

注册账号：

编译：

```
mkdir -p amazon-kinesis-video-streams-webrtc-sdk-c/build
cd amazon-kinesis-video-streams-webrtc-sdk-c/build
cmake ..
make VERBOSE=1
```

生成4个可执行文件：

```
discoverNatBehavior  
kvsWebrtcClientMaster
kvsWebrtcClientViewer  
kvsWebrtcClientMasterGstSample
```



相关环境变量的设置：

1. `LOG_LEVEL_VERBOSE` ---- 1
2. `LOG_LEVEL_DEBUG`   ---- 2
3. `LOG_LEVEL_INFO`    ---- 3
4. `LOG_LEVEL_WARN`    ---- 4
5. `LOG_LEVEL_ERROR`   ---- 5
6. `LOG_LEVEL_FATAL`   ---- 6
7. `LOG_LEVEL_SILENT`  ---- 7

```
 #switches on DEBUG level logs while runnning the samples
 export AWS_KVS_LOG_LEVEL = 2
```

```
#set path to SSL CA certificate
export AWS_KVS_CACERT_PATH=../certs/cert.pem
```

```
#DEBUG
#Getting the SDPs
export DEBUG_LOG_SDP=TRUE

#File logging
export AWS_ENABLE_FILE_LOGGING=TRUE
```

账户相关：

```
export AWS_ACCESS_KEY_ID= <AWS account access key>
export AWS_SECRET_ACCESS_KEY= <AWS account secret key>
export AWS_SESSION_TOKEN=<session token>
export AWS_DEFAULT_REGION= <AWS region>
```



---

错误及解决：

```
rck@rck:/vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/build$ cmake ..
-- The C compiler identification is GNU 4.8.4
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Looking for include file ifaddrs.h
-- Looking for include file ifaddrs.h - found
-- Looking for getifaddrs
-- Looking for getifaddrs - found
-- Kinesis Video WebRTC Client path is /vtcs/amazon-kinesis-video-streams-webrtc-sdk-c
-- dependencies install path is /vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/open-source
-- Begin building dependencies.
-- Copying patches for dependency openssl: 
-- Configuring done
-- Generating done
-- Build files have been written to: /vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/open-source/libopenssl
Scanning dependencies of target project_libopenssl
[ 11%] Creating directories for 'project_libopenssl'
[ 22%] Performing download step (git clone) for 'project_libopenssl'
Cloning into 'project_libopenssl'...
Note: checking out 'OpenSSL_1_1_1g'.

You are in 'detached HEAD' state. You can look around, make experimental
changes and commit them, and you can discard any commits you make in this
state without impacting any branches by performing another checkout.

If you want to create a new branch to retain commits you create, you may
do so (now or later) by using -b with the checkout command again. Example:

  git checkout -b new_branch_name

HEAD is now at e2e09d9... Prepare for 1.1.1g release
Submodule 'boringssl' (https://boringssl.googlesource.com/boringssl) registered for path 'boringssl'
Submodule 'krb5' (https://github.com/krb5/krb5) registered for path 'krb5'
Submodule 'pyca.cryptography' (https://github.com/pyca/cryptography.git) registered for path 'pyca-cryptography'
Cloning into 'boringssl'...
fatal: unable to access 'https://boringssl.googlesource.com/boringssl/': Failed to connect to boringssl.googlesource.com port 443: Connection refused
Clone of 'https://boringssl.googlesource.com/boringssl' into submodule path 'boringssl' failed
CMake Error at /vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/open-source/libopenssl/build/tmp/project_libopenssl-gitclone.cmake:52 (message):
  Failed to update submodules in:
  '/vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/open-source/libopenssl/build/src/project_libopenssl'


make[2]: *** [build/src/project_libopenssl-stamp/project_libopenssl-download] Error 1
make[1]: *** [CMakeFiles/project_libopenssl.dir/all] Error 2
make: *** [all] Error 2
CMake Error at CMake/Utilities.cmake:72 (message):
  CMake step for libopenssl failed: 2
Call Stack (most recent call first):
  CMakeLists.txt:107 (build_dependency)


-- Configuring incomplete, errors occurred!
See also "/vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/build/CMakeFiles/CMakeOutput.log".

```

解决方法：

下载源文件zip包，/CMake/Dependencies/libopenssl-CMakeLists.txt

```
diff --git a/CMake/Dependencies/libopenssl-CMakeLists.txt b/CMake/Dependencies/libopenssl-CMakeLists.txt
old mode 100644
new mode 100755
index cf95d01..c50eaf1
--- a/CMake/Dependencies/libopenssl-CMakeLists.txt
+++ b/CMake/Dependencies/libopenssl-CMakeLists.txt
@@ -25,8 +25,10 @@ endif()
 
 include(ExternalProject)
 ExternalProject_Add(project_libopenssl
-    GIT_REPOSITORY    https://github.com/openssl/openssl.git
-    GIT_TAG           OpenSSL_1_1_1g
+    #GIT_REPOSITORY    https://github.com/openssl/openssl.git
+    #GIT_TAG           OpenSSL_1_1_1g
+    URL               "/vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/local_source_package/openssl-OpenSSL_1_1_1g.zip"
+    URL_MD5           ed978ed4ed4c9781b15b90ffb85ef659
     PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/build
     CONFIGURE_COMMAND ${CONFIGURE_COMMAND}
     BUILD_COMMAND     ${MAKE_EXE}


```



问题：

```
[100%] Built target libkvsCommonLws-download
-- Finished building dependencies.
OPEN_SRC_INSTALL_PREFIX=/vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/open-source
-- Checking for module 'gstreamer-1.0'
--   No package 'gstreamer-1.0' found
gstreamer not found. Will not build gstreamer samples
-- Configuring done
-- Generating done
-- Build files have been written to: /vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/build
rck@rck:/vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/build$ 
```

解决：

下载安装gstreamer-1.0

linux学习笔记2---gstreamer1.0 安装与简介_禾刀白水-CSDN博客 - https://blog.csdn.net/a125930123/article/details/48110421

```
sudo apt-get install gstreamer1.0
```



问题：

make 编译出现的错误：

```
-sdk-c/src/ice -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/gstreamer-1.0  -fPIC   -o CMakeFiles/discoverNatBehavior.dir/discoverNatBehavior.c.o   -c /vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/samples/discoverNatBehavior.c
[ 89%] Linking C executable discoverNatBehavior
cd /vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/build/samples && /usr/local/bin/cmake -E cmake_link_script CMakeFiles/discoverNatBehavior.dir/link.txt --verbose=1
/usr/bin/cc  -fPIC   CMakeFiles/discoverNatBehavior.dir/discoverNatBehavior.c.o  -o discoverNatBehavior   -L/vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/open-source/lib  -Wl,-rpath,/vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/open-source/lib:/vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/build: ../libkvsWebrtcClient.so 
../libkvsWebrtcClient.so: undefined reference to `DTLS_method'
../libkvsWebrtcClient.so: undefined reference to `OPENSSL_init_ssl'
../libkvsWebrtcClient.so: undefined reference to `TLS_method'
../libkvsWebrtcClient.so: undefined reference to `X509_getm_notAfter'
../libkvsWebrtcClient.so: undefined reference to `SSL_is_init_finished'
../libkvsWebrtcClient.so: undefined reference to `X509_getm_notBefore'
../libkvsWebrtcClient.so: undefined reference to `ASN1_INTEGER_set_uint64'
collect2: error: ld returned 1 exit status
make[2]: *** [samples/discoverNatBehavior] Error 1
make[2]: Leaving directory `/vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/build'
make[1]: *** [samples/CMakeFiles/discoverNatBehavior.dir/all] Error 2
make[1]: Leaving directory `/vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/build'
make: *** [all] Error 2
rck@rck:/vtcs/amazon-kinesis-video-streams-webrtc-sdk-c/build$
```

解决：

这是一个**临时解决**的方法：在相应link.txt 文件里，缺什么链接库，加上什么链接，

例如上面报的错误加上，-lssl

修改build/samples/CMakeFiles/discoverNatBehavior.dir/link.txt，加上-lssl

---

**760259411175**

资源链接：

 AWS KVS(Kinesis Video Streams)之WebRTC移植编译（一）_Swallow_he的博客-CSDN博客 - https://blog.csdn.net/Swallow_he/article/details/113935613

SDP协议 - 简书 - https://www.jianshu.com/p/94b118b8fd97

WebRTC之初探（一）_Swallow_he的博客-CSDN博客 - https://blog.csdn.net/Swallow_he/article/details/77367029

WebRTC_Swallow_he的博客-CSDN博客 - https://blog.csdn.net/swallow_he/category_7004467.html

WebRTC 服务器搭建 局域网 Ubuntu16.04 - 简书 - https://www.jianshu.com/p/690dd39724a5

WebRTC入门与提高1：WebRTC基础入门 - 知乎 - https://zhuanlan.zhihu.com/p/93107411

```
v=0
//sdp版本号，一直为0,rfc4566规定
o=- 7017624586836067756 2 IN IP4 127.0.0.1
// o=<username> <sess-id> <sess-version> <nettype> <addrtype> <unicast-address>
//username如何没有使用-代替，7017624586836067756是整个会话的编号，2代表会话版本，如果在会话
//过程中有改变编码之类的操作，重新生成sdp时,sess-id不变，sess-version加1
s=-
//会话名，没有的话使用-代替
t=0 0
//两个值分别是会话的起始时间和结束时间，这里都是0代表没有限制
a=group:BUNDLE audio video data
//需要共用一个传输通道传输的媒体，如果没有这一行，音视频，数据就会分别单独用一个udp端口来发送
a=msid-semantic: WMS h1aZ20mbQB0GSsq0YxLfJmiYWE9CBfGch97C
//WMS是WebRTC Media Stream简称，这一行定义了本客户端支持同时传输多个流，一个流可以包括多个track,
//一般定义了这个，后面a=ssrc这一行就会有msid,mslabel等属性
m=audio 9 UDP/TLS/RTP/SAVPF 111 103 104 9 0 8 106 105 13 126
//m=audio说明本会话包含音频，9代表音频使用端口9来传输，但是在webrtc中一现在一般不使用，如果设置为0，代表不
//传输音频,UDP/TLS/RTP/SAVPF是表示用户来传输音频支持的协议，udp，tls,rtp代表使用udp来传输rtp包，并使用tls加密
//SAVPF代表使用srtcp的反馈机制来控制通信过程,后台111 103 104 9 0 8 106 105 13 126表示本会话音频支持的编码，后台几行会有详细补充说明
c=IN IP4 0.0.0.0
//这一行表示你要用来接收或者发送音频使用的IP地址，webrtc使用ice传输，不使用这个地址
a=rtcp:9 IN IP4 0.0.0.0
//用来传输rtcp地地址和端口，webrtc中不使用
a=ice-ufrag:khLS
a=ice-pwd:cxLzteJaJBou3DspNaPsJhlQ
//以上两行是ice协商过程中的安全验证信息
a=fingerprint:sha-256 FA:14:42:3B:C7:97:1B:E8:AE:0C2:71:03:05:05:16:8F:B9:C7:98:E9:60:43:4B:5B:2C:28:EE:5C:8F3:17
//以上这行是dtls协商过程中需要的认证信息
a=setup:actpass
//以上这行代表本客户端在dtls协商过程中，可以做客户端也可以做服务端，参考rfc4145 rfc4572
a=mid:audio
//在前面BUNDLE这一行中用到的媒体标识
a=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level
//上一行指出我要在rtp头部中加入音量信息，参考 rfc6464
a=sendrecv
//上一行指出我是双向通信，另外几种类型是recvonly,sendonly,inactive
a=rtcp-mux
//上一行指出rtp,rtcp包使用同一个端口来传输
//下面几行都是对m=audio这一行的媒体编码补充说明，指出了编码采用的编号，采样率，声道等
a=rtpmap:111 opus/48000/2
a=rtcp-fb:111 transport-cc
//以上这行说明opus编码支持使用rtcp来控制拥塞，参考https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions-01
a=fmtp:111 minptime=10;useinbandfec=1
//对opus编码可选的补充说明,minptime代表最小打包时长是10ms，useinbandfec=1代表使用opus编码内置fec特性
a=rtpmap:103 ISAC/16000
a=rtpmap:104 ISAC/32000
a=rtpmap:9 G722/8000
a=rtpmap:0 PCMU/8000
a=rtpmap:8 PCMA/8000
a=rtpmap:106 CN/32000
a=rtpmap:105 CN/16000
a=rtpmap:13 CN/8000
a=rtpmap:126 telephone-event/8000
a=ssrc:18509423 cname:sTjtznXLCNH7nbRw
//cname用来标识一个数据源，ssrc当发生冲突时可能会发生变化，但是cname不会发生变化，也会出现在rtcp包中SDEC中，
//用于音视频同步
a=ssrc:18509423 msid:h1aZ20mbQB0GSsq0YxLfJmiYWE9CBfGch97C 15598a91-caf9-4fff-a28f-3082310b2b7a
//以上这一行定义了ssrc和WebRTC中的MediaStream,AudioTrack之间的关系，msid后面第一个属性是stream-d,第二个是track-id
a=ssrc:18509423 mslabel:h1aZ20mbQB0GSsq0YxLfJmiYWE9CBfGch97C
a=ssrc:18509423 label:15598a91-caf9-4fff-a28f-3082310b2b7a
m=video 9 UDP/TLS/RTP/SAVPF 100 101 107 116 117 96 97 99 98
//参考上面m=audio,含义类似
c=IN IP4 0.0.0.0
a=rtcp:9 IN IP4 0.0.0.0
a=ice-ufrag:khLS
a=ice-pwd:cxLzteJaJBou3DspNaPsJhlQ
a=fingerprint:sha-256 FA:14:42:3B:C7:97:1B:E8:AE:0C2:71:03:05:05:16:8F:B9:C7:98:E9:60:43:4B:5B:2C:28:EE:5C:8F3:17
a=setup:actpass
a=mid:video
a=extmap:2 urn:ietf:params:rtp-hdrext:toffset
a=extmap:3 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
a=extmap:4 urn:3gpp:video-orientation
a=extmap:5 http://www.ietf.org/id/draft-hol ... de-cc-extensions-01
a=extmap:6 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay
a=sendrecv
a=rtcp-mux
a=rtcp-rsize
a=rtpmap:100 VP8/90000
a=rtcp-fb:100 ccm fir
//ccm是codec control using RTCP feedback message简称，意思是支持使用rtcp反馈机制来实现编码控制，fir是Full Intra Request
//简称，意思是接收方通知发送方发送幅完全帧过来
a=rtcp-fb:100 nack
//支持丢包重传，参考rfc4585
a=rtcp-fb:100 nack pli
//支持关键帧丢包重传,参考rfc4585
a=rtcp-fb:100 goog-remb
//支持使用rtcp包来控制发送方的码流
a=rtcp-fb:100 transport-cc
//参考上面opus
a=rtpmap:101 VP9/90000
a=rtcp-fb:101 ccm fir
a=rtcp-fb:101 nack
a=rtcp-fb:101 nack pli
a=rtcp-fb:101 goog-remb
a=rtcp-fb:101 transport-cc
a=rtpmap:107 H264/90000
a=rtcp-fb:107 ccm fir
a=rtcp-fb:107 nack
a=rtcp-fb:107 nack pli
a=rtcp-fb:107 goog-remb
a=rtcp-fb:107 transport-cc
a=fmtp:107 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f
//h264编码可选的附加说明
a=rtpmap:116 red/90000
//fec冗余编码，一般如果sdp中有这一行的话，rtp头部负载类型就是116，否则就是各编码原生负责类型
a=rtpmap:117 ulpfec/90000
//支持ULP FEC，参考rfc5109
a=rtpmap:96 rtx/90000
a=fmtp:96 apt=100
//以上两行是VP8编码的重传包rtp类型
a=rtpmap:97 rtx/90000
a=fmtp:97 apt=101
a=rtpmap:99 rtx/90000
a=fmtp:99 apt=107
a=rtpmap:98 rtx/90000
a=fmtp:98 apt=116
a=ssrc-group:FID 3463951252 1461041037
//在webrtc中，重传包和正常包ssrc是不同的，上一行中前一个是正常rtp包的ssrc,后一个是重传包的ssrc
a=ssrc:3463951252 cname:sTjtznXLCNH7nbRw
a=ssrc:3463951252 msid:h1aZ20mbQB0GSsq0YxLfJmiYWE9CBfGch97C ead4b4e9-b650-4ed5-86f8-6f5f5806346d
a=ssrc:3463951252 mslabel:h1aZ20mbQB0GSsq0YxLfJmiYWE9CBfGch97C
a=ssrc:3463951252 label:ead4b4e9-b650-4ed5-86f8-6f5f5806346d
a=ssrc:1461041037 cname:sTjtznXLCNH7nbRw
a=ssrc:1461041037 msid:h1aZ20mbQB0GSsq0YxLfJmiYWE9CBfGch97C ead4b4e9-b650-4ed5-86f8-6f5f5806346d
a=ssrc:1461041037 mslabel:h1aZ20mbQB0GSsq0YxLfJmiYWE9CBfGch97C
a=ssrc:1461041037 label:ead4b4e9-b650-4ed5-86f8-6f5f5806346d
m=application 9 DTLS/SCTP 5000
c=IN IP4 0.0.0.0
a=ice-ufrag:khLS
a=ice-pwd:cxLzteJaJBou3DspNaPsJhlQ
a=fingerprint:sha-256 FA:14:42:3B:C7:97:1B:E8:AE:0C2:71:03:05:05:16:8F:B9:C7:98:E9:60:43:4B:5B:2C:28:EE:5C:8F3:17
a=setup:actpass
a=mid:data
a=sctpmap:5000 webrtc-datachannel 1024
```


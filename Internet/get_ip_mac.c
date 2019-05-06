1.下载的网址：http://www.opus-codec.org/downloads/
2.解压
3.打开opus-1.2.1里面的README(按照里面的步骤进行编译)，针对ubuntu64详细步骤如下：
（1）sudo apt-get install git autoconf automake libtool gcc make
（2）sudo apt-get install git autoconf automake libtool gcc make
（3）cd opus
（4）./autogen.sh
（5）./configure
（6）Make
以上步骤是完成编译
生成压缩文件的指令
./opus_demo -e voip 16000 1 16000 ../tts-16k.pcm  ../b.opus
opus_demo是可执行文件，
-e voip是固定的
16000是采样率
 1 单声道
16000比特率
tts-16k.pcm需要压缩的pcm文件
b.opus 生成的opus文件
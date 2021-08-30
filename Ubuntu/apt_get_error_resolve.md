

Ubuntu apt-get/dpkg 无法下载安装软件

```
错误：
子进程 dpkg-deb --control 返回了错误号 2
#linux(Ubuntu)中，更新列表（sudo apt-get update）或着安装g++等软件包时，出现 子进程 dpkg-deb --control 返回了错误号 2的解决方案_叮叮.一叮点的博客-CSDN博客 - https://blog.csdn.net/qq_44667165/article/details/108560434
```

解决方法：

```
#备份
sudo cp /etc/apt/sources.list /etc/apt/sources.list.bak
#修改
sudo gedit /etc/apt/sources.list
```

```
//阿里云源
deb http://mirrors.aliyun.com/ubuntu/ trusty main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ trusty-security main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ trusty-updates main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ trusty-proposed main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ trusty-backports main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ trusty main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ trusty-security main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ trusty-updates main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ trusty-proposed main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ trusty-backports main restricted universe multiverse

```

```
#更新列表
sudo apt-get update
```

可能会出现以下错误，我选择忽略：

```
获取：91 http://mirrors.aliyun.com trusty/universe Translation-zh_CN [180 kB]                                                                                         
获取：92 http://mirrors.aliyun.com trusty/universe Translation-en [4,089 kB]                                                                                          
忽略 http://mirrors.aliyun.com trusty-proposed/main Translation-zh_CN                                                                                                 
忽略 http://mirrors.aliyun.com trusty-proposed/main Translation-zh                                                                                                    
下载 33.4 MB，耗时 27秒 (1,195 kB/s)                                                                                                                                  
W: 以下 ID 的密钥没有可用的公钥：
3B4FE6ACC0B21F32
W: 以下 ID 的密钥没有可用的公钥：
3B4FE6ACC0B21F32
W: 以下 ID 的密钥没有可用的公钥：
3B4FE6ACC0B21F32
W: 以下 ID 的密钥没有可用的公钥：
3B4FE6ACC0B21F32
W: 以下 ID 的密钥没有可用的公钥：
3B4FE6ACC0B21F32
W: 无法下载 http://mirrors.aliyun.com/ubuntu/dists/trusty-security/InRelease  Unable to find expected entry 'multivrse/source/Sources' in Release file (Wrong sources.list entry or malformed file)

E: Some index files failed to download. They have been ignored, or old ones used instead.
```

然后就能用apt-get 正常下载安装文件了

```
sudo apt-get install p7zip
sudo apt-get install gawk
```


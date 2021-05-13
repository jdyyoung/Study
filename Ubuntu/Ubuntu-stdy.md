



---

2021-05-13：

Ubuntu14.04. 升级cmake 2.8 => 3.16

```
sudo apt remove cmake
cd /tmp
wget https://github.com/Kitware/CMake/releases/download/v3.16.0/cmake-3.16.0.tar.gz
tar -zxvf cmake-3.16.0.tar.gz
cd cmake-3.16.0
./configure
make
```

执行make 出现以下错误：

```
In file included from /usr/include/openssl/ssl.h:18:0,
                 from /tmp/cmake-3.16.0/Utilities/cmcurl/lib/curl_ntlm_core.h:41,
                 from /tmp/cmake-3.16.0/Utilities/cmcurl/lib/http_ntlm.c:40:
/usr/include/openssl/bio.h:686:1: error: old-style parameter declarations in prototyped function definition
 DEPRECATEDIN_1_1_0(struct hostent *BIO_gethostbyname(const char *name))
 ^
/tmp/cmake-3.16.0/Utilities/cmcurl/lib/http_ntlm.c:257:1: error: expected ‘{’ at end of input
 }
 ^
make[2]: *** [Utilities/cmcurl/lib/CMakeFiles/cmcurl.dir/http_ntlm.c.o] Error 1
make[1]: *** [Utilities/cmcurl/lib/CMakeFiles/cmcurl.dir/all] Error 2
make: *** [all] Error 2
```



```
#解决方法
sudo apt-get remove libssl-dev
sudo apt-get install libssl-dev

#然后再重新编译安装
make
sudo make install

#可能需要：添加PATH环境变量 /usr/local/bin
#验证
cmake --version
cmake version 3.16.0

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```


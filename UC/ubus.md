



---

 [openwrt] 使用ubus实现进程通信_落尘纷扰的专栏-CSDN博客 - https://blog.csdn.net/jasonchen_gbd/article/details/45627967

[openwrt] ubus实现进程间通信举例_落尘纷扰的专栏-CSDN博客 - https://blog.csdn.net/jasonchen_gbd/article/details/46055885

ubus对client端的消息处理抽象出**“对象（object）”和“方法（method）”**的概念。一个对象中包含多个方法，client需要向server注册收到特定json消息时的处理方法。对象和方法都有自己的名字，发送请求方只需在消息中指定要调用的对象和方法的名字即可。

---

jdy：

ubus用于进程间通信，ubusd是服务端，进程间通信是两个客户端进程之间的通信，通信是通过服务端ubusd中转。

ubus进程间通信是以对象为操作单位，一个客户端可添加多个对象，一个对象可包含多个方法，以json格式的消息内容传递。



invoke：

启用，调用，激活，

 在框架中经常会会用到method.invoke()方法,用来执行某个的对象的目标方法

用于启动某个服务端的功能，例如摄像头，提供视频流服务，启动视频流传输服务。

---

json-c: Main Page - http://json-c.github.io/json-c/json-c-current-release/doc/html/index.html



```
ubus：git://nbd.name/luci2/ubus.git或者http://git.openwrt.org/project/ubus.git
ubox：git://nbd.name/luci2/ubox.git 或者 https://git.openwrt.org/project/libubox.git
```

---

参考文章：Ubus移植到openwrt - 程序园 - http://www.voidcn.com/article/p-ogucepqh-ma.html

下载json-c-json-c-0.12.zip:

```
https://github.com/json-c/json-c
https://github.com/json-c/json-c/tree/json-c-0.12 //json-c-0.12 分支 下载.zip
```

编译安装：

```
unzip json-c-json-c-0.12.zip
mkdir lib_arm/json-c -p
cd json-c-json-c-0.12/
vi config.h.in
./configure --host=arm-linux --prefix=/home/young/ubus/lib_arm/json-c/
 make
 make install
```

```
编辑config.h.in 文件，一定要去掉下面两行内容，红色字体为要删除的
/*Define to rpl_malloc if the replacement function should be used. */
#undef malloc
/*Define to rpl_realloc if the replacement function should be used. */
#undef realloc
```

---

编译安装libubox

```cmake
#修改CMakeList.txt:添加一下内容

#告知当前使用的是交叉编译方式，必须配置
SET(CMAKE_SYSTEM_NAME Linux)

#指定C交叉编译器,必须配置
#或交叉编译器使用绝对地址
SET(CMAKE_C_COMPILER "arm-linux-gcc")

#指定C++交叉编译器
SET(CMAKE_CXX_COMPILER "arm-linux-g++")

#json-c 头文件的路径
include_directories("/home/young/ubus/lib_arm/json-c/include/json-c")
include_directories("/home/young/ubus/lib_arm/json-c/include")

#json-c 库文件路径
link_directories("/home/young/ubus/lib_arm/json-c/lib")

#修改成：
#INCLUDE(FindPkgConfig)
#PKG_CHECK_MODULES(JSONC json-c json)
#IF(JSONC_FOUND)
#  ADD_DEFINITIONS(-DJSONC)
#  INCLUDE_DIRECTORIES(${JSONC_INCLUDE_DIRS})
#ENDIF()
ADD_DEFINITIONS(-DJSONC)

#去掉条件判断
# find_library(json NAMES json-c json )
# IF(EXISTS ${json})
	# ADD_LIBRARY(blobmsg_json SHARED blobmsg_json.c)
	# TARGET_LINK_LIBRARIES(blobmsg_json ubox ${json})

	# ADD_EXECUTABLE(jshn jshn.c)
	# TARGET_LINK_LIBRARIES(jshn blobmsg_json ${json})

	# ADD_LIBRARY(json_script SHARED json_script.c)
	# TARGET_LINK_LIBRARIES(json_script ubox)

	# INSTALL(TARGETS blobmsg_json jshn json_script
		# LIBRARY DESTINATION lib
		# RUNTIME DESTINATION bin
	# )

	# FILE(GLOB scripts sh/*.sh)
	# INSTALL(FILES ${scripts}
		# DESTINATION share/libubox
	# )

# ENDIF()

#修改成：
ADD_LIBRARY(blobmsg_json SHARED blobmsg_json.c)
TARGET_LINK_LIBRARIES(blobmsg_json ubox json-c)

ADD_EXECUTABLE(jshn jshn.c)
TARGET_LINK_LIBRARIES(jshn blobmsg_json json-c)

ADD_LIBRARY(json_script SHARED json_script.c)
TARGET_LINK_LIBRARIES(json_script ubox)

INSTALL(TARGETS blobmsg_json jshn json_script
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
)

FILE(GLOB scripts sh/*.sh)
INSTALL(FILES ${scripts}
	DESTINATION share/libubox
)
```

编译：

```makefile
mkdir /home/young/ubus/lib_arm/libubox
cmake -DCMAKE_INSTALL_PREFIX:PATH=/vtcs/ubus_test/lib_arm/libubox  -DBUILD_LUA=off
make
make install
```

----

```cmake
#告知当前使用的是交叉编译方式，必须配置
SET(CMAKE_SYSTEM_NAME Linux)

#指定C交叉编译器,必须配置
#或交叉编译器使用绝对地址
SET(CMAKE_C_COMPILER "arm-linux-gcc")

#指定C++交叉编译器
SET(CMAKE_CXX_COMPILER "arm-linux-g++")

#json-c 头文件的路径
include_directories("/home/young/ubus/lib_arm/json-c/include/json-c")
include_directories("/home/young/ubus/lib_arm/json-c/include")

#json-c 库文件路径
link_directories("/home/young/ubus/lib_arm/json-c/lib")

INCLUDE_DIRECTORIES("/home/young/ubus/lib_arm/libubox/include/libubox")
INCLUDE_DIRECTORIES("/home/young/ubus/lib_arm/libubox/include")
LINK_DIRECTORIES("/home/young/ubus/lib_arm/libubox/lib")

#增加一行
ADD_DEFINITIONS( -DJSONC )

#注释掉这一行
# find_library(json NAMES json-c json)

#修改成
TARGET_LINK_LIBRARIES(cli ubus ubox blobmsg_json json-c)

```



```
cmake -DCMAKE_INSTALL_PREFIX:PATH=/home/young/ubus/lib_arm/ubus  -DBUILD_LUA=off -DENABLE_SYSTEMD=off
make
make install
```

---

```C
#ifndef __UBUS_INVOKE_H__
#define __UBUS_INVOKE_H__
#include <json/json.h>
#include <libubox/blobmsg_json.h>
 
 
struct prog_attr {
	char name[64];
	int chn_id;
};
#define PROG_MAX	8
 
 
#endif  /* __UBUS_INVOKE_H__ */
```

```

```

(1条消息) OpenWrt开发:22---软件模块之ubus（系统总线）_江南、董少-CSDN博客 - https://blog.csdn.net/qq_41453285/article/details/102595943


20210709：

```
file(REMOVE_RECURSE ${OPEN_SRC_INSTALL_PREFIX}/lib${lib_name})
```

```
file(WRITE filename "message to write"... )
file(APPEND filename "message to write"... )
file(READ filename variable [LIMIT numBytes] [OFFSET offset] [HEX])
file(<MD5|SHA1|SHA224|SHA256|SHA384|SHA512> filename variable)
file(STRINGS filename variable [LIMIT_COUNT num]
     [LIMIT_INPUT numBytes] [LIMIT_OUTPUT numBytes]
     [LENGTH_MINIMUM numBytes] [LENGTH_MAXIMUM numBytes]
     [NEWLINE_CONSUME] [REGEX regex]
     [NO_HEX_CONVERSION])
file(GLOB variable [RELATIVE path] [globbing expressions]...)
file(GLOB_RECURSE variable [RELATIVE path]
     [FOLLOW_SYMLINKS] [globbing expressions]...)
file(RENAME <oldname> <newname>)
file(REMOVE [file1 ...])
file(REMOVE_RECURSE [file1 ...])
file(MAKE_DIRECTORY [directory1 directory2 ...])
file(RELATIVE_PATH variable directory file)
file(TO_CMAKE_PATH path result)
file(TO_NATIVE_PATH path result)
file(DOWNLOAD url file [INACTIVITY_TIMEOUT timeout]
     [TIMEOUT timeout] [STATUS status] [LOG log] [SHOW_PROGRESS]
     [EXPECTED_HASH ALGO=value] [EXPECTED_MD5 sum]
     [TLS_VERIFY on|off] [TLS_CAINFO file])
file(UPLOAD filename url [INACTIVITY_TIMEOUT timeout]
     [TIMEOUT timeout] [STATUS status] [LOG log] [SHOW_PROGRESS])
file(TIMESTAMP filename variable [<format string>] [UTC])
file(GENERATE OUTPUT output_file
     <INPUT input_file|CONTENT input_content>
     [CONDITION expression])
     
file — CMake 3.0.2 Documentation - https://cmake.org/cmake/help/v3.0/command/file.html
```



```
list(FIND supported_libs ${lib_name} index)
if(${index} EQUAL -1)
    message(WARNING "${lib_name} is not supported to build from source")
    return()
 endif()
```

```
list(LENGTH <list><output variable>)
list(GET <list> <elementindex> [<element index> ...]<output variable>)
list(APPEND <list><element> [<element> ...])
list(FIND <list> <value><output variable>)
list(INSERT <list><element_index> <element> [<element> ...])
list(REMOVE_ITEM <list> <value>[<value> ...])
list(REMOVE_AT <list><index> [<index> ...])
list(REMOVE_DUPLICATES <list>)
list(REVERSE <list>)
list(SORT <list>)
```

```
LENGTH　　　　   　　　　 返回list的长度
GET　　　　　　    　　　　返回list中index的element到value中
APPEND　　　　    　　　　添加新element到list中
FIND　　　　　　   　　　　返回list中element的index，没有找到返回-1
INSERT 　　　　　　　　　 将新element插入到list中index的位置
REMOVE_ITEM　　　　　　从list中删除某个element
REMOVE_AT　　　　　　　从list中删除指定index的element
REMOVE_DUPLICATES       从list中删除重复的element
REVERSE 　　　　　　　　将list的内容反转
SORT 　　　　　　　　　　将list按字母顺序排序
```

```
	LIST与SET命令类似，即使列表本身是在父域中定义的，LIST命令也只会在当前域创建新的变量，
要想将这些操作的结果向上传递，需要通过SET　PARENT_SCOPE， SET CACHE INTERNAL或运用其他值域扩展的方法。
　　注意：cmake中的list是以分号隔开的一组字符串。
　　可以使用set命令创建一个列表。例如：set(var a b c d e)创建了一个这样的列表：a;b;c;d;e。 
　　set(var “a b c d e”)创建了一个字符串或只有一个元素的列表。
　　当指定index时，如果<element index>为大于或等于0的值，它从列表的开始处索引，0代表列表的第一个元素。
　　如果<element index>为小于或等于-1的值，它从列表的结尾处索引，-1代表列表的最后一个元素
　　
　　CMakeLists 关于List的使用方法_地表最强菜鸡的博客-CSDN博客 - https://blog.csdn.net/qq_19734597/article/details/101755468
```

---

---

2021-0516：

CMake上手：学习和练习案例素材 - https://www.icode9.com/content-4-784818.html

使用cmake --help-variable-list可以查看cmake中默认变量

使用cmake --help-variable <cmake变量名> 可以查看该变量的默认值和使用场景

```
cmake --help-variable CMAKE_INSTALL_PREFIX
```

---

CMake的几种Include_fb_help的专栏-CSDN博客 - https://blog.csdn.net/fb_help/article/details/81382746

Cmake命令之include_directories介绍 - 简书 - https://www.jianshu.com/p/e7de3de1b0fa

```
#CMake的几种Include
include_directories(sub)
target_include_directories（）
add_executable(xx xx.cpp xx.h)
```

```
#将${include_dir}头文件库路径只添加到了myproject项目
project(myproject)
target_include_directories（myproject PRIVATE ${include_dir}）
```

```
#
project(cmake_examples_install)
#Generate the shared library from the library sources
add_library(cmake_examples_inst SHARED
    src/Hello.cpp
)
target_include_directories(cmake_examples_inst
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)


#
# Set the project name
project (hello_headers)
# Add an executable with the above sources
add_executable(hello_headers ${SOURCES})
target_include_directories(hello_headers
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)
```



---

cmake 之install：

```
# Binaries
#TARGETS 安装编译生成的目标
#DESTINATION <dir> 指定环境变量${DESTDIR}/${CMAKE_INSTALL_PREFIX}下的的<dir>,如果<dir>不存在则创建
install (TARGETS cmake_examples_inst_bin DESTINATION bin)

# Library
# Note: may not work on windows
#安装库的路径需要LIBRARY DESTINATION？？？
install (TARGETS cmake_examples_inst LIBRARY DESTINATION lib)

# Header files
#DIRECTORY 安装某路径下的所有文件
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

# Config
#FILES 安装已存在的文件
install (FILES cmake-examples.conf DESTINATION etc)
```

cmake 的install 注意两个关键字：

```
cmake with `cmake .. -DCMAKE_INSTALL_PREFIX=/install/location`
make install DESTDIR=/tmp/stage
```

```
cmake 
make 
#此时的make install 会把相关安装在${DESTDIR}/${CMAKE_INSTALL_PREFIX}
#而DESTDIR的值是空，CMAKE_INSTALL_PREFIX默认值是/usr/local
#所以测试时可能会出现没有权限失败
make install
#把相关文件安装在/tmp/usr/local
make install DESTDIR=/tmp/

#亦可修改,则相关文件会安装在/tmp 目录下
cmake -DCMAKE_INSTALL_PREFIX=/tmp
make
make install
```

cmake 的环境变量：PROJECT_SOURCE_DIR 为所在项目CMakeLists.txt 所在路径

```
#cmake 内置变量:
PROJECT_SOURCE_DIR 
CMAKE_CXX_FLAGS 
CMAKE_C_FLAGS
```

---

```cmake
# Set a default C++ compile flag
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEX2" CACHE STRING "Set C++ Compiler Flags" FORCE)
```

CMake set 语法 - 白菜菜白 - 博客园 - https://www.cnblogs.com/lvchaoshun/p/10431380.html

```
1. 普通变量
set(<variable> <value>... [PARENT_SCOPE])

2. 设置缓存条目
set(<variable> <value>... CACHE <type> <docstring> [FORCE])
设置给定的缓存变量<variable> 的值为 <value>.

由于缓存项旨在提供用户可设置的值，因此默认情况下不会覆盖现有缓存项。使用FORCE选项覆盖现有条目。
<type> 必须是下列类型之一：
BOOL:Boolean ON/OFF value. 
FILEPATH:文件路径. 
PATH:目录路径. 
STRING：一行文本. 
INTERNAL：一行文本. Use of this type implies FORCE.
注意：当使用了FOECE,cmake命令行选项不会覆盖变量值，FORCE的优先级高于命令行-D … =“…”参数。

3. 设置环境变量
set(ENV{<variable>} [<value>])
设置环境变量<variable>的值为<value>. 后面使用 $ENV{<variable>} 可以获取该值.
```

---

add_compile_options命令添加的编译选项是针对所有编译器的(包括c和c++编译器)，

而set命令设置CMAKE_C_FLAGS或CMAKE_CXX_FLAGS变量则是分别只针对c和c++编译器的。

cmake --build . 与make一样的效果

make VERBOSE=1

---

---

2021-05-14：

option:

option(BUILD_TEST "Build the testing tree." OFF)

(e.g. via `-D<name>=ON` on the command line).

```
option(<variable> "<help_text>" [value])
```

```
Provides an option for the user to select as ON or OFF. If no initial <value> is provided,
OFF is used. If <variable> is already set as a normal or cache variable, then the command does nothing .
```

message:

message(STATUS "Finished building dependencies.")

message(FATAL_ERROR "No crypto library selected.")

```
message([<mode>] "message to display" ...)
```

```
(none)         = Important information
STATUS         = Incidental information
WARNING        = CMake Warning, continue processing
AUTHOR_WARNING = CMake Warning (dev), continue processing
SEND_ERROR     = CMake Error, continue processing,but skip generation
FATAL_ERROR    = CMake Error, stop processing and generation
DEPRECATION    = CMake Deprecation Error or Warning if variable
                 CMAKE_ERROR_DEPRECATED or CMAKE_WARN_DEPRECATED
                 is enabled, respectively, else no message.
```

if



get_filename_component(ROOT "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)

file(MAKE_DIRECTORY ${KINESIS_VIDEO_OPEN_SOURCE_SRC}/local_bsp4)

link_directories(${LIBCURL_LIBRARY_DIRS})

find_path

find_library

find_package(OpenSSL REQUIRED)

---

----

2021-05-13：

```
rck@rck:/vtcs/code/cmake-examples/01-basic/111$ ls
rck@rck:/vtcs/code/cmake-examples/01-basic/111$ 
rck@rck:/vtcs/code/cmake-examples/01-basic/111$ cmake ../A-hello-cmake
-- The C compiler identification is GNU 4.8.4
-- The CXX compiler identification is GNU 4.8.4
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /vtcs/code/cmake-examples/01-basic/111
rck@rck:/vtcs/code/cmake-examples/01-basic/111$ 
rck@rck:/vtcs/code/cmake-examples/01-basic/111$ ls 
CMakeCache.txt  CMakeFiles  cmake_install.cmake  Makefile
rck@rck:/vtcs/code/cmake-examples/01-basic/111$ 
```

```
//在路径下寻找CMakeLists.txt，在执行cmake的路径下生成Makefile
cmake + 路径 //没有说明路径则指当前路径下

```

----

CMake常用语法总结 - 简书 - https://www.jianshu.com/p/8909efe13308

基本语法
COMMAND(args...)，多个参数用空白符分隔

```
基本语法
COMMAND(args...)，多个参数用空白符分隔

常用命令
1. cmake_minimum_required(VERSION 3.4.1)
指定需要的最小的cmake版本


```



### 2. aux_source_directory

查找源文件并保存到相应的变量中:

```bash
#查找当前目录下所有源文件并保存至SRC_LIST变量中
aux_source_directory(. SRC_LIST)
```

# CMake常用命令

## 基本语法

`COMMAND(args...)`，多个参数用空白符分隔

```
1. cmake_minimum_required(VERSION 3.4.1)
指定需要的最小的cmake版本

2. aux_source_directory
查找源文件并保存到相应的变量中:
#查找当前目录下所有源文件并保存至SRC_LIST变量中
aux_source_directory(. SRC_LIST)

3. add_library
	3.1 添加一个库
	add_library(<name> [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL] source1 source2 ... sourceN)
	- 添加一个名为<name>的库文件
	- 指定STATIC, SHARED, MODULE参数来指定要创建的库的类型, STATIC对应的静态库(.a)，SHARED对应共享动态库(.so)
	-  [EXCLUDE_FROM_ALL], 如果指定了这一属性，对应的一些属性会在目标被创建时被设置(**指明此目录和子目录中所有的目标，是否应当从默认构建中排除, 子目录的IDE工程文件/Makefile将从顶级IDE工程文件/Makefile中排除**)
	-  source1 source2 ... sourceN用来指定源文件
	
	3.2 导入已有的库
	add_library(<name> [STATIC | SHARED | MODULE | UNKNOWN] IMPORTED)
	导入了一个已存在的<name>库文件，导入库一般配合set_target_properties使用，这个命令用来指定导入库的路径,比如
	add_library(test SHARED IMPORTED)
	set_target_properties(  test #指定目标库名称
                        PROPERTIES IMPORTED_LOCATION #指明要设置的参数
                        libs/src/${ANDROID_ABI}/libtest.so #设定导入库的路径)
                        
4. set
设置CMake变量
例子：
# 设置可执行文件的输出路径(EXCUTABLE_OUTPUT_PATH是全局变量)
set(EXECUTABLE_OUTPUT_PATH [output_path])
# 设置库文件的输出路径(LIBRARY_OUTPUT_PATH是全局变量)
set(LIBRARY_OUTPUT_PATH [output_path])
# 设置C++编译参数(CMAKE_CXX_FLAGS是全局变量)
set(CMAKE_CXX_FLAGS "-Wall std=c++11")
# 设置源文件集合(SOURCE_FILES是本地变量即自定义变量)
set(SOURCE_FILES main.cpp test.cpp ...)

5. include_directories
设置头文件位置
# 可以用相对货绝对路径，也可以用自定义的变量值
include_directories(./include ${MY_INCLUDE})

6. add_executable
#添加可执行文件
add_executable(<name> ${SRC_LIST})

7. target_link_libraries
#将若干库链接到目标库文件
target_link_libraries(<name> lib1 lib2 lib3)

将lib1, lib2, lib3链接到<name>上
NOTE: 链接的顺序应当符合gcc链接顺序规则，被链接的库放在依赖它的库的后面，即如果上面的命令中，lib1依赖于lib2, lib2又依赖于lib3，则在上面命令中必须严格按照lib1 lib2 lib3的顺序排列，否则会报错
也可以自定义链接选项, 比如针对lib1使用-WL选项,target_link_libraries(<name> lib1 -WL, lib2 lib3)

8. add_definitions
#为当前路径以及子目录的源文件加入由-D引入得define flag
add_definitions(-DFOO -DDEBUG ...)

9. add_subdirectory
#如果当前目录下还有子目录时可以使用add_subdirectory，子目录中也需要包含有CMakeLists.txt
# sub_dir指定包含CMakeLists.txt和源码文件的子目录位置
# binary_dir是输出路径， 一般可以不指定
add_subdirecroty(sub_dir [binary_dir])

10. file
文件操作命令
# 将message写入filename文件中,会覆盖文件原有内容
file(WRITE filename "message")

# 将message写入filename文件中，会追加在文件末尾
file(APPEND filename "message")

# 从filename文件中读取内容并存储到var变量中，如果指定了numBytes和offset，
# 则从offset处开始最多读numBytes个字节，另外如果指定了HEX参数，则内容会以十六进制形式存储在var变量中
file(READ filename var [LIMIT numBytes] [OFFSET offset] [HEX])

# 重命名文件
file(RENAME <oldname> <newname>)

# 删除文件， 等于rm命令
file(REMOVE [file1 ...])

# 递归的执行删除文件命令, 等于rm -r
file(REMOVE_RECURSE [file1 ...])

# 根据指定的url下载文件
# timeout超时时间; 下载的状态会保存到status中; 下载日志会被保存到log; sum指定所下载文件预期的MD5值,如果指定会自动进行比对，如果不一致，则返回一个错误; SHOW_PROGRESS，进度信息会以状态信息的形式被打印出来
file(DOWNLOAD url file [TIMEOUT timeout] [STATUS status] [LOG log] [EXPECTED_MD5 sum] [SHOW_PROGRESS])

# 创建目录
file(MAKE_DIRECTORY [dir1 dir2 ...])

# 会把path转换为以unix的/开头的cmake风格路径,保存在result中
file(TO_CMAKE_PATH path result)

# 它会把cmake风格的路径转换为本地路径风格：windows下用"\"，而unix下用"/"
file(TO_NATIVE_PATH path result)

# 将会为所有匹配查询表达式的文件生成一个文件list，并将该list存储进变量variable里, 如果一个表达式指定了RELATIVE, 返回的结果将会是相对于给定路径的相对路径, 查询表达式例子: *.cxx, *.vt?
NOTE: 按照官方文档的说法，不建议使用file的GLOB指令来收集工程的源文件
file(GLOB variable [RELATIVE path] [globbing expressions]...)
​```
11. set_directory_properties
设置某个路径的一种属性
set_directory_properties(PROPERTIES prop1 value1 prop2 value2)
prop1 prop代表属性，取值为：
- INCLUDE_DIRECTORIES
- LINK_DIRECTORIES
- INCLUDE_REGULAR_EXPRESSION
- ADDITIONAL_MAKE_CLEAN_FILES

12. set_property
在给定的作用域内设置一个命名的属性
set_property(<GLOBAL | 
            DIRECTORY [dir] | 
            TARGET [target ...] | 
            SOURCE [src1 ...] | 
            TEST [test1 ...] | 
            CACHE [entry1 ...]>
             [APPEND] 
             PROPERTY <name> [value ...])

第一个参数决定了属性可以影响的作用域,必须为以下值：
- GLOBAL 全局作作用域,不接受名字
- DIRECTORY 默认为当前路径，但是同样也可以用[dir]指定路径
- TARGET 目标作用，可以是0个或多个已有的目标
- SOURCE 源作用域， 可以是0个过多个源文件
- TEST 测试作用域, 可以是0个或多个已有的测试
- CACHE 必须指定0个或多个cache中已有的条目

PROPERTY参数是必须的

```

---

CMake中include指令用法:类似C语言的include，当前CMakeLists.txt 包含include修饰文件的内容

CMake中include指令用法介绍_liitdar的博客-CSDN博客_cmake include - https://blog.csdn.net/liitdar/article/details/81144461



 CMake option选项开关_chouhuan1877的博客-CSDN博客 - https://blog.csdn.net/chouhuan1877/article/details/100808689

---

---



如何使用CMAKE_INSTALL_PREFIX - IT屋-程序员软件开发技术分享社区 - https://www.it1352.com/516278.html

```
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..
make
make install
```

---

CMake 查找库所在目录FIND_LIBRARY | 浩瀚宇宙 灿烂星空 - http://shaoguangleo.github.io/2015/12/17/cmake-find-library/

find_path、find_library备忘录_湖北荆楚闲人_新浪博客 - http://blog.sina.com.cn/s/blog_825ad93f0102yl59.html



---



CMake 如何入门？ - 知乎 - https://www.zhihu.com/question/58949190

cmake-examples/01-basic at master · ttroy50/cmake-examples · GitHub - https://github.com.cnpmjs.org/ttroy50/cmake-examples/tree/master/01-basic

(1条消息) cmake快速入门_不忘初心-CSDN博客_cmake - https://blog.csdn.net/kai_zone/article/details/82656964

CMake基础教程 - https://www.missshi.cn/api/view/blog/5c013479753a7308cc000007

CMake 语法 - 详解 CMakeLists.txt - 简书 - https://www.jianshu.com/p/528eeb266f83

CMake教程（一） - 知乎 - https://zhuanlan.zhihu.com/p/119426899

CMake 手册详解（一） - SirDigit - 博客园 - https://www.cnblogs.com/coderfenghc/archive/2012/06/16/CMake_ch_01.html

---



命令 关键字参数 语法

===> 多层CMake ？？

编译路径编译

---


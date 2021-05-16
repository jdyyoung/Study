makefile--函数定义 函数调用 - 简书 - https://www.jianshu.com/p/e2c78c8fb4a5

---

```
目标：依赖
	执行过程
```



---

Makefile变量定义赋值：

=, :=, ?=, += 

```
SHELL = /bin/sh
```

变量定义赋值与shell 脚本的变量定义赋值区别：shell脚本的变量定义：**变量名与=之间不能有空格**！

关于：.PHONY

```
.PHONY : all deps objs clean veryclean rebuild info install
```



---

自动化变量：

$< ：依赖目标集

$@ ：目标集，表示规则中的目标文件集

$^：依赖目标集，去重

```makefile
$(TARGET) : $(SRVOBJ)
	$(CC) -o $(TARGET) $(SRVOBJ) $(SHARE_LIB_PATH) $(SHARE_LIB) -lpthread -ldl -lm $(LIBS)
$(SRVOBJ):%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ 
$(LIBTGT): $(SRVOBJ)
	$(RA) -ruv $@ $^
```



---

```makefile
# define the work dir and files
#
SRVSRC := $(wildcard *.c) $(wildcard $(addsuffix /*.c, $(SRCDIR))) #src/*.c
SRVOBJ := $(patsubst %.c,%.o,$(SRVSRC))
SRVDEP := $(patsubst %.o,%.d,$(SRVOBJ))
#MISSING_DEPS := $(filter-out $(wildcard $(SRVDEP)),$(SRVDEP))
#MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.cpp,$(MISSING_DEPS)))
```

1.wildcard : 扩展通配符

2.addsuffix ：添加后缀 ,例如路径后 + *.c

3.patsubst   :替换通配符

在$(patsubst %.c,%.o,$(dir) )中，patsubst把$(dir)中的变量符合后缀是.c的全部替换成.o，

4.filter-out:

$(filter-out PATTERN...,TEXT) 
函数名称：反过滤函数—filter-out。 
函数功能：和“filter”函数实现的功能相反。过滤掉字串“TEXT”中所有符合模式
“PATTERN”的单词，保留所有不符合此模式的单词。可以有多个模式。
存在多个模式时，模式表达式之间使用空格分割。。 
返回值：空格分割的“TEXT”字串中所有不符合模式“PATTERN”的字串。 
函数说明： “filter-out”函数也可以用来去除一个变量中的某些字符串， （实现和
“filter”函数相反）。

---

Makefile 调用shell脚本：@shell脚本命令

```
@echo "hello world!"
```

---

执行makefile:

make -f xxxx.mk   #指定Makefile执行

make -C  /xxx/dir/  #到指定路径下执行Makefile
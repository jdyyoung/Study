

### 						0920-01-libxlog.so  说明

**1.Log level**

```C
//Log level
enum {
    XLOG_NONE,       /*!< No log output */
    XLOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
    XLOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
    XLOG_INFO,       /*!< Information messages which describe normal flow of events */
    XLOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    XLOG_VERBOSE,     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
};
```

**2.default level**

```C

//system_log_level决定是否保存log 到文件里,初始化默认设置是：XLOG_INFO
int system_log_level = XLOG_INFO;
//system_console_level决定是否输出log 到控制台,初始化默认设置是：XLOG_DEBUG
int system_console_level = XLOG_DEBUG;
```

**3.logic**

```C
//判断是否保存log到文件里，判断是否输出log到控制台逻辑
if (severity <= system_console_level && !(severity >= XLOG_DEBUG &&  error != 0) ) {
    console_output("%s-%s().%d:[%s]\n", severity_string(severity), function,lineno,trim_buf); 
}
if(severity <= system_log_level  && !(severity >= XLOG_DEBUG &&  error != 0) ){
    log_output("%s-%s().%d:[%s]\n", severity_string(severity), function,lineno,trim_buf); 
}
```

**4.log API**

```C
//源码中打印的接口
#define DBUG(level, comment...)\
    system_recovery(XLOG_DEBUG, __FUNCTION__, __LINE__, level, ##comment)
#define ALERT(error, comment...) \
    system_recovery(XLOG_INFO, __FUNCTION__, __LINE__, error, ##comment)
#define WARN(error, comment...) \
    system_recovery(XLOG_WARN, __FUNCTION__, __LINE__, error, ##comment)
#define ERROR(error, comment...) \
    system_recovery(XLOG_ERROR, __FUNCTION__, __LINE__, error, ##comment)
```

所以当使用默认设置的log_level:XLOG_INFO时,ALERT(),WARN(),ERROR()能打印到控制台，能保存log到文件。

注意：其中DBUG(level, comment...)的level也决定DBUG时是否保存log到文件里，是否输出log到控制台。
当level >=XLOG_DEBUG ， DBUG(0, comment...) 保存log到文件里，输出log到控制台

当level >=XLOG_DEBUG ， DBUG(1, comment...)不保存log到文件里，不输出log到控制台。

所以DBUG(0/1, comment...) 可灵活用在单独的源文件调试。

**5.adjust log level API**

```C
//调整保存log到文件的接口
int system_debug_level_update(int level)
{
    int old = system_log_level;
    system_log_level = level;
    return old;
}

//调整打印log到控制台的接口
int system_console_level_update(int level)
{
    int old = system_console_level;
    system_console_level = level;
    return old;
}
```

**7.linux shell adjust log level**

```shell
#linux命令行调整保存log到文件level
#设置system_log_level = XLOG_NONE,关闭保存log到文件
ubus send "d3_sys_setting" '{"debug_level": 0 }'

#设置system_log_level = XLOG_ERROR,仅保存ERROR的log到文件
ubus send "d3_sys_setting" '{"debug_level": 1 }'

#设置system_log_level = XLOG_WARN，保存ERROR、WARN的log到文件
ubus send "d3_sys_setting" '{"debug_level": 2 }'

#设置system_log_level = XLOG_INFO，保存ERROR、WARN、ALERT的log到文件
ubus send "d3_sys_setting" '{"debug_level": 3 }'

#设置system_log_level = XLOG_DEBUG，保存ERROR、WARN、ALERT、DBUG(0,...)的log到文件
ubus send "d3_sys_setting" '{"debug_level": 4 }'


#linux命令行调整打印log到控制台
#system_console_level = XLOG_NONE,关闭打印log到控制台
ubus send "d3_sys_setting" '{"console_level": 0 }'

#system_console_level = XLOG_ERROR,仅打印ERROR的log到控制台
ubus send "d3_sys_setting" '{"console_level": 1 }'

#设置system_console_level = XLOG_WARN，打印ERROR、WARN的log到控制台
ubus send "d3_sys_setting" '{"console_level": 2 }'

#设置system_console_level = XLOG_INFO，打印ERROR、WARN、ALERT的log到控制台
ubus send "d3_sys_setting" '{"console_level": 3 }'

#设置system_console_level = XLOG_DEBUG，打印ERROR、WARN、ALERT、DBUG(0,...)的log到控制台
ubus send "d3_sys_setting" '{"console_level": 4 }'

```

8.查看机子运行libxlog.so版本号

```
root@XDC02-00602:/# strings /usr/lib/libxlog.so | grep XLOG_VER
XLOG_VER
XLOG_VER_2022-09-20-01
root@XDC02-00602:/# 
```


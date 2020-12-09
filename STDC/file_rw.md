

文件操作：

C 标准库 – <stdio.h> | 菜鸟教程 - https://www.runoob.com/cprogramming/c-standard-library-stdio-h.html

fopen():

```
描述
C 库函数 FILE *fopen(const char *filename, const char *mode) 使用给定的模式 mode 打开 filename 所指向的文件。

声明
FILE *fopen(const char *filename, const char *mode)

参数
filename -- 这是 C 字符串，包含了要打开的文件名称。
mode -- 这是 C 字符串，包含了文件访问模式

返回值
该函数返回一个 FILE 指针。否则返回 NULL，且设置全局变量 errno 来标识错误。
```

| 模式  | 描述                                                         |
| ----- | ------------------------------------------------------------ |
| "r"   | 以“只读”方式打开文件。只允许读取，不允许写入。文件必须存在，否则打开失败。 |
| “w”   | 以“写入”方式打开文件。如果文件不存在，那么创建一个新文件；如果文件存在，那么清空文件内容（相当于删除原文件，再创建一个新文件）。 |
| "a"   | 以“追加”方式打开文件。如果文件不存在，那么创建一个新文件；如果文件存在，那么将写入的数据追加到文件的末尾（文件原有的内容保留）。 |
| "r+"  | 以“读写”方式打开文件。既可以读取也可以写入，也就是随意更新文件。文件必须存在，否则打开失败。 |
| "w+"  | 以“写入/更新”方式打开文件，相当于w和r+叠加的效果。既可以读取也可以写入，也就是随意更新文件。如果文件不存在，那么创建一个新文件；如果文件存在，那么清空文件内容（相当于删除原文件，再创建一个新文件）。 |
| "a+"  | 以“追加/更新”方式打开文件，相当于a和r+叠加的效果。既可以读取也可以写入，也就是随意更新文件。如果文件不存在，那么创建一个新文件；如果文件存在，那么将写入的数据追加到文件的末尾（文件原有的内容保留）。 |
| ----- | 以下控制读写方式的字符串（可以不写）                         |
| "t"   | 文本文件。如果不写，默认为"t"。                              |
| "b"   | 二进制文件。                                                 |

fclose():

fgets()：

```
描述
C 库函数 char *fgets(char *str, int n, FILE *stream) 从指定的流 stream 读取一行，并把它存储在 str 所指向的字符串内。当读取 (n-1) 个字符时，或者读取到换行符时，或者到达文件末尾时，它会停止，具体视情况而定。

声明：
char *fgets(char *str, int n, FILE *stream)

参数
str -- 这是指向一个字符数组的指针，该数组存储了要读取的字符串。
n -- 这是要读取的最大字符数（包括最后的空字符）。通常是使用以 str 传递的数组长度。
stream -- 这是指向 FILE 对象的指针，该 FILE 对象标识了要从中读取字符的流。

返回值
如果成功，该函数返回相同的 str 参数。如果到达文件末尾或者没有读取到任何字符，str 的内容保持不变，并返回一个空指针。
如果发生错误，返回一个空指针。
```



fputs()：

```
描述
C 库函数 int fputs(const char *str, FILE *stream) 把字符串写入到指定的流 stream 中，但不包括空字符。

声明：
int fputs(const char *str, FILE *stream)

参数
str -- 这是一个数组，包含了要写入的以空字符终止的字符序列。
stream -- 这是指向 FILE 对象的指针，该 FILE 对象标识了要被写入字符串的流。

返回值
该函数返回一个非负值，如果发生错误则返回 EOF
```



fscantf()：

```
描述
C 库函数 int fscanf(FILE *stream, const char *format, ...) 从流 stream 读取格式化输入。

声明：
int fscanf(FILE *stream, const char *format, ...)

参数
stream -- 这是指向 FILE 对象的指针，该 FILE 对象标识了流。
format -- 这是 C 字符串，包含了以下各项中的一个或多个：空格字符、非空格字符 和 format 说明符。

返回值
如果成功，该函数返回成功匹配和赋值的个数。如果到达文件末尾或发生读错误，则返回 EOF。
```



fprintf()：

```
描述
C 库函数 int fprintf(FILE *stream, const char *format, ...) 发送格式化输出到流 stream 中。

声明：
int fprintf(FILE *stream, const char *format, ...)

参数：
stream -- 这是指向 FILE 对象的指针，该 FILE 对象标识了流。
format -- 这是 C 字符串，包含了要被写入到流 stream 中的文本。它可以包含嵌入的 format 标签，format 标签可被随后的附加参数中指定的值替换，并按需求进行格式化。

返回值
如果成功，则返回写入的字符总数，否则返回一个负数。
```



fread():

```
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
从给定流 stream 读取数据到 ptr 所指向的数组中。
```



fwrite():

```
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
把 ptr 所指向的数组中的数据写入到给定流 stream 中
```



ftell():

```
long int ftell(FILE *stream)
返回给定流 stream 的当前文件位置。
```



fseek():

```
描述
C 库函数 int fseek(FILE *stream, long int offset, int whence) 设置流 stream 的文件位置为给定的偏移 offset，参数 offset 意味着从给定的 whence 位置查找的字节数。

声明：
int fseek(FILE *stream, long int offset, int whence)

参数
stream -- 这是指向 FILE 对象的指针，该 FILE 对象标识了流。
offset -- 这是相对 whence 的偏移量，以字节为单位。
whence -- 这是表示开始添加偏移 offset 的位置。它一般指定为下列常量之一：

返回值
如果成功，则该函数返回零，否则返回非零值。
```

| 常量     | 描述               |
| -------- | ------------------ |
| SEEK_SET | 文件的开头         |
| SEEK_CUR | 文件指针的当前位置 |
| SEEK_END | 文件的末尾         |

----

```c
void ReadHostapdConf(char* filename, u32 sn)
{
    int iRet = 0;
    FILE* stream = NULL;
    struct stat status;
    char szBuf[256];
    char szSsid[32];
    unsigned long pos = 0;
    unsigned long len = 0;

	sprintf(szSsid, "ssid=SkyBell_%-10u  ", sn);

    DBUG(0,"SSID[%s]\n",szSsid);
    iRet = stat(filename, &status);
    
    if(iRet != 0 || status.st_size <=0)
    {
        DBUG(0,"can't found file :%s\n", filename);
        return;
    }

    stream = fopen(filename, "r+");
    if(NULL == stream)
    {
        perror("fopen");
        return;
    }

    while(fgets(szBuf, sizeof(szBuf), stream) != NULL)
    {
        pos = ftell(stream);
        //DBUG(0,"offset = %ld\n",pos);
        
        len = strlen(szBuf);
        //DBUG(0,"line:[%lu] %s", len, szBuf);
        if(strstr(szBuf,"ssid=SkyBell"))
        {
            DBUG(0,"Change: %s=====>: %s\n", szBuf, szSsid);
            fseek(stream, pos-len,SEEK_SET);
            fputs(szSsid, stream);
        }
        usleep(500);
    }
	fclose(stream);
}
```



---

c语言一行一行的读取txt文件_u010957054的博客-CSDN博客_c语言怎么用数组一行一行读txt文件 - https://blog.csdn.net/u010957054/article/details/58602342/

```
c语言一行一行的读取txt文件:
while(fscanf(pFile,"%[^\n]",CmdLine)!=EOF)
{ 
fgetc(pFile);
...
}

其中，pFile是调用fopen函数返回的文件句柄，cmdline是定义的char型数组，用来存储一行的内容。此code运用了fscanf的方式，一行一行的读取txt文件，在while中做处理。其中的%[^\n]属于正则表达式，代表不遇到换行符就一直读下去，直到遇到换行符\n，fscanf函数就退出。退出后进入while，执行fgetc函数，读取一个字符，文件句柄向后移动一个字符，相当于跳过了\n换行符，下一次就从下一行开始读了。
也有用下面的方法读取的：

while(fscanf(pFile,"%[^\n]%*c\n",CmdLine)!=EOF）
{
...
}

相当于将fgetc函数移除，改为正则表达式中%*c的方式跳过\n换行字符。但是我在工作中发现这种方法有一个问题，那就是遇到第一行是空的，从第二行才开始有内容的文件的时候，程序会一直停在第一行无限循环下去，CmdLine中什么也读不到。我觉得貌似是fscanf中使用正则表达式的bug，毕竟正则表达式的支持还是shell或python这种脚本语言比较好。用fgetc就不会出现这种情况。所以还是建议用第一种方法读取文件。
```



```c
//读取每一行文件
//使用fscanf函数实现的读一行的代码，遇到空格不会切断。
/*
C语言怎样读取文本的每一行_百度知道 - https://zhidao.baidu.com/question/188964112.html
*/
static int read_wechat_message_to_list()
{
	char r_buf[256];
	FILE *fp = fopen(WECHAT_MESSAGE, "r");
	if (fp == NULL)
	{
		perror("fopen");
		return -1;
	}
	while (fscanf(fp, "%[^\n]%*c\n", r_buf) != EOF)
	{
		add_wechat_message(r_buf + strlen(SAVE_URL_PRE));
	}
	fclose(fp);
	return 0;
}
#include <stdio.h>
int main(int argc,char* argv[])
{
    FILE* fin;
    char line[100];
    fin = fopen("test.txt","r");
    while(fscanf(fin,"%[^\n]%*c\n",line)!=EOF){
        1653printf("%s\n",line);
    }
     
    fclose(fin);
    return 1;
}

//写入一行文件
static int write_wechat_message_to_file(const char* buf){
	FILE* fp = fopen(WECHAT_MESSAGE,"a+");
	if(fp == NULL){
		perror("fopen");
		return -1;
	}
	//fwrite(buf,size,1,fp);
	fprintf(fp,"%s\n",buf);
	fclose(fp);
	return 0;
}
```


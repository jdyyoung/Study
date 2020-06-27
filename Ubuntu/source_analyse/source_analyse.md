Ubuntu 下的源码分析：

源码分析：静态分析 C 程序函数调用关系图 - 泰晓科技 - http://tinylab.org/callgraph-draw-the-calltree-of-c-functions/

安装软件：


```
sudo apt-get install cflow graphviz gawk
```

安装脚本：tree2dotx 和callgraph

```
$ sudo cp tree2dotx callgraph /usr/local/bin
$ sudo chmod +x /usr/local/bin/{tree2dotx,callgraph}
```

运用：

```
$ callgraph -f start_kernel -d init/main.c
$ callgraph -f start_kernel -d init/main.c -F printk
$ callgraph -f start_kernel -d init/main.c -F "printk boot_cpu_init rest_init"
```

-f ：指定函数名

-F : 砍掉不感兴趣的函数分支，如果要砍掉很多函数，则可以指定一个函数列表，所以可写相应的脚本来分析源码

-d：指定函数所在文件（或者文件所在的目录）

-D：指定函数调用深度



TODO:是否自己加个选项排除指定头文件的函数
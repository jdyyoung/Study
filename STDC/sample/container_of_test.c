#include <stdio.h>
/*
这个宏的作用就是:
通过一个容器（结构体）中某个成员的指针得到指向这个容器（结构体）的指针，
简单的说就是通过成员找容器。
*/
/**
 * container_of - 通过结构体的一个成员获取容器结构体的指针
 * @ptr: 指向成员的指针。
 * @type: 成员所嵌入的容器结构体类型。
 * @member: 结构体中的成员名。
 *
 */
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({              \         
const typeof( ((type *)0)->member ) *__mptr = (ptr);    \         
(type *)( (char *)__mptr - offsetof(type,member) );})

int main(int argc, char *argv[])
{
    struct test{
        int num;
        char ch; 
    }t1={100,'c'};
    char *pch=&t1.ch;
    struct test *ptt=container_of(pch,struct test,ch);
    printf("num=%d\n",ptt->num);

    return 0;
}


/*
container of()函数简介_遇见你是我最美丽的意外-CSDN博客_container_of函数 - https://blog.csdn.net/s2603898260/article/details/79371024
Linux内核中container_of的原理及其使用详解 - 灰信网（软件开发博客聚合） - https://www.freesion.com/article/74441050994/
Linux内核container_of详解_Tony的博客-CSDN博客 - https://blog.csdn.net/wangliang888888/article/details/51138664
container_of学习笔记 - 立体风 - 博客园 - https://www.cnblogs.com/litifeng/p/7690585.html
*/
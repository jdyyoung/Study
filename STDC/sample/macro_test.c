/*
make DFLAGS=-DLOG_SAVE_DIR=\\\"/tmp/log_test1\\\"

rck@rck:/vtcs/code/log_lib_0831/log_lib$ gcc -o macro_test macro_test.c -DS=\"abc\" -DA=100
rck@rck:/vtcs/code/log_lib_0831/log_lib$ ./macro_test   
a:100
s:abc
rck@rck:/vtcs/code/log_lib_0831/log_lib$ gcc -o macro_test macro_test.c 
rck@rck:/vtcs/code/log_lib_0831/log_lib$ ./macro_test 
a:10
s:ab
rck@rck:/vtcs/code/log_lib_0831/log_lib$
*/

#include <stdio.h>
#ifndef S
#define S "ab"
#endif
#ifndef A
#define A 10
#endif
void main()
{
	char s[]=S;
	char a=A;
	printf("a:%d\n",a);
	printf("s:%s\n",s);
}

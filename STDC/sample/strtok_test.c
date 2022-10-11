#include<stdio.h>
#include<string.h>

int main(){
    char* log_rootpathname = "/tmp/logs/debug";
    char dirname[128];
    char* tok;
    snprintf(dirname, sizeof(dirname), "%s/%s", log_rootpathname, "debug");
    strcat(dirname, ".crc");
    printf("before --> dirname=%s\n",dirname);
    tok = strtok(dirname,".");
    printf("tok=%s\n",tok);
    sprintf(dirname, "%s", tok);
    printf("after --> dirname=%s\n",dirname);
    return 0;
}
/*
rck@rck:/vtcs/code$ ./a.out 
before --> dirname=/tmp/logs/debug/debug.crc
tok=/tmp/logs/debug/debug
after --> dirname=/tmp/logs/debug/debug
rck@rck:/vtcs/code$
*/
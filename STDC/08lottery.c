/*
    彩票练习
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main() {
    int lottery[7] = {}, num = 0;
    srand(time(0));
    for (num = 0;num <= 6;num++) {
        lottery[num] = rand() % 36 + 1;
    }
    for (num = 0;num <= 6;num++) {
        printf("%d ", lottery[num]);
    }
    printf("\n");
    return 0;
}






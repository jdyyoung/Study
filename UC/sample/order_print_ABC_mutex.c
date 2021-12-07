/*
互斥锁
标志

flag  = 0 
metux_lock
if(flag % 3 ==0){
	printf()
	flag= 1	
}
metux_unlock

metux_lock
if(flag % 3 ==1){
	printf()
	flag= 2	
}
metux_unlock

metux_lock
if(flag % 3 ==2){
	printf()
	flag= 0
}
metux_unlock
*/

#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

#define MAX_PRINT_CNT 30
pthread_mutex_t mutex;
static int print_flag = 0;
static int print_cnt = 0;
 
void* print_A(void* arg){
    while(print_cnt < MAX_PRINT_CNT){
		//进行加锁
		pthread_mutex_lock(&mutex);
		if(print_flag % 3 ==0){
			printf("A");
			print_flag = 1;
			print_cnt ++;
		}
		//进行解锁
		pthread_mutex_unlock(&mutex);
    }
	return NULL;
}

void* print_B(void* arg){
    while(print_cnt < MAX_PRINT_CNT){
		//进行加锁
		pthread_mutex_lock(&mutex);
		if(print_flag % 3 == 1){
			printf("B");
			print_flag = 2;
			print_cnt ++;
		}
		//进行解锁
		pthread_mutex_unlock(&mutex);
    }
	return NULL;
}

void* print_C(void* arg){
    while(print_cnt < MAX_PRINT_CNT){
		//进行加锁
		pthread_mutex_lock(&mutex);
		if(print_flag % 3 == 2){
			printf("C");
			print_flag = 0;
			print_cnt ++;
		}

		//进行解锁
		pthread_mutex_unlock(&mutex);
    }
	return NULL;
}

int main()
{
	pthread_t id[3];
	//初始化互斥量
	pthread_mutex_init(&mutex,NULL);

	pthread_create(&id[0],NULL,print_A,NULL);
	pthread_create(&id[1],NULL,print_B,NULL);
	pthread_create(&id[2],NULL,print_C,NULL);

	pthread_join(id[0],NULL);
	pthread_join(id[1],NULL);
	pthread_join(id[2],NULL);
	
	//销毁互斥量
	pthread_mutex_destroy(&mutex);
 
    return 0；
}
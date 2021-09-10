/*
	使用三个线程下的信号量实现顺序打印ABC_嵌入式-CSDN博客 
	- https://blog.csdn.net/zxy131072/article/details/108117973?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_title~default-4.control&spm=1001.2101.3001.4242
*/
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define PTHREAD_NUM 3

void * print_a(void *);
void * print_b(void *);
void * print_c(void *);

typedef void *(*pthread_func_t)(void *);

typedef struct abc_demo{
	sem_t sem_array[PTHREAD_NUM];
	char *retval[PTHREAD_NUM];
	pthread_t tid_array[PTHREAD_NUM];
	pthread_func_t pfunc_array[PTHREAD_NUM];
}abc_demo;

abc_demo abc = {
	.pfunc_array = {print_a, print_b, print_c},
};

static int g_count = 0;
void *print_a(void *arg)
{
	while(1){
		sem_wait(&(abc.sem_array[0]));
		printf("A------%d----------\n", g_count++);
		sem_post(&(abc.sem_array[1]));
        if(g_count == 5 )
            pthread_exit((void*)1);
		sleep(1);
	}
}
void *print_b(void *arg)
{
	while(1){
		sem_wait(&(abc.sem_array[1]));
		printf("B+++++++++++++++++++++++++++++\n");
		sem_post(&(abc.sem_array[2]));
        if(g_count == 5 )
            pthread_exit((void*)1);
		sleep(1);
	}
}
void *print_c(void *arg)
{	
	while(1){
		sem_wait(&(abc.sem_array[2]));
		printf("C\n");
    	sem_post(&(abc.sem_array[0]));
        if(g_count == 5 )
            pthread_exit((void*)1);
		sleep(1);
	}
}

int main(int argc, const char *argv[])
{
	int i,ret;

	sem_init(&(abc.sem_array[0]),0, 1);
	sem_init(&(abc.sem_array[1]),0, 0);
	sem_init(&(abc.sem_array[2]),0, 0);

	for(i = 0; i < PTHREAD_NUM; i++)
    {
		ret = pthread_create(abc.tid_array+i, NULL, abc.pfunc_array[i], NULL);
		if(ret < 0)
        {
			perror("pthread_create");
			return -1;
		}
	} 

	for(i = 0; i < PTHREAD_NUM; i++)
    {
    		ret = pthread_join(abc.tid_array[i], (void **)abc.retval+i);
            if(ret < 0)
             {
    			perror("pthread_join");
    			return -1;
		     }
	}
    
	sem_destroy(&(abc.sem_array[0]));
	sem_destroy(&(abc.sem_array[1]));
	sem_destroy(&(abc.sem_array[2]));
	
	return 0;
}



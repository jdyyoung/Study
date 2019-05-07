#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

pthread_t tid_1,tid_2,tid_3;
int run_1,run_2;

void* thread_2(void* parm){
	while(run_2){
		printf("%s:this second thread\n",__func__);
		sleep(1);
	}
}
void* thread_1(void* parm){
	run_2 = 1;
	pthread_create(&tid_2,NULL,thread_2,NULL);
	while(run_1){
		printf("%s:this first thread\n",__func__);
		sleep(1);
	}
}


int main(){
	run_1 = 1;
	printf("this is %d line\n",__LINE__);
	pthread_create(&tid_1,NULL,thread_1,NULL);
	sleep(3);
	printf("this is %d line\n",__LINE__);
	run_1 = 0;
	pthread_join(tid_1,NULL);
	printf("%d:first thread is dead \n",__LINE__);
	sleep(2);
	//测试cancel 已经死掉的线程
	pthread_create(&tid_3,NULL,thread_1,NULL);

	printf("%d:---------------------%p\n",__LINE__,tid_1);
	pthread_cancel(tid_1);
	run_2 = 0;
	pthread_join(tid_2,NULL);
	printf("%d:second thread is dead \n",__LINE__);
	sleep(1);
	printf("this is %d end end line\n",__LINE__);
	return 0;
}

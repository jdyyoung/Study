#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#define MAX_STOCK 5 // 仓库容量
char g_storage[MAX_STOCK]; // 仓库
size_t g_stock = 0; // 当前库存
pthread_mutex_t g_mtx = PTHREAD_MUTEX_INITIALIZER;
// 非满，生产的条件
pthread_cond_t g_full = PTHREAD_COND_INITIALIZER;
// 非空，消费的条件
pthread_cond_t g_empty = PTHREAD_COND_INITIALIZER;
// 显示库存
void show (char const* who, char const* op,
	char prod) {
	printf ("%s：", who);
	size_t i;
	for (i = 0; i < g_stock; ++i)
		printf ("%c", g_storage[i]);
	printf ("%s%c\n", op, prod);
}
// 生产者线程
void* producer (void* arg) {
	char const* who = (char const*)arg;
	for (;;) {
		pthread_mutex_lock (&g_mtx);
		
		//判断当前库存 是否大于等于 仓库容量 ，条件成立则满仓啦 停止生产 进程进入休眠等待状态
		while (g_stock >= MAX_STOCK)
		{
			printf ("\033[;;32m%s：满仓！\033[0m\n",who);
			//执行这一句使当前线程进入睡眠等待状态，设置使用条件变量g_full可以唤醒
			pthread_cond_wait (&g_full, &g_mtx);
		}
		//------------------------------生产的代码-----------------------------
		char prod = 'A' + rand () % 26;
		show (who, "<-", prod);
		g_storage[g_stock++] = prod;
		//------------------------------生产的代码-----------------------------
//		pthread_cond_signal (&g_empty);

		//生产过后仓库就不为空了，可以唤醒消费者消费了
		pthread_cond_broadcast (&g_empty);//唤醒条件变量g_empty的等待进程
		
		pthread_mutex_unlock (&g_mtx);
		usleep ((rand () % 100) * 1000);
	}
	return NULL;
}
// 消费者线程
void* customer (void* arg) {
	char const* who = (char const*)arg;
	for (;;) {
		pthread_mutex_lock (&g_mtx);
		
		//判断当前库存是否为空  条件成立则没东西可以消费啦 进程进入休眠等待状态
		while (! g_stock) {
			printf ("\033[;;31m%s：空仓！\033[0m\n",who);
			//执行这一句使当前线程进入睡眠等待状态，设置使用条件变量g_empty可以唤醒
			pthread_cond_wait (&g_empty, &g_mtx);
		}
		
		//-------------------消费的代码---------------------------------------
		char prod = g_storage[--g_stock];
		show (who, "->", prod);
		//-------------------消费的代码---------------------------------------
		
//		pthread_cond_signal (&g_full);
		//消费过后仓库就不会爆仓啦 可以唤醒生产者生产  
		pthread_cond_broadcast (&g_full);//唤醒条件变量g_full的等待进程
		
		pthread_mutex_unlock (&g_mtx);
		usleep ((rand () % 100) * 1000);
	}
	return NULL;
}
int main (void) {
	srand (time (NULL));
	pthread_t tid;
	pthread_create (&tid, NULL, producer, "生产者1");
	pthread_create (&tid, NULL, producer, "生产者2");
	pthread_create (&tid, NULL, producer, "生产者3");
	pthread_create (&tid, NULL, producer, "生产者4");
	pthread_create (&tid, NULL, customer, "消费者1");
	pthread_create (&tid, NULL, customer, "消费者2");
	pthread_create (&tid, NULL, customer, "消费者3");
	pthread_create (&tid, NULL, customer, "消费者4");
	getchar ();
	return 0;
}

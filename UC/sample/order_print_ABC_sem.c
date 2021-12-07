/*
使用三个线程下的信号量实现顺序打印ABC_嵌入式-CSDN博客 - https://blog.csdn.net/zxy131072/article/details/108117973
linux编程实现三个线程ABC，并让它们顺次打印自己的ID，使用信号量_gdut17的博客-CSDN博客 - https://blog.csdn.net/beyond706/article/details/102861917?utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromMachineLearnPai2%7Edefault-4.essearch_pc_relevant&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromMachineLearnPai2%7Edefault-4.essearch_pc_relevant
Linux编程题：创建3个线程分别打印abc，用信号量进行同步_cleverlemon的博客-CSDN博客 - https://blog.csdn.net/cleverlemon/article/details/104280306?utm_medium=distribute.pc_relevant.none-task-blog-2~default~BlogCommendFromBaidu~default-1.essearch_pc_relevant
*/
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<stdlib.h>

sem_t sem[3];
 
void* fun1(void* arg)
{
     while(1)
     {
           sem_wait(&sem[0]);
           write(1,"a",1);
           sem_post(&sem[1]);
     }
}
void* fun2(void* arg)
{
     while(1)
     {
          sem_wait(&sem[1]);
          write(1,"b",1);
          sem_post(&sem[2]);
     }
}
void* fun3(void* arg)
{
      while(1)
      {
          sem_wait(&sem[2]);
          write(1,"c",1);
          sem_post(&sem[0]);
      }
}
int main()
{
      pthread_t id[3];

      sem_init(&sem[0],0,1);
      sem_init(&sem[1],0,0);
      sem_init(&sem[2],0,0);
 
      pthread_create(&id[0],NULL,fun1,NULL);
      pthread_create(&id[1],NULL,fun2,NULL);
      pthread_create(&id[2],NULL,fun3,NULL);
 
      pthread_join(id[0],NULL);
      pthread_join(id[1],NULL);
      pthread_join(id[2],NULL);
 
      sem_destroy(&sem[0]);
      sem_destroy(&sem[1]);
      sem_destroy(&sem[2]);
 
      exit(0);
}
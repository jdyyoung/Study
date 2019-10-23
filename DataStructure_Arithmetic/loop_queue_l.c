/*
(2条消息)循环队列-链式存储结构-c语言实现 - 王腾涛 - CSDN博客 - https://blog.csdn.net/u010187139/article/details/46742711
循环队列-链式存储结构-c语言实现
*/

/*
    循环队列-线性表-链式结构 
*/
#include<stdio.h>
#include<stdlib.h>
#define OK 1
#define ERROR 0

typedef int Status;
typedef int QElemType; 

//节点 定义 
typedef struct QNode{       
    QElemType data;
    struct QNode *next;
} QNode, *QueuePtr;
// 队列 结构 
typedef struct {    
    QueuePtr front, rear;   //队头和队尾指针 
} LinkQueue;

//初始化 队列
Status initQueue(LinkQueue *q){
    QueuePtr s = (QueuePtr)malloc(sizeof(QNode));   //定义一个头结点
    s->next = NULL;
    q->rear = q->front = s; //front和rear都指向头结点
    printf("队列初始化完成 \n"); 
    return OK;
} 

//入队 插入元素e到队尾
Status enQueue(LinkQueue *q ,QElemType e){
    QueuePtr s = (QueuePtr)malloc(sizeof(QNode));   //定义一个头结点
    if(!s){
        printf("存储分配失败 \n");
        return ERROR;
    }
    s->next = NULL;
    s->data = e;
    q->rear->next = s;  //新节点为原队尾节点的后继 
    q->rear = s;    //当前的s为队尾节点
    return OK; 
} 

//出队 ,删除队头元素,用e返回要删除的元素 q->front 为头结点 
Status deQueue(LinkQueue *q, QElemType *e){
    //判断是否为null
    if(q->front == q->rear){
        printf("队列为null 无法出队 \n");
        return ERROR;
    } 
    QueuePtr p; 
    p = q->front->next; //将出队的节点 存在p中 
    q->front->next = p->next;   //重新构造队头元素的后继 
    *e = p->data;   //保存出队的数据 
    //判断删除的节点是否为队尾元素
    if(q->rear == p) {
        q->rear = q->front;
    }
    free(p);
    return OK;
}

//遍历队列
void queueTraverse(LinkQueue q) {
    //判断队列是否为null
    if(q.front == q.rear){
        printf("队列为null \n");
    } else{
        QueuePtr p = q.front->next;
        while(p){
            printf("%d ", p->data);
            p = p->next;
        }
        printf("\n");
    }
}
int main(void){
    LinkQueue q;
    QElemType e; 
    initQueue(&q);
    printf("\n 1.初始化队列 \n 2.遍历队列 \n 3.入队 \n 4.出队 \n 0.退出\n");
    int option = 1, value;
    while(option){
        scanf("%d",&option);
        switch(option){
            case 1:
                initQueue(&q);
                break;
            case 2:
                queueTraverse(q);
                break;
            case 3:
                printf("请输入要入队的数据\n");
                scanf("%d",&e);
                enQueue(&q, e); 
                queueTraverse(q);
                break;
            case 4:
                value = deQueue(&q, &e);
                if(value == 1){
                    printf("出队的元素是:%d\n",e);
                }
                break;
            case 0:
                return OK;
        }
    }   
    return OK;
} 

/*
队列: 先进先出型数据结构

链式存储的优点:

使用空间上,链式存储更加灵活.不需要提前分配空间,不会造成溢出
时间上,入队和出队的操作都是常数,即O(1)
*/

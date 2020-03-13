/*
 *循环队列-顺序存储-c语言实现 - 王腾涛 - CSDN博客
http://blog.csdn.net/u010187139/article/details/46742967
 *循环队列--线性表--顺序结构
 */

#include<stdio.h>
#include<stdlib.h>

#define OK 1
#define ERROR 0
#define MAXSIZE 20

typedef int Status;
typedef int QElemType;

//定义节点 
typedef struct QNode{
    QElemType data[MAXSIZE];
    int front;//头指针
    int rear;//尾指针
}sqQueue;

//初始化队列front=rear 则队列为空
Status initQueue(sqQueue* q)
{
    q->front=0;
    q->rear=0;
    printf("队列初始化成功！\n");
    return OK;
}

//求队列长度
Status queueLength(sqQueue* q)
{
    return (q->rear-q->front+MAXSIZE)%MAXSIZE;
}

//入队，如果队没有满，插入元素到e到q的队尾元素
Status enQueue(sqQueue* s,QElemType* e)
{
    //判断队列是否为满
    if((s->rear+1)%MAXSIZE==s->front)
    {
        printf("队列已满了无法插入队列\n");
        return ERROR;
    }
    s->data[s->rear]=*e;
    s->rear=(s->rear+1)%MAXSIZE;//队尾指针后移
    return OK;
}

//出队，删除队头元素，返回删除的元素值
Status DeQueue(sqQueue* q,QElemType* e)
{
    //判断队列是否为空
    if(q->rear == q->front)
    {
        printf("队列为空，无法出队\n");
        return ERROR;
    }

    *e=q->data[q->front];//保存数值
    q->front=(q->front+1)%MAXSIZE;//对头指针后移
}

//遍历队列
Status queueTraverse(sqQueue* q)
{
    //判断队列是否为空
    if(q->rear==q->front)
    {
        printf("队列为空，无法输出\n");
        return ERROR;
    }
    else
    {
        int i,length;
        length=queueLength(q);
        for(i=0;i<length;i++)
        {
            printf("%d ",q->data[q->front]);
            q->front=(q->front+1)%MAXSIZE;
        }
        printf("\n");
    }
}
int main(void){
    sqQueue q;
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
                queueTraverse(&q);
                break;
            case 3:
                printf("请输入要入队的数据\n");
                scanf("%d",&e);
                enQueue(&q,&e); 
                //queueTraverse(&q);
                break;
            case 4:
                value = DeQueue(&q, &e);
                if(value){
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
 *
 * 队列: 先进先出型数据结构
 *
 * 循环队列计算队列长度: (rear - front + QueueSize) % QueueSize 
 * 队列满的条件是: (rear + 1) % QueueSize == front [ 需要数组保留一个元素空间 ] 
 * 队列空的条件是: front = rear
 *
 * 顺序存储的优点和缺点:
 *
 * 时间上,入队和出队的操作都是常数,即O(1)
 * 事先需要申请好空间,使用期间不释放.
 * 必须有一个固定的长度,也就有了存储元素个数和空间浪费的问题
 *
 * 如果能确定队列长度的最大值情况下, 建议使用循环队列, 如果无法确定队列的长度时, 用链队列比较好.
 //----------------------20200302------------------
 重点关键点：循环：+1取余就是开始循环了
 * */

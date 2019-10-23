#ifndef _yc_queue_h_
#define _yc_queue_h_
#include <stdio.h>
#include <stdlib.h>
//数据域
typedef struct EleType
{
	char res[4096];
	int len;
} EleType;

//链队列结点
typedef struct QueueNode
{
	EleType data;	 //数据域
	struct QueueNode *next; //指针域
} QueueNode, *LinkQueuePoi;

typedef struct LinkQueue
{
	LinkQueuePoi front; //指向头结点
	LinkQueuePoi rear;  //指向队尾
} LinkQueue;

/*
初始化链队列
链队列为空时，链队列队头指针队尾指针均指向头结点
*/
int InitLinkQueue(LinkQueue *queue);

/*
清空链队列
将所有元素释放
*/
int ClearLinkQueue(LinkQueue *queue);

/*
判断链队列是否为空队列
*/
int EmptyLinkQueue(LinkQueue *queue);

/*
获取链队列长度
*/
int LengthLinkQueue(LinkQueue *queue);

/*
在链队列队尾添加元素
先将新元素添加到链表尾部，然后将队列尾指针指向这个新元素
*/
int AddQueue(LinkQueue *queue, EleType data);

/*
从链队列中删除队头元素
先将头结结点指向新的队头结点，然后释放原来的队头结点
*/
int DelQueue(LinkQueue *queue, EleType *data);

/*
打印链队列元素
*/
void PrintfLinkQueue(LinkQueue *queue);

#endif //_yc_queue_h_
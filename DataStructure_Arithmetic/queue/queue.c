#include <stdio.h>
#include <stdlib.h>
#include "queue.h"


/*
(2条消息)队列的链式存储结构及实现 - 李四老师 - CSDN博客 - https://blog.csdn.net/qq_29542611/article/details/78907339
*/
/*
初始化链队列
链队列为空时，链队列队头指针队尾指针均指向头结点
*/
int InitLinkQueue(LinkQueue* queue)
{
	//空指针
	if (NULL == queue)
	{
		return -1;
	}
	QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));//头结点
	node->next = NULL;
	queue->front = queue->rear = node;
	return 0;
}
/*
清空链队列
将所有元素释放
*/
int ClearLinkQueue(LinkQueue* queue)
{
	//空指针
	if (NULL == queue)
	{
		return -1;
	}
	//空链队列
	if (queue->front == queue->rear)
	{
		return -1;
	}
	QueueNode* node = queue->front->next;//队头元素
	while (node)
	{
 
		queue->front->next = node->next;//指向新的队头结点
		if (queue->rear == node)//当删除的是队尾元素时，将队尾指针指向头结点
		{
			queue->rear = queue->front;
		}
		free(node);//释放旧的队头结点
		node = queue->front->next;
	}
	return 0;
}
/*
判断链队列是否为空队列
*/
int EmptyLinkQueue(LinkQueue* queue)
{
	//空指针
	if (NULL == queue)
	{
		return -1;
	}
	//空链队列
	if (queue->front == queue->rear)
	{
		return 0;
	}
	return 1;
}
/*
获取链队列长度
*/
int LengthLinkQueue(LinkQueue* queue)
{
	//空指针
	if (NULL == queue)
	{
		return -1;
	}
	//空链队列
	if (queue->front == queue->rear)
	{
		return 0;
	}
	QueueNode* node = queue->front->next;
	int num = 0;
	while (node)
	{
		node = node->next;
		num++;
	}
	return num;
}
/*
在链队列队尾添加元素
先将新元素添加到链表尾部，然后将队列尾指针指向这个新元素
*/
int AddQueue(LinkQueue* queue,EleType data)
{
	//空指针
	if (NULL == queue)
	{
		return -1;
	}
	QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));
	if (NULL == node)
	{
		return -1;
	}
	node->next = NULL;
	node->data = data;
	queue->rear->next = node;//将新结点添加到链表表中
	queue->rear = node;//队尾指针指向新的队尾结点
	return 0;
}
/*
从链队列中删除队头元素
先将头结结点指向新的队头结点，然后释放原来的队头结点
*/
int DelQueue(LinkQueue* queue, EleType *data)
{
	//空指针
	if (NULL == queue)
	{
		return -1;
	}
	//注意queue->front是头结点，头结点指向的结点才是队头结点
	QueueNode* node = queue->front->next;//旧队头结点
	*data = node->data;
	queue->front->next = node->next;//队头指针指向新的队头结点
	//当删除的是队尾元素时，将队尾指针指向头结点
	if (node == queue->rear)
	{
		queue->rear = queue->front;
	}
	free(node);
	return 0;
}
/*
打印链队列元素
*/
void PrintfLinkQueue(LinkQueue* queue)
{
	if (NULL == queue)
	{
		return;
	}
	QueueNode* node = queue->front->next;
	while (node)
	{
		printf("%s %d,", node->data.res,node->data.len);
		node = node->next;
	}
	printf("\n");
	return;
}

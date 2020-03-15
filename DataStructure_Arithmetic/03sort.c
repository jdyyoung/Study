//常见的排序算法的使用
#include <stdio.h>

//实现冒泡排序算法
void bubble(int arr[],int len)
{
	//1.首先使用外层循环控制比较的轮数
	int i = 0,j = 0;
	for(i = 1; i < len; i++) 
	{
		//2.针对每一轮进行下标控制
		for(j = 0; j < len-i; j++) 
		{
			//3.比较相邻元素的大小，进行交换
			if(arr[j] > arr[j+1])
			{
				int temp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = temp;
			}
		}
	}
}

//实现插入排序算法
void insert(int arr[],int len)
{
	//1.从第二个元素起，依次取出
	int i = 0,j = 0;
	for(i = 1; i < len; i++)
	{
		//单独保存当前取出的元素，避免覆盖
		int temp = arr[i];
		//2.取出的元素依次与左边的元素进行比较,如果左边有元素并且大于取出的元素，则左边元素右移
		for(j = i; arr[j-1] > temp && j >=1; j--)
		{
			arr[j] = arr[j-1];
		}
		//3.直到左边元素小于取出/左边无元素，将取出元素插入到左边元素的右边/最左边
		if(j != i)
		{
			arr[j] = temp;
		}
	}
}

//实现选择排序算法
void choose(int arr[],int len)
{
	//1.从第一个数起，依次取出
	int i = 0,j = 0;
	for(i = 0; i < len-1; i++)
	{
		//2.假定取出的当前元素是最小的,使用min记录
		int min = i;
		//3.使用min记录的元素与后续元素比较，如果找到比min记录元素还小的元素，则使用min重新记录
		for(j = i+1; j < len; j++)
		{
			if(arr[j] < arr[min])
			{
				min = j;
			}
		}
		//4.直到与后续元素比较完毕，则交换min记录的元素与最开始假定的最小值
		if(i != min)
		{
			int temp = arr[i];
			arr[i] = arr[min];
			arr[min] = temp;
		}
	}
}

//实现快速排序算法
void quick(int arr[],int left,int right)
{
	//1.计算中间元素的下标
	int p = (left+right)/2;
	//2.选择中间元素作为基准值，单独保存起来
	int pivot = arr[p];
	//3.将所有小于基准值的元素放在基准值的左边，将所有大于等于基准值的元素放在基准值的右边
	int i = 0,j = 0;
	for(i = left,j = right; i < j; )
	{
		//如果左边有元素，并且左边元素小于基准值，则使用下一个左边元素与基准值比较
		while(arr[i] < pivot && i < p)
		{
			i++;
		}
		//如果左边有元素，并且不小于基准值
		if(i < p)
		{
			//把i指向的元素赋值给p指向的位置
			arr[p] = arr[i];
			//变量p指向i指向的位置
			p = i;
		}
		//下面开始处理右边的元素
		while(arr[j] >= pivot && j > p)
		{
			j--;
		}
		if(j > p)
		{
			arr[p] = arr[j];
			p = j;
		}
	}
	//4.i和j重合，将基准值放到重合的位置
	arr[p] = pivot;
	//5.使用递归重复以上过程
	//保证基准值的左右两侧存在至少一个元素时才需要递归，否则不需要递归
	if(p - left > 1)
	{
		quick(arr,left,p-1);
	}
	if(right - p > 1)
	{
		quick(arr,p+1,right);
	}

}


int main(void)
{
	int arr[9] = {20,8,18,3,15,5,25,10,22};
	//bubble(arr,9);
	//insert(arr,9);
	//choose(arr,9);
	quick(arr,0,8);
	printf("排序之后的结果是：");
	int i = 0;
	for(i = 0; i < 9; i++)
	{
		printf("%d ",arr[i]);
	}
	printf("\n");
	return 0;
}


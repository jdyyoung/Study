//二分查找算法的实现
#include <stdio.h>


//实现二分查找的功能函数
int find(int arr[],int left,int right,int data)
{
	//保证数组中确实存在元素
	if(left <= right)
	{
		//计算中间元素的下标
		int p = (left+right)/2;
		//寻找中间元素进行比较
		if(data == arr[p])
		{
			return p;
		}
		//如果目标元素小于中间元素，左边查找
		else if(data < arr[p])
		{
			return find(arr,left,p-1,data);
		}
		//如果目标元素大于中间元素，右边查找
		else
		{
			return find(arr,p+1,right,data);
		}
	}
	return -1;//表示查找失败
}

int main(void)
{
	int arr[5] = {1,2,3,4,5};
	printf("目标元素所在的下标是：%d\n",find(arr,0,4,4));//3
	return 0;
}



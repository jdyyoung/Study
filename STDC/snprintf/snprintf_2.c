#include <stdio.h>
#include <strings.h>
#include <string.h>
/*
 *【情形二】:源串等于目标字符串

 源串为"abcdefghi\0"，正好将源串拷贝到目标字符串中,返回值为源串的strlen为9
 * */
int main(void){
	char a[10] = {'\0'};
	int i = 0;
	int ret = 0;
	memset(a, 1, sizeof(a));
	for(i = 0; i < 10; i++){
		printf("a[%d] = %d\n",i,a[i]);
	}
	//ret = snprintf(a, 10, "%d", 123);
	ret = snprintf(a, 10, "%s", "abcdefghi");
	printf("ret = %d\n",ret);
	for(i = 0; i < 10; i++)
	{
		printf("a[%d] = %d\n",i,a[i]);
	}
	return 0;
}

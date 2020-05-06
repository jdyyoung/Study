/*
	判断一个整数是否为2的幂 以及 上向2的幂扩展 - 你好阿汤哥 - 博客园 - https://www.cnblogs.com/tangxin-blog/p/6131326.html
*/
#include <stdio.h>
#include <stdint.h>

// 判断整数是否为2幂
/*
if (a & (a - 1))
{
    // not
}
else
{
    // yes
}
*/

uint32_t roundup_pow_of_two(const uint32_t x)
{
    if (x == 0){ return 0; }
    if (x == 1){ return 2; }
    uint32_t ret = 1;
    while (ret < x)
    {
        ret = ret << 1;
    }
    return ret;
}

int32_t main()
{
    uint32_t a = 0;
    while (scanf_s("%u",&a))
    {
        printf("%u\n", roundup_pow_of_two(a));
    }
    return 0;
}
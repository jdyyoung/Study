#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LED_ON  0x100001 //开灯
#define LED_OFF 0x100002 //关灯

int main(void)
{
    int fd;
    int index; //用户缓冲区

    //open->软中断->sys_open->led_open
    fd = open("/dev/myled", O_RDWR);
    if (fd < 0)
        return -1;

    while(1) {
        index = 1;
        ioctl(fd, LED_ON, &index);
        usleep(100000);
        
        index = 1;
        ioctl(fd, LED_OFF, &index);
        usleep(100000);
    }
    
    //close->软中断->sys_close->led_close
    close(fd);
    return 0;
}

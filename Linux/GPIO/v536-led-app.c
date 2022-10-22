#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int main(int argc, char **argv)
{
    int fd;
    char LED_ON = 1, LED_OFF = 0;

    fd = open("/dev/V853_LED", O_RDWR);
    if (fd < 0)
    {
        perror("open device V853_LED error");
    }
    while (1)
    {
        write(fd, &LED_ON, 1);
        sleep(1);
        write(fd, &LED_OFF, 1);
        sleep(1);
    }
    return 0;
}
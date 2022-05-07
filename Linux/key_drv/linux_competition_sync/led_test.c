#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
    int fd;

    fd = open("/dev/myled", O_RDWR);
    if (fd < 0)
        return -1;

    sleep(10000);

    close(fd);
    return 0;
}

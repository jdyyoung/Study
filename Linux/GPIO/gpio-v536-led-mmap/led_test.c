#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    int fd;
    unsigned char *gpiobase;
    unsigned long *gpiocon, *gpiodata;

    if (argc != 2) {
        printf("Usage: %s <on|off>\n", argv[0]);
        return -1;
    }

    fd = open("/dev/myled", O_RDWR);
    if (fd < 0)
        return -1;

    //gpiobase虚拟地址对应的物理地址0x0300B000 
	/*
	PH Configure Register:0x0300B000 + 0x00FC = **0x0300B0FC**
	PH Data Register: 0x0300B000 + 0x010C = **0x0300B10
	*/
    gpiobase = mmap(0, 0x1000, PROT_READ|PROT_WRITE, 
                        MAP_SHARED, fd, 0);

    //获取对应的寄存器的用户虚拟地址
    gpiocon = (unsigned long *)(gpiobase + 0xFC);
    gpiodata = (unsigned long *)(gpiobase + 0x10C);
   
    //配置为输出口
    *gpiocon &= ~(0xf << 8);
    *gpiocon |= (1 << 8);
    
    if (!strcmp(argv[1], "on"))
        *gpiodata |= (1 << 2);
    else
        *gpiodata &= ~(1 << 2);

    munmap(gpiobase, 0x1000);

    close(fd);
    return 0;
}




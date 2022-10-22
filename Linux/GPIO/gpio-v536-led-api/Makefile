obj-m += led_drv.o
KERNEL_SRC=~/lindenis-v536/lichee/linux-4.9
all:
	make -C $(KERNEL_SRC) SUBDIRS=$(PWD) modules
clean:
	make -C $(KERNEL_SRC) SUBDIRS=$(PWD) clean

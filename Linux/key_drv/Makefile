KERNELDIR := /vtcs/d3tech/Kernel_v2.4-rc1

obj-m += dts_drv_test.o

all:
	make -C $(KERNELDIR) SUBDIRS=$(PWD) modules
clean:
	make -C $(KERNELDIR) SUBDIRS=$(PWD) clean

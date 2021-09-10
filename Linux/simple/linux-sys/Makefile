KERNELDIR := /vtcs/vtcs_project/Kernel
obj-m += kobj_test.o
obj-m += kernel_sys_test.o
all:
	make -C $(KERNELDIR) SUBDIRS=$(PWD) modules
clean:
	make -C $(KERNELDIR) SUBDIRS=$(PWD) clean

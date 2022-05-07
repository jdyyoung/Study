KERNELDIR := /vtcs/vtcs_project/Kernel
#obj-m += kobj_test.o
#obj-m += key_drv_test.o
# obj-m += key_drv_test_tasklet.o
#obj-m += key_drv_test_workqueue.o
obj-m += linux_timer_drv.o
all:
	make -C $(KERNELDIR) SUBDIRS=$(PWD) modules
clean:
	make -C $(KERNELDIR) SUBDIRS=$(PWD) clean

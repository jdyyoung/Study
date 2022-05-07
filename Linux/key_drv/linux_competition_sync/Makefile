OBJ_M ?=
KERNELDIR := /vtcs/vtcs_project/Kernel
obj-m += ${OBJ_M}_drv.o
#obj-m += spin_lock_drv.o
#obj-m += semaphore_drv.o
all:
	make -C $(KERNELDIR) SUBDIRS=$(PWD) modules
clean:
	make -C $(KERNELDIR) SUBDIRS=$(PWD) clean

GPIO:

上拉，开漏

gpio 的相关编程：

电气原理图 --> 芯片datasheet (芯片原厂的文档) --> 查表计算，找出gpio序号

可能涉及内核的相关gpio的头文件？修改dts

应用层echo 与 cat的操作

---



```
以前在sugr:
ls /sys/kernel/debug/aml_reg
echo addr > paddr
cat paddr
echo addr val > paddr
cat paddr

下面两种方法，
1写驱动，给应用提供控制接口。驱动也不要用地址映射的方法去控制，要用标准的gpio接口
2、使用gpio的 sysfs interface。参考：
https://blog.csdn.net/mirkerson/article/details/8464231
https://blog.csdn.net/q1070292175/article/details/17307381

uboot下打印出dts:
fdt addr 0x1000000
fdt print
```


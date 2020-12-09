linux nand flash常用命令_kunkliu的博客-CSDN博客_linux flashdb - https://blog.csdn.net/kunkliu/article/details/88871268

```
cat /proc/mtd
flash_eraseall /dev/mtd4
nandwrite -p /dev/mtd4 /tmp/rootfs.sqfs
nandcomp 
```

---

Linux mtd system - 简书 - https://www.jianshu.com/p/89a94c1d3e72
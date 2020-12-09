1.PC端安装nfs服务器软件：

```
sudo apt-get install nfs-kernel-server nfs-common portmap -d
```

2.配置服务器：

```
sudo vi /etc/exports
```

添加一行配置：

```
/opt/rootfs *(rw,sync,no_root_squash)
```

​	/opt/rootfs 是NFS的共享目录，

​	*:这里可以指定IP,  *代表所有客户端无论IP是多少都可以访问该目录

​	rw:客户端对该目录的操作权限

​	sync:读写同步

​	no_root_squash:权限相关

还有其它参数具体意义可参考：

nfs原理及安装配置 - 风住 - 博客园 - https://www.cnblogs.com/whych/p/9196537.html

3.重启服务器：

```
sudo /etc/init.d/nfs-kernel-server restart
```

4.挂载测试：

```
sudo mount -t nfs 192.168.43.82:/opt/rootfs /mnt
```

结果如图：

![2020-05-13 00-58-45屏幕截图](./md_att/2020-05-13 00-58-45屏幕截图.png)

遇到的问题：

1.

```
young@young-desktop:~$ sudo /etc/init.d/nfs-kernel-server restart
[....] Restarting nfs-kernel-server (via systemctl): nfs-kernel-server.serviceJob for nfs-server.service failed because the control process exited with error code. See "systemctl status nfs-server.service" and "journalctl -xe" for details.
 failed!
```

因为配置文件出错

```
/opt/rootfs *(rw,sync.no_root_squash)
改成：
/opt/rootfs *(rw,sync,no_root_squash)
就OK了！
```

2.

```
young@young-desktop:~$ sudo mount -t nfs 192.168.43.82:/tmp /mnt
mount.nfs: access denied by server while mounting 192.168.43.82:/tmp
```

因为nfs共享的路径出错，配置文件没有这个路径，改成

```
young@young-desktop:~$ sudo mount -t nfs 192.168.43.82:/opt/rootfs /mnt
```

就可以了。

----

问题：

```
mount -o nolock -t nfs failed: Protocol not supported挂载失败
```

解决：

```
1.内核Network File Systems选项配置选上：

2.修改nfs配置文件：/etc/default/nfs-kernel-server
解决办法： 
Ubuntu 17.10 之后 nfs 默认就只支持协议3和协议4，但是 uboot 默认启动的是协议2，若想要求 nfs 支持协议2，需要在/etc/default/nfs-kernel-server末尾加一句：
	RPCNFSDOPTS="--nfs-version 2,3,4 --debug --syslog" 
然后重启nfs:sudo /etc/init.d/nfs-kernel-server restart 
```



解决在使用RK3308板卡使用NFS挂在服务器时，出现“mount.nfs: Protocol not supported”的问题_b7376811的博客-CSDN博客 - https://blog.csdn.net/b7376811/article/details/104764017?utm_medium=distribute.pc_relevant_t0.none-task-blog-BlogCommendFromMachineLearnPai2-1.edu_weight&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-BlogCommendFromMachineLearnPai2-1.edu_weight

u-boot无法通过nfs挂载处于Ubuntu 18.04服务器的根文件系统的解决_colin.tan-CSDN博客 - https://blog.csdn.net/qq_33475105/article/details/81628795
关于linux 登录密码相关:

➜  ~ openssl passwd -1 -salt '6AHjBnTn' cloud39ev200
$1$6AHjBnTn$LvoexcPTiWwZP5fLfCGdv1

---

[root@anyka ~]$ cat /etc/shadow 
root:$1$6AHjBnTn$LvoexcPTiWwZP5fLfCGdv1:0:0:99999:7:::
bin:*:10933:0:99999:7:::
daemon:*:10933:0:99999:7:::
nobody:*:10933:0:99999:7:::

[root@anyka ~]$

```
[root@anyka ~]$ cat /etc/inittab 
# /etc/inittab

# Copyright (C) 2010 Anyka

# Note: BusyBox init doesn't support runlevels.  The runlevels field is
# completely ignored by BusyBox init. If you want runlevels, use
# sysvinit.
#
# Format for each entry: <id>:<runlevels>:<action>:<process>
#
# id        == tty to run on, or empty for /dev/console
# runlevels == ignored
# action    == one of sysinit, respawn, askfirst, wait, and once
# process   == program to run

# Startup the system
::sysinit:/etc/init.d/rcS

ttySAK0::respawn:/sbin/getty -L ttySAK0 115200 vt100 # GENERIC_SERIAL

::restart:/sbin/init
::ctrlaltdel:/sbin/reboot
::shutdown:/bin/umount -a -r
::shutdown:/sbin/swapoff -a

[root@anyka ~]$ 
```



/etc/shadow中密码段的生成方式_python_贾己人的博客-CSDN博客 - https://blog.csdn.net/jiajiren11/article/details/80376371

Busybox 设置登录用户名、密码_操作系统_跬步千里，窥叶知秋-CSDN博客 - https://blog.csdn.net/JerryGou/article/details/79068144

/etc/passwd 和 /etc/shadow 文件内容及其解释_shell_只有十分努力，才能看起来毫不费力-CSDN博客 - https://blog.csdn.net/u012359618/article/details/51887117

手动生成/etc/shadow文件中的密码 - 骏马金龙 - 博客园 - https://www.cnblogs.com/f-ck-need-u/p/7545187.html



使用BusyBox制作根文件系统的操作步骤_python_清风徐来-CSDN博客 - https://blog.csdn.net/oqqHuTu12345678/a

使用busybox制作rootfs - 奇手 - https://www.veryarm.com/11535.html

busybox 文件系统设置 登陆 login 密码 password shadow_widibi2029_新浪博客 - http://blog.sina.com.cn/s/blog_1385b43530102vfs3.html

shadow文件中密码的加密方式 - xfangbao - 博客园 - https://www.cnblogs.com/xfbx/p/7464427.html

Linux shadow文件中密码的加密方式_操作系统_zwbill的专栏-CSDN博客 - https://blog.csdn.net/zwbill/article/details/79322374

```
passwd
hostname
```


### 					openert 驱动模块相关

1.openwrt 驱动模块加载的路程：

驱动模块放置的位置：

```
root@3c842:/# ls /lib/modules/4.9.198/
cfg80211.ko      dns_resolver.ko  nfs.ko           rtl88x2cs.ko
compat.ko        grace.ko         nfsv3.ko         sunrpc.ko
cryptodev.ko     lockd.ko         nfsv4.ko
```

build_dir/target-arm-linux_glibc/procd-2018-03-28-dfb68f85/initd/init.c

init 进程--> { "/sbin/kmodloader", "/etc/modules-boot.d/", NULL };

build_dir/target-arm-linux_glibc/ubox-2018-02-14-128bc35f/kmodloader.c

kmodloader 进程 --> lsmod/modprobe

会按数字大小加载以下路径下的驱动模块：

```
root@3c842:/# ls /etc/modules.d/
02-crypto-hash              30-fs-nfs-common
09-crypto-aead              40-fs-nfs
09-crypto-manager           41-fs-nfs-v3
09-crypto-null              41-fs-nfs-v4
10-crypto-hw-icatchtek-aes  50-cryptodev
25-nls-utf8                 50-rtl88x2cs
30-dnsresolver              80-fuse
```

2.添加一个openwrt 驱动模块

主要学习了这两篇关于openwrt 驱动开发的文章：

OpenWRT(五)驱动开发_嵌入式软件开发交流-CSDN博客 - https://blog.csdn.net/u012041204/article/details/54646691

超详细！手把手演示编译OpenWrt内核驱动模块_董哥的黑板报-CSDN博客 - https://blog.csdn.net/qq_41453285/article/details/102760270

编译遇到的问题一：

```
rck@rck:/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4$ make package/kernel/hello-kernel/compile V=s
Collecting package info: done
make[1]: Entering directory `/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4'
make[2]: Entering directory `/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/package/kernel/hello-kernel'
Makefile:43: *** missing separator.  Stop.
make[2]: Leaving directory `/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/package/kernel/hello-kernel'
time: package/kernel/hello-kernel/compile#0.04#0.00#0.17
make[1]: *** [package/kernel/hello-kernel/compile] Error 2
make[1]: Leaving directory `/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4'
make: *** [package/kernel/hello-kernel/compile] Error 2
rck@rck:/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4$ 
```

以上的问题，是因为Makefile没有涉及好（空格、缩进没有做好）。



编译遇到的问题二：

```
rck@rck:/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4$ make package/kernel/hello-kernel/compile V=s
Collecting package info: done
make[1]: Entering directory `/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4'
make[2]: Entering directory `/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/package/kernel/hello-kernel'
touch /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/.prepared_0b04c0676480b901c585cdd126e1ac03_6664517399ebbbc92a37c5bb081b5c53_check
true
[ ! -d ./src/ ] || cp -fpR ./src/. /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel
touch /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/.prepared_0b04c0676480b901c585cdd126e1ac03_6664517399ebbbc92a37c5bb081b5c53
rm -f /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/.configured_*
rm -f /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/staging_dir/target-arm-linux_glibc/stamp/.hello-kernel_installed
(cd /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/./; if [ -x ./configure ]; then find /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/ -name config.guess | xargs -r chmod u+w; find /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/ -name config.guess | xargs -r -n1 cp --remove-destination /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/scripts/config.guess; find /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/ -name config.sub | xargs -r chmod u+w; find /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/ -name config.sub | xargs -r -n1 cp --remove-destination /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/scripts/config.sub; AR="arm-openwrt-linux-ar" AS="arm-openwrt-linux-gcc -c -Os -pipe -mcpu=cortex-a7 -mfpu=vfp -fno-caller-saves -fno-plt -mfloat-abi=hard  -Wformat -Werror=format-security -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro" LD=arm-openwrt-linux-ld NM="arm-openwrt-linux-nm" CC="arm-openwrt-linux-gcc" GCC="arm-openwrt-linux-gcc" CXX="arm-openwrt-linux-g++" RANLIB="arm-openwrt-linux-ranlib" STRIP=arm-openwrt-linux-strip OBJCOPY=arm-openwrt-linux-objcopy OBJDUMP=arm-openwrt-linux-objdump SIZE=arm-openwrt-linux-size CFLAGS="-Os -pipe -mcpu=cortex-a7 -mfpu=vfp -fno-caller-saves -fno-plt -mfloat-abi=hard  -Wformat -Werror=format-security -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro   " CXXFLAGS="-Os -pipe -mcpu=cortex-a7 -mfpu=vfp -fno-caller-saves -fno-plt -mfloat-abi=hard  -Wformat -Werror=format-security -D_FORTIFY_SOURCE=1 -Wl,-z,now -Wl,-z,relro   " CPPFLAGS="-I/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/staging_dir/target-arm-linux_glibc/usr/include -I/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/staging_dir/target-arm-linux_glibc/include -I/opt/openwrt_toolchain/arm-openwrt-icatchtek-bsp4-glibc/usr/include -I/opt/openwrt_toolchain/arm-openwrt-icatchtek-bsp4-glibc/include " LDFLAGS="-L/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/staging_dir/target-arm-linux_glibc/usr/lib -L/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/staging_dir/target-arm-linux_glibc/lib -L/opt/openwrt_toolchain/arm-openwrt-icatchtek-bsp4-glibc/usr/lib -L/opt/openwrt_toolchain/arm-openwrt-icatchtek-bsp4-glibc/lib -znow -zrelro "   ./configure --target=arm-linux --host=arm-linux --build=x86_64-pc-linux-gnu --program-prefix="" --program-suffix="" --prefix=/usr --exec-prefix=/usr --bindir=/usr/bin --sbindir=/usr/sbin --libexecdir=/usr/lib --sysconfdir=/etc --datadir=/usr/share --localstatedir=/var --mandir=/usr/man --infodir=/usr/info --disable-nls  ; fi; )
touch /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/.configured_68b329da9893e34099c7d8ad5cb9c940
rm -f /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/.built
touch /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/.built_check
cat /dev/null > /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/staging_dir/target-arm-linux_glibc/pkginfo/hello-kernel.symvers; for subdir in .; do cat /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/staging_dir/target-arm-linux_glibc/pkginfo/*.symvers 2>/dev/null > /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/$subdir/Module.symvers; done
make -C "/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/linux-4.9.198" ARCH="arm" CROSS_COMPILE="arm-openwrt-linux-" SUBDIRS="/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel" EXTRA_CFLAGS="  "  CONFIG_HELLO-KERNEL=m modules
make[3]: Entering directory `/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/target/linux/v37/linux-4.9.198'
  Building modules, stage 2.
  MODPOST 0 modules
make[3]: Leaving directory `/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/target/linux/v37/linux-4.9.198'
for subdir in .; do grep -F $(readlink -f /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel) /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/$subdir/Module.symvers >> /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/Module.symvers.tmp; done; sort -u /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/Module.symvers.tmp > /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/Module.symvers; mv /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/Module.symvers /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/staging_dir/target-arm-linux_glibc/pkginfo/hello-kernel.symvers
touch /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/.built
rm -rf /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/.pkgdir/kmod-hello-kernel.installed /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/.pkgdir/kmod-hello-kernel
mkdir -p /vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/.pkgdir/kmod-hello-kernel
ERROR: module '/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/hello-kernel.ko' is missing.
make[2]: *** [/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/.pkgdir/kmod-hello-kernel.installed] Error 1
make[2]: Leaving directory `/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4/package/kernel/hello-kernel'
time: package/kernel/hello-kernel/compile#0.55#0.33#1.65
make[1]: *** [package/kernel/hello-kernel/compile] Error 2
make[1]: Leaving directory `/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4'
make: *** [package/kernel/hello-kernel/compile] Error 2
rck@rck:/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4$
```

`MODPOST 0 modules`:不能生成hello-kernel.ko

MODPOST 0 modules的解决办法_simon.pure.peak-CSDN博客 - https://blog.csdn.net/x854870745/article/details/12877999

发现：package/kernel/hello-kernel/src/Makefile 里写成：

```
obj-${CONFIG_HELLO_KERNEL}    += hello_kernel.o
```

改成以下(**中划线与下划线问题**)：

```
obj-${CONFIG_HELLO-KERNEL}    += hello-kernel.o
```

编译成功！

```
rck@rck:/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4$ find . -name hello-kernel.ko
./build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/.pkgdir/kmod-hello-kernel/lib/modules/4.9.198/hello-kernel.ko
./build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/ipkg-arm_cortex-a7_vfp/kmod-hello-kernel/lib/modules/4.9.198/hello-kernel.ko
./build_dir/target-arm-linux_glibc/linux-v37/hello-kernel/hello-kernel.ko
./staging_dir/target-arm-linux_glibc/root-v37/lib/modules/4.9.198/hello-kernel.ko
rck@rck:/vtcs/0002_iCatch/svn2git/vi37/Linux/bsp4$
```

---

在：`package/kernel/hello-kernel/Makefile`

```makefile
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hello-kernel
PKG_RELEASE:=1
PKG_MAINTAINER:=
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/hello-kernel
	SUBMENU:=Other modules
	TITLE:=Hello kernel drive
	FILES:=$(PKG_BUILD_DIR)/hello-kernel.ko
	#AUTOLOAD:=$(call AutoProbe,60,hello-kernel)
	AUTOLOAD:=$(call AutoLoad,60,hello-kernel)
endef


EXTRA_KCONFIG:= \
	CONFIG_HELLO-KERNEL=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \

MAKE_OPTS:=ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/
	$(CP) -R ./src/*  $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,hello-kernel))
```

设置成：`AUTOLOAD:=$(call AutoProbe,60,hello-kernel)`时，打包在`/etc/modules.d/`只会生成`hello-kernel`文件，没有`60-hello-kernel`,这样开机就不会自动加载hello-kernel.ko模块。
改成`AUTOLOAD:=$(call AutoLoad,60,hello-kernel)`,则在`/etc/modules.d/`生成`60-hello-kernel`,这样开机就会自动加载hello-kernel.ko模块。

```
rck@rck:/vtcs/0002_i/svn2git/v7/Linux/bsp4$ ls ./staging_dir/target-arm-linux_glibc/root-v37/etc/modules.d/
02-crypto-hash  09-crypto-manager  10-crypto-hw-icatchtek-aes  30-dnsresolver    40-fs-nfs     41-fs-nfs-v4  50-rtl88x2cs     80-fuse
09-crypto-aead  09-crypto-null     25-nls-utf8                 30-fs-nfs-common  41-fs-nfs-v3  50-cryptodev  60-hello-kernel
rck@rck:/vtcs/0002_i/svn2git/v7/Linux/bsp4$ 
```

---


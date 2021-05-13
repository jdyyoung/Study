

怎么连接本地文件，不用下载的

指定版本

```
buildroot重新编译package：
buildroot在编译之前会根据.config 文件来检查output/build/package 的6个文件，做相应的步骤：
.stamp_configured,  此文件表示已经配置过
.stamp_downloaded, 此文件表示源码已经下载过，没有此文件会重新下载
.stamp_patched, 此文件表示已经打过补丁
.stamp_extracted  此文件表示已经压过
.stamp_builted 此文件表示源码已经编译过   
.stamp_target_installed  此文件表示软件已经安装过
想要从新执行哪一步，就把对应的.stamp_文件删除就行
```



```
package/Config.in
package/Config.in.host
package/Makefile.in
package/doc-asciidoc.mk
package/pkg-cmake.mk 
package/pkg-generic.mk
package/pkg-kernel-module.mk
package/pkg-perl.mk
package/pkg-rebar.mk
package/pkg-virtual.mk
package/pkg-autotools.mk
package/pkg-download.mk
package/pkg-kconfig.mk 
package/pkg-luarocks.mk
package/pkg-python.mk
package/pkg-utils.mk
package/pkg-waf.mk

```



```
package/Config.in
检查程序目录是否指定：
    打开buildroot/package/Config.in文件，这里用来配置package指定工具(应用)的Config.in方式；
    搜索"poco" 如果找到source "package/poco/Config.in",把#去掉，如果没有找到可以这样增加：
      menu " poco "      # 引号内的信息可以任意添加，保证不与文件中相同即可
          source "package/poco/Config.in"
      endmenu

```



---

buildroot使用介绍 - ArnoldLu - 博客园 - https://www.cnblogs.com/arnoldlu/p/9553995.html

(1条消息) Rockchip(瑞芯微)平台高速上手指南_unbroken-CSDN博客_rockchip - https://blog.csdn.net/u010164190/article/details/85329069

buildroot教程 - fuzidage - 博客园 - https://www.cnblogs.com/fuzidage/p/12049442.html

Buildroot用户手册-Buildroot的一般用法_海漠的博客-CSDN博客 - https://blog.csdn.net/haimo_free/article/details/107723343

(1条消息) Buildroot构建指南--快速上手与实用技巧_zhou_chenz的博客-CSDN博客_buildroot教程 - https://blog.csdn.net/zhou_chenz/article/details/52335634

```
$(eval $(generic-package))                                                                        |159 $(eval $(autotools-package))
$(eval $(host-generic-package))

$(eval $(autotools-package))
$(eval $(host-autotools-package))

package/Config.in
package/Config.in.host
package/Makefile.in
```

(1条消息) 第17章添加新的软件包到Buildroot_小宋的博客-CSDN博客 - https://blog.csdn.net/qq_39101111/article/details/78685736

(1条消息) 【Linux 系统】文件系统--- Buildroot 从零开始制作文件系统 史上最详细_ICEDustpan-CSDN博客 - https://blog.csdn.net/weixin_44205779/article/details/107330375
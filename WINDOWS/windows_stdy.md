window10 删除系统启动项：

如何删除其余操作系统的开机启动项 - Microsoft Community - https://answers.microsoft.com/zh-hans/windows/forum/windows8_1-performance/%E5%A6%82%E4%BD%95%E5%88%A0%E9%99%A4%E5%85%B6/a26d22d7-d2ef-407b-960b-0e85ae224c2d

```
bcdedit

此命令将会显示启动配置数据存储中的所有项目，格式如下：


标识符                  {24ea9d48-c5f6-11e3-8343-047d7bbebac2}

（标识符用于识别每个项目，相当于项目的身份证）

device                  partition=C:
path                    \WINDOWS\system32\winload.exe
description             Windows 8.1

（该项目在电脑启动时所显示的名称）
locale                  zh-CN
inherit                 {bootloadersettings}
recoverysequence        {e8e4d820-1188-11e4-8278-047d7bbebac2}
integrityservices       Enable
recoveryenabled         Yes
allowedinmemorysettings 0x15000075
osdevice                partition=C:
systemroot              \WINDOWS
resumeobject            {d0396169-dc15-11e3-83be-047d7bbebac2}
nx                      OptIn
bootmenupolicy          Legacy
detecthal               Yes

要删除指定项目则执行下面的命令：

bcdedit /delete <标识符>

例如，假设Ubuntu的标识符是{24ea9d48-c5f6-11e3-8343-047d7bbebac2}：

bcdedit /delete  {24ea9d48-c5f6-11e3-8343-047d7bbebac2}
```

注意：在左下角搜索框输入：cmd

并以管理员身份打开！！！

运行bcdedit 命令删除。



----

win +x :打开系统配置快捷键

---

Tinkpad T480设置U盘启动_Stone的专栏-CSDN博客_t480 u盘启动 - https://blog.csdn.net/beijingzhengzhiwen/article/details/84112387

Thinkpad T480:

设置由U盘启动：

开机 --> Enter键进入 BIOS 选择界面 --> F1 --> Security 标签 --> Secure Boot选项 --> 设置成Disabled

--> Startup 标签 --> UEFI/Legacy Bootz置为【UEFI Only】，将CSM Support置为【Yes】

--同在startup标签下，选择boot进入boot引导顺序页面，将USB HDD调整到第一位引导 (shift + ‘+’ 调整)

-->F10，保存并重启，自动会引导到U盘。



---

解决 Windows 10 家庭版无法使用NFS服务的问题_zywvvd的博客-CSDN博客 - https://blog.csdn.net/zywvvd/article/details/106501200



-----------------

用secureCRT串口工具发送hex字符串的方法_zkw_1998的博客-CSDN博客_securecrt发送十六进制 - https://blog.csdn.net/zkw_1998/article/details/103889724


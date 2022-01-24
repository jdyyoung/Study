#2022-01-13

```shell
# 查看命令帮助
wpa_cli help
# 搜索可见wifi
wpa_cli -i wlan0 scan
# 获取搜索结果
wpa_cli -i wlan0 scan_result
# 获取已经配置的wifi设置
wpa_cli -i wlan0 list_networks
# 查看当前wifi状态
wpa_cli -i wlan0 status
# 新建wifi配置，会返回一个新的网络整数ID，比如：0
wpa_cli -i wlan0 add_network
# 假设新wifi的网络ID为0
# 设置wifi的ssid
# ssid需要添加双引号，双引号还要加斜杠或者单引号
wpa_cli -i wlan0 set_network 0 ssid '"TP-LINK"'
wpa_cli -i wlan0 set_network 0 ssid \"TP-LINK\"
# 设置加密方式，WPA-PSK代表：WPA-PSK/WPA2-PSK
wpa_cli -i wlan0 set_network 0 key_mgmt WPA-PSK
# 设置wifi密码，和账号一样需要双引号
wpa_cli -i wlan0 set_network 0 psk '"12345678"'
wpa_cli -i wlan0 set_network 0 psk \"12345678\"
# 新配置的wifi默认是禁用的，启用，启用后如果没有连接wifi，新增wifi可见就会自动连接的
wpa_cli -i wlan0 enable_network 0
# 如果有多个wifi，可以通过如下命令选择wifi
wpa_cli -i wlan0 select_network 0
# 再次查看状态，查看是否已经连接网络
# wpa_state=COMPLETED表示完成连接，但是如果signal_level比较小时表示实际没有连接
wpa_cli -i wlan0 status
# 保存wifi配置，最好在启用wifi后再保存，否则保存的wifi是默认禁用的
wpa_cli -i wlan0 save_config
```

 arm 使用wpa_cli连接wifi_坤仔的专栏-CSDN博客_wpa_cli 连接wifi - https://blog.csdn.net/chenjk10/article/details/90521093



//查看wifi的一些信息

```
[root@anyka ~]$ iwconfig wlan0
wlan0     IEEE 802.11bgn  ESSID:"wifi520"  Nickname:"<WIFI@REALTEK>"
          Mode:Managed  Frequency:2.437 GHz  Access Point: EC:56:23:15:C9:8B   
          Bit Rate:72.2 Mb/s   Sensitivity:0/0  
          Retry:off   RTS thr:off   Fragment thr:off
          Encryption key:****-****-****-****-****-****-****-****   Security mode:open
          Power Management:off
          Link Quality=100/100  Signal level=96/100  Noise level=0/100
          Rx invalid nwid:0  Rx invalid crypt:0  Rx invalid frag:0
          Tx excessive retries:0  Invalid misc:0   Missed beacon:0

[root@anyka ~]$ 

```

```
[root@anyka ~]$ wpa_cli status
Selected interface 'wlan0'
bssid=ec:56:23:15:c9:8b
ssid=wifi520
id=1
passphrase=jdy1160.
psk=48e6ca79105922ff212888814b94d2e8e456a31d57cfd5969c9a2c2ede
mode=station
pairwise_cipher=CCMP
group_cipher=CCMP
key_mgmt=WPA2-PSK
wpa_state=COMPLETED
ip_address=192.168.43.249
address=30:4a:26:af:10:ae
signal_level=90
[root@anyka ~]$ 
```

```
//查看连接到路由器wifi:operstaet 是 up 且 level 大于 一定值，表明连上wifi,且连接质量很好
[root@anyka ~]$ ls /sys/class/net/wlan0/wireless/
beacon    fragment  link      noise     retries
crypt     level     misc      nwid      status
[root@anyka ~]$ 
[root@anyka ~]$ cat /sys/class/net/wlan0/wireless/link
100
[root@anyka ~]$ cat /sys/class/net/wlan0/wireless/status
0x0
[root@anyka ~]$ cat /sys/class/net/wlan0/wireless/level 
100
[root@anyka ~]$ cat /sys/class/net/wlan0/operstate 
up
[root@anyka ~]$ 
```

```c
int net_check()
{
	int net_fd;
	char statue[20] = {0};
	//以只读的方式打开/sys/class/net/wlan0/operstate
	net_fd = open("/sys/class/net/wlan0/operstate", O_RDONLY);
	if (net_fd < 0)
	{
		pr("open err\n");
		return -1;
	}

	read(net_fd, statue, 10);
	if (NULL == strstr(statue, "up"))
	{
		close(net_fd);
		return -1;
	}
	close(net_fd);

	net_fd = open("/sys/class/net/wlan0/wireless/link", O_RDONLY);
	if (net_fd < 0)
	{
		pr("open err\n");
		return -1;
	}
	if (read(net_fd, statue, 10) < 0)
	{
		pr("read err\n");
		close(net_fd);
		return -1;
	}
	// pr("statue is %s", statue);
	close(net_fd);
	return atoi(statue);
}
```

```
//查看加载wifi模块驱动的一些信息，如rtl8189fs
[root@anyka ~]$ lsmod
    Tainted: G  
8189fs 1210238 0 - Live 0xbf014000 (O)
ak_info_dump 1227 0 - Live 0xbf010000
akcamera 14248 2 ak_info_dump, Live 0xbf008000
sensor_h62 3136 0 - Live 0xbf004000
sensor_gc0328 2771 0 - Live 0xbf000000
[root@anyka ~]$ 
[root@anyka ~]$ ls /proc/net/rtl8189fs/wlan0/
adapters_status         kfree_thermal           scan_param
all_sta_info            linked_info_dump        sd_f0_reg_dump
ampdu_enable            mac_qinfo               sdio_local_reg_dump
ap_info                 mac_reg_dump            sec_cam
bb_reg_dump             mac_rptbuf              sec_cam_cache
bw_mode                 macid_info              sec_info
chan_plan               mlmext_state            sink_udpport
change_bss_chbw         monitor                 sreset
country_code            new_bcn_max             survey_info
current_tx_rate         odm                     suspend_info
customer_str            qos_option              target_tx_power
del_rx_ampdu_test_case  rate_ctl                trx_info
dis_pwt_ctl             read_reg                tx_ampdu_density
efuse_map               rf_info                 tx_bw_mode
en_fwps                 rf_reg_dump             tx_gain_offset
fwdl_test_case          roam_flags              tx_power_by_rate
fwstate                 roam_param              tx_power_ext_info
hal_spec                roam_tgt_addr           tx_power_idx
hal_txpwr_info          rx_ampdu                tx_power_limit
ht_enable               rx_ampdu_density        tx_rate_bmp
ht_option               rx_ampdu_factor         wait_hiq_empty
hw_info                 rx_signal               wifi_spec
kfree_bb_gain           rx_stbc                 write_reg
kfree_flag              scan_abort
[root@anyka ~]$ 
```

```
# cat /proc/net/rtl88x2cs/wlan0/rx_signal 
rssi:-36
signal_strength:64
signal_qual:97
# 
```

无线信号强度解析及linux如何查看wifi信号强弱等_lbyyy的专栏-CSDN博客_linux查看wifi信号强度 - https://blog.csdn.net/lbyyy/article/details/51392629



---

c语言获取linux的CPU、内存、IO、磁盘、网速（本机编译通过）_p312011150的博客-CSDN博客_c语言 cpu 内存 - https://blog.csdn.net/p312011150/article/details/81278775?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-1.nonecase&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-1.nonecase



用Linux C语言获取当前网速_zhx862012的专栏-CSDN博客_linux 网络测速 c - https://blog.csdn.net/zhx862012/article/details/79731129
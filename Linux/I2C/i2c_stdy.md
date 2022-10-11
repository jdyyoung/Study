



```
static struct i2c_board_info IMX327_board_info = 
{
	I2C_BOARD_INFO(DEV_NAME, 0x6C),
	.platform_data = NULL,
};
struct i2c_client *ptI2CClient;
i2c_master_send()
i2c_master_recv()

struct i2c_client *i2c_client = NULL;
struct i2c_adapter *adapter;

adapter = i2c_get_adapter(i2c_bus_num);
i2c_put_adapter(adapter);

i2c_client = i2c_new_probed_device(adapter, 
							&IMX327_board_info, 
							awIMX327I2cAddr, 
							0);
#include <linux/i2c.h>
	if (i2c_master_send(ptInfo->ptI2CClient, abyData, dwI2CDataLen) != dwI2CDataLen)
	if (i2c_master_send(ptInfo->ptI2CClient, abyData, 2) != 2)
	if (i2c_master_recv(ptInfo->ptI2CClient, abyData, 1) != 1)
static struct i2c_board_info IMX327_board_info = 
	struct i2c_client *i2c_client = NULL;
	struct i2c_client *i2c_client = NULL;
	struct i2c_adapter *adapter;
	unsigned int i2c_bus_num;
	unsigned int i2c_addr;
	if (of_property_read_u32(np, "i2c_bus_num", &i2c_bus_num) != 0)
	if (of_property_read_u32(np, "i2c_bus_num", &i2c_bus_num) != 0)
		MSGPRINT("Fail to find and read 'i2c_bus_num' from a property in device node.\n");
	if (of_property_read_u32(np, "i2c_addr", &i2c_addr) != 0)
	if (of_property_read_u32(np, "i2c_addr", &i2c_addr) != 0)
		MSGPRINT("Fail to find and read 'i2c_addr' from a property in device node.\n");
	if (IMX327_PadSet(i2c_bus_num, ref_clock_from_soc, frame_sync_mode) < 0)
	awIMX327I2cAddr[0] = (i2c_addr>>1);
	adapter = i2c_get_adapter(i2c_bus_num);
	adapter = i2c_get_adapter(i2c_bus_num);
		MSGPRINT("i2c_get_adapter(%d) failed\n", i2c_bus_num);
		MSGPRINT("i2c_get_adapter(%d) failed\n", i2c_bus_num);
	i2c_client = i2c_new_probed_device(adapter, 
	i2c_client = i2c_new_probed_device(adapter, 
	if (!i2c_client)
		MSGPRINT("i2c_new_probed_device() failed\n");
	ptDevInfo->ptI2CClient = i2c_client;
	ptDevInfo->dwI2CNum = i2c_bus_num;
	i2c_put_adapter(adapter);
	i2c_unregister_device(i2c_client);
	i2c_unregister_device(i2c_client);
	i2c_put_adapter(adapter);
		i2c_unregister_device(ptDevInfo->ptI2CClient);
```

----

----

IMX327 驱动中I2C的操作：

```C
IMX327_0:IMX327@0 {
		compatible = "vatics,imx327";
		data_io_num = <4>; /*Input port used by sensor, DVP0=0, DVP1=1, DVP2=2, DVP3=3, MIPI0=4, MIPI1=5, LVDS0=6, LVDS1=7*/
		ref_clock_from_soc = <1>; /*Enable SOC PLL clock as sensor input clock, disable=0, enable=1*/
		reset_gpio_num = <0x0B>; /*GPIO number used for sensor HW reset. (0xFFF : Ignore)*/
		standby_gpio_num = <0xFFF>; /*GPIO number used for sensor Power down. (0xFFF : Ignore)*/
		i2c_bus_num = <1>; /*I2C bus num used by sensor*/
		i2c_addr = <0x34>; /*sensor I2C ID select, could be 0x6C or 0x20*/
		frame_sync_mode = <0>; /*sensor FrameSync mode. (Disable=0, ONE_SYNC=1, CONTINUOUS_SYNC=2)*/
		frame_sync_signal_num = <0xFFF>; /*AGPO or GPIO number used for sensor frame sync. (0xFFF : Ignore)*/
		serial_lane_num = <2>; /*serial lane number : 2 or 4*/
	};
```



```C
/**指定i2c device的信息
 * downey_i2c 是device中的name元素，当这个模块被加载时，i2c总线将使用这个名称匹配相应的drv。
 * I2C_DEVICE_ADDR  为设备的i2c地址 
 * */
static struct i2c_board_info IMX327_board_info = 
{
	I2C_BOARD_INFO(DEV_NAME, 0x6C),
	.platform_data = NULL,
};
#define IMX327_DEAFULT_DEVICE_0_ADDR	0x34
#define IMX327_DEAFULT_DEVICE_1_ADDR	0x34

static WORD awIMX327I2cAddr[] = 
{
	IMX327_DEAFULT_DEVICE_0_ADDR>>1, 
	I2C_CLIENT_END
};
```



```C
struct i2c_client *i2c_client = NULL;
struct i2c_adapter *adapter;

unsigned int i2c_bus_num;
unsigned int i2c_addr;

if (of_property_read_u32(np, "i2c_bus_num", &i2c_bus_num) != 0)
{
MSGPRINT("Fail to find and read 'i2c_bus_num' from a property in device node.\n");
return -1;
}

if (of_property_read_u32(np, "i2c_addr", &i2c_addr) != 0)
{
MSGPRINT("Fail to find and read 'i2c_addr' from a property in device node.\n");
return -1;
}


awIMX327I2cAddr[0] = (i2c_addr>>1);
/*获取i2c适配器，适配器一般指板上I2C控制器，实现i2c底层协议的字节收发，特殊情况下，用普通gpio模拟I2C也可作为适配器*/
adapter = i2c_get_adapter(i2c_bus_num);

if (!adapter)
{
MSGPRINT("i2c_get_adapter(%d) failed\n", i2c_bus_num);
goto fail1;
}
/*创建一个I2C device，并注册到i2c bus的device链表中*/
i2c_client = i2c_new_probed_device(adapter, 
&IMX327_board_info, 
awIMX327I2cAddr, 
0);

if (!i2c_client)
{
MSGPRINT("i2c_new_probed_device() failed\n");
goto fail2;
}
/*使能相应适配器*/
i2c_put_adapter(adapter);

```

```C
static SOCKET IMX327_WriteReg(TDevInfo *ptInfo, EIMX327Regs eRegAddr, QWORD qwData, DWORD dwDataLen, BOOL bWaitCmpt)
{
	BYTE abyData[10];
	DWORD dwI2CDataLen, dwIdx;

	abyData[0] = (BYTE)((eRegAddr&0xFF00)>>8);
	abyData[1] = (BYTE)(eRegAddr&0xFF);

	for (dwIdx=0; dwIdx<dwDataLen; dwIdx++)
	{
		abyData[2+dwIdx] = (BYTE)((qwData>>(dwIdx<<3))&0xFF);
	}

	dwI2CDataLen = dwDataLen + 2;

	if (i2c_master_send(ptInfo->ptI2CClient, abyData, dwI2CDataLen) != dwI2CDataLen)
	{
		MSGPRINT("Write 0x%x to 0x%x failed!!\n", (int)qwData, eRegAddr);
		return S_FAIL;
	}
	else
		return S_OK;
}

/*-------------------------------------------------------------------------------------*/
static SOCKET IMX327_ReadReg(TDevInfo *ptInfo, EIMX327Regs eRegAddr, DWORD *pdwData, DWORD dwDataLen, BOOL bWaitCmpt)
{
	BYTE abyData[2];

	abyData[0] = (BYTE)((eRegAddr>>8)&0xFF);
	abyData[1] = (BYTE)(eRegAddr&0xFF);

	if (i2c_master_send(ptInfo->ptI2CClient, abyData, 2) != 2)
	{
		return S_FAIL;
	}

	if (i2c_master_recv(ptInfo->ptI2CClient, abyData, 1) != 1)
	{
		return S_FAIL;
	}

	*pdwData = abyData[0];

	return S_OK;
}
```

使用了i2c_new_device()接口，值得一提的是，这个接口并不会检测设备是否存在，只要对应的device-driver存在，就会调用driver中的probe函数。

但是有时候会有这样的需求：在匹配时需要先检测设备是否插入，如果没有i2c设备连接在主板上，就拒绝模块的加载，这样可以有效地管理i2c设备的资源，避免无关设备占用i2c资源。

新的创建方式接口为：

```C
/*
这个函数添加了在匹配模块时的检测功能：

参数1：adapter，适配器
参数2：board info，包含名称和i2c地址
参数3：设备地址列表，既然参数2中有地址，为什么还要增加一个参数列表呢？咱们下面分解
参数4：probe检测函数，此probe非彼probe，这个probe函数实现的功能是检测板上是否已经物理连接了相应的设备，
当传入NULL时，就是用默认的probe函数，建议使用默认probe。
*/
struct i2c_client *i2c_new_probed_device(struct i2c_adapter *adap,struct i2c_board_info *info,unsigned short const *addr_list,int (*probe)(struct i2c_adapter *, unsigned short addr))  
```


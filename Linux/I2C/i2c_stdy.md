



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


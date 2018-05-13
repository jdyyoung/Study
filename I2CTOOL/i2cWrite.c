/*
 * i2c read and write  device lib 
 */
#include <error.h>
#include "i2cWrite.h"

int i2cInit(int* i2cID) {
    *i2cID = open(I2C_DEVICE, O_RDWR);
    if (*i2cID < 0) {
        printf("%s, open device file %s error: %s.\n",I2C_DEVICE, __FUNCTION__, strerror(errno));
        return -1;
    }
    return 0;
}

int i2cRelease(int* i2cID){
    if (*i2cID >= 0) {
        close(*i2cID);
        *i2cID = -1;
        return 0;
    }
    return -1;
}

int i2cWriteNbyte(int* i2cID, int slave_addr, int dev_addr, int len, unsigned char data_buf[])
{
    if (i2cID == NULL) {
		printf("i2cID = NULL error!\n");
		return -1;
	}
    if (data_buf == NULL) {
		printf("data_buf = NULL error!\n");
        return -1;
    }
    int tmp_ret = -1;
    struct i2c_rdwr_ioctl_data ctl_data;
    struct i2c_msg msg;
    unsigned char msg_buf[52];

    memset((void *) msg_buf, 0, 52);

    msg_buf[0] = (unsigned char) (dev_addr & 0x00ff);

    if (len < 50) {
        memcpy((void *) &msg_buf[1], data_buf, len);
    } else {
        printf("I2C_WriteNbyte len(%d) > 50, error!\n", len);
        return -1;
    }

    msg.addr = slave_addr;
    msg.flags = I2C_M_WR;
    msg.len = 1 + len;
    msg.buf = msg_buf;
    ctl_data.nmsgs = 1;
    ctl_data.msgs = &msg;

    tmp_ret = ioctl(*i2cID, I2C_RDWR, &ctl_data);
	if(tmp_ret <0 ){
		perror("ioctl");
	}

    usleep(10 * 1000);
    return tmp_ret;
}

int i2cReadbyte(int* i2cID, int slave_address, int reg_address, unsigned char* data_buf)
{
    if (i2cID == NULL) {
		printf("i2cID = NULL error!\n");
        return -1;
    }

    if (data_buf == NULL) {
		printf("data_buf = NULL error!\n");
        return -1;
    }
    int tmp_ret = -1;
    struct i2c_rdwr_ioctl_data ctl_data;
    struct i2c_msg msg[2];
    ctl_data.nmsgs = 2;

    msg[0].len = 1;
    msg[0].flags = I2C_M_WR;
    msg[0].addr = slave_address;
    msg[0].buf = (unsigned char*)&reg_address;

	msg[1].len = 1;
	msg[1].flags = I2C_M_RD;
	msg[1].addr = slave_address;
	msg[1].buf = data_buf;

    ctl_data.msgs = msg;

    tmp_ret = ioctl(*i2cID, I2C_RDWR, &ctl_data);
	if(tmp_ret <0 ){
		perror("ioctl");
	}
	else{
		printf("reg_address=0x%02x *data_buf=0x%02x\n", reg_address, *data_buf);
	}

    usleep(10 * 1000);
    return tmp_ret;
}



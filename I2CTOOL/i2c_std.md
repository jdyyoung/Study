

编译：

make CC=arm-linux-gcc USE_STATIC_LIB=1

i2ctool 的使用：

```
# i2cdetect -l
i2c-1   i2c             Synopsys DesignWare I2C adapter         I2C adapter
i2c-2   i2c             Synopsys DesignWare I2C adapter         I2C adapter
i2c-0   i2c             Synopsys DesignWare I2C adapter         I2C adapter
# 
#查到的是7位地址
# i2cdetect -r -y 1
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- -- 
10: 10 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
40: -- -- -- -- -- -- -- -- 48 -- -- -- -- -- -- -- 
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- --                         
# 
# 
# MCU VERSION:v0.47

# 
# i2cdump -f -y 1 0x48
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f    0123456789abcdef
00: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
10: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
20: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
30: 00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 00    .....?..........
40: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
50: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
60: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
70: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
90: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
a0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
b0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
c0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
e0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
#

i2cset -f -y 1 0x48 0x77 0x3f （设置i2c-1上0x48器件的0x77寄存器值为0x3f）

i2cget -f -y 1 0x48 0x77     （读取i2c-1上0x48器件的0x77寄存器值）

# /skybell/misc/i2ctransfer -y -f 1 w2@0x10 0x50 0x00 r16
0x07 0x20 0x10 0x10 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x02 0x00 0x00 0x00 0x00
```

(1条消息) i2c-tools使用及调试_不忘初心-CSDN博客_i2cdetect - https://blog.csdn.net/kai_zone/article/details/80491706

(1条消息) i2c-tool使用详细说明_nancy的专栏-CSDN博客_i2c tool使用说明 - https://blog.csdn.net/u011784994/article/details/105136412/
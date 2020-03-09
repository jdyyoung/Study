谈谈对SPI总线的认识
SPI总线是一种全双工同步串行总线， 
1.	SPI总线采用主-从模式的控制方式，有一个主设备多个从设备挂接在总线上；

SPI总线进行数据通信时用到四条信号线：
（1）	MOSI– 主设备输出从设备输入信号线
SPI总线主机输出/ 从机输入（SPI Bus Master Output/Slave Input）；
（2）	MISO– 主设备输入从设备输出信号线
SPI总线主机输入/ 从机输出（SPI Bus Master Input/Slave Output)；
（3）SCLK –时钟信号信号线，由主设备产生；
（4）CS – 从设备使能信号线，由主设备控制（Chip select）

传输数据时，时钟信号通过时钟极性 (CPOL) 和 时钟相位 (CPHA) 控制着主从 设备间何时数据交换以及何时对接收到的数据进行采样, 来保证数据在主从设备之间是同步传输的.

SPI 设备间的数据传输又被称为数据交换, 是因为 SPI 协议规定一个 SPI 设备不能在数据通信过程中仅仅只充当一个 "发送者(Transmitter)" 或者 "接收者(Receiver)". 在每个时钟周期内, SPI 设备都会发送并接收一个 bit 的数据, 相当于该设备有一个 bit 的数据被交换了.

发送数据高位在前低位在后

SPI总线四种工作方式 SPI 模块为了和外设进行数据交换，根据外设工作要求，其输出串行同步时钟极性和相位可以进行配置，时钟极性（CPOL）对传输协议没有重大的影响。如果 CPOL=0，串行同步时钟的空闲状态为低电平；如果CPOL=1，串行同步时钟的空闲状态为高电平。时钟相位（CPHA）能够配置用于选择两种不同的传输协议之一进行数据传输。如果CPHA=0，在串行同步时钟的第一个跳变沿（上升或下降）数据被采样；如果CPHA=1，在串行同步时钟的第二个跳变沿（上升或下降）数据被采样。
假设主机和从机初始化就绪：并且主机的sbuff=0xaa，从机的sbuff=0x55，下面将分步对spi的8个时钟周期的数据情况演示一遍：假设上升沿发送数据
脉冲主机sbuff 从机sbuff sdi sdo
0 10101010 01010101 0 0
1上 0101010x 1010101x 0 1
1下 01010100 10101011 0 1
2上 1010100x 0101011x 1 0
2下 10101001 01010110 1 0
3上 0101001x 1010110x 0 1
3下 01010010 10101101 0 1
4上 1010010x 0101101x 1 0
4下 10100101 01011010 1 0
5上 0100101x 1011010x 0 1
5下 01001010 10110101 0 1
6上 1001010x 0110101x 1 0
6下 10010101 01101010 1 0
7上 0010101x 1101010x 0 1
7下 00101010 11010101 0 1
8上 0101010x 1010101x 1 0
8下 01010101 10101010 1 0

//单片机模拟spi串行接口程序，在keilc51下编写 
//CPOL=0,CPHA=1  上升沿输出下降沿采样
//移位寄存器，先输出数据，再移位，然后获取数据
sbit CS=P3^5;
sbit CLK= P1^5;
sbit DataI=P1^7;
sbit DataO=P1^6;
#define SD_Disable() CS=1 //片选关
#define SD_Enable() CS=0 //片选开
unsigned char SPI_TransferByte(unsigned char val)
{
unsigned char BitCounter;
for(BitCounter=8; BiCounter!=0; BitCounter--)
    {
	CLK=0;
	DataI=0;     // write   MOSI  主机（val）输出数据，写到这条信号线上
	if(val&0x80) DataI=1;  //判断最高位
	val<<=1;    //移位
	
	CLK=1;
	if(DataO)val|=1; // read  MISO  主机（val）取数据，从这条信号线读取
	    }
	CLK=0;   
	return val;  
}

---

时钟极性(CPOL：polarity)：同步时钟空闲状态时的电平值
时钟相位(CPHA：phase)：  决定在同步时钟的哪个位置进行数据采样或输出

**CPHA=0：前沿采样，后沿输出；CPHA=1：前沿输出，后沿采样；**

***0采输，1出样***

**采样：读取IO管脚的状态，到内存；**

**输出：根据内存改变IO状态，输出到IO管脚；**

组合起来有四种传输模式

SPI初始化：波特率（设置始终频率），数据模式（MSB/LSB 选择），设置极性与相位，主/从模式选择，控制使能信号 

ss(cs) sck sdi(miso) sdo(mosi)

发送数据

在sck的控制下，两个双向移位寄存器进行数据交换   

sck提供时钟脉冲将数据一位位地传送

缺点：没有指定的流控制，没有应答机制确认是否接到数据

---

数据输出通过 SDO线，数据在时钟上升沿或下降沿时改变，在紧接着的下降沿或上升沿被读取。

发送数据高位在前低位在后：这一点和I2C一样

----

SPI协议详解_运维_嵌入式Linux-CSDN博客 - https://blog.csdn.net/weiqifa0/article/details/82765892

SPI通信协议（SPI总线）学习 - 涛少& - 博客园 - https://www.cnblogs.com/deng-tao/p/6004280.html

SPI协议（上）——基础介绍 - 知乎 - https://zhuanlan.zhihu.com/p/27376153

SPI工作模式及时序波形_Python_tommy_ly的博客-CSDN博客 - https://blog.csdn.net/tommy_ly/article/details/94568313
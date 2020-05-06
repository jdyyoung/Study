/*
	写出如下串口通信协议的数据打包及解包
	Header(0x1b)+payload length(2 Byte,低位在前)+Payload(n Bytes)+CheckSum(1Byte)
	除Header外，其他数据0x1B换成0x1C和数据值，遇0x1C换成0x1C和0x1C
*/
/*
思路：也就是0x1B -> 0x1C 0x1B 
			0x1c -> 0x1C 0x1C
			
			
*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
typedef unsigned char uint8;

static void print_hex_data(uint8* data,unsigned short data_len ){
	printf("print data is:\n");
	for(int i=0;i<data_len;i++){
		if(i>0 && 0 == i%16){
			printf("\n");
		}
		printf("0x%02x ",*(data + i));
	}
	printf("\n");
}


static uint8 checksum_cal(uint8* raw_data ,unsigned short data_len){
	uint8 chksum_val = 0;
	for(int i = 0;i < data_len;i++){
		chksum_val = raw_data[i] ^ chksum_val;
	}
	return chksum_val;
}
/*封包函数*/
//P.S:发包结束释放内存
uint8 * packet_hanlde(uint8* raw_data ,unsigned short data_len){
	//断言
	
	//计算校验值
	uint8 chksum_value = checksum_cal(raw_data,data_len);
	//原始数据封包
	uint8* raw_packet = (uint8*)calloc(data_len + 4,1);
	*raw_packet = 0x1B;
	*(raw_packet + 1) = (uint8)(data_len & 0x00FF);
	*(raw_packet + 2) = (uint8)(data_len >> 8);
	memcpy(raw_packet +3,raw_data,data_len);
	*(raw_packet + data_len + 3) = chksum_value;
	
	//申请内存：注意膨胀时内存越界
	uint8* packet_data = (uint8*)calloc(2*(data_len+3) + 1,1);
	uint8* p_mov = packet_data;
	
	for(int i =0;i< data_len + 4;i++){
		if(0 == i){
			*p_mov = *(raw_packet+i);
			p_mov++;
		}
		else{
			if(0x1B == *(raw_packet+i)  || 0x1C == *(raw_packet+i)){
				*p_mov = 0x1C;
				*(p_mov + 1) = *(raw_packet+i);
				p_mov = p_mov +2;
			}
			else{
				*p_mov = *(raw_packet+i);
				p_mov++;
			}
		}
	}
	//释放raw_packet内存
	free(raw_packet);
	
	print_hex_data(packet_data,2*(data_len+3) + 1);
	return packet_data;
}

uint8 de_packet(uint8* rev_buf,unsigned short buf_len){
	//断言
	
	if(0x1B != *(rev_buf)){
		printf("Header fail,! = 0x1B\n");
		return 0;
	}
	
	
	uint8* de_pkg_data = (uint8*)calloc(buf_len,1);
	
	uint8* p_mov = de_pkg_data;
	uint8 set_flag = 0;
	for(int i=0;i< buf_len;i++){
		if (0x1C == *(rev_buf+i) && 0 == set_flag){
			set_flag = 1;
			continue;
		}
		set_flag = 0;
		*p_mov = *(rev_buf+i);
		p_mov++;
	}
	print_hex_data(de_pkg_data,buf_len);
	free(de_pkg_data);
	return 1;
}

int main(){
	uint8 test_cmd_1[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C};
	uint8* enc_data = packet_hanlde(test_cmd_1,sizeof(test_cmd_1));
	printf("-------------------send end-----------------\n");
	de_packet(enc_data,2*(sizeof(test_cmd_1)+3)+1);
	
	free(enc_data);
	return 1;
}




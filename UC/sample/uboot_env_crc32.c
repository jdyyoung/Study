#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
	把指定文件4个字节后的内容保存到指定文件名.ok里;
	uboot.env 前4个字节是后面内容的CRC32 校验值 
*/
//#include"crc32.h"
static int file_exists(char* filename, size_t* filesize)
{
    if (filename) {
        struct stat sbuf;
        int rc = stat(filename, &sbuf);
        if (rc == 0 && filesize)
            *filesize = sbuf.st_size; 
        return rc == 0;
    }
    return 0;
}

int main(int argc,char** argv){
	
	if(argc != 2){
		printf("Usage：uboot_env_crc32 uboot.env\n");
		return -1;
	}
	int fd = open(argv[1], O_RDONLY|O_CLOEXEC);
	if(fd < 0){
		printf("open %s fail,return!\n",argv[1]);
		goto err;
	}
	
	size_t intput_file_size ;
	file_exists(argv[1],&intput_file_size);
	
	char save_file_name[128] = {0};
	sprintf(save_file_name,"%s.ok",argv[1]);
	int fd1 = open(save_file_name,O_RDWR|O_CREAT|O_TRUNC);
	if(fd1 < 1){
		printf("open %s fail,return!\n",save_file_name);
		goto err;
	}
	
	char crc32_buf[4] = {0};
	read(fd,crc32_buf,4);
	printf("%s crc32 = %02x %02x %02x %02x\n",argv[1],crc32_buf[0],crc32_buf[1],crc32_buf[2],crc32_buf[3]);
	char* env_data = (char*)malloc(intput_file_size -4);
	if(!env_data){
		printf("FAIL:env_data == NULL\n");
		goto err;
	}
	read(fd,env_data,intput_file_size -4);
	write(fd1,env_data,intput_file_size -4);

err:
	if(fd > 0){
		close(fd);
	}
	if(fd1 > 0){
		close(fd1);
	}
	if(env_data){
		free(env_data);
		env_data = NULL;
	}
	
	return 0;
}
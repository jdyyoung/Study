#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define TEST_FILE "./hostapd.conf"
typedef unsigned int  u32;
void ReadHostapdConf(char* filename)
{
    int iRet = 0;
    FILE* stream = NULL;
    struct stat status;
    char szBuf[256];
    char szSsid[32];
    unsigned long pos = 0;
    unsigned long len = 0;
	unsigned long n_len = 0;
	

	sprintf(szSsid, "ssid=SkyBell_%s","AAAAA123450000");

    printf("SSID[%s]\n",szSsid);
    iRet = stat(filename, &status);
    
    if(iRet != 0 || status.st_size <=0)
    {
        printf("can't found file :%s\n", filename);
        return;
    }

    stream = fopen(filename, "r+");
    if(NULL == stream)
    {
        perror("fopen");
        return;
    }

    while(fgets(szBuf, sizeof(szBuf), stream) != NULL)
    {
        pos = ftell(stream);
        //printf("offset = %ld\n",pos);
        
		n_len = 0;
        len = strlen(szBuf);
		if(len > strlen(szSsid)){
			n_len = len - strlen(szSsid) -1;
		}
        //printf("line:[%lu] %s", len, szBuf);
        if(strstr(szBuf,"ssid=SkyBell"))
        {
            printf("Change: %s=====>: %s\n", szBuf, szSsid);
            fseek(stream, pos-len,SEEK_SET);
            fputs(szSsid, stream);
			if(n_len !=0){
				while(n_len --){
					//补空格
					fputs(" ", stream);
				}
				fputs("\n", stream);
			}
			break;
        }
        usleep(500);
    }
	fclose(stream);
}

int main(){
	ReadHostapdConf(TEST_FILE);
	return 0;
}
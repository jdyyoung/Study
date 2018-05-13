#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include "inand_utils.h"
#include "mtdutils.h"
#include "debug.h"

int log_level = L_INFO;

#define SUGR_TAG	0xCCCCAAAA
#define MAX_SN		32
#define MAX_MAC		32
#define MAX_CPUID	40
#define CPUID_SIZE	35
#define FILE_SERIAL	"/etc/SN"
#define FILE_REGDEV	"/tmp/regdev_info"

#define FILE_CPUID	"/sys/devices/platform/jz4775-efuse.0/chip_id"
#define CPUID_PRFIX "Serial\t\t: "

static int cpuid_type = 1;
static int flash_type = 1;
static char serial_num[MAX_SN];

typedef struct {
unsigned int tag;
char sn[MAX_SN];
char mac[MAX_MAC];
} sugr_device_id_t;

static int set_new_device_id(sugr_device_id_t *dev_id);
static int set_new_serial_number(const char *new_id);

static int read_cpuid_from_cpuinfo(char *cpuid)
{
	FILE * pFile;
	char buf[512];
	int r = -1;

	pFile = fopen ("/proc/cpuinfo" , "r");
	if (pFile == NULL) {
		LOGE("Error open cpuinfo fail!\n");
		return -1;
	}
	while( fgets (buf , 512 , pFile) != NULL ){
		if (strstr(buf, CPUID_PRFIX) != NULL){
			int la = strlen(buf);
			int lb = la - strlen(CPUID_PRFIX);
			char *p = buf + strlen(CPUID_PRFIX); 
			memcpy(cpuid, p, lb);
			cpuid[ lb - 1 ] = 0;
			printf("cpuid:%s\n", cpuid);
			r = 0;
			break;
		}
	}

	fclose (pFile);
	return r;
}

static int read_cpuid_from_chip_id(char *cpuid)
{
	FILE * pFile;

	pFile = fopen (FILE_CPUID , "r");
	if (pFile == NULL) {
		LOGE("Error opening file: %s\n", FILE_CPUID);
		return -1;
	}
	if ( fgets (cpuid , MAX_CPUID , pFile) != NULL ){
		int len = strlen(cpuid);
		if(len > CPUID_SIZE){
			LOGE("%s: CPU ID Read Fail!\n", __func__);
			return -1;
		}else{
			LOGI("CPU ID: %s\n", cpuid);
		}
	}

	fclose (pFile);
	return 0;
}
static int read_cpuid(char *cpuid)
{
	int r = 0;
	if(cpuid_type == 0){
		r = read_cpuid_from_chip_id(cpuid);
	}else if(cpuid_type == 1){
		r = read_cpuid_from_cpuinfo(cpuid);
	}
	return r;
}
static int read_regdev_file(char *sn, char *mac)
{
	int err = 0, size = 0;
	FILE * pFile;

	pFile = fopen (FILE_REGDEV , "r");
	if (pFile == NULL) {
		LOGE("Error opening file: %s\n", FILE_REGDEV);
		return -1;
	}
	fseek(pFile, 0L, SEEK_SET);
	fseek(pFile, 0L, SEEK_END);   
	size = ftell(pFile);
	if(size < 15 || size > 40){
		LOGE("%s: size error (%d)!\n", __func__, size);
		fclose (pFile);
		return -1;
	}
	fseek(pFile, 0L, SEEK_SET);
	if ( fgets (sn , MAX_SN , pFile) != NULL ){
		int len = strlen(sn);
		if(len >= MAX_SN){
			LOGE("%s: SN Read Fail!\n", __func__);
			err = -1;
		}else if( ! strncmp(sn, "<html", 5)){
			LOGE("%s: content error!\n", __func__);
			err = -1;	
		}else if( sn[12] != 0xa ){
			LOGE("%s: not serial number!\n", __func__);
			err = -1;	
		}else{
			if(sn[len-1] == '\n') sn[len-1] = 0;
			LOGI("%s: SN: %s\n",__func__, sn);
			err = 0;
		}
	}
	if(err){
		fclose (pFile);
		return -1;
	}
	if ( fgets (mac , MAX_MAC , pFile) != NULL ){
		int len = strlen(mac);
		if(len >= MAX_MAC){
			LOGE("%s: Mac Read Fail!\n", __func__);
			err = -1;
		}else{
			LOGI("%s: Mac: %s\n",__func__, mac);
			err = 0;
		}
	}

	fclose (pFile);
	return err;
}
static int register_device(sugr_device_id_t *dev_id)
{
	int i, err;
	char cmd[128];
	char cpuid[MAX_CPUID];

	if(read_cpuid(cpuid)) return -1;
	sprintf(cmd, "curl regdev.sugrsugr.com/?cpuid=%s > %s", cpuid, FILE_REGDEV);
	for(i=0; i<3; i++){
		err = system(cmd);
		if(!err) break;
		sleep(1);
	}
	if(err){
		LOGE("%s: Fail!\n", __func__);
		return -1;
	}
	if(read_regdev_file(dev_id->sn, dev_id->mac)) 
		return -1;

	return 0;
}

static int inand_data_read(void *data, int len)
{
	iNandContext *p;

	inand_scan_partitions();
	//inand_print_partitions();
	p = inand_open_partition("device_id");
	if(p == NULL) return -1;
	inand_read_partition(p, data, len);
	inand_close_partition(p);
	return 0;
}
static int inand_data_write(void *data, int len)
{
	iNandContext *p;

	inand_scan_partitions();
	p = inand_open_partition("device_id");
	if(p == NULL) return -1;
	inand_write_partition(p, data, len);
	inand_close_partition(p);

	return 0;
}
static int nand_data_read(void *data, int len)
{
	const MtdPartition *partition;
	MtdReadContext *in;
	int s, r = 0;

	if(mtd_scan_partitions() < 0) return -1;

	partition = mtd_find_partition_by_name("logo");
	in = mtd_read_partition(partition);
	if(in == NULL) return -1;

    s = mtd_read_data(in, data, len);
	if(s != len) r = -1;

	mtd_read_close(in);

	return r;
}
static int nand_data_write(void *data, int len)
{
	const MtdPartition *partition;
	MtdWriteContext *out;
	int s, r = 0;

	if(mtd_scan_partitions() < 0) return -1;

	partition = mtd_find_partition_by_name("logo");
	out = mtd_write_partition(partition);
	if(out == NULL) return -1;

    s = mtd_write_data(out, data, len);
	if(s != len) r = -1;

	mtd_write_close(out);

	return r;
}
static int get_device_id(sugr_device_id_t *dev_id)
{
	int r = 0, len = sizeof(sugr_device_id_t);
	if(flash_type == 0)
		r = inand_data_read(dev_id, len);
	else
		r = nand_data_read(dev_id, len);

	return r;
}
static int set_device_id(sugr_device_id_t *dev_id)
{
	int r = 0, len = sizeof(sugr_device_id_t);
	if(flash_type == 0)
		r = inand_data_write(dev_id, len);
	else
		r = nand_data_write(dev_id, len);

	return r;
}
static int serial_number_has(char *num)
{
	int found = 0;
	sugr_device_id_t dev_id;
	memset(&dev_id, 0, sizeof(sugr_device_id_t));

	if(get_device_id((void *)&dev_id) < 0){
		LOGE("get device id fail!\n");
		return 0;
	}

	if(dev_id.tag != SUGR_TAG ){
		LOGE("invalid device id !!!\n");
		return 0;	
	}
	strncpy(num, dev_id.sn, MAX_SN);
	found = 1;

	LOGD("SN: %s\n", serial_num);

	return found;
}
static int new_device_request(sugr_device_id_t *dev_id)
{
	memset(dev_id, 0, sizeof(sugr_device_id_t));
	dev_id->tag = SUGR_TAG;

	return register_device(dev_id);
}
static int serial_number(int need_sync)
{
	int err = -1;
	int sync = 0, has = 0;
	sugr_device_id_t new_id;

	memset(serial_num, 0, sizeof(serial_num));
	while(1){
		if(serial_number_has(serial_num)){
			LOGI("\nSerial Number: %s\n\n", serial_num);
			has = 1;
			if(! need_sync) return 0;	
			if(sync) return 0;
		}
		if(!new_device_request(&new_id)){
			if(! strcmp(serial_num, new_id.sn)){
				LOGD("Same with server!\n");
			}else{
				if(has) LOGW("Old SN: '%s', New SN: '%s'\n", serial_num, new_id.sn);
				if(need_sync || !has)
					err = set_new_device_id(&new_id);
			}
			sync = 1;
		}
		if(err) sleep(3);
	}
}
static int create_sn_file(const char *sn)
{
	int err = 0;
	FILE*fp = NULL;
	char buf[MAX_SN];
	char sn_read[MAX_SN];

	memset(sn_read, 0, MAX_SN);
	strncpy(sn_read, sn, MAX_SN);

	fp = fopen(FILE_SERIAL, "w+");
	if(fp == NULL){
		LOGE("open '%s' fail !\n", FILE_SERIAL);
		return -1;
	}
	/* write */
	fwrite(sn_read, MAX_SN, 1, fp);
	/* sync */
	fflush(fp);
	/* check */
	fseek(fp, 0, SEEK_SET);
	fread(buf, MAX_SN, 1, fp);
	if(strcmp(buf, sn_read)){
		LOGE("'%s' check fail! [F:%s, R:%s]\n", FILE_SERIAL, buf, sn_read);
		err = -1;
	}else{
		LOGD("'%s' check ok. \n", FILE_SERIAL);
		err = 0;
	}
	fclose(fp);
	if(chmod(FILE_SERIAL, S_IREAD)){
		LOGE("%s: chmod fail !\n", __func__);
	}
	LOGD("create: %s\n", FILE_SERIAL);
	return err;
}
static int set_new_serial_number(const char *new_id)
{
	char sn_read[MAX_SN];
	sugr_device_id_t dev_id;
	memset(&dev_id, 0, sizeof(sugr_device_id_t));
	dev_id.tag = SUGR_TAG;
	strncpy(dev_id.sn, new_id, MAX_SN);
	dev_id.sn[MAX_SN - 1] = 0;

	return set_new_device_id(&dev_id);
}
static int set_new_device_id(sugr_device_id_t *dev_id)
{
	char sn_read[MAX_SN];

	memset(sn_read, 0, MAX_SN);
	if(serial_number_has(sn_read)){
		LOGI("Old SN: %s\n", serial_num);
	}

	if(set_device_id(dev_id)){
		LOGE("set serial number fail!\n");
		return -1;
	}
	memset(sn_read, 0, MAX_SN);
	if(!serial_number_has(sn_read)){
		LOGE("read serial number fail after set!\n");
		return -2;
	}
	if(strncmp(sn_read, dev_id->sn, MAX_SN)){
		LOGE("serial number write error!\n");
		return -3;
	}
	LOGI("SN:'%s' set ok!\n", sn_read);

	if(create_sn_file(sn_read)){
		LOGE("create serial_number file fail!\n");
	}
 	return 0;
}
static int clear_device_id(void)
{
	int err;
	char cmd[64];
	sugr_device_id_t dev_id;

	memset(&dev_id, 0, sizeof(sugr_device_id_t));
	if(set_device_id(&dev_id)){
		LOGE("%s fail!\n", __func__);
		return -1;
	}
	sprintf(cmd, "rm -f %s", FILE_SERIAL);
	err = system(cmd);
	LOGI("Clear Done.\n");
	return 0;	
}
static void usage(int argc, char *argv[])
{
	fprintf(stderr, "Usage: %s [-l loglevel] [-h]\n", argv[0]);
	fprintf(stderr, "    -c: clear device id\n");
	fprintf(stderr, "    -f: foreground run\n");
	fprintf(stderr, "    -y: sync SN with server\n");
	fprintf(stderr, "    -l: set log level\n");
	fprintf(stderr, "    -s: set new serial number\n");
	fprintf(stderr, "    -h: print this menu\n");
}
int main(int argc, char *argv[])
{
	int c;
	int log_type;
	int stay_foreground = 0;
	int need_sync = 0;
	char *new_id;

	LOGV("stdout mode:_IOLBF\n");
	setvbuf(stdout, (char *)NULL, _IOLBF, 0);

	opterr = 0;
	do {
		c = getopt(argc, argv, "cfl:s:yh");
		if (c == EOF)
			break;
		switch (c) {
			case 'c':
				clear_device_id();
				return 0;
				break;
			case 'f':
				stay_foreground = 1;
				break;
			case 'y':
				need_sync = 1;
				break;
			case 's':
				new_id = optarg;
				set_new_serial_number(new_id);
				return 0;
			case 't':
				log_type = atoi(optarg);
				break;
			case 'l':
				log_level = atoi(optarg);
				break;
			case '?':
				fprintf(stderr, "%s: invalid option -%c\n",
						argv[0], optopt);
			case 'h':
				usage(argc, argv);
				exit(1);
		}
	}while(1);

	LOGV("Log Level: %d\n\n", log_level);

	if(serial_number_has(serial_num)){
		LOGI("Serial Number: %s\n", serial_num);
		if(create_sn_file(serial_num)){
			LOGE("create serial_number file fail!\n");
		}
		if(!need_sync) return 0;	
	}
	if (!stay_foreground) {
		switch (fork()) {
			case -1: 
				LOGE("serial_number can't fork(%d)\n", getpid());
				exit(0);
				break;
			case 0:
				/* child process */
				LOGE("serial_number fork ok(%d)\n", getpid());
				(void) setsid();
				//freopen("/dev/null", "r", stdin);
				//freopen("/dev/null", "w", stdout);
				//freopen("/dev/null", "w", stderr);
				break;
			default:
				/* parent process should just die */
				_exit(0);
		}   
	}

	serial_number(need_sync);

	LOGI("serial exit!\n");

	return 0;
}

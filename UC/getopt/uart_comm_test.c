#include <stdio.h>
#include <unistd.h>

#define SERIAL_DEVFILE_NAME "/dev/ttyS2"


unsigned char cmd[14][4] = {
        {0x63,0x01,0x02,0x99},//circle 3K Hz
        {0x63,0x01,0x07,0xCF},//circle 2K Hz
        {0x63,0x02,0x0A,0x8C},// WW 2700K
        {0x63,0x02,0x0B,0xB8},//SW 3000K
        {0x63,0x02,0x0D,0xAC},//W 3500K
        {0x63,0x02,0x0F,0xA0},//DW 4000K
        {0x63,0x02,0x13,0x88},//CW 5000K
        {0x63,0x03,0x00,0x64},//100%
        {0x63,0x03,0x00,0x32},//50%
        {0x63,0x03,0x00,0x00},//0%
        {0x62,0x01},//Light on
        {0x62,0x00},//Light off
        {0x68},//get version info
        {0x69},//get CCT and intensity
    };
unsigned char* testArr = cmd[0];
unsigned char* revMessage = cmd[1];
//unsigned char cmd[2] = {0x62,0x01};

static void ledCtrlCommon_test(unsigned char* parm){

    switch(parm[0]){
        case 0x62:
			printf("[0]=0x%02x,[1]=0x%02x\n",parm[0],parm[1]);
            break;
        case 0x63:
			for(int i=0;i<4;i++){
				printf("[%d]=0x%02x ",i,parm[i]);
			}
			printf("\n");
            break;
        case 0x68:
        case 0x69:
			printf("parm=0x%02x \n",parm[0]);
            break;
        default:
            break;
    }

}

static void usage(int argc, char *argv[])
{
    fprintf(stderr, "Usage: %s [-abcdefgijklmnoh:]\n", argv[0]);
    fprintf(stderr, "    -a: PWM 3K Hz Frq\n");
    fprintf(stderr, "    -b: PWM 2K Hz Frq\n");
    fprintf(stderr, "    -c: warm white 2700K \n");
    fprintf(stderr, "    -d: soft white 3000K \n");
    fprintf(stderr, "    -e: white 3500K \n");
    fprintf(stderr, "    -f: DayLight White 4000K\n");
    fprintf(stderr, "    -g: cool White 5000K\n");
    fprintf(stderr, "    -i: adjust to 100 intensity\n");
    fprintf(stderr, "    -j: adjust to 50 intensity\n");
    fprintf(stderr, "    -k: adjust to 0 intensity\n");
    fprintf(stderr, "    -l: light on\n");
    fprintf(stderr, "    -m: light off\n");
    fprintf(stderr, "    -n: get version infomation\n");
    fprintf(stderr, "    -o: get light control infomation\n");
    fprintf(stderr, "    -h: print this menu\n");
}

int main(int argc, char *argv[])
{
    int c;
    int mode = 0;
    printf("test start...\n");
    opterr = 0;
    do {
        c = getopt(argc, argv, "abcdefgijklmnoh");
        if (c == EOF)
        printf("in here ----------\n");
            break;
        switch (c) {
            case 'a':
                testArr = cmd[0];
                break;
            case 'b':
                testArr = cmd[1];
                break;
            case 'c':
                testArr = cmd[2];
                break;
            case 'd':
                testArr = cmd[3];
                break;
            case 'e':
                testArr = cmd[4];
                break;
            case 'f':
                testArr = cmd[5];
                break;
            case 'g':
                testArr = cmd[6];
                break;
            case 'i':
                testArr = cmd[7];
                break;
            case 'j':
                testArr = cmd[8];
                break;
            case 'k':
                testArr = cmd[9];
                break;
            case 'l':
                testArr = cmd[10];
                break;
            case 'm':
                testArr = cmd[11];
                break;
            case 'n':
                testArr = cmd[12];
                break;
            case 'o':
                testArr = cmd[13];
                break;
            case '?':
                fprintf(stderr, "%s: invalid option -%c\n",
                        argv[0], optopt);
            case 'h':
			default:
                usage(argc, argv);
                return -1;
        }
    }while(1);

    ledCtrlCommon_test(testArr);

    return 0;
}

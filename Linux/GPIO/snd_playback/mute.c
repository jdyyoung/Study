#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <pthread.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/time.h>

//#include <audiotk/audio_playback_mmap.h>
//#include <audiotk/audio_vol_ctrl.h>
#include "gpio_sysfs_ctrl.h"



int main(int argc, char* argv[])
{
    if(argc<=1 || strcmp(argv[1],"--help")==0)
    {
        printf("\tUsage: %s [num]\n", argv[0]);
        printf("\t1:turn on; 0:turn off\n");
        return 0;
    }
    if(argc == 2 && strcmp(argv[1],"0")==0)
    {
        printf("unexport,Turn off spk!\n");

        gpio_export(44);      //gpio 44

        gpio_set_dir(44, 1); //1:out;  0:in
        
        gpio_set_value(44, 0);  //value

        gpio_unexport(44);
    }
    else
    {
        printf("export,Turn on spk!\n");

        gpio_export(44);      //gpio 44

        gpio_set_dir(44, 1); //1:out;  0:in
        
        gpio_set_value(44, 1);  //value

    }
/*
    printf("skp no unexport\n");
    
    gpio_export(44);      //gpio 44

	gpio_set_dir(44, 1); //1:out;  0:in
    
    gpio_set_value(44, 1);  //value
    
    //gpio_unexport(44);
*/
}


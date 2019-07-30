#include <stdio.h>

#include "uinput.h"
#include "input.h"


int main(int argc, char ** argv){
	int i;
	int keycode = KEY_A;
	if(argc > 1){
        keycode = atoi(argv[1]);
	}
	printf("uinput: keycode = %d\n",keycode);

	if (setup_uinput_device("send_test") < 0) {
		printf("Unable to find uinput device!!!\n\n");
		return -1;
	}
	for(i=0; i<30; i++){
		printf("send: %d\n", keycode);
		send_button(keycode); // Send a "A" key
		sleep(5);
	}
	destroy_device_and_close();
}

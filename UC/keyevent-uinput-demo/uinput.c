#include<string.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<linux/input.h>
#include<linux/uinput.h>
#include<sys/time.h>
#include<unistd.h>
#include<errno.h>
#include <sys/ioctl.h>
#include "uinput.h"

#define UINPUT_DEV "/dev/input/uinput"

static int uinp_fd = -1;
struct uinput_user_dev uinp; // uInput device structure 
struct input_event event; // Input device structure 
/* Setup the uinput device */
int setup_uinput_device(const char* dev_name) {
	//Temporary variable 
	int i = 0;
	// // Open the input device 
	uinp_fd = open(UINPUT_DEV, O_WRONLY | O_NDELAY);
	if (uinp_fd < 0) {
		printf("Unable to open /dev/uinput\n");
		return -1;
	}
	memset(&uinp, 0, sizeof(uinp)); // Intialize the uInput device to NULL
	strncpy(uinp.name, dev_name, UINPUT_MAX_NAME_SIZE);
	uinp.id.version = 4;
	uinp.id.bustype = BUS_USB;
	// // Setup the uinput device 
	ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY);
	ioctl(uinp_fd, UI_SET_EVBIT, EV_REL);
	ioctl(uinp_fd, UI_SET_RELBIT, REL_X);
	ioctl(uinp_fd, UI_SET_RELBIT, REL_Y);
	for (i = 0; i < 256; i++) {
		ioctl(uinp_fd, UI_SET_KEYBIT, i);
	}
	ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MOUSE);
	ioctl(uinp_fd, UI_SET_KEYBIT, BTN_TOUCH);
	//ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MOUSE); 
	ioctl(uinp_fd, UI_SET_KEYBIT, BTN_LEFT);
	ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MIDDLE);
	ioctl(uinp_fd, UI_SET_KEYBIT, BTN_RIGHT);
	ioctl(uinp_fd, UI_SET_KEYBIT, BTN_FORWARD);
	ioctl(uinp_fd, UI_SET_KEYBIT, BTN_BACK);
	/* Create input device into input sub-system */
	write(uinp_fd, &uinp, sizeof(uinp));
	if (ioctl(uinp_fd, UI_DEV_CREATE)) {
		printf("Unable to create UINPUT device.\n");
		return -1;
	}
	return 1;
}
void send_click_events(int x, int y) {
	// // Move pointer to (0,0) location 
	if (uinp_fd < 0) {
		printf("Unable to open /dev/uinput\n");
		return; 
	}
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	event.type = EV_REL;
	event.code = REL_X;
	event.value = x;
	write(uinp_fd, &event, sizeof(event));
	event.type = EV_REL;
	event.code = REL_Y;
	event.value = y;
	write(uinp_fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));
	// // Report BUTTON CLICK - PRESS event 
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	event.type = EV_KEY;
	event.code = BTN_LEFT;
	event.value = 1;
	write(uinp_fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));
	// // Report BUTTON CLICK - RELEASE event 
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	event.type = EV_KEY;
	event.code = BTN_LEFT;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));
}
void send_a_button() {
	// Report BUTTON CLICK - PRESS event 
	if (uinp_fd < 0) {
		printf("Unable to open /dev/uinput\n");
		return; 
	}
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	event.type = EV_KEY;
	event.code = KEY_A;
	event.value = 1;
	write(uinp_fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));
	// Report BUTTON CLICK - RELEASE event 
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	event.type = EV_KEY;
	event.code = KEY_A;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));
}

int send_input_event(int keycode, int value) 
{
	// Report BUTTON CLICK - PRESS event 
	if (uinp_fd < 0) {
		printf("%s:Unable to open /dev/uinput\n", __func__);
		return -1;
	}
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	// key 
	event.type = EV_KEY;
	event.code = keycode;
	event.value = value;
	write(uinp_fd, &event, sizeof(event));
	// sync
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));

	return 0;
}

void send_button(int keycode) 
{
	printf("%s:%d\n", __func__, keycode);
	// Report BUTTON CLICK - PRESS event 
	if (uinp_fd < 0) {
		printf("Unable to open /dev/uinput\n");
		return;
	}
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	event.type = EV_KEY;
	event.code = keycode;
	event.value = 1;
	write(uinp_fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));

	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	event.type = EV_KEY;
	event.code = keycode;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));
}

int open_device_and_init(const char *dev_name) {
	return setup_uinput_device(dev_name);
}

void mouse_move(int x, int y) {
	// // Move pointer to (0,0) location
	if (uinp_fd < 0) {
		printf("Unable to open /dev/uinput\n");
		return ;
	}
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	event.type = EV_REL;
	event.code = REL_X;
	event.value = x;
	write(uinp_fd, &event, sizeof(event));
	event.type = EV_REL;
	event.code = REL_Y;
	event.value = y;
	write(uinp_fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));
}

void key_event(int keycode) {
	// Report BUTTON CLICK - PRESS event
	if (uinp_fd < 0) {
		printf("Unable to open /dev/uinput\n");
		return ;
	}
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	event.type = EV_KEY;
	event.code = keycode;
	event.value = 1;
	write(uinp_fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));
	// Report BUTTON CLICK - RELEASE event
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	event.type = EV_KEY;
	event.code = keycode;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));
}

void destroy_device_and_close() {
	/* Destroy the input device */
	ioctl(uinp_fd, UI_DEV_DESTROY);
	/* Close the UINPUT device */
	close(uinp_fd);
}

#if 0
/* This function will open the uInput device. Please make
 sure that you have inserted the uinput.ko into kernel. */
int main(int argc, char ** argv){
	int keycode = KEY_A;
	if(argc > 1)
        {
                keycode = atoi(argv[1]);
	}
	printf("uinput: keycode = %d\n",keycode);

	// Return an error if device not found.
	if (setup_uinput_device("test_dev") < 0) {
		printf("Unable to find uinput device!!!\n\n");
		return -1;
	}
	int i;
	for (i = 0; i < 20; i++) {
		send_button(keycode); // Send a "A" key
		sleep(1);
	}
	/* Destroy the input device */
	ioctl(uinp_fd, UI_DEV_DESTROY);
	/* Close the UINPUT device */
	close(uinp_fd);
}
#else
// **********/
#endif

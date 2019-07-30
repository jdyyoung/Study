#ifndef UINPUT_H
#define UINPUT_H

int open_device_and_init(const char* dev_name);
int setup_uinput_device(const char* dev_name);
void destroy_device_and_close();

void send_click_events(int x, int y);

void send_a_button();
void send_button(int keycode);
int send_input_event(int keycode, int value);

void mouse_move(int x, int y);

void key_event(int keycode);


#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <syslog.h>
#include <stdarg.h>

#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/vfs.h>

#include <linux/netlink.h>
#include <linux/version.h>
#include <linux/input.h>


enum KEY_OPERATE_VALUE
{
    STARTUP_NOTIFY_END = 199,
    WECHAT_STOP_AUDIO_INPUT = 200,
    WECHAT_START_AUDIO_INPUT = 201,
    PAUSE_OR_PLAY = 202,
    NEXT_ONE = 203,
    INTO_NONE_MODE = 204,
    READING_AND_STORY_MODE_CHANGE = 205,

    CONFIG_WIFI = 206,
    LAST_ONE = 207,
    READ_WECHAT_MESSAGE = 208,
    VOLUME_DOWN = 209,
    VOLUME_UP = 210,
    TOUCH_AI_BREAK_WAKEUP = 211,
    INTO_STORY = 212,
    INTO_RECOGNIZE = 213,
    INTO_AI_VOICE = 214,
    AI_START_AUDIO_INPUT = 215,
    AI_STOP_AUDIO_INPUT = 216,
    UPDATE_SYSTEM = 217,
    USB_INSERT = 218,
    RESTORE_FACTORY = 219,
    STARTUP_NOTIFY_STORY = 220,
    INTO_AGEING_MODE = 221,
    INTO_UPDATE_MODE = 222,
    VOICE_PAUSE_TEST = 223,
    VOICE_RESUME_TEST = 224,
    VOICE_USER_TEST = 225,
    //无AI语音模式切换
    READING_AND_STORY_MODE_CHANGE_NO_AI = 226,

    //无微聊信息时按键加，有微聊信息时读取微聊信息
    VOLUME_UP_OR_READ_WECHAT_MESSAGE = 227,
    INTO_LOCAL_STORY_SONG_MODE = 228,
    INTO_NET_STORY_SONG_MODE = 229,
    SWICH_NET_LOCAL_STORY_MODE = 230,       //切换网络/本地故事机模式
    SWITCH_STORY_MACHINE_FUNC_MODULE = 231, //切换故事机功能模块
    INTO_HOMEWORK_MODE = 232,               //作业模式
    INTO_EN_TALK_MODE = 233,                //英语对话模式
    HOMEWORK_OR_READ_WECHAT_MESSAGE = 234,  //英语对话模式
    HOMEWORK_BIND_ACCOUT = 235,
    SWITCH_NET_LOCAL_STORY_SONG_MODE = 236, //切换网络儿歌/故事/本地音乐
};

#define KEY_GPIO_DEV "/dev/input/event0"

#define RECOVER_DEV (5 * 1000)
#define LONG_PRESS_200_MS 300         //300 ms
#define LONG_PRESS_2000_MS (2 * 1000) //2 seconds
#define LONG_PRESS_3000_MS (3 * 1000) //3 seconds
#define KEY_DOWN_INTERVAL 600

enum key_press_status_e
{
	KEY_PRESS_RESERVED = -1,
	KEY_PRESS_UP = 0x00,
	KEY_PRESS_DOWN,
	KEY_PRESS_NONE,
};

static int g_key_event = 0;

static unsigned char timer_run_flag = 0;
// static unsigned char dev_di = 0;
// static unsigned char image_di = 0;

static struct timeval start_time;
//static struct timeval last_key_time;
static struct input_event last_key_event;

enum key_press_status_e key_press_status = KEY_PRESS_RESERVED;
char last_file[20] = {0};
static int g_cur_key_down = -1;
static void check_key_hold(void);
static unsigned long daemon_diff_timeval(struct timeval *new_tv, struct timeval *old_tv)
{
	return ((new_tv->tv_sec - old_tv->tv_sec) * 1000) + ((new_tv->tv_usec - old_tv->tv_usec) / 1000);
}
static unsigned char imx327_PT_run = 1;
static unsigned char imx327_PT_lock = 0;
static void imx327_PT_test(int val){
	printf("2022062000-----------val = %d -------\n",val);
	if(imx327_PT_lock){
		printf("imx327_PT changing,please retry!\n");
		return;
	}
	imx327_PT_lock = 1;
	if(imx327_PT_run){
		system("sh /skybell/bin/imx327_PT.sh stop");
		imx327_PT_run = 0;
		
	}
	else{
		system("sh /skybell/bin/imx327_PT.sh start");
		imx327_PT_run = 1;
	}
	sleep(2);
	imx327_PT_lock = 0;
}

static void key_notify_anyka_ipc(enum KEY_OPERATE_VALUE value)
{
	printf("KEY_OPERATE_VALUE value = %d\n",value);
	g_key_event = value;
}

static int do_gpio_key_6(unsigned long period)
{
	if (period < LONG_PRESS_200_MS)
	{
		printf("short press  PAUSE_OR_PLAY --------------------\n");
		key_notify_anyka_ipc(PAUSE_OR_PLAY);
	}
	else
	{
		printf("long press  WECHAT_STOP_AUDIO_INPUT --------------------\n");
		key_notify_anyka_ipc(WECHAT_STOP_AUDIO_INPUT);
	}
	return 0;
}

/**
 * do_gpio_key_6
 * @event[IN]: input_event struct
 * return: 1 key press down; 0 key press up
 *
 */
static int daemon_do_gpio_key_6(struct input_event *event)
{
	int ret = 0;
	unsigned long period = 0;

	switch (event->value)
	{
	case KEY_PRESS_UP:
		timer_run_flag = 0;
		period = daemon_diff_timeval(&event->time, &start_time);
		printf("soft reset key press up, period=%lu(ms)\n", period);
		do_gpio_key_6(period);
		ret = 0;
		break;
	case KEY_PRESS_DOWN:
		start_time.tv_sec = event->time.tv_sec;
		start_time.tv_usec = event->time.tv_usec;
		key_press_status = KEY_PRESS_DOWN;
		printf("soft reset key press down, start calculating time...\n\n");
		timer_run_flag = 1;
		ret = 1;
		break;
	default:
		printf("timer_run_flag = %d---------------\n",timer_run_flag);
		if (timer_run_flag)
		{
			check_key_hold();
		}
		break;
	}

	return ret;
}


static int do_key(struct input_event *key_event, int key_cnt)
{
	int i = 0;
	int ret = -1;
	struct input_event *event;

	if (key_cnt < (int)sizeof(struct input_event))
	{
		printf("expected %d bytes, got %d\n",sizeof(struct input_event), key_cnt);
		return -1;
	}
	int key_event_cnt = key_cnt / sizeof(struct input_event);

	for (i = 0; (i < key_cnt / sizeof(struct input_event)); i++)
	{
		event = (struct input_event *)&key_event[i];
		if (EV_KEY != event->type)
		{
			continue;
		}
		printf("---count=%d, code=%d, value=%d\n", key_event_cnt, event->code, event->value);

		g_cur_key_down = event->code;
		printf("handler event:");
		switch (event->code)
		{
		case 110:
		case 111:
		case 112:
		case 113:
			printf(" %d\n",event->code);
			ret = daemon_do_gpio_key_6(event);
			break;
		default:
			printf("event->code: %d, Error key code!", event->code);
			ret = -1;
			break;
		}
		if (ret == 0 && key_event_cnt <= 2)
		{
			break;
		}
	}

	return ret;
}

static void check_key_hold(void)
{
	struct timeval cur_time;

	gettimeofday(&cur_time, NULL);
	unsigned long period = daemon_diff_timeval(&cur_time, &start_time);
	printf("period=%lu(ms)\n", period);

	if (period >= LONG_PRESS_200_MS && KEY_0 == g_cur_key_down)
	{
		timer_run_flag = 0;
		printf("long press 200ms WECHAT_START_AUDIO_INPUT===================\n");
		// key_notify_anyka_ipc(WECHAT_START_AUDIO_INPUT);
		return;
	}

	if (period >= LONG_PRESS_2000_MS)
	{
		timer_run_flag = 0;
		switch (g_cur_key_down)
		{
		case KEY_1:
		case 100:
			if (period <= LONG_PRESS_3000_MS)
			{
				timer_run_flag = 1;
			}
			else
			{
				printf("long press 5S INTO AGEING TEST===================\n");
				// key_notify_anyka_ipc(INTO_AGEING_MODE);
			}
			break;
		case KEY_2:
		case 101:
			if (period <= LONG_PRESS_3000_MS)
			{
				timer_run_flag = 1;
			}
			else
			{
				key_notify_anyka_ipc(UPDATE_SYSTEM);
			}
			break;
		case KEY_3:
		case 102:
			printf("long press 2S CONFIG_WIFI===================\n");
			key_notify_anyka_ipc(CONFIG_WIFI);
			break;
		case KEY_4:
		case 103:
			if (period <= RECOVER_DEV)
			{
				timer_run_flag = 1;
			}
			else
			{
				key_notify_anyka_ipc(RESTORE_FACTORY);
			}
			break;
		default:
			break;
		}
	}
}

int product_key(void)
{
	int ret = 0;
	int gpio_fd, adc_fd;

	/* open gpio key device */
	if ((gpio_fd = open(KEY_GPIO_DEV, O_RDONLY)) < 0)
	{
		perror("Open gpio key dev fail");
		return -ENOENT;
	}

	int rd = 0;
	struct input_event key_event[64];
	struct timeval tv;
	fd_set readfds, tempfds;

	gettimeofday(&last_key_event.time, NULL);
	//printf("gettimeofday: tv_sec=%ld, tv_usec=%ld\n", last_key_time.tv_sec, last_key_time.tv_usec);

	FD_ZERO(&readfds);
	/* add the gpio_fd to the readfds set */
	FD_SET(gpio_fd, &readfds);
	/* set the gpio_fd's close-on-exec attribute */
	fcntl(gpio_fd, F_SETFD, FD_CLOEXEC);

	int max = gpio_fd ;

	// struct timeval os_start_tv;
	// do
	// {
		// get_ostime((struct timeval*)&os_start_tv);
		// printf("系统启动没有20s不接收按键信息\n");
		//如果系统启动没有20s不接收按键信息
		// sleep(1);
		// continue;
	// } while (os_start_tv.tv_sec < 20);

	/* check key pressed */
	while (1)
	{
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		tempfds = readfds;
		/**** monitor the tempfds no-blocking ****/
		ret = select((max + 1), &tempfds, NULL, NULL, &tv);

		switch (ret)
		{
		case -1:
			perror("select");
		case 0: //timeout
			// if (timer_run_flag){
			// 	printf("select timeout ===================\n");
			// 	check_key_hold();
			// }
			break;
		default:
			/** To test whether the gpio_fd's status has changed **/
			if (FD_ISSET(gpio_fd, &tempfds))
			{
				/** read the event to the buf **/
				rd = read(gpio_fd, key_event,sizeof(key_event));
				/*** parse the event ***/
				do_key(key_event, rd);
			}
			break;
		}
	}

	close(gpio_fd);
	close(adc_fd);
	return 0;
}

static void *key_event_product(void *parm)
{
	printf("thread ID = %lu\n", pthread_self());
	pthread_detach(pthread_self());
	product_key();
	return NULL;
}

static void *key_event_handle(void *parm)
{
	int key_event;
	printf("thread ID = %lu\n", pthread_self());
	pthread_detach(pthread_self());
	while (1)
	{
		if (g_key_event)
		{
			key_event = g_key_event;
			g_key_event = 0;
			imx327_PT_test(key_event);
		}
		//10ms
		usleep(10*1000);
	}
	
	return NULL;
}

void key_product_th(void)
{
	printf("key_product thread\n");
	pthread_t key_product_tid;
	pthread_create(&key_product_tid, NULL,key_event_product, NULL);
	printf("key_product thread ok\n");
}

void key_handle_th(void)
{
	printf("key_handle thread\n");
	pthread_t key_handle_tid;
	pthread_create(&key_handle_tid, NULL,key_event_handle, NULL);
	printf("key_handle thread ok\n");
}

int main(){
	printf("-------------------gpio key test start --------\n");
	key_handle_th();
	key_product_th();
	while(1){
		sleep(1);
	}
}

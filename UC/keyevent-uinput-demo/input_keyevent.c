#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/inotify.h>
//#include <sys/limits.h>
#include <sys/poll.h>
//#include <linux/input.h>
#include <errno.h>

#include "getevent.h"

#include "system_server.h"
#include "input_manager.h"

#include "debug.h"

#define GETEVENT_TIMEOUT_MS  2000

static struct pollfd *ufds;
static char **device_names;
static int nfds;


enum {
    PRINT_DEVICE_ERRORS     = 1U << 0,
    PRINT_DEVICE            = 1U << 1,
    PRINT_DEVICE_NAME       = 1U << 2,
    PRINT_DEVICE_INFO       = 1U << 3,
    PRINT_VERSION           = 1U << 4,
    PRINT_POSSIBLE_EVENTS   = 1U << 5,
    PRINT_INPUT_PROPS       = 1U << 6,
    PRINT_HID_DESCRIPTOR    = 1U << 7,

    PRINT_ALL_INFO          = (1U << 8) - 1,

    PRINT_LABELS            = 1U << 16,
};


#define EL(x,y...)  {printf("[*** %s : %d] ",__FILE__,__LINE__); printf(x,##y); printf("\n");}

extern int g_ctrl_command;
extern int g_player_mode;
int g_play_pending = 0;


static int timeval_to_ms(const struct timeval *tv)
{
	return (tv->tv_sec * 1000 + tv->tv_usec);
}
#if 1
static int modekey_last_time = 0;
static int keyevent_process(struct input_event *event)
{
	switch (event->code)
	{
		case KEY_MODE://mode
			{
				if(event->value == 1){
				EL("KEY_MODE **************");
				g_ctrl_command = 'M';//KEY_MODE;
				}
			}
			break;
		case KEY_PLAYPAUSE://playpause
			{
				if(event->value == 1){
				EL("KEY_PLAYPAUSE **************");
				g_ctrl_command = 'p';//KEY_PAUSE;
				}
			}
			break;
		case KEY_PLAY://playpause
			{
				if(event->value == 1){
					EL("KEY_PLAY **************");
					if(g_play_pending)
						g_ctrl_command = 'p';//KEY_PAUSE;
				}
			}
			break;
		case KEY_STOP://stop
			{
				if(event->value == 1){
					EL("KEY_STOP **************");
					if(!g_play_pending)
						g_ctrl_command = 'p';//KEY_PAUSE;
					//g_ctrl_command = 's';//KEY_STOP;
				}
			}
			break;
		case KEY_FORWARD://next
			{
				if(event->value == 1){
				EL("KEY_FORWARD **************");
				g_ctrl_command = 'f';//KEY_FORWARD;
				}
			}
			break;
		case KEY_BACK:// back
            {
                if(event->value == 1){
                EL("KEY_BACK **************");
                g_ctrl_command = 'b';//KEY_BACK;
                }
            }
                        break;
		case KEY_VOLUMEUP://V+
			{
				if(event->value == 1){
					modekey_last_time = event->time.tv_sec;
					g_ctrl_command = '+';//KEY_GAININCR;
				}else{
					if((event->time.tv_sec - modekey_last_time) > 5){
							
					}else if((event->time.tv_sec - modekey_last_time) > 3){
						g_player_mode = 1;
						g_ctrl_command = 'f';
						EL("g_ctrl_command goto FMT mode");
					}
				}
			}
			break;
		case KEY_VOLUMEDOWN://V-
			{
				if(event->value == 1){
					EL("KEY_VOLUMEDOWN **************");
					g_ctrl_command = '-';//KEY_GAINDECR;
				}
			}
			break;
		case KEY_CLOSE:// quit
			{
				if(event->value == 1){
				EL("KEY_QUIT **************");
				g_ctrl_command = 'q';//KEY_QUIT;
				}
			}
			break;
		default:
			EL("defualt **************");
			break;
	}

	return 0;
}
#endif

static int last_xyz[3] = { -1, -1, -1,};
static int  cur_xyz[3] = { -1, -1, -1,};
static int last_type = -1;
static int handle_event(int type, int code, int value)
{
	int flag = 0;

	if(type == EV_ABS){
		if(code == 0) cur_xyz[0] = value;
		if(code == 1) cur_xyz[1] = value;
		if(code == 2) cur_xyz[2] = value;
	}
	if(type == EV_SYN && last_type == EV_ABS){
		flag = 1;
	}

	last_type = type;
	
	if(flag == 0) {
	/* Not a complete event*/
		if(type == EV_SYN){
		/* not a ABS event sync */
			cur_xyz[0] = -1;
			cur_xyz[1] = -1;
			cur_xyz[2] = -1;
		}
		return -1;
	}

	/* New ABS event come */

	if(cur_xyz[0] == -1 || cur_xyz[1] == -1 || cur_xyz[2] == -1){
		cur_xyz[0] = -1;
		cur_xyz[1] = -1;
		cur_xyz[2] = -1;
		return -2;
	}
	LOGV("new :[%d,%d,%d]\n",cur_xyz[0],cur_xyz[1],cur_xyz[2]);
	LOGV("last:[%d,%d,%d]",last_xyz[0],last_xyz[1],last_xyz[2]);
	last_xyz[0] = cur_xyz[0];
	last_xyz[1] = cur_xyz[1];
	last_xyz[2] = cur_xyz[2];
	cur_xyz[0] = -1;
	cur_xyz[1] = -1;
	cur_xyz[2] = -1;
	return 0;
}
#define MI_EVENTS_MAX	16
#define MI_EVENT_LEN	32
struct h_events{
char * dev_name;
int fd;
int len;
struct input_event events[MI_EVENT_LEN];
};
static struct h_events mi_events[MI_EVENTS_MAX];


int handle_abs_event(struct input_event *event, int len)
{
	int type = event[0].type;
	int code = event[0].code;
	int value = event[0].value;

	input_dispatch(event,len);	
    LOGV("%s:[%d] %04x %04x %08x\n",__func__,len, type, code, value);
	return 0;
}
int handle_key_event(struct input_event *event, int len)
{
	int type = event[0].type;
	int code = event[0].code;
	int value = event[0].value;

//	keyevent_process(event);
	input_dispatch(event,len);	

    LOGV("%s:[%d] %04x %04x %08x\n",__func__,len, type, code, value);
	return 0;
}
int handle_switch_event(struct input_event *event, int len)
{
	int type = event[0].type;
	int code = event[0].code;
	int value = event[0].value;

	input_dispatch(event,len);	

    LOGV("%s:[%d] %04x %04x %08x\n",__func__,len, type, code, value);
	return 0;
}

static int handle_events(int fd, struct input_event *event)
{
	int i,cur_id = -1;

	for(i=0; i<8; i++){
		if(fd == mi_events[i].fd){
			if(mi_events[i].len >= MI_EVENT_LEN){
				LOGE("[%s:%d]%s: events too large! Fixme!!!\n", __FILE__, __LINE__, __func__);
				mi_events[i].len = 0;
			}
			memcpy(&mi_events[i].events[mi_events[i].len],event,sizeof(struct input_event));
			mi_events[i].len++;
			cur_id = i;
			break;
		}
	}
	/* new device event */
	if(cur_id == -1){
		for(i=0; i<MI_EVENTS_MAX; i++){
			if(mi_events[i].len == 0){
				memcpy(&mi_events[i].events[0],event,sizeof(struct input_event));
				mi_events[i].len++;
				mi_events[i].fd = fd;
				cur_id = i;
				break;
			}
		}
	}
    LOGD("%s:%04x %04x %08x {%d:%d, len=%d}\n",__func__, event->type, 
			event->code, event->value, fd, cur_id, mi_events[i].len);
	if(cur_id == -1){
		LOGE("[%s:%d] mi_events is FULL !  Fixme!!!\n", __FILE__, __LINE__);
		return -1;
	}
	/* hande the events */
	if(event->type == EV_SYN){
		struct input_event *pevent = &mi_events[cur_id].events[0];
		int type,len;

		type = pevent->type;
		len  = mi_events[cur_id].len;
		if(len > 1) len -= 1;

		switch(type){
			case EV_SYN:
				if(event->value == 1){
					LOGD("\nSync report !\n\n");
				}else{
					LOGE("EV_SYN ,please Check it!\n");
				}
				mi_events[cur_id].len = 0;
				break;
			case EV_SW:
				handle_switch_event(pevent,len);
				mi_events[cur_id].len = 0;
				break;
			case EV_KEY:
				handle_key_event(pevent,len);
				mi_events[cur_id].len = 0;
				break;
			case EV_REL:
				LOGD("EV_REL, Fixme!\n");
				mi_events[cur_id].len = 0;
				break;
			case EV_MSC:
				mi_events[cur_id].len = 0;
				break;
			case EV_ABS:
				handle_abs_event(pevent,len);
				mi_events[cur_id].len = 0;
				break;
			default:
				LOGD("unkown event type [%d]\n",type);
				mi_events[cur_id].len = 0;
				break;
		}
	}
    LOGV("%s:%04x %04x %08x {%d:%d, len=%d} done!\n",__func__, event->type, 
			event->code, event->value, fd, cur_id, mi_events[i].len);
	return 0;
}

static int open_device(const char *device, int print_flags)
{
    int version;
    int fd;
    struct pollfd *new_ufds;
    char **new_device_names;
    char name[80];
    char location[80];
    char idstr[80];
    struct input_id id;

    fd = open(device, O_RDWR);
    if(fd < 0) {
        fprintf(stderr, "could not open %s, %s\n", device, strerror(errno));
        return -1;
    }
     
    name[sizeof(name) - 1] = '\0';
    location[sizeof(location) - 1] = '\0';
    idstr[sizeof(idstr) - 1] = '\0';
    if(ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1) {
        //fprintf(stderr, "could not get device name for %s, %s\n", device, strerror(errno));
        name[0] = '\0';
    }

	if(strcmp(name , "lis3dh_acc") == 0) {	
		LOGD("is lis3dh_acc !!!!!\n");
		return 0;
	}
	if(strlen(name) == 0){
		LOGV("no name device %s\n", device);
		return -1;
	}

	if(ioctl(fd, EVIOCGVERSION, &version)) {
        fprintf(stderr, "could not get driver version for %s, %s\n", device, strerror(errno));
        return -1;
    }
    if(ioctl(fd, EVIOCGID, &id)) {
        fprintf(stderr, "could not get driver id for %s, %s\n", device, strerror(errno));
        return -1;
    }

    if(ioctl(fd, EVIOCGPHYS(sizeof(location) - 1), &location) < 1) {
        //fprintf(stderr, "could not get location for %s, %s\n", device, strerror(errno));
        location[0] = '\0';
    }
    if(ioctl(fd, EVIOCGUNIQ(sizeof(idstr) - 1), &idstr) < 1) {
        //fprintf(stderr, "could not get idstring for %s, %s\n", device, strerror(errno));
        idstr[0] = '\0';
    }

    new_ufds = realloc(ufds, sizeof(ufds[0]) * (nfds + 1));
    if(new_ufds == NULL) {
        fprintf(stderr, "out of memory\n");
        return -1;
    }
    ufds = new_ufds;
    new_device_names = realloc(device_names, sizeof(device_names[0]) * (nfds + 1));
    if(new_device_names == NULL) {
        fprintf(stderr, "out of memory\n");
        return -1;
    }
    device_names = new_device_names;

    if(print_flags & PRINT_DEVICE_INFO)
		LOGD("add device %d: %s\n", nfds, device);
    if(print_flags & PRINT_DEVICE_INFO)
        LOGD("  bus:      %04x\n"
               "  vendor    %04x\n"
               "  product   %04x\n"
               "  version   %04x\n",
               id.bustype, id.vendor, id.product, id.version);
    if(print_flags & PRINT_DEVICE_NAME)
        LOGD("  name:     \"%s\"\n", name);
    if(print_flags & PRINT_DEVICE_INFO)
        LOGD("  location: \"%s\"\n"
               "  id:       \"%s\"\n", location, idstr);
    if(print_flags & PRINT_VERSION)
        LOGD("  version:  %d.%d.%d\n",
               version >> 16, (version >> 8) & 0xff, version & 0xff);

    ufds[nfds].fd = fd;
    ufds[nfds].events = POLLIN;
	ufds[nfds].revents = 0;
    device_names[nfds] = strdup(device);
    nfds++;

    return 0;
}

int close_device(const char *device, int print_flags)
{
    int i;
    for(i = 1; i < nfds; i++) {
        if(strcmp(device_names[i], device) == 0) {
            int count = nfds - i - 1;
            LOGD("remove device %d: %s\n", i, device);
            free(device_names[i]);
            memmove(device_names + i, device_names + i + 1, sizeof(device_names[0]) * count);
            memmove(ufds + i, ufds + i + 1, sizeof(ufds[0]) * count);
            nfds--;
            return 0;
        }
    }
    fprintf(stderr, "remote device: %s not found\n", device);
    return -1;
}

static int read_notify(const char *dirname, int nfd, int print_flags)
{
    int i, res;
    char devname[PATH_MAX];
    char *filename;
    char event_buf[512];
    int event_size;
    int event_pos = 0;
    struct inotify_event *event;

    res = read(nfd, event_buf, sizeof(event_buf));
    if(res < (int)sizeof(*event)) {
        if(errno == EINTR)
            return 0;
        fprintf(stderr, "could not get event, %s\n", strerror(errno));
        return 1;
    }

    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';

    while(res >= (int)sizeof(*event)) {
        event = (struct inotify_event *)(event_buf + event_pos);
        if(event->len) {
            strcpy(filename, event->name);
			if(event->mask & IN_CREATE) {
                open_device(devname, print_flags);
            }
            else {
                close_device(devname, print_flags);
            }
        }
        event_size = sizeof(*event) + event->len;
        res -= event_size;
        event_pos += event_size;
    }
    return 0;
}

static int scan_dir(const char *dirname, int print_flags)
{
    char devname[PATH_MAX];
    char *filename;
    DIR *dir;
    struct dirent *de;
    dir = opendir(dirname);
    if(dir == NULL)
        return -1;
    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';
    while((de = readdir(dir))) {
        if(de->d_name[0] == '.' &&
           (de->d_name[1] == '\0' ||
            (de->d_name[1] == '.' && de->d_name[2] == '\0')))
            continue;
        strcpy(filename, de->d_name);
        open_device(devname, print_flags);
    }
    closedir(dir);
    return 0;
}
static int keyevent_pollfail_handle(void)
{
	FUNC();
	LOGE("[%s:%d]%s: Need Fix!!!\n", __FILE__, __LINE__, __func__);
	exit(-1);
	return 0;
}
void KeysProcess(void* data)
{
    int c;
    int i;
    int res;
    int pollres;
    int get_time = 0;
    int print_device = 0;
    char *newline = "\n";
    uint16_t get_switch = 0;
    struct input_event event;
    int version;
    int print_flags = 0;
    int print_flags_set = 0;
    int event_count = 0;
    int sync_rate = 0;
    int64_t last_sync_time = 0;
    const char *device = NULL;
    const char *device_path = "/dev/input";
	int poll_faild_count = 0;

	input_manager_t * im = (input_manager_t *)data;
     
    nfds = 1;
    ufds = calloc(1, sizeof(ufds[0]));
    ufds[0].fd = inotify_init();
    ufds[0].events = POLLIN;

	res = inotify_add_watch(ufds[0].fd, device_path, IN_DELETE | IN_CREATE);
	if(res < 0) {
		fprintf(stderr, "could not add watch for %s, %s\n", device_path, strerror(errno));
		return ;
	}
	res = scan_dir(device_path, print_flags);
	if(res < 0) {
		fprintf(stderr, "scan dir failed for %s\n", device_path);
		return ;
	}

	im->input_dev_ready = 1;

    memset(mi_events,0,sizeof(mi_events));

	poll_faild_count = 0;
    while(1) {
		im->sleep_permit = 1;
		pollres = poll(ufds, nfds, -1);
		im->sleep_permit = 0;
//		pollres = poll(ufds, nfds, GETEVENT_TIMEOUT_MS);
        LOGV("poll %d, returned %d\n", nfds, pollres);
		if(pollres < 1){
			LOGE("pollres=%d, poll Faild Count=%d\n", pollres, poll_faild_count);
			if(pollres == 0){
				LOGE("keyevent poll timeout\n\n");	
			}else if(pollres < 0){
				perror("keyevent poll fail !!!\n\n");	
				if(poll_faild_count > 1){
					keyevent_pollfail_handle();	
					poll_faild_count = 0;
				}
				poll_faild_count++;
			}
			continue;
		}
        if(ufds[0].revents & POLLIN) {
            read_notify(device_path, ufds[0].fd, print_flags);
        }
        for(i = 1; i < nfds; i++) {
            if(ufds[i].revents) {
                if(ufds[i].revents & POLLIN) {
                    res = read(ufds[i].fd, &event, sizeof(event));
                    if(res < (int)sizeof(event)) {
                        fprintf(stderr, "INPUT_DEV[%s] could not get event, continue ...{res:%d,%d}\n", 
								device_names[i], res, (int)sizeof(event));
						continue;
                    }
					handle_events(ufds[i].fd, &event);
                }
            }
        }
    }
}

int get_switch_state(int sw)
{
	int i, swState;
	char *device = "gpio-keys";
	int fd = 0;
	char name[80];

	name[sizeof(name) - 1] = '\0';

	for(i = 1; i < nfds; i++) {
		if(ioctl(ufds[nfds - i].fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1) {
			name[0] = '\0';
			continue;
		}   
		LOGV("name : %s\n", name);
		if(strcmp(name, device) == 0) {
			fd = ufds[nfds - i].fd;
			LOGV("Find %s\n", device);
			break;
		}
	}	
	if(fd && ioctl(fd, EVIOCGSW(SW_MAX+1), &swState) >= 0){
		LOGV("swState=%04x\n", swState);
		return (1 << sw) & swState ? 1 : 0;
	}
	return -1;
}
int get_power_switch_state(void)
{
	int state = get_switch_state(SW_TABLET_MODE); 
	LOGW("Power Switch State: %d\n", state);
	return state;
}

int keyEventThreadCreate(void)
{
	int i_ret;
	pthread_t thread_id;
	pthread_create( &thread_id, NULL, (void *)KeysProcess, NULL/*p_data*/ );                                   
	return 0;
}


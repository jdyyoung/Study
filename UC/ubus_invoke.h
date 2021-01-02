#ifndef __UBUS_INVOKE_H__
#define __UBUS_INVOKE_H__
#include <json/json.h>
#include <libubox/blobmsg_json.h>
 
 
struct prog_attr {
	char name[64];
	int chn_id;
};
#define PROG_MAX	8
 
 
#endif  /* __UBUS_INVOKE_H__ */
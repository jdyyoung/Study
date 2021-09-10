#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>
#include <json/json.h>
#include <libubox/blobmsg_json.h>
#include "ubus_invoke.h"
 
static struct ubus_context * ctx = NULL;
static struct blob_buf b;
static const char * sock_path;
 
static struct prog_attr uri_list[PROG_MAX] = 
{
	{"program_beijing", 1},
	{"program_guangzhou", 2},
	{"program_shanghai", 3},
	{"", -1},
};
 
enum
{
	SCAN_CHNID,
	SCAN_POLICY_MAX,
};
 
static const struct blobmsg_policy scan_policy[SCAN_POLICY_MAX] = {
	[SCAN_CHNID] = {.name = "chnID", .type = BLOBMSG_TYPE_INT32},
};
 
static int ubus_start_scan(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	int ret = -1;
	void * json_list = NULL;
	void * json_uri = NULL;
	int idx;
 
	blob_buf_init(&b, 0);
	
	struct blob_attr *tb[SCAN_POLICY_MAX];
	blobmsg_parse(scan_policy, SCAN_POLICY_MAX, tb, blob_data(msg), blob_len(msg));
 
	/*
	本例子中，如果请求特定的节目号，返回节目名称。
	如果请求节目号是0，则返回所有节目列表。
	*/
	if (tb[SCAN_CHNID] != NULL)
	{
		int chnid = blobmsg_get_u32(tb[SCAN_CHNID]);
 
		if (chnid == 0)
		{
			json_uri = blobmsg_open_array(&b, "prog_list");
			for (idx = 0; idx < PROG_MAX; idx++)
			{
				if ('\0' != uri_list[idx].name[0])
				{
					json_list = blobmsg_open_table(&b, NULL);
					blobmsg_add_string(&b, "name", uri_list[idx].name);
					blobmsg_add_u32(&b, "channel", uri_list[idx].chn_id);
					blobmsg_close_table(&b, json_list);
				}
			}
			blobmsg_close_array(&b, json_uri);
			ret = 0;
		}
		else
		{
			for (idx = 0; idx < PROG_MAX; idx++)
			{
				if ('\0' != uri_list[idx].name[0] && uri_list[idx].chn_id == chnid)
				{
					blobmsg_add_string(&b, "name", uri_list[idx].name);
					ret = 0;
				}
			}
		}
	}
	
	blobmsg_add_u32(&b, "result", ret);
	ubus_send_reply(ctx, req, b.head);
	
	return 0;
}
 
/* 方法列表 */
static struct ubus_method scan_methods[] = 
{
	UBUS_METHOD("scan", ubus_start_scan, scan_policy),
};
 
/* type目前没有实际用处 */
static struct ubus_object_type scan_obj_type
= UBUS_OBJECT_TYPE("scan_prog", scan_methods);
 
static struct ubus_object scan_obj = 
{
	.name = "scan_prog", /* 对象的名字 */
	.type = &scan_obj_type,
	.methods = scan_methods,
	.n_methods = ARRAY_SIZE(scan_methods),
};
 
static void ubus_add_fd(void)
{
	ubus_add_uloop(ctx);
 
#ifdef FD_CLOEXEC
	fcntl(ctx->sock.fd, F_SETFD,
		fcntl(ctx->sock.fd, F_GETFD) | FD_CLOEXEC);
#endif
}
 
static void ubus_reconn_timer(struct uloop_timeout *timeout)
{
	static struct uloop_timeout retry =
	{
		.cb = ubus_reconn_timer,
	};
	int t = 2;
 
	if (ubus_reconnect(ctx, sock_path) != 0) {
		uloop_timeout_set(&retry, t * 1000);
		return;
	}
 
	ubus_add_fd();
}
 
static void ubus_connection_lost(struct ubus_context *ctx)
{
	ubus_reconn_timer(NULL);
}
 
static int display_ubus_init(const char *path)
{
	uloop_init();
	sock_path = path;
 
	ctx = ubus_connect(path);
	if (!ctx)
	{
		printf("ubus connect failed\n");
		return -1;
	}
	
	printf("connected as %08x\n", ctx->local_id);
	ctx->connection_lost = ubus_connection_lost;
 
	ubus_add_fd();
 
	/* 向ubusd注册对象和方法，供其他ubus客户端调用。 */
	if (ubus_add_object(ctx, &scan_obj) != 0)
	{
		printf("ubus add obj failed\n");
		return -1;
	}
 
	return 0;
}
 
static void display_ubus_done(void)
{
	if (ctx)
		ubus_free(ctx);
}
 
int main(int argc, char * argv[])
{
	char * path = NULL;
	
	if (-1 == display_ubus_init(path))
	{
		printf("ubus connect failed!\n");
		return -1;
	}
 
	uloop_run();
 
	display_ubus_done();
 
	return 0;
}
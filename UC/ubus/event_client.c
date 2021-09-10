#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>
#include <json/json.h>
#include <libubox/blobmsg_json.h>
 
static struct ubus_context * ctx = NULL;
static const char * cli_path;
 
#define	UBUS_EVENT_ADD_DEVICE	"add_device"
#define	UBUS_EVENT_REMOVE_DEVICE	"rm_device"
 
static void ubus_probe_device_event(struct ubus_context *ctx, struct ubus_event_handler *ev,
			  const char *type, struct blob_attr *msg)
{
	char *str;
 
	if (!msg)
		return;
 
	/* 
	在这里实现收到事件后的动作。
	event也可以传递消息，放在msg中。
	
	本例子只是将返回的消息打印出来。
	*/
	str = blobmsg_format_json(msg, true);
	printf("{ \"%s\": %s }\n", type, str);
	free(str);
}
 
static int client_ubus_register_events()
{
	static struct ubus_event_handler listener;
	int ret = 0;
 
	/* 注册特定event的listener。多个event可以使用同一个listener */
	memset(&listener, 0, sizeof(listener));
	listener.cb = ubus_probe_device_event;
	
	ret = ubus_register_event_handler(ctx, &listener, UBUS_EVENT_ADD_DEVICE);
	if (ret)
	{
		fprintf(stderr, "register event failed.\n");
		return -1;
	}
 
	ret = ubus_register_event_handler(ctx, &listener, UBUS_EVENT_REMOVE_DEVICE);
	if (ret)
	{
		ubus_unregister_event_handler(ctx, &listener);
		fprintf(stderr, "register event failed.\n");
		return -1;
	}
 
	return 0;
}
 
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
 
	if (ubus_reconnect(ctx, cli_path) != 0) {
		uloop_timeout_set(&retry, t * 1000);
		return;
	}
 
	ubus_add_fd();
}
 
static void ubus_connection_lost(struct ubus_context *ctx)
{
	ubus_reconn_timer(NULL);
}
 
static int client_ubus_init(const char *path)
{
	uloop_init();
	cli_path = path;
	
	ctx = ubus_connect(path);
	if (!ctx)
	{
		printf("ubus connect failed\n");
		return -1;
	}
	
	printf("connected as %08x\n", ctx->local_id);
	ctx->connection_lost = ubus_connection_lost;
 
	ubus_add_fd();
 
	return 0;
}
 
static void client_ubus_done(void)
{
	if (ctx)
		ubus_free(ctx);
}
 
int main(int argc, char * argv[])
{
	char * path = NULL;
 
	/* 连接ubusd */
	if (UBUS_STATUS_OK != client_ubus_init(path))
	{
		printf("ubus connect failed!\n");
		return -1;
	}
 
	/* 注册某个事件的处理函数 */
	client_ubus_register_events();
 
	uloop_run();
	
	client_ubus_done();
 
	return 0;
}
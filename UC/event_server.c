#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>
#include <json/json.h>
#include <libubox/blobmsg_json.h>
 
static struct ubus_context * ctx = NULL;
static struct blob_buf b;
static const char * sock_path;
 
static int server_ubus_send_event(void)
{
	blob_buf_init(&b, 0);
 
	/* 需要传递的参数 */
	blobmsg_add_u32(&b, "major", 3);
	blobmsg_add_u32(&b, "minor", 56);
	blobmsg_add_string(&b, "name", "mmc01");
 
	/* 广播名为"add_device"的事件 */
	return ubus_send_event(ctx, "add_device", b.head);
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
 
	server_ubus_send_event();
 
	display_ubus_done();
 
	return 0;
}
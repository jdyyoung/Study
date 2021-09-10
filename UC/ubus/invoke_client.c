#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>
#include <json/json.h>
#include <libubox/blobmsg_json.h>
#include "ubus_invoke.h"
 
static struct ubus_context * ctx = NULL;
static struct blob_buf b;
static const char * cli_path;
 
enum
{
	SCAN_CHNID,
	SCAN_POLICY_MAX,
};
 
static const struct blobmsg_policy scan_policy[SCAN_POLICY_MAX] = {
	[SCAN_CHNID] = {.name = "chnID", .type = BLOBMSG_TYPE_INT32},
};
 
static int timeout = 30;
static bool simple_output = false;
 
static void scanreq_prog_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	char *str;
	if (!msg)
		return;
 
	/* 
	在这里处理返回的消息。
	本例子只是将返回的消息打印出来。
	*/
	str = blobmsg_format_json_indent(msg, true, simple_output ? -1 : 0);
	printf("%s\n", str);
	free(str);
}
 
static int client_ubus_call()
{
	unsigned int id;
	int ret;
 
	blob_buf_init(&b, 0);
 
	/* 需要传递的参数 */
	blobmsg_add_u32(&b, scan_policy[SCAN_CHNID].name, 0);
 
	/*
	向ubusd查询是否存在"scan_prog"这个对象，
	如果存在，返回其id
	*/
	ret = ubus_lookup_id(ctx, "scan_prog", &id);
	if (ret != UBUS_STATUS_OK) {
		printf("lookup scan_prog failed\n");
		return ret;
	}
	else {
		printf("lookup scan_prog successs\n");
	}
	
	/* 调用"scan_prog"对象的"scan"方法 */
	return ubus_invoke(ctx, id, "scan", b.head, scanreq_prog_cb, NULL, timeout * 1000);
}
 
/*
ubus_invoke()的声明如下:
int ubus_invoke(struct ubus_context *ctx, uint32_t obj, const char *method,
                struct blob_attr *msg, ubus_data_handler_t cb, void *priv, int timeout);
                
其中cb参数是消息回调函数，其函数类型定义为:
typedef void (*ubus_data_handler_t)(struct ubus_request *req,
				    int type, struct blob_attr *msg);
参数type是请求消息的类型，如UBUS_MSG_INVOKE，UBUS_MSG_DATA等。
参数msg存放从服务端得到的回复消息，struct blob_attr类型，同样用blobmsg_parse()来解析。
参数req保存了请求方的消息属性，其中req->priv即ubus_invoke()中的priv参数，
用这个参数可以零活的传递一些额外的数据。
*/
 
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
 
	return 0;
}
 
static void client_ubus_done(void)
{
	if (ctx)
		ubus_free(ctx);
}
 
int main(int argc, char * argv[])
{
	/* ubusd创建的unix socket，默认值为"/var/run/ubus.sock" */
	char * path = NULL;
 
	/* 连接ubusd */
	if (UBUS_STATUS_OK != client_ubus_init(path))
	{
		printf("ubus connect failed!\n");
		return -1;
	}
 
	/* 调用某个ubus方法并处理返回结果 */
	client_ubus_call();
 
	client_ubus_done();
 
	return 0;
}
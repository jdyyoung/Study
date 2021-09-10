#include <unistd.h>
#include <libubox/blobmsg_json.h>
#include <libubox/uloop.h>
#include <libubus.h>
 
static struct ubus_context *ctx;
 
static void test_client_subscribe_cb(struct ubus_context *ctx, struct ubus_object *obj)
{
	fprintf(stderr, "Subscribers active: %d\n", obj->has_subscribers);
}
 
/* 这个空的method列表，只是为了让object有个名字，这样client可以通过object name来订阅。 */
static struct ubus_method test_methods[] = {};
 
static struct ubus_object_type test_obj_type = 
	UBUS_OBJECT_TYPE("test", test_methods);
 
static struct ubus_object test_object = {
	.name = "test", /* object的名字 */
	.type = &test_obj_type,
	.subscribe_cb = test_client_subscribe_cb,
};
 
static void notifier_main(void)
{
	int ret;
 
	/* 注册一个object，client可以订阅这个object */
	ret = ubus_add_object(ctx, &test_object);
	if (ret) {
		fprintf(stderr, "Failed to add object: %s\n", ubus_strerror(ret));
		return;
	}
 
	/* 在需要的时候，向所有客户端发送notify消息 */
	
	/* step1: 如果需要传递参数，则保存到struct blob_attr类型的结构体中。 */
 
	/* 
	int ubus_notify(struct ubus_context *ctx, struct ubus_object *obj, const char *type, struct blob_attr *msg, int timeout);
	type是一个字符串，自定义的。msg是需要携带的参数。如果需要等待回复，timeout需设置为>=0。
	*/
	while (1) {
		sleep(2);
		/* step2: 广播notification消息。 */
		ubus_notify(ctx,  &test_object, "say Hi!", NULL, -1);
	}
}
 
int main(int argc, char **argv)
{
	const char *ubus_socket = NULL;
 
	uloop_init();
 
	ctx = ubus_connect(ubus_socket);
	if (!ctx) {
		fprintf(stderr, "Failed to connect to ubus\n");
		return -1;
	}
 
	ubus_add_uloop(ctx);
 
	notifier_main();
	
	uloop_run();
 
	ubus_free(ctx);
	uloop_done();
 
	return 0;
}
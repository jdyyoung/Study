

list.h 经典应用：client 节点

注册客户端，把事件分发到每个客户端处理，注意不阻塞！！！

```C
//声明相关结构体
typedef struct {
    struct list_head list;
    int              refcnt;
    void             (*handler)(int button_event);
} CLIENT;

//定义必要变量
static MUTEX            client_mutex;
static struct list_head client_list;
static int              client_count;

//客户端注册函数
void* button_register_client(void(*handler)(int button_event))
{
    CLIENT* client = (CLIENT*) malloc(sizeof(CLIENT));

    if (client) {
        client->handler = handler;
        MUTEX_LOCK(&client_mutex);
        list_add_tail(&client->list, &client_list);
        client_count++;
        MUTEX_UNLOCK(&client_mutex);
    }
    return client;
}

//客户端注销函数
int button_unregister_client(void* this)
{
    CLIENT* client = (CLIENT*)this;

    if (client) {
        if (client_count == 0)
            return -1;
        MUTEX_LOCK(&client_mutex);
        list_del(&client->list);
        client_count--;
        free(client);
        MUTEX_UNLOCK(&client_mutex);
    }
    return 0;
}

//分发事件给客户端处理
static void route_to_clients(int button_event)
{
    MUTEX_LOCK(&client_mutex);
    if (client_count) {
        struct list_head* this;
        list_for_each(this, &client_list) {
            CLIENT* client = list_entry(this, CLIENT, list);
            if (client->handler) {
                client->handler(button_event);
            }
        }
    }
    MUTEX_UNLOCK(&client_mutex);
}

//初始化相关变量
MUTEX_INIT(&client_mutex, "button_client");
INIT_LIST_HEAD(&client_list);
client_count = 0;
```

别的文件调用注册：

```C
static void button_handler(int button_event)
{
    if (STA_ACTIVE()) {
        int rc;

        switch (button_event) {
        case BUTTON_PRESS_CALL:
        case BUTTON_PRESS_CONFIG:
        case BUTTON_PRESS_RESET:
        case BUTTON_PRESS_ERASE:
            rc = queue_put(sta_queue, STAQ_BUTTON_MSG + button_event, NULL, 0);
            if (rc)
                ERROR(EINVAL, "queue_put(sta_queue)=%d", rc);
            break;
        }
    }
}

button_client = button_register_client(button_handler); 
if (button_client == NULL) {
    queue_destroy(sta_queue);
    sta_queue = NULL;
    REBOOT(ENOMEM, "button_register_client(button_handler)");
    return -1;
}
```


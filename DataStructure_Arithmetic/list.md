

2022-01-18：

```C
//list 的客户端注册制应用
//声明结构类型
typedef struct {
    struct list_head list;
    int              refcnt;
    void             (*handler)(int button_event);
} CLIENT;

//声明相关变量
static MUTEX            client_mutex;
static struct list_head client_list;
static int              client_count;

//初始化相关变量
MUTEX_INIT(&client_mutex, "button_client");
INIT_LIST_HEAD(&client_list);
client_count = 0;

void* button_register_client(void(*handler)(int button_event))
{
    CLIENT* client = (CLIENT*) HEAP_ALLOC(sizeof(CLIENT));

    if (client) {
        client->handler = handler;
        MUTEX_LOCK(&client_mutex);
        list_add_tail(&client->list, &client_list);
        client_count++;
        MUTEX_UNLOCK(&client_mutex);
    }
    return client;
}

int button_unregister_client(void* this)
{
    CLIENT* client = (CLIENT*)this;

    if (client) {
        if (client_count == 0 || client->refcnt)
            return -1;
        MUTEX_LOCK(&client_mutex);
        list_del(&client->list);
        client_count--;
        heap_free(client);
        MUTEX_UNLOCK(&client_mutex);
    }
    return 0;
}

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
```



----

1.定义结构体：节点数据结构，是否首个成员要以struct list_head 定义？？？

```c
typedef struct {
    struct list_head  list;
    char              name[128];
} FILE_LIST;
```

2.实例化节点，申请动态内存并赋值 ,添加到链表

```c
struct list_head *hits; 
INIT_LIST_HEAD(hits);

FILE_LIST *file = (FILE_LIST *)HEAP_ALLOC(sizeof(FILE_LIST)); 
if (file) {
    local_strncpy(file->name, this.d_name, sizeof(file->name));
    list_add_tail(&file->list, hits);
}
```



删除链表

```C

static void delete_list(struct list_head* the_list)
{
    struct list_head* this;
    struct list_head* next;

    //list_for_each_safe 用于获取到节点并销毁，常带free()
    list_for_each_safe(this, next, the_list) {
        ap_list_t* ap = list_entry(this, ap_list_t, list);
        heap_free(ap);
    }
    INIT_LIST_HEAD(the_list);
}

static ap_list_t* find_ap(char* essid)
{
    struct list_head* this;
	//list_for_each 用于取出每个节点的内容处理
    list_for_each(this, &ap_list) {
        ap_list_t* ap = list_entry(this, ap_list_t, list);
        if (strcmp(ap->essid, essid) == 0)
            return ap;
    }
    
    return NULL;
}

```

双向链表：

游标：

容器:

条目，节点

注意：线程不安全，需要加锁控制
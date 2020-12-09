



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
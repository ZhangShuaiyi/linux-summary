#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>

#define print_data_struct(data) pr_info("data address:%p list:%p id:%d c:%c\n", \
                                        &(data), &(data).list, (data).id, (data).c)

struct foo_struct {
    int n;
    struct list_head datas;
};

struct data_struct {
    int id;
    char c;
    struct list_head list;
};

static struct foo_struct myfoo;

static int __init zlist_init(void) {
    struct data_struct data1, data2;
    struct data_struct *tmp_data;

    pr_info("zlist2 init\n");
    INIT_LIST_HEAD(&myfoo.datas);

    data1.id = 1;
    data1.c = 'a';
    data2.id = 2;
    data2.c = 'b';
    print_data_struct(data1);
    print_data_struct(data2);

    list_add_tail(&data1.list, &myfoo.datas);
    list_add_tail(&data2.list, &myfoo.datas);

    list_for_each_entry(tmp_data, &myfoo.datas, list) {
        pr_info("tmp_data:%p id:%d c:%c\n", tmp_data, tmp_data->id, tmp_data->c);
    }
    return 0;
}

static void __exit zlist_exit(void) {
    pr_info("zlist2 exit\n");
}

module_init(zlist_init);
module_exit(zlist_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi zhang_syi@163.com");

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>

struct foo_struct {
    int id;
    char c;
    struct list_head list;
};

static struct foo_struct myfoo;

static int __init ztest_init(void) {
    struct foo_struct foo1, foo2;
    struct list_head *tmp_list;
    struct foo_struct *tmp_foo;

    pr_info("ztest_init\n");
    myfoo.id = 0;
    myfoo.c = 'a';
    foo1.id = 1;
    foo1.c = 'b';
    foo2.id = 2;
    foo2.c = 'c';

    pr_info("myfoo:%p list:%p\n", &myfoo, &myfoo.list);
    pr_info("foo1:%p list:%p\n", &foo1, &foo1.list);
    pr_info("foo2:%p list:%p\n", &foo2, &foo2.list);
    INIT_LIST_HEAD(&myfoo.list);
    list_add_tail(&foo1.list, &myfoo.list);
    list_add_tail(&foo2.list, &myfoo.list);

    pr_info("list by list_for_each and list_entry:::\n");
    list_for_each(tmp_list, &myfoo.list) {
        pr_info("tmp_list:%p\n", tmp_list);
        tmp_foo = list_entry(tmp_list, struct foo_struct, list);
        pr_info("tmp_foo:%p id:%d c:%c\n", tmp_foo, tmp_foo->id, tmp_foo->c);
    }
    pr_info("list by list_for_each_entry:::\n");
    list_for_each_entry(tmp_foo, &myfoo.list, list) {
        pr_info("tmp_foo:%p id:%d c:%c\n", tmp_foo, tmp_foo->id, tmp_foo->c);
    }
    return 0;
}

static void __exit ztest_exit(void) {
    pr_info("ztest_exit\n");
}

module_init(ztest_init);
module_exit(ztest_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi zhang_syi@163.com");
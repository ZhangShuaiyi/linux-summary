#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static char *name = "world";
module_param(name, charp, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(name, "The name to display");

static int __init hello_init(void) {
    pr_debug("Hello (%s) enter!\n", name);
    return 0;
}

static void __exit hello_exit(void) {
    pr_debug("Hello (%s) exit!\n", name);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi");
MODULE_DESCRIPTION("A simple linux driver with module_param");
MODULE_VERSION("0.1");
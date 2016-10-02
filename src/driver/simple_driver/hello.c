#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init hello_init(void) {
    pr_debug("Hello World enter!\n");
    return 0;
}

static void __exit hello_exit(void) {
    pr_debug("Hello World exit!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi");
MODULE_DESCRIPTION("A simple linux driver");
MODULE_VERSION("0.1");
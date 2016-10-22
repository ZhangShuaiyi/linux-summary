#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>

static int __init ofd_init(void) {
    pr_debug("Our first driver registered\n");
    return 0;
}

static void __exit ofd_exit(void) {
    pr_debug("Our first driver unregistered\n");
}

module_init(ofd_init);
module_exit(ofd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi zhang_syi@163.com");
MODULE_DESCRIPTION("Our first driver");
MODULE_VERSION("0.1");
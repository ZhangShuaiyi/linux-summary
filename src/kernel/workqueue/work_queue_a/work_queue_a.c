#include <linux/module.h>
#include <linux/init.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

static struct work_struct work;

static void work_handler(struct work_struct *data) {
    int i = 0;
    for (i = 0; i < 3; i++) {
        mdelay(100);
        pr_info("This is a work_queue work_handler\n");
    }
}

static int __init workqueue_init(void) {
    pr_info("Demo work_queue init\n");
    INIT_WORK(&work, work_handler);
    schedule_work(&work);
    pr_info("Demo work_queue init end\n");
    return 0;
}

static void __exit workqueue_exit(void) {
    pr_info("Demo work_queue exit\n");
}

module_init(workqueue_init);
module_exit(workqueue_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi zhang_syi@163.com");
MODULE_DESCRIPTION("First work_queue demo");
MODULE_VERSION("0.1");

?/**
  * After 'insmod work_queue_b.ko', 'ps aux | grep workqueue' can find process 'workqueue demo'
  */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

static struct workqueue_struct *queue = NULL;
static struct work_struct work;

static void work_handler(struct work_struct *data) {
    int i = 0;
    for (i = 0; i < 3; i++) {
        mdelay(100);
        pr_info("This is a work_queue work_handler\n");
    }
}

static int __init workqueue_init(void) {
    pr_info("Demo work_queue_b init\n");
    queue = create_singlethread_workqueue("workqueue demo");
    if (!queue) {
        pr_err("Demo work_queue fail create_singlethread_workqueue\n");
        return -1;
    }
    INIT_WORK(&work, work_handler);
    queue_work(queue, &work);
    pr_info("Demo work_queue_b init end\n");
    return 0;
}

static void __exit workqueue_exit(void) {
    pr_info("Demo work_queue_b exit\n");
    destroy_workqueue(queue);
}

module_init(workqueue_init);
module_exit(workqueue_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi zhang_syi@163.com");
MODULE_DESCRIPTION("work_queue demo use workqueue_struct");
MODULE_VERSION("0.1");

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <asm/pgtable.h>
#include <asm/uaccess.h>
#include <linux/highmem.h>

#include "zpage.h"

#define DEV_NAME "zpage"

static struct page *walk_task_page(struct task_struct *task, unsigned long addr) {
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *ptep, pte;

    struct page *page = NULL;
    struct mm_struct *mm = task->mm;

    pgd = pgd_offset(mm, addr);
    if (pgd_none(*pgd) || pgd_bad(*pgd)) {
        goto out;
    }
    pr_info("valid pgd\n");

    pud = pud_offset(pgd, addr);
    if (pud_none(*pud) || pud_bad(*pud)) {
        goto out;
    }
    pr_info("valid pud\n");

    pmd = pmd_offset(pud, addr);
    if (pmd_none(*pmd) || pmd_bad(*pmd)) {
        goto out;
    }
    pr_info("valid pmd\n");

    ptep = pte_offset_map(pmd, addr);
    if (!ptep) {
        goto out;
    }

    pte = *ptep;
    page = pte_page(pte);
    if (page) {
        pr_info("page struct is @ %p\n", page);
    }
    pte_unmap(ptep);

out:
    return page;
}

static int my_open(struct inode *inodep, struct file *filep) {
    return 0;
}

static int my_close(struct inode *inodep, struct file *filep) {
    return 0;
}

static long my_ioctl(struct file *filep, unsigned int cmd, unsigned long arg) {
    int r;
    void __user *argp = (void __user *)arg;

    r = -EFAULT;
    switch (cmd) {
    case ZPAGE_GET_PID_ADDR: {
        struct zpage_region zpage;
        struct task_struct *task;
        struct page *page;
        void *p;
        unsigned long offset;
        if (copy_from_user(&zpage, argp, sizeof(struct zpage_region))) {
            pr_err("my_ioctl copy_from_user error\n");
            goto out;
        }
        r = 0;
        pr_info("my_ioctl pid:%u addr:0x%llx\n", zpage.pid, zpage.addr);
        task = pid_task(find_vpid(zpage.pid), PIDTYPE_PID);
        page = walk_task_page(task, zpage.addr);
        pr_info("page flags:0x%lx _count:%d\n", page->flags, atomic_read(&page->_count));
        p = kmap(page);
        offset = zpage.addr & (PAGE_SIZE - 1);
        pr_info("offset:%lu\n", offset);
        r = *(int *)(p+offset);
        pr_info("p:0x%x\n", r);
        /* change memory data */
        *(int *)(p+offset) = 0x12345678;
        kunmap(page);
    }
    }
out:
    return r;
}

static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .unlocked_ioctl = my_ioctl
};

static struct miscdevice test_miscdevice = {
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &my_fops,
};

static int __init my_init(void) {
    pr_info("zpage init\n");
    test_miscdevice.name = DEV_NAME;
    misc_register(&test_miscdevice);
    return 0;
}

static void __exit my_exit(void) {
    pr_info("zpage exit\n");
    misc_deregister(&test_miscdevice);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi zhang_syi@163.com");
MODULE_DESCRIPTION("Get page of userspace address");
MODULE_VERSION("0.1");

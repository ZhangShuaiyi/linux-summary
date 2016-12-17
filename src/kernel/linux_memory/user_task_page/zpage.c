#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <asm/pgtable.h>
#include <asm/uaccess.h>
#include <linux/highmem.h>
#include <linux/version.h>

#include "zpage.h"

#define DEV_NAME "zpage"

static void *mmap_data;

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

static int my_mmap(struct file *filep, struct vm_area_struct *vma) {
    unsigned long start = vma->vm_start;
    unsigned long page;
    size_t size = vma->vm_end - vma->vm_start;
    pr_info("vma start:0x%lx size:%lu\n", start, size);

    if (size>MMAP_LEN) {
        return -EINVAL;
    }
    page = virt_to_phys(mmap_data) >> PAGE_SHIFT;
    pr_info("In mmap virtual:%p, physical:0x%llx, pfn:%lu\n", mmap_data, virt_to_phys(mmap_data), page);

    if (remap_pfn_range(vma, start, page, MMAP_LEN, PAGE_SHARED)) {
        return -EAGAIN;
    }
    vma->vm_flags |= (VM_DONTEXPAND | VM_DONTDUMP);
    return 0;
}


static long my_ioctl(struct file *filep, unsigned int cmd, unsigned long arg) {
    int r;
    void __user *argp = (void __user *)arg;
    struct task_struct *task;

    r = -EFAULT;
    switch (cmd) {
    case ZPAGE_GET_MM_INFO: {
        struct mm_struct *mm = NULL;
        task = pid_task(find_vpid(arg), PIDTYPE_PID);
        if (!task) {
            goto out;
        }
        r = 0;
        mm = task->mm;
        r += snprintf(mmap_data + r, MMAP_LEN - r, "start_stack:0x%lx\n", mm->start_stack);
        r += snprintf(mmap_data + r, MMAP_LEN - r, "start_code:0x%lx end_code:0x%lx\n",
                      mm->start_code, mm->end_code);
        r += snprintf(mmap_data + r, MMAP_LEN - r, "start_data:0x%lx end_data:0x%lx\n",
                      mm->start_data, mm->end_data);
        r += snprintf(mmap_data + r, MMAP_LEN - r, "total_vm:%lu arg_start:0x%lx arg_end:0x%lx\n",
                      mm->total_vm, mm->arg_start, mm->arg_end);
        *(char *)(mmap_data + r) = '\0';
        break;
    }
    case ZPAGE_GET_VMA_INFO: {
        struct mm_struct *mm = NULL;
        struct vm_area_struct *vma;
        task = pid_task(find_vpid(arg), PIDTYPE_PID);
        if (!task) {
            goto out;
        }
        r = 0;
        mm = task->mm;
        r += snprintf(mmap_data + r, MMAP_LEN - r, "map_count:%d\n", mm->map_count);
        for (vma = mm->mmap; vma; vma = vma->vm_next) {
            r += snprintf(mmap_data + r, MMAP_LEN - r, "vma start:0x%lx end:0x%lx\n",
                          vma->vm_start, vma->vm_end);
        }
        *(char *)(mmap_data + r) = '\0';
        break;
    }
    case ZPAGE_GET_PID_ADDR: {
        struct zpage_region zpage;
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
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0)
        pr_info("page flags:0x%lx _count:%d\n", page->flags, atomic_read(&page->_mapcount));
#else
        pr_info("page flags:0x%lx _count:%d\n", page->flags, atomic_read(&page->_count));
#endif
        p = kmap(page);
        offset = zpage.addr & (PAGE_SIZE - 1);
        pr_info("offset:%lu\n", offset);
        r = *(int *)(p+offset);
        pr_info("p:0x%x\n", r);
        /* change memory data */
        *(int *)(p+offset) = 0x12345678;
        kunmap(page);
        break;
    }
    }
out:
    return r;
}

static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .unlocked_ioctl = my_ioctl,
    .mmap = my_mmap
};

static struct miscdevice test_miscdevice = {
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &my_fops,
};

static int __init my_init(void) {
    pr_info("zpage init\n");
    test_miscdevice.name = DEV_NAME;
    misc_register(&test_miscdevice);
    mmap_data = kzalloc(MMAP_LEN, GFP_KERNEL);
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

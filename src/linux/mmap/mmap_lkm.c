#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>

#define DEV_NAME "ztest"

struct {
    char *data;
    unsigned long size;
} my_dev;

static int my_open(struct inode *inodep, struct file *filep) {
    pr_debug("ztest open\n");
    return 0;
}

static int my_close(struct inode *inodep, struct file *filep) {
    pr_debug("ztest release\n");
    return 0;
}

static int my_mmap(struct file *filep, struct vm_area_struct *vma) {
    unsigned long start = vma->vm_start;
    unsigned long page;
    size_t size = vma->vm_end - vma->vm_start;
    pr_debug("vma start:0x%lx size:%lu\n", start, size);

    if (size>my_dev.size) {
        return -EINVAL;
    }
    page = virt_to_phys(my_dev.data) >> PAGE_SHIFT;
    pr_debug("In mmap virtual:%p, physical:0x%llx, pfn:%lu\n", my_dev.data, virt_to_phys(my_dev.data), page);

    if (remap_pfn_range(vma, start, page, my_dev.size, PAGE_SHARED)) {
        return -EAGAIN;
    }
    vma->vm_flags |= (VM_DONTEXPAND | VM_DONTDUMP);
    return 0;
}

static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .mmap = my_mmap,
};

static struct miscdevice test_miscdevice = {
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &my_fops,
};

static int __init my_init(void) {
    char *str = "Hello kernel memeory!";
    pr_debug("My test module register\n");
    my_dev.size = 4096;
    /* my_dev.data = kmalloc(my_dev.size, GFP_KERNEL); */
    my_dev.data = kzalloc(my_dev.size, GFP_KERNEL);
    strncpy(my_dev.data, str, strlen(str));
    test_miscdevice.name = DEV_NAME;
    misc_register(&test_miscdevice);
    return 0;
}

static void __exit my_exit(void) {
    pr_debug("My test module exit\n");
    misc_deregister(&test_miscdevice);
    kfree(my_dev.data);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZhangShuaiyi zhang_syi@163.com");
MODULE_DESCRIPTION("My module test mmap");
MODULE_VERSION("0.1");
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/desc.h>
#include <asm/pgtable.h>
#include <asm/desc.h>
#include<linux/init.h>

static char modname[] = "sys_reg";
struct gdtr_struct{
    short limit;
    unsigned long address __attribute__((packed));
};

static unsigned long cr0,cr3,cr4;
static struct gdtr_struct gdtr;
static struct proc_dir_entry *gdt_cr_entry =NULL;
static ssize_t my_get_info(struct file *filep, char *buf, size_t count, loff_t *offp)
{
    int	len = 0;
    struct mm_struct *mm;

    mm = current->active_mm;
    cr0 = read_cr0();
    cr3 = read_cr3();
    cr4 = __read_cr4_safe();
    //asm(" sgdt gdtr");
    asm("sgdt %0":"=m"(gdtr));


    len += sprintf( buf+len, "cr4=%lX  ", cr4 );
    len += sprintf( buf+len, "PSE=%lX  ", (cr4>>4)&1 );
    len += sprintf( buf+len, "PAE=%lX  ", (cr4>>5)&1 );
    len += sprintf( buf+len, "\n" );
    len += sprintf( buf+len, "cr3=%lX cr0=%lX\n",cr3,cr0);
    len += sprintf( buf+len, "pgd:0x%lX\n",(unsigned long)mm->pgd);
    len += sprintf( buf+len, "pgd pa:0x%llX\n", virt_to_phys(mm->pgd));
    len += sprintf( buf+len, "gdtr address:%lX, limit:%X\n", gdtr.address, gdtr.limit);
//    len += sprintf( buf+len, "cpu_gdt_table address:0x%08lX\n", cpu_gdt_table);

    return count;
}

static const struct file_operations sys_reg_fops = {
    .read = my_get_info,
};

static int __init test_init( void )
{
    printk( "<1>\nInstalling \'%s\' module\n", modname );
    gdt_cr_entry = proc_create(modname, 0, NULL, &sys_reg_fops);
    if(!gdt_cr_entry){
        printk(KERN_ERR "can't create /proc/sys_reg \n");
        return -EFAULT;
    }
    return	0;
}


static void __exit test_exit( void )
{
    proc_remove(gdt_cr_entry);
    printk( "<1>Removing \'%s\' module\n", modname );
}

module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");


/**
 * 在Intel手册卷三 5.9 Privileged Instructions中列出特权指令
 */
#include <stdio.h>

int main(int argc, char *argv[]) {
    unsigned long val;
    /* mov cr3属于特权指令，在用户空间执行时会出现general protection */
    asm volatile("mov %%cr3, %0\n\t" : "=r" (val));
    printf("cr3:0x%x\n", val);
    return 0;
}

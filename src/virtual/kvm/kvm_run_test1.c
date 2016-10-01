#include <stdio.h>
#include <stdlib.h>
#include <linux/kvm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#define err_exit(x) do{perror((x));return 1;}while(0)

int main(int argc, char *argv[]) {
    int kvm, vmfd, vcpufd, ret;
    /* 输出al+bl的值 */
    const uint8_t code[] = {
        0xba, 0xf8, 0x03, /* mov $0x3f8,%dx */
        0x00, 0xd8,  /* add %bl,%al */
        0x04, '0',   /* add $0x30,%al */
        0xee,        /* out %al,(%dx) */
        0xb0, '\n',  /* mov $0x5c,%al */
        0xee,        /* out %al,(%dx) */
        0xf4,        /* hlt */
    };
    uint8_t *mem;
    struct kvm_sregs sregs;
    size_t mmap_size;
    struct kvm_run *run;

    kvm = open("/dev/kvm", O_RDWR|O_CLOEXEC);
    if (kvm==-1) {
        err_exit("Open /dev/kvm failed!\n");
    }

    ret = ioctl(kvm, KVM_GET_API_VERSION, NULL);
    if (ret==-1) {
        err_exit("KVM_GET_API_VERSION");
    } else if(ret!=12) {
        err_exit("KVM_GET_API_VERSION not 12");
    }

    /* 检查是否存在KVM_CAP_USER_MEMORY extension */
    ret = ioctl(kvm, KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY);
    if (ret==-1) {
        err_exit("KVM_CHECK_EXTENSION");
    }
    if (!ret) {
        err_exit("Required extension KVM_CAP_USER_MEM not available");
    }

    /* 创建虚拟机 */
    vmfd = ioctl(kvm, KVM_CREATE_VM, (unsigned long)0);
    if (vmfd==-1) {
        err_exit("KVM_CREATE_VM");
    }

    /* 申请4096字节内存 */
    mem = mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if (!mem) {
        err_exit("allocation guest memory");
    }
    /* 将测试代码复制到这块内存 */
    memcpy(mem, code, sizeof(code));

    struct kvm_userspace_memory_region region= {
        .slot = 0,
        .guest_phys_addr= 0x1000,
        .memory_size = 0x1000,
        .userspace_addr = (uint64_t)mem,
    };
    /* 通知VM内存区域 */
    ret = ioctl(vmfd, KVM_SET_USER_MEMORY_REGION, &region);
    if (ret==-1) {
        err_exit("KVM_SET_USER_MEMORY_REGION");
    }

    /* 创建VCPU */
    vcpufd= ioctl(vmfd, KVM_CREATE_VCPU, (unsigned long)0);
    if (vcpufd==-1) {
        err_exit("KVM_CREATE_VCPU");
    }
    /* 获取可映射的内存的大小 */
    ret = ioctl(kvm, KVM_GET_VCPU_MMAP_SIZE, NULL);
    if (ret==-1) {
        err_exit("KVM_GET_VCPU_MMAP_SIZE");
    }

    mmap_size = ret;
    if (mmap_size<sizeof(*run)) {
        err_exit("KVM_GET_VCPU_MMAP_SIZE unexpectedly small");
    }

    /* 将kvm_run结构体映射到用户空间 */
    run = mmap(NULL, mmap_size, PROT_READ|PROT_WRITE, MAP_SHARED, vcpufd, 0);
    if (!run) {
        err_exit("mmap vcpu");
    }

    /* 设置cs寄存器 */
    ret = ioctl(vcpufd, KVM_GET_SREGS, &sregs);
    if (ret==-1) {
        err_exit("KVM_GET_SREGS");
    }
    sregs.cs.base = 0;
    sregs.cs.selector = 0;
    ret = ioctl(vcpufd, KVM_SET_SREGS, &sregs);
    if (ret==-1) {
        err_exit("KVM_SET_SREGS");
    }

    struct kvm_regs regs = {
        .rip = 0x1000,
        .rax = 2,
        .rbx = 2,
        .rflags = 0x2,
    };
    ret = ioctl(vcpufd, KVM_SET_REGS, &regs);
    if (ret==-1) {
        err_exit("KVM_SET_REGS");
    }

    while(1) {
        ret = ioctl(vcpufd, KVM_RUN, NULL);
        if (ret==-1) {
            err_exit("KVM_RUN");
        }
        switch (run->exit_reason) {
        case KVM_EXIT_HLT:
            puts("KVM_EXIT_HLT");
            return 0;
        case KVM_EXIT_IO:
            /* 因为IO退出虚机*/
            if (run->io.direction==KVM_EXIT_IO_OUT && run->io.size==1 && run->io.port==0x3f8 && run->io.count==1) {
                putchar(*(((char *)run) + run->io.data_offset));
            } else {
                fprintf(stderr, "unhandled KVM_EXIT_IO\n");
                fprintf(stdout, "KVM_EXIT_IO size:%d port:0x%x count:%d\n", run->io.size, run->io.port, run->io.count);
                return 1;
            }
            break;
        case KVM_EXIT_FAIL_ENTRY:
            fprintf(stderr, "KVM_EXIT_FAIL_ENTRY: hardware_entry_failure_reason=0x%llx\n",
                    (unsigned long long)run->fail_entry.hardware_entry_failure_reason);
            return 1;
        case KVM_EXIT_INTERNAL_ERROR:
            fprintf(stderr, "KVM_EXIT_INTERNAL_ERROR: suberror=0x%x\n", run->internal.suberror);
            return 1;
        default:
            fprintf(stderr, "exit_reason=0x%x\n", run->exit_reason);
            return 0;
        }
    }
}
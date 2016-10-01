## 1. 源码

### 1.1 下载内核源码
[kernel-3.10.0-327.el7.src.rpm](http://vault.centos.org/7.2.1511/os/Source/SPackages/kernel-3.10.0-327.el7.src.rpm)，kvm代码位置arch/x86/kvm下

### 1.2 单独编译kvm模块
参考[How to recompile just a single kernel module?](http://stackoverflow.com/questions/8744087/how-to-recompile-just-a-single-kernel-module)，编译完内核后，如果想单独编译KVM模块
```
$ make modules SUBDIRS=arch/x86/kvm
```
编译后安装
```
$ make modules_install SUBDIRS=arch/x86/kvm
```

## 2. 测试
参考
+ [KVM API 使用实例](http://smilejay.com/2013/03/use-kvm-api/)
+ [Using the KVM API](https://lwn.net/Articles/658511/)
+ [The Definitive KVM (Kernel-based Virtual Machine) API Documentation](https://www.kernel.org/doc/Documentation/virtual/kvm/api.txt)
```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>

#define KVM_FILE "/dev/kvm"

int main(int argc, char *argv[]) {
    int dev;
    int ret;
    dev = open(KVM_FILE, O_RDWR|O_CLOEXEC);
    if (dev == -1) {
        perror("Open /dev/kvm");
        return -1;
    }
    /* 查看KVM版本 */
    ret = ioctl(dev, KVM_GET_API_VERSION, 0);
    fprintf(stdout, "KVM api version:%d\n", ret);
    /* 查看推荐的最大vcpu数 */
    ret = ioctl(dev, KVM_CHECK_EXTENSION, KVM_CAP_NR_VCPUS);
    if (ret==-1) {
        perror("KVM KVM_CAP_NR_VCPUS");
    } else {
        fprintf(stdout, "KVM recommended max_vcpus:%d\n", ret);
    }
    /* 查看最大支持的最大vcpu数 */
    ret = ioctl(dev, KVM_CHECK_EXTENSION, KVM_CAP_MAX_VCPUS);
    if (ret==-1) {
        perror("KVM KVM_CAP_MAX_VCPUS");
    } else {
        fprintf(stdout, "KVM maximum possible value of max_vcpus:%d\n", ret);
    }
    close(dev);
    return 0;
}
```
运行结果:
```sh
zsy@zsy-ThinkPad-X220:~/Workspace/code/linux-summary/src/virtual/kvm$ gcc kvm_api_test.c 
zsy@zsy-ThinkPad-X220:~/Workspace/code/linux-summary/src/virtual/kvm$ ./a.out 
KVM api version:12
KVM recommended max_vcpus:160
KVM maximum possible value of max_vcpus:255
```
源码位置[kvm_api_test.c](https://github.com/ZhangShuaiyi/linux-summary/blob/master/src/virtual/kvm/kvm_api_test.c)


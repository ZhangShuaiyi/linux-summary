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
    return 0;
}

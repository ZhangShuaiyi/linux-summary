## 1. KVM创建启动虚机流程
```
graph TD
A[open /kvm/dev设备]-->B[使用KVM_GET_API_VERSION获取API版本]
B-->C[是否支持KVM_CAP_USER_MEM功能]
C-->D[KVM_CREATE_VM创建虚机vmfd]
D-->E[使用mmap获得一块页对齐初始值为0的内存mem]
E-->F[将要运行的机器码数据使用memcpy复制到mem中]
F-->G[使用KVM_SET_USER_MEMORY_REGION通知虚机vmfd内存地址mem]
G-->H[使用KVM_CREATE_VCPU为虚机vmfd创建虚拟cpu vcpufd]
H-->I[使用KVM_GET_VCPU_MMAP_SIZE获取KVM_RUN和用户空间交互的共享内存区域的大小mmap_size]
I-->J[使用mmap将vcpufd中0-mmap_size的地址映射到kvm_run结构体]
J-->K[使用KVM_GET_SREGS和KVM_SET_SREGS设置vcpufd的cs寄存器的base和selector为0]
K-->L[使用KVM_SET_REGS设置寄存器值]
L-->M[使用KVM_RUN启动虚拟cpu vcpufd]
M-->N[当退出时可是有run->exit_reason获得退出原因]
```
简单流程：
+ 创建VM和VCPU
+ 使用mmap映射内存，并初始化
+ 初始化VCPU寄存器
+ 启动VCPU，监控VCPU的退出

## 2. 代码说明

### 2.1 测试代码
为测试KVM虚拟机，编写测试代码计算al和bl寄存器的和
```asm
[BITS 16]
mov dx, 0x3f8
add al, bl
add al, '0'
out dx, al
mov al, '\n'
out dx, al
hlt
```
编译:
```
zsy$ nasm -f bin -o add.bin add.asm
```
反编译
```sh
zsy$ objdump -D -b binary -mi386 -Maddr16,data16 add.bin 

add.bin:     file format binary


Disassembly of section .data:

00000000 <.data>:
   0:	ba f8 03             	mov    $0x3f8,%dx
   3:	00 d8                	add    %bl,%al
   5:	04 30                	add    $0x30,%al
   7:	ee                   	out    %al,(%dx)
   8:	b0 5c                	mov    $0x5c,%al
   a:	ee                   	out    %al,(%dx)
   b:	f4                   	hlt  
```

### 2.2 创建虚拟机
+ 首先open /dev/kvm
```c
kvm = open("/dev/kvm", O_RDWR|O_CLOEXEC);
```
我们需要通过读写操作来设置这个虚拟机，设置close-on-exec。
+ 在使用KVM API之前要确认API版本，目前KVM API版本为12
```c
ret = ioctl(kvm, KVM_GET_API_VERSION, NULL);
if (ret==-1) {
    err_exit("KVM_GET_API_VERSION");
} else if(ret!=12) {
    err_exit("KVM_GET_API_VERSION not 12");
}
```
+ 检测完API版本后，使用KVM_CHECK_EXTENSION ioctl()检测我们需要的extensions是否存在，KVM_CAP_USER_MEMORY可以通过KVM_SET_USER_MEMORY_REGION ioctl()设置guest的内存。
```c
ret = ioctl(kvm, KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY);
if (ret==-1) {
    err_exit("KVM_CHECK_EXTENSION");
}
if (!ret) {
    err_exit("Required extension KVM_CAP_USER_MEM not available");
}
```
+ 创建虚拟机(VM)，VM包括模拟系统的一切东西：内存、单个或多个CPU，KVM系统提供文件描述符来控制虚拟机
```c
vmfd = ioctl(kvm, KVM_CREATE_VM, (unsigned long)0);
```
+ 虚拟机需要我们提供内存，对应虚拟机中的物理地址空间。对于我们这个简单的测试程序，我们申请一页内存来保存测试代码，直接使用mmap获得页对齐的、初始值为0的内存
```c
mem = mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
```
将我们的机器码复制到这块内存
```c
memcpy(mem, code, sizeof(code));
```
使用KVM_SET_USER_MEMORY_REGION通知KVM虚拟机有4096字节的内存
```c
struct kvm_userspace_memory_region region= {
    .slot = 0,
    .guest_phys_addr= 0x1000,
    .memory_size = 0x1000,
    .userspace_addr = (uint64_t)mem,
};
ret = ioctl(vmfd, KVM_SET_USER_MEMORY_REGION, &region);
```
slot字段表示KVM中内存空间的索引，当我们使用相同的slot调用KVM_SET_USER_MEMORY_REGION时会替换掉这个mapping，当使用新的slot调用KVM_SET_USER_MEMORY_REGION时会创建新的mapping。guest_phys_addr虚机中的基础物理地址，userspace_addr指向我们通过mmap申请的内存，注意这是一个64-bit的值，memory_size表示要映射的内存的大小: 一页，0x1000字节。
+ 现在我们的VM中有内存，内存中有要运行的代码，现在我们创建一个VCPU去运行这些代码，KVM提供给我们控制这个VCPU的文件描述符，0表示虚拟CPU的索引，索引的最大值可通过KVM_CAP_MAX_VCPUS获得。
```c
vcpufd= ioctl(vmfd, KVM_CREATE_VCPU, (unsigned long)0);
```
+ 每个虚拟CPU都关联一个kvm_run结构体，这个结构体用来在内核和用户空间传递CPU的信息。尤其是当硬件虚拟化停止时(vmexit)，kvm_run结构体包含停止原因。我们将这个结构体通过mmap映射到用户空间，首先我们通过KVM_GET_VCPU_MMAP_SIZE得知有多少内存需要映射
```c
mmap_size = ioctl(kvm, KVM_GET_VCPU_MMAP_SIZE, NULL);
```
一般mmap_size大于kvm_run结构体的大小，因为内核会使用这片区域去存其它的瞬态信息。使用mmap映射kvm_run结构体
```c
run = mmap(NULL, mmap_size, PROT_READ|PROT_WRITE, MAP_SHARED, vcpufd, 0);
```
+ VCPU同样包括寄存器，KVM将寄存器分为两类：标准寄存器和特殊寄存器，分别使用kvm_regs和kvm_sregs结构体表示。在运行我们的代码前要先初始化这个寄存器，对于特殊寄存器，我们只需设置cs寄存器，将cs的base和selector设为0
```c
ret = ioctl(vcpufd, KVM_GET_SREGS, &sregs);
sregs.cs.base = 0;
sregs.cs.selector = 0;
ret = ioctl(vcpufd, KVM_SET_SREGS, &sregs);
```
对于标准寄存器，我们大部分设为0，instruction pointer设为0x1000，al和bl设为2，flags寄存器设置为2。
```c
struct kvm_regs regs = {
    .rip = 0x1000,
    .rax = 2,
    .rbx = 2,
    .rflags = 0x2,
};
ret = ioctl(vcpufd, KVM_SET_REGS, &regs);
```
+ 通过KVM_RUN ioctl运行vcpufd中的指令
```c
while(1) {
    ret = ioctl(vcpufd, KVM_RUN, NULL);
    switch (run->exit_reason) {
        /*Handle exit*/
    }
}
```
当stop的时候我们可以通过run->exit_reason查看退出原因。为了让虚机显示运行结果，我们在I/O端口0x3f8上模拟了一个串口。run->io字段表明了方向、size、端口和数量，为了传递实际数据，内核中在kvm_run结构体后使用一个buffer，run->io.data_offset提供了偏移。
```c
case KVM_EXIT_IO:
    if (run->io.direction==KVM_EXIT_IO_OUT && run->io.size==1 && run->io.port==0x3f8 && run->io.count==1) {
        putchar(*(((char *)run) + run->io.data_offset));
    } else {
        fprintf(stderr, "unhandled KVM_EXIT_IO\n");
        fprintf(stdout, "KVM_EXIT_IO size:%d port:0x%x count:%d\n", run->io.size, run->io.port, run->io.count);
        return 1;
    }
    break;
```
+ 运行结果
```sh
zsy$ ./a.out 
4
KVM_EXIT_HLT
```
代码位置[kvm_run_test1.c](https://github.com/ZhangShuaiyi/linux-summary/blob/master/src/virtual/kvm/kvm_run_test1.c)

参考
+ [Using the KVM API](https://lwn.net/Articles/658511/)
+ [The Definitive KVM (Kernel-based Virtual Machine) API Documentation](https://www.kernel.org/doc/Documentation/virtual/kvm/api.txt)


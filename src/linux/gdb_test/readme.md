## 1. 编译
使用gcc编译
```sh
gcc -g -O2 bar.c
```
+ -g: 添加调试信息。
+ -O2: gcc优化。

## 2. gdb调试

### 2.1 不使用-O2优化
在不使用-O2选项优化时使用gdbtui调试时可看到类似
```
movl $0x0,-0xc(%rbp)
```
这样的汇编代码，这行代码应该对应c代码的
```
int a = 0;
```
使用gdb查看变量a的地址就是$rdp-0xc
```
(gdb) p &a
$1 = (int *) 0x7fffffffdb34
(gdb) p $rbp - 0xc
$2 = (void *) 0x7fffffffdb34
```

### 2.2 使用-O2优化
-O2优化后使用gdb查看对应汇编代码，没有变量赋值的操作。


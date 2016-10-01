## 1. 循环打印
+ 汇编中使用loop进行循环时cx寄存器值用来表示循环次数，每loop一次，cx中的值自减1，到0为至。
```
    ;; 编译: nasm -f bin -o loop.bin loop.asm
    [BITS 16]
    mov dx, 0x3f8
    mov cx, 10                  ; 循环10次，cx用于loop
    mov al, 'A'
OutLoop:
    out dx, al
    inc al                      ; 自增，输出ABCD...
    loop OutLoop
    mov al, 0xa                 ; 0xa是换行符'\n'
    out dx, al
    hlt
```
+ 使用nasm进行编译
```sh
zsy$ nasm -f bin -o hello.bin hello.asm
```
+ 修改kvm测试代码，添加读取文件功能
```c
FILE *fc;
uint8_t *code = NULL;
int code_len = 0;
if (argc!=2) {
    fprintf(stderr, "Parameter error, need machine code input!\n");
    return 1;
}
fc = fopen(argv[1], "rb");
if (!fc) {
    err_exit("fopen");
}
/* 获取文件内容长度 */
fseek(fc, 0L, SEEK_END);
code_len = ftell(fc);
fseek(fc, 0L, SEEK_SET);
code = (uint8_t *)malloc(code_len);
code_len = fread(code, 1, code_len, fc);
fclose(fc);
```
+ 测试
```
zsy$ ./a.out loop.bin 
ABCDEFGHIJ
KVM_EXIT_HLT
```

## 2. 输出字符串
```asm
    [BITS 16]
    mov dx, 0x3f8
    mov si, hellostring
    mov cx, stringlen
PrintLoop:
    mov al, [si]
    out dx, al
    inc si
    loop PrintLoop

    hlt
    ;; data
    hellostring db 'Hello World!', 0x0a
    stringlen equ $-hellostring
```
+ 编译
```sh
zsy$ nasm -f bin -o hello.bin hello.asm
```
+ 反编译
```sh
$ objdump -D -b binary -mi386 -Maddr16,data16 hello.bin 

hello.bin:     file format binary


Disassembly of section .data:

00000000 <.data>:
   0:	ba f8 03             	mov    $0x3f8,%dx
   3:	be 10 00             	mov    $0x10,%si
   6:	b9 0d 00             	mov    $0xd,%cx
   9:	8a 04                	mov    (%si),%al
   b:	ee                   	out    %al,(%dx)
   c:	46                   	inc    %si
   d:	e2 fa                	loop   0x9
   f:	f4                   	hlt    
  10:	48                   	dec    %ax
  11:	65 6c                	gs insb (%dx),%es:(%di)
  13:	6c                   	insb   (%dx),%es:(%di)
  14:	6f                   	outsw  %ds:(%si),(%dx)
  15:	20 57 6f             	and    %dl,0x6f(%bx)
  18:	72 6c                	jb     0x86
  1a:	64 21 0a             	and    %cx,%fs:(%bp,%si)
```
+ 如果还是设置.guest_phys_addr = 0x1000，.rip = 0x1000，则运行该程序出现异常
```sh
zsy$ ./a.out hello.bin 
exit_reason=0x6
```
+ 修改为.guest_phys_addr = 0x0，.rip = 0x0，程序才可以正常运行
```sh
zsy$ ./a.out hello.bin 
Hello World!
KVM_EXIT_HLT
```

代码位置
+ [kvm_run_test2.c](https://github.com/ZhangShuaiyi/linux-summary/blob/master/src/virtual/kvm/kvm_run_test2.c)
+ [add.asm](https://github.com/ZhangShuaiyi/linux-summary/blob/master/src/virtual/kvm/add.asm)
+ [loop.asm](https://github.com/ZhangShuaiyi/linux-summary/blob/master/src/virtual/kvm/loop.asm)
+ [hello.asm](https://github.com/ZhangShuaiyi/linux-summary/blob/master/src/virtual/kvm/hello.asm)


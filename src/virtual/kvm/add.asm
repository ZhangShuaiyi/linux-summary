    ;; 编译: nasm -f bin -o add.bin add.asm
    [BITS 16]
    mov dx, 0x3f8
    add al, bl
    add al, '0'
    out dx, al
    mov al, '\n'
    out dx, al
    hlt
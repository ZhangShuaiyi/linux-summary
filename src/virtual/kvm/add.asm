    ;; 编译: nasm -f bin -o add.bin add.asm
    [BITS 16]
    mov dx, 0x3f8
    add al, bl
    add al, '0'
    out dx, al
    mov al, 0x0a  ; '\n'的ascii为0x0a
    out dx, al
    hlt

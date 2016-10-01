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

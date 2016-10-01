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

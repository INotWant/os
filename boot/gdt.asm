;;;;;;;;;;;;;;;;;;;
; desc: GDT
; data: 2020.10.26
;;;;;;;;;;;;;;;;;;;


gdt_start:      ; GDT 以 8字节 null 开始
    dd 0x0      ; 4 byte
    dd 0x0      ; 4 byte

gdt_code:       ; 代码段，8字节
    dw 0xffff   ; 段长度，bits 0-15
    dw 0x0      ; 段基址，bits 0-15
    db 0x0      ; 段基址，bits 16-23
    db 10011010b; 标志（8bits）
    db 11001111b; 标志（4bits）+ 段长度（bits 16-19）--> 第一个标志 G=1 表段边界的单位是 4 KB
    db 0x0      ; 段基址，bits 24-31

gdt_data:       ; 数据段，8字节
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:


; GDT 描述符
gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; gdt 大小，总是小 1
    dd gdt_start                ; gdt 地址

; 一些常量
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

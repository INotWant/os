;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; desc: 实现“实模式”下把整数以十六进制形式打印
; data: 2020.10.14
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;; 功能：实模式下把整数以十六进制形式打印
;;; 参数：dx 存储整数
print_hex:
    pusha
    mov cx, 0

ph_loop:
    cmp cx, 4
    je ph_end
    
    mov ax, dx
    and ax, 0x000f
    add al, 0x30
    cmp al, 0x39    ; 若大于 9 ，则需要表示为 'A-F'
    jle ph_inner
    add al, 7       ; 'A' 的 ASCII 是 65 而 '0' 的是 58, 所以加 65-58=7
ph_inner:
    mov bx, HEX_OUT + 5
    sub bx, cx
    mov [bx], al
    ror dx, 4       ; 循环右移 0x1234 -> 0x4123 -> 0x3412 -> 0x2341 -> 0x1234

    add cx, 1
    jmp ph_loop

ph_end:
    mov bx, HEX_OUT
    call print

    popa
    ret

HEX_OUT:
    db '0x0000', 0

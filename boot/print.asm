;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; desc: 实现“实模式”下的打印字符
; data: 2020.10.14
;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;; 功能：实模式下打印字符串
;;; 参数：bx 存储待打印字符串的首地址
print:
    pusha       ; 把 ax cx dx bx sp bp si di 依次压入堆栈

print_start:
    mov al, [bx]
    cmp al, 0
    je print_done

    mov ah, 0x0e
    int 0x10    ; 借助 BIOS 中断打印字符，ah=0x0e 表示在 TTY 模式下写字符，al 指定字符

    add bx, 1
    jmp print_start

print_done:
    popa        ; 从堆栈中依次取出 di si bp sp bx dx cx ax
    ret


;;; 功能：实模式下回车换行
print_nl:
    pusha

    mov al, 0x0d    ; 回车
    mov ah, 0x0e
    int 0x10

    mov al, 0x0a    ; 换行
    int 0x10

    popa
    ret
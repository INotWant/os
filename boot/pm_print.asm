;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; desc: 32 位保护模式下的字符串打印
; note: cpu 与 io 有多种交互方式
;   1) MMIO -> 内存映射IO（✔️）
;   2) PMIO -> 端口映射IO
; data: 2020.11.10
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


[bits 32]
VIDEO_MEMORY equ 0xb8000
WHITE_OB_BLACK equ 0x0f     ; 字符颜色

;;; 功能：32 位保护模式下打印字符串
;;; 参数：ebx 存放待打印字符串的首地址
print_string_pm:
    pusha
    mov edx, VIDEO_MEMORY

print_string_pm_loop:
    mov al, [ebx]           ; 设置字符
    mov ah, WHITE_OB_BLACK  ; 设置字符颜色

    cmp al, 0               ; 判断是否已结束
    je print_string_pm_done

    mov [edx], ax           ; to related VIDEO_MEMORY
    add ebx, 1              ; next char
    add edx, 2              ; next video memory position

print_string_pm_done:
    popa
    ret

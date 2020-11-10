;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; desc: 指定内核入口（以 kernel_main 为入口）
; data: 2020.11.10
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _start

[bits 32]
_start:
    [extern kernel_main]    ; 引入外部定义
    call kernel_main
    jmp $

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; desc: 生成（填充）第 0 扇区 -> 代码（机器指令） + 数据
;   代码逻辑:
;      1) 把内核从磁盘中加载至内存；
;      2) 切换至 32 位保护模式。
; data: 2020.10.13
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;; 代码
[org 0x7c00] ; boot_loader 被映射至内存的 0x7c00 位置
KERNEL_OFFSET equ 0x7e00

    mov [BOOT_DRIVE], dl
    mov bp, 0x7b00      ; 初始栈底指针（指向内存）--> base pointer
    mov sp, bp          ; 初始栈顶指针          --> stack pointer

    ; mov bx, MSG_REAL_MODE
    ; call print
    ; call print_nl

    call load_kernel    ; 1）把内核从磁盘中加载至内存
    call switch_to_pm   ; 2）切换至 32 位保护模式
    jmp $               ; Never executed

%include "boot/print.asm"
%include "boot/print_hex.asm"
%include "boot/load_disk.asm"
%include "boot/gdt.asm"
%include "boot/pm_print.asm"
%include "boot/switch_pm.asm"

[bits 16]
load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print
    call print_nl

    mov bx, KERNEL_OFFSET   ; 内核被加载至内存的偏移，在 disk_load 中以 bx 为准 
    mov cl, 0x02            ; 指定从哪个扇区开始（0x01 是 boot 扇区）
    mov ch, 0x00            ; 指定柱面
    mov dl, [BOOT_DRIVE]    ; 指定从哪个（哪种）存储设备加载，此值来自 BIOS 的 dl
    mov dh, 111             ; 被加载至内存的扇区数目，在 disk_load 中以 dh 为准
    call disk_load
    ret

[bits 32]
BEGIN_PM:
    ; mov ebx, MSG_PROT_MODE
    ; call print_string_pm
    call KERNEL_OFFSET      ; 运行内核
    jmp $


;;; 数据
BOOT_DRIVE db 0
; MSG_REAL_MODE db "Started in 16-bit Real Mode", 0
; MSG_PROT_MODE db "Landed in 32-bit Protected Mode", 0
MSG_LOAD_KERNEL db "Loading kernel into memory...", 0

    times 510 - ($-$$) db 0 ; 填充 0
    dw 0xaa55               ; boot 要求以 0xaa55 结尾
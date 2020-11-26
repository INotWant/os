;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; desc: 实现“实模式”下从磁盘加载数据至内存
; data: 2020.10.26
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;; 功能：实模式下从磁盘加载数据至内存
;;; 参数：dh 指定被加载至内存的扇区数目，dl 指定从哪个（哪种）存储设备加载
;;;      ch 指定柱面，cl 指定从哪个扇区开始（0x01 是 boot 扇区），
disk_load:
    pusha
    push dx

    mov ah, 0x02    ; 指定 'read'
    mov al, dh      ; 指定扇区数目
    mov dh, 0x00    ; 指定磁头号

    int 0x13        ; BIOS 中断 --> https://stanislavs.org/helppc/int_13-2.html
    jc disk_error

    pop dx
    cmp al, dh      ; al 被设置为加载的扇区数
    jne sectors_error
    popa
    ret

disk_error:
    mov bx, DISK_ERROR
    call print
    call print_nl
    mov dh, ah      ; ah 存储了错误码 -> dx 作为 print_hex 参数
    call print_hex
    jmp disk_loop

sectors_error:
    mov bx, SECTORS_ERROR
    call print

disk_loop:
    jmp $

DISK_ERROR: db "Disk read error", 0
SECTORS_ERROR: db "Incorrect number of sectors read", 0
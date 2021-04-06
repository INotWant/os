;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; desc: 从硬盘中查找 内核 文件，若查找到则把它加载至内存后跳转至内核执行
; data: 2021.04.01
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; 一些常量
SECTOR_PER_TRACK equ 0x3F       ; 每磁道扇区数
TRACK_PER_CYLINDER equ 0xFF     ; 每柱面磁道数（磁头数）
SECTOR_PER_CYLINDER equ 0x3EC1  ; 每柱面含有的扇区数
FAT_OFFSET equ 0x8200           ; FAT表加载至内存的位置
DIR_OFFSET equ 0x8400           ; 目录项加载至内存的位置
KERNEL_OFFSET equ 0x8400        ; 内核被加载至内存的位置
STACK_OFFSET equ 0x7B00         ; 栈在内存中的偏移

;;; 指定数据在内存中的偏移
BOOT_DRIVE_OFFSET equ 0x7DF0
DBR_LBA_OFFSET equ BOOT_DRIVE_OFFSET + 0x01
FAT_LBA_OFFSET equ DBR_LBA_OFFSET + 0x02
DATA_LBA_OFFSET equ FAT_LBA_OFFSET + 0x02
SECTOR_PER_CLUSTER_OFFSET equ DATA_LBA_OFFSET + 0x02
BYTE_PER_CLUSTER_OFFSET equ SECTOR_PER_CLUSTER_OFFSET + 0x01

;;; 代码（MAIN）
[org 0x7E00]
    call find_boot_dir
    call find_kernel_file

    call load_kernel            ; 1）把内核从磁盘中加载至内存
    call switch_to_pm           ; 2）切换至 32 位保护模式
    jmp $                       ; Never executed

%include "../boot/print.asm"
%include "../boot/print_hex.asm"
%include "./load_disk.asm"

; desc: 从根目录项查找 boot 文件夹
; 返回：
;   存在，则返回对应簇号（ax）
;   不存在，输出错误信息，进入死等
find_boot_dir:
    mov ax, 0x02
    find_boot_dir_new_cluster:
        push ax
        mov bx, DIR_OFFSET
        call read_cluster_to_memory

        mov cx, [BYTE_PER_CLUSTER_OFFSET]
        add cx, DIR_OFFSET             ; cx 为 簇 加载至内存的最大边界
        mov ax, DIR_OFFSET

    find_boot_dir_loop:
        cmp ax, cx
        je find_boot_dir_end
        mov bx, ax
        cmp byte [bx], 0x00             ; [目录项第一个字节] 0x00
        je find_boot_dir_error
        cmp byte [bx], 0xE5             ; [目录项第一个字节] 0xE5 表示该目录项已被删除
        je find_boot_dir_next
        cmp byte [bx + 0x0B], 0x0F      ; [目录项第 0x0B 字节] 0x0F 表示长文件
        je find_boot_dir_next
        mov bl, [bx + 0x0B]
        and bl, 0x10
        cmp bl, 0x10                    ; [目录项第 0x0B 字节] 第3bit 为 1 表示此文件项代表文件夹
        jne find_boot_dir_next

        ; __START 比较文件名
        mov esi, BOOT_DIR
        mov edi, 0
        mov di, ax
        mov ecx, 5
        cld
        repe cmpsb
        cmp ecx, 0
        jne find_boot_dir_next
        ; __END__

        pop bx
        add ax, 0x1A
        mov bx, ax
        mov ax, [bx]
        ret
        find_boot_dir_next:
            add ax, 0x20
            jmp find_boot_dir_loop
    find_boot_dir_end:
        pop ax
        call next_cluster
        cmp ax, 0xFFFF
        jne find_boot_dir_new_cluster
    find_boot_dir_error:
        mov bx, NOT_FIND_BOOT
        call print
        call print_nl
        jmp $

; desc: 在 boot 目录下查找 kernel 文件
; 参数：
;   ax -> 起始簇号
; 返回：
;   存在，则返回对应簇号（ax）
;   不存在，输出错误信息，进入死等
find_kernel_file:
    find_kernel_file_new_cluster:
        push ax
        mov bx, DIR_OFFSET
        call read_cluster_to_memory

        mov cx, [BYTE_PER_CLUSTER_OFFSET]
        add cx, DIR_OFFSET             ; cx 为 簇 加载至内存的最大边界
        mov ax, DIR_OFFSET
    find_kernel_file_loop:
        cmp ax, cx
        je find_kernel_file_end
        mov bx, ax
        cmp byte [bx], 0x00             ; [目录项第一个字节] 0x00
        je find_kernel_file_error
        cmp byte [bx], 0xE5             ; [目录项第一个字节] 0xE5 表示该目录项已被删除
        je find_kernel_file_next
        cmp byte [bx + 0x0B], 0x0F      ; [目录项第 0x0B 字节] 0x0F 表示长文件
        je find_kernel_file_next
        mov bl, [bx + 0x0B]
        and bl, 0x20
        cmp bl, 0x20                    ; [目录项第 0x0B 字节] 第2bit 为 1 表示此文件项代表文件
        jne find_kernel_file_next

        ; __START 比较文件名
        mov esi, KERNEL_FILE
        mov edi, 0
        mov di, ax
        mov ecx, 7
        cld
        repe cmpsb
        cmp ecx, 0
        jne find_kernel_file_next
        ; __END__

        pop bx
        add ax, 0x1A
        mov bx, ax
        mov ax, [bx]
        ret
        find_kernel_file_next:
            add ax, 0x20
            jmp find_kernel_file_loop
    find_kernel_file_end:
        pop ax
        call next_cluster
        cmp ax, 0xFFFF
        jne find_kernel_file_new_cluster
    find_kernel_file_error:
        mov bx, NOT_FIND_KERNEL
        call print
        call print_nl
        jmp $

; desc: 加载 'kernel'
; 参数：kernel 文件对应的簇号（ax）
load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print
    call print_nl

    mov bx, KERNEL_OFFSET
    load_kernel_new_cluster:
        push bx
        push ax
        call read_cluster_to_memory
        pop ax
        call next_cluster
        pop bx
        cmp ax, 0xFFFF
        je load_kernel_end
        add bx, [BYTE_PER_CLUSTER_OFFSET]
        jmp load_kernel_new_cluster
    load_kernel_end:
        ret

; desc: 把 lba 转为 chs
; 参数：dx:ax 存 lba
; 返回：
;	CH = track/cylinder number  (0-1023 dec., see below)
;	CL = sector number
;	DH = head number
lba_2_chs:
    mov bx, SECTOR_PER_CYLINDER
    push dx
    push ax
    div bx                  ; ax 柱面
    mov ch, al
    shl ah, 0x06
    mov cl, ah              ; ch（柱面的低 8 位），cl（7-6bit，柱面的高 2 位）
    pop ax
    pop dx
    mov bx, SECTOR_PER_TRACK
    push dx
    push ax
    div bx
    add dl, 1               ; dl 扇区
    add cl, dl              ; cl（5-0bit，扇区）
    pop ax
    pop dx
    mov bx, SECTOR_PER_TRACK
    div bx
    mov bl, TRACK_PER_CYLINDER
    div bl                  ; ah 磁头
    mov dh, ah
    ret

; desc: 由簇号获取 LBA
; 参数：ax 存簇号
; 返回：
;   簇号对应起始扇区的 LBA（dx:ax）
cluster_lba:
    sub ax, 2
    mov bh, 0
    mov bl, [SECTOR_PER_CLUSTER_OFFSET]
    mul bx
    mov bx, [DATA_LBA_OFFSET]
    add ax, bx
    adc dx, 0x00
    ret

; desc: 获取指定簇号的下一簇号
; 参数：ax 存簇号
; 返回：
;   下一簇号（ax），若为 0xFFFF 则表明结束
next_cluster:
    push ax
    mov bl, 0x80
    div bl
    mov ah, 0x00
    add ax, [FAT_LBA_OFFSET]
    mov dx, 0x00
    call lba_2_chs
    mov ax, 0x00
    mov es, ax
    mov bx, FAT_OFFSET
    mov al, 0x01
    mov dl, [BOOT_DRIVE_OFFSET]
    call disk_load
    pop ax
    mov bl, 0x80
    div bl
    mov al, ah
    mov bl, 0x04
    mul bl
    add ax, FAT_OFFSET
    mov bx, ax
    mov ax, [bx]
    ret

; desc: 把指定 簇 加载至内存中的指定位置
; 参数：
;   ax -> 簇号
;   bx -> 指定加载至内存的位置
read_cluster_to_memory:
    push bx
    call cluster_lba
    call lba_2_chs
    mov ax, 0x00
    mov es, ax
    pop bx
    mov al, [SECTOR_PER_CLUSTER_OFFSET]
    mov dl, [BOOT_DRIVE_OFFSET]
    call disk_load
    ret

%include "../boot/switch_pm.asm"
%include "../boot/gdt.asm"

[bits 32]
BEGIN_PM:
    jmp KERNEL_OFFSET           ; 运行内核
    jmp $

;;; 数据
BOOT_DIR db "BOOT", 0x20
KERNEL_FILE db "KERNEL", 0x20

NOT_FIND_BOOT db "not find boot dir!", 0
NOT_FIND_KERNEL db "not find kernel file!", 0
MSG_LOAD_KERNEL db "Loading kernel into memory...", 0
MSG_KERNEL_START db "Start from kernel...", 0

    times 1024 - ($-$$) db 0

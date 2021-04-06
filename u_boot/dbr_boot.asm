;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; desc: DBR 中的启动代码
;   代码逻辑: 
;       1) 初始化一些 FAT32 元信息
;       2) 把 bootloader（存于保留区的第 3-4 扇区）加载至内存
;       3) 跳转到 bootloader
; data: 2021.04.01
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; 一些常量
BYTE_PER_SECTOR equ 0x200       ; 每扇区所含字节数
SECTOR_PER_TRACK equ 0x3F       ; 每磁道扇区数
TRACK_PER_CYLINDER equ 0xFF     ; 每柱面磁道数（磁头数）
SECTOR_PER_CYLINDER equ 0x3EC1  ; 每柱面含有的扇区数

MBR_DPT_OFFSET equ 0x07BE       ; 第一个分区表在内存的位置   
STACK_OFFSET equ 0x7B00         ; 栈在内存中的偏移
BOOTLOADER_OFFSET equ 0x7E00    ; bootloader 加载至内存的位置

;;; 指定数据在内存中的偏移
BOOT_DRIVE_OFFSET equ 0x7DF0
DBR_LBA_OFFSET equ BOOT_DRIVE_OFFSET + 0x01
FAT_LBA_OFFSET equ DBR_LBA_OFFSET + 0x02
DATA_LBA_OFFSET equ FAT_LBA_OFFSET + 0x02
SECTOR_PER_CLUSTER_OFFSET equ DATA_LBA_OFFSET + 0x02
BYTE_PER_CLUSTER_OFFSET equ SECTOR_PER_CLUSTER_OFFSET + 0x01

;;; 代码（MAIN）
[org 0x7C5A]
    mov [BOOT_DRIVE_OFFSET], dl
    mov bp, STACK_OFFSET        ; 初始栈底指针（指向内存）--> base pointer
    mov sp, bp                  ; 初始栈顶指针          --> stack pointer
    call init_dbr_lba
    call init_fat_and_data_lba
    call init_sector_and_byte_per_cluster
    call load_bootloader
    jmp BOOTLOADER_OFFSET
    jmp $                       ; Never executed

%include "../boot/print.asm"
%include "../boot/print_hex.asm"
%include "./load_disk.asm"

; desc: 计算 DBR 所在扇区（LBA）
init_dbr_lba:
    mov bx, MBR_DPT_OFFSET
    init_dbr_lba_loop:
        mov al, [bx]
        cmp al, 0x80
        jne init_dbr_lba_f
        init_dbr_lba_t:
            mov ax, [bx + 0x08]
            mov [DBR_LBA_OFFSET], ax
            ret
        init_dbr_lba_f:
            add bx, 0x10
            jmp init_dbr_lba_loop

; desc: 计算 FAT、DATA 所在扇区（LBA）
init_fat_and_data_lba:
    mov ax, [0x7C0E]
    add ax, [DBR_LBA_OFFSET]
    mov [FAT_LBA_OFFSET], ax
    mov bx, [0x7C24]        ; 注：此处假设 FAT 占扇区总数不超过 2^16
    add bx, bx
    add ax, bx
    mov [DATA_LBA_OFFSET], ax
    ret

init_sector_and_byte_per_cluster:
    mov al, [0x7C0D]
    mov [SECTOR_PER_CLUSTER_OFFSET], al
    mov ah, 0x00
    mov bx, BYTE_PER_SECTOR
    mul bx
    mov [BYTE_PER_CLUSTER_OFFSET], ax
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

; desc: 加载 BootLoader，于 FAT32 保留分区的第 3-4 扇区，至内存的 BOOTLOADER_OFFSET（0x7E00）
load_bootloader:
    mov ax, [DBR_LBA_OFFSET]
    add ax, 0x02
    mov dx, 0x00
    call lba_2_chs
    mov ax, 0x00
    mov es, ax
    mov bx, BOOTLOADER_OFFSET
    mov al, 0x02
    mov dl, [BOOT_DRIVE_OFFSET]
    call disk_load
    ret

    times 420 - ($-$$) db 0 ; 填充 0 (注：此处为 420 非 510 ，因为 DBR 中代码从 0x5A 开始)
    dw 0xaa55               ; boot 要求以 0xaa55 结尾
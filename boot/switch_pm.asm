;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; desc: 切换至 32 位保护模式的具体逻辑
; data: 2020.11.10
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


[bits 16]
switch_to_pm:
    cli                     ; 1. 关中断
    lgdt [gdt_descriptor]   ; 2. 把 GDT 描述符加载至 lgdt 寄存器
    mov eax, cr0            
    or eax, 0x1
    mov cr0, eax            ; 3. 把 cr0 寄存器中表示进入 32 位保护模式的位置 1
    jmp CODE_SEG:init_pm

[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax              ; 4. 更新各段寄存器

    mov ebp, 0x9f000
    mov esp, ebp            ; 5. 更新堆栈指针

    call BEGIN_PM           ; 6. 调用开始程序
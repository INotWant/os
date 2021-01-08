[bits 32]

global ELE_SIZE, INTEGER_T, FLOAT_T, STRING_T, PAIR_POINT_T
global pair_pool_init, cons, car, cdr, caar, cadr, cddr, cdar, caddr, cdddr, caadr, cdadr, set_car, set_cdr

extern EXP_OFFSET, ENV_OFFSET, PROC_OFFSET, ARGL_OFFSET, UENV_OFFSET, STACK_OFFSET

; for pair pool
NEW_PP equ 0x100000
OLD_PP equ 0x119000
NEW_PP_OFFSET equ 0x9f01c
OLD_PP_OFFSET equ 0x9f020
FREE_OFFSET equ 0x9f024     ; index, 16bits
SCAN_OFFSET equ 0x9f026     ; index, 16bits

; for pair
ELE_SIZE equ 5
PAIR_SIZE equ (ELE_SIZE * 2)
%ifndef GC_TEST
PAIR_MAX_NUM equ 10240
%else
global FREE_OFFSET
PAIR_MAX_NUM equ 9
%endif
CONS_ARGS_SAVE_INDEX equ 6

;; define the type of element
INTEGER_T equ 0x0
FLOAT_T equ 0x1
STRING_T equ 0x2
PAIR_POINT_T equ 0x3
NON_EXIST_T equ 0xe
BROKEN_HEART_T equ 0xf

;;; save 1st parameter of cons, only used by cons
;;; params: al -> type , ebx -> value
put_1:
    push ecx    ; save ecx
    mov ecx, [NEW_PP_OFFSET]
    add ecx, CONS_ARGS_SAVE_INDEX * PAIR_SIZE
    mov [ecx], al
    mov [ecx + 1], ebx
    add ecx, ELE_SIZE
    mov byte [ecx], PAIR_POINT_T
    push edx    ; save edx
    mov edx, ecx
    add edx, ELE_SIZE
    mov [ecx + 1], edx
    pop edx     ; restore edx
    pop ecx     ; restore ecx
    ret

;;; save 2nd parameter of cons, only used by cons
;;; params: cl -> type, edx -> value
put_2:
    push ecx    ; save ecx
    mov ecx, [NEW_PP_OFFSET]
    add ecx, CONS_ARGS_SAVE_INDEX * PAIR_SIZE + ELE_SIZE
    mov [ecx], cl
    mov [ecx + 1], edx
    add ecx, ELE_SIZE
    mov byte [ecx], PAIR_POINT_T
    mov dword [ecx + 1], 0
    pop ecx     ; restore ecx
    ret

;;; get type of the special parameter of cons
;;; params: al -> 1 to show 1st parameter , 2 to show 2nd parameter
;;; return: al
get_t:
    push ecx
    mov ecx, [NEW_PP_OFFSET]
    cmp al, 1
    jnz get_t_2
    get_t_1:
        add ecx, CONS_ARGS_SAVE_INDEX * PAIR_SIZE
        jmp get_t_e
    get_t_2:
        add ecx, CONS_ARGS_SAVE_INDEX * PAIR_SIZE + PAIR_SIZE
    get_t_e:
        mov al, [ecx]
        pop ecx
        ret

;;; get value of the special parameter of cons
;;; params: al -> 1 to show 1st parameter , 2 to show 2nd parameter
;;; return: eax
get_v:
    push ecx
    mov ecx, [NEW_PP_OFFSET]
    cmp al, 1
    jnz get_v_2
    get_v_1:
        add ecx, CONS_ARGS_SAVE_INDEX * PAIR_SIZE + 1
        jmp get_v_e
    get_v_2:
        add ecx, CONS_ARGS_SAVE_INDEX * PAIR_SIZE + PAIR_SIZE + 1
    get_v_e:
        mov eax, [ecx]
        pop ecx
        ret

;;; params: ax -> index, bx -> 0 or ELE_SIZE, ecx -> base address
;;; return: ecx
;;; broke: eax, ebx
compute_address:
    push edx
    mov dx, PAIR_SIZE
    mul dx
    shl edx, 16
    and eax, 0x0000ffff
    and ebx, 0x0000ffff
    add edx, eax
    add edx, ebx
    add ecx, edx
    pop edx
    ret

;;; params: al -> type, ebx -> value, ecx -> address
put_ele:
    mov [ecx], al
    mov [ecx + 1], ebx
    ret

;;; params: ecx -> src, edx -> dest
copy_pair:
    push ax
    push ebx
    ; car
    mov al, [ecx]
    mov [edx], al
    mov ebx, [ecx + 1]
    mov [edx + 1], ebx
    ; cdr
    mov al, [ecx + ELE_SIZE]
    mov [ecx + ELE_SIZE], al
    mov ebx, [ecx + ELE_SIZE + 1]
    mov [edx + ELE_SIZE + 1], ebx
    pop ebx
    pop ax
    ret

;;; params: ecx -> current scanned address
;;; broke: eax, ebx, edx
gc_helper:
    push ecx
    mov eax, ecx
    mov ecx, [eax + 1]
    ; is BROKEN_HEART?
    cmp byte [ecx], BROKEN_HEART_T
    jne gc_helper_2
    gc_helper_1:
        mov eax, [ecx + 1]
        pop ecx
        mov [ecx + 1], eax
        ret
    gc_helper_2:
        push ecx                ; src
        mov ax, [FREE_OFFSET]
        mov bx, 0
        mov ecx, [NEW_PP_OFFSET]
        call compute_address    ; dest
        mov edx, ecx
        pop ecx
        call copy_pair
        ; maintain BROKEN_HEART
        mov byte [ecx], BROKEN_HEART_T
        mov dword [ecx + 1], edx
        ; change old pair point
        pop ecx
        mov dword [ecx + 1], edx
        ; maintain FREE
        add word [FREE_OFFSET], 1
        ret

;;; GC: stop & copy
;;; broke: eax, ebx, ecx, edx
gc:
    ; 1. swap 'NEW_PP' & 'OLD_PP'
    mov eax, [NEW_PP_OFFSET]
    mov ebx, [OLD_PP_OFFSET]
    mov [OLD_PP_OFFSET], eax
    mov [NEW_PP_OFFSET], ebx
    ; 2. reset FREE_OFFSET & SCAN_OFFSET
    mov word [FREE_OFFSET], 0
    mov word [SCAN_OFFSET], 0
    ; 3. fill root table
    call fill_root_table
    ; 4. copy CONS_ARGS_SAVE
    ; 4.1 1st param
    mov ax, CONS_ARGS_SAVE_INDEX
    mov bx, 0
    mov ecx, [OLD_PP_OFFSET]
    call compute_address    ; src
    push ecx
    mov ax, [FREE_OFFSET]
    mov ecx, [NEW_PP_OFFSET]
    call compute_address    ; dest
    mov edx, ecx
    pop ecx
    call copy_pair
    ; 4.2 2nd param
    add ecx, PAIR_SIZE
    add edx, PAIR_SIZE
    call copy_pair
    add word [FREE_OFFSET], 2
    ; 5. scan
    gc_loop:
        ; if there is room in pool
        cmp word [FREE_OFFSET], PAIR_MAX_NUM
        je cons_no_space
        ; cmp scan & free
        mov ax, [SCAN_OFFSET]
        cmp [FREE_OFFSET], ax
        jbe gc_e        ; <=
        gc_loop_car:    ; car 
            mov bx, 0
            mov ecx, [NEW_PP_OFFSET]
            call compute_address
            cmp dword [ecx + 1], 0
            je gc_loop_cdr
            cmp byte [ecx], PAIR_POINT_T
            jne gc_loop_cdr
            push ax
            call gc_helper
            pop ax
        gc_loop_cdr:    ; cdr
            ; skip if in root table(contain 'CONS_ARGS_SAVE')
            cmp word [SCAN_OFFSET], CONS_ARGS_SAVE_INDEX
            jbe gc_loop_n
            mov bx, ELE_SIZE
            mov ecx, [NEW_PP_OFFSET]
            call compute_address
            cmp dword [ecx + 1], 0
            je gc_loop_n
            cmp byte [ecx], PAIR_POINT_T
            jne gc_loop_n
            call gc_helper
        gc_loop_n:
            add word [SCAN_OFFSET], 1
            jmp gc_loop
    gc_e:
        ; 6. update the special register
        ; exp
        mov ecx, [NEW_PP_OFFSET]
        call car
        mov [EXP_OFFSET], ebx
        ; env
        call cdr
        mov ecx, ebx
        call car
        mov [ENV_OFFSET], ebx
        ; proc 
        call cdr
        mov ecx, ebx
        call car
        mov [PROC_OFFSET], ebx
        ; argl
        call cdr
        mov ecx, ebx
        call car
        mov [ARGL_OFFSET], ebx
        ; uenv
        call cdr
        mov ecx, ebx
        call car
        mov [UENV_OFFSET], ebx
        ; stack
        call cdr
        mov ecx, ebx
        call car
        mov [STACK_OFFSET], ebx
        ret

;;; construct pair
;;; params: al -> the type of 1st element, cl -> the type of 2st element;
;;;         ebx -> the data of 1st element, edx -> the data of 2st element           
;;; return: eax -> address ; 0 show no space(at this time, the value of the register is unknown)
;;; broke: eax, ecx
cons:
    ; 1. enough space?
    cmp word [FREE_OFFSET], PAIR_MAX_NUM
    jb cons_no_gc
    cons_gc:
        ; [2] GC
        ; [2.1] save params, for GC -> only not enough
        call put_1
        call put_2
        ; [2.2] GC
        call gc
        ; [2.3] restore params
        mov al, 1
        call get_t
        push ax
        mov al, 1
        call get_v
        push eax
        mov al, 2
        call get_t
        push ax
        mov al, 2
        call get_v
        push eax
        pop edx
        pop cx
        pop ebx
        pop ax
    cons_no_gc:
        ; 3. save
        push edx
        push cx
        push ebx
        push ax
        ; 3.1 save car
        mov ax, [FREE_OFFSET]
        mov bx, 0
        mov ecx, [NEW_PP_OFFSET]
        call compute_address
        pop ax
        pop ebx
        call put_ele
        ; 3.2 save cdr
        add ecx, ELE_SIZE
        pop ax
        pop ebx
        call put_ele
        ; 4 return
        add word [FREE_OFFSET], 1
        sub ecx, ELE_SIZE
        mov eax, ecx
        ; 5 restore ebx edx
        mov ebx, [eax + 1]
        mov edx, [eax + ELE_SIZE + 1]
        ret
    cons_no_space:
        pop eax     ; clean 'eip' in 'stack'
        mov eax, 0
        ret

;;; params: ebx -> value, ecx -> address, dl -> is last
;;; broke: al, ebx, ecx = ecx + PAIR_SIZE
fill_root_table_ele:
    mov al, PAIR_POINT_T
    call put_ele
    cmp dl, 0
    je fill_root_table_ele_1
    fill_root_table_ele_0:
        mov ebx, 0
        jmp fill_root_table_ele_e
    fill_root_table_ele_1:
        mov ebx, ecx
        add ebx, PAIR_SIZE
    fill_root_table_ele_e:
        add ecx, ELE_SIZE
        call put_ele
        add ecx, ELE_SIZE
        ret

;;; fill root table
fill_root_table:
    pushad
    ; exp
    mov ebx, [EXP_OFFSET]
    mov ecx, [NEW_PP_OFFSET]
    mov dl, 0
    call fill_root_table_ele
    ; env
    mov ebx, [ENV_OFFSET]
    call fill_root_table_ele
    ; proc
    mov ebx, [PROC_OFFSET]
    call fill_root_table_ele
    ; argl
    mov ebx, [ARGL_OFFSET]
    call fill_root_table_ele
    ; uenv
    mov ebx, [UENV_OFFSET]
    call fill_root_table_ele
    ; stack
    mov ebx, [STACK_OFFSET]
    call fill_root_table_ele
    ; maintaining FREE
    mov word [FREE_OFFSET], CONS_ARGS_SAVE_INDEX
    popad
    ret

;;; init pair pool
pair_pool_init:
    ; setting NEW_PP_OFFSET OLD_PP_OFFSET
    mov dword [NEW_PP_OFFSET], NEW_PP
    mov dword [OLD_PP_OFFSET], OLD_PP
    ; fill root table
    call fill_root_table
    ; retain CONS_ARGS_SAVE
    add word [FREE_OFFSET], 2
    ret

;;; params: ecx -> address
;;; return: al -> type, ebx -> value
car:
    mov al, [ecx]
    mov ebx, [ecx + 1]
    ret

;;; params: ecx -> address
;;; return: al -> type, ebx -> value
cdr:
    mov al, [ecx + ELE_SIZE]
    mov ebx, [ecx + ELE_SIZE + 1]
    ret

;;; params: ecx -> address
;;; return: al -> type, ebx -> value
caar:
    push ecx
    call car
    mov ecx, ebx
    call car
    pop ecx
    ret

;;; params: ecx -> address
;;; return: al -> type, ebx -> value
cadr:
    push ecx
    call cdr
    mov ecx, ebx
    call car
    pop ecx
    ret

;;; params: ecx -> address
;;; return: al -> type, ebx -> value
cddr:
    push ecx
    call cdr
    mov ecx, ebx
    call cdr
    pop ecx
    ret

;;; params: ecx -> address
;;; return: al -> type, ebx -> value
cdar:
    push ecx
    call car
    mov ecx, ebx
    call cdr
    pop ecx
    ret

;;; params: ecx -> address
;;; return: al -> type, ebx -> value
caddr:
    push ecx
    call cddr
    mov ecx, ebx
    call car
    pop ecx
    ret

;;; params: ecx -> address
;;; return: al -> type, ebx -> value
caadr:
    push ecx
    call cadr
    mov ecx, ebx
    call car
    pop ecx
    ret

;;; params: ecx -> address
;;; return: al -> type, ebx -> value
cdadr:
    push ecx
    call cadr
    mov ecx, ebx
    call cdr
    pop ecx
    ret

;;; params: ecx -> address
;;; return: al -> type, ebx -> value
cdddr:
    push ecx
    call cddr
    mov ecx, ebx
    call cdr
    pop ecx
    ret

;;; params: al -> type, ebx -> value, ecx -> address of the pair
set_car:
    mov [ecx], al
    mov [ecx + 1], ebx
    ret

;;; params: al -> type, ebx -> value, ecx -> address of the pair
set_cdr:
    mov [ecx + ELE_SIZE], al
    mov [ecx + ELE_SIZE + 1], ebx
    ret

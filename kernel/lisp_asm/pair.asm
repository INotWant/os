[bits 32]

global ELE_SIZE, INTEGER_T, FLOAT_T, STRING_T, PAIR_POINT_T, FUNC_POINT_T
global pair_pool_init, cons, car, cdr, caar, cadr, cddr, cdar, caddr, cdddr, caadr, cdadr, cadddr, set_car, set_cdr, list

extern EXP_OFFSET, ENV_OFFSET, CONTINUE_OFFSET, ARGL_OFFSET, UENV_OFFSET, STACK_OFFSET
extern eval_error_handler

; for pair pool
NEW_PP equ 0x100000
OLD_PP equ 0x164000
NEW_PP_OFFSET equ 0x9f01c
OLD_PP_OFFSET equ 0x9f020
FREE_OFFSET equ 0x9f024     ; index, 16bits
SCAN_OFFSET equ 0x9f026     ; index, 16bits

; for pair
ELE_SIZE equ 5
PAIR_SIZE equ (ELE_SIZE * 2)
%ifndef GC_TEST
PAIR_MAX_NUM equ 40960
%else
global FREE_OFFSET
PAIR_MAX_NUM equ 9
%endif
CONS_ARGS_SAVE_INDEX equ 6
RESERVED_SPACE_FOR_LIST equ 8

;; define the type of element
INTEGER_T equ 0x0
FLOAT_T equ 0x1
STRING_T equ 0x2
PAIR_POINT_T equ 0x3
FUNC_POINT_T equ 0xd
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
    push ebx    ; save ecx
    mov ebx, [NEW_PP_OFFSET]
    add ebx, CONS_ARGS_SAVE_INDEX * PAIR_SIZE + PAIR_SIZE
    mov [ebx], cl
    mov [ebx + 1], edx
    add ebx, ELE_SIZE
    mov byte [ebx], PAIR_POINT_T
    mov dword [ebx + 1], 0
    pop ebx     ; restore ecx
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
compute_address:
    push eax
    push ebx
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
    pop ebx
    pop eax
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
    mov [edx + ELE_SIZE], al
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
        mov [ecx + 1], edx
        ; change old pair point
        pop ecx
        mov [ecx + 1], edx
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
        cmp word [FREE_OFFSET], PAIR_MAX_NUM - RESERVED_SPACE_FOR_LIST
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
            call gc_helper
        gc_loop_cdr:    ; cdr
            ; skip if in root table(contain 'CONS_ARGS_SAVE')
            mov ax, [SCAN_OFFSET]
            cmp word ax, CONS_ARGS_SAVE_INDEX + 2
            jb gc_loop_n
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
        mov [CONTINUE_OFFSET], ebx
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
;;; return: eax -> address
;;; broke: eax, ecx
cons:
    ; 1. enough space?
    cmp word [FREE_OFFSET], PAIR_MAX_NUM - RESERVED_SPACE_FOR_LIST
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
        mov ebx, NO_SPACE_CONS
        jmp eval_error_handler

NO_SPACE_CONS db "no space -- cons", 0

;;; construct list (the maximum length is 8)
;;; params: non, but in stack
;;; return: eax -> address
;;; broke: all
list:
    pop ecx                 ; save -> return address 
    mov [CONTINUE_OFFSET], ecx
    pop eax                 ; eax -> list length
    ; 0.1 compute remain number
    mov ecx, 0
    add cx, [FREE_OFFSET]
    add ecx, eax
    ; 0.2 if there is enough space
    cmp ecx, PAIR_MAX_NUM - RESERVED_SPACE_FOR_LIST
    ja list_try_gc
    ; 1
    list_can:
        push eax            ; eax -> list length (note only ah)
        mov edx, 0          ; edx -> pre point
        list_can_loop:
            pop eax
            cmp eax, 0
            je list_can_end
            sub eax, 1
            pop ebx
            pop cx
            push eax
            mov ax, cx
            mov cl, PAIR_POINT_T
            call cons
            mov edx, eax
            jmp list_can_loop
        list_can_end:
            mov eax, [CONTINUE_OFFSET]
            mov dword [CONTINUE_OFFSET], 0
            push eax        ; restore return address
            mov eax, edx
            ret
    ; 2 no space
    list_try_gc:
        ; put the parameters in the reserved space
        push eax
        add ax, PAIR_MAX_NUM - RESERVED_SPACE_FOR_LIST - 1
        mov bx, 0
        mov ecx, [NEW_PP_OFFSET]
        call compute_address    ; ecx -> the address where the last parameter is stored
        ; save last params
        pop eax
        sub eax, 1
        pop ebx
        pop dx
        push eax
        mov [ecx], dl
        mov [ecx + 1], ebx
        mov byte [ecx + ELE_SIZE], PAIR_POINT_T
        mov dword [ecx + ELE_SIZE + 1], 0
        sub ecx, PAIR_SIZE
        list_try_gc_loop:
            pop eax
            cmp eax, 0
            je list_try_gc_loop_end
            sub eax, 1
            pop ebx
            pop dx
            push eax
            mov [ecx], dl
            mov [ecx + 1], ebx
            mov byte [ecx + ELE_SIZE], PAIR_POINT_T
            mov ebx, ecx
            add ebx, PAIR_SIZE
            mov [ecx + ELE_SIZE + 1], ebx
            sub ecx, PAIR_SIZE
            jmp list_try_gc_loop
        list_try_gc_loop_end:
            ; save "first address" to CONS_ARGS_SAVE
            add ecx, PAIR_SIZE      ; ecx -> first address
            mov al, PAIR_POINT_T
            mov ebx, ecx
            call put_1
            ; try gc
            ; if there is enough space, it will return
            ; if not, it will exit in the process of GC
            call gc
            mov al, 1
            call get_v
            mov edx, eax
            jmp list_can_end

;;; params: al -> type, ebx -> value, ecx -> address, dl -> is last
;;; broke: al, ebx, ecx = ecx + PAIR_SIZE
fill_root_table_ele:
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
    mov al, PAIR_POINT_T
    mov ebx, [EXP_OFFSET]
    mov ecx, [NEW_PP_OFFSET]
    mov dl, 0
    call fill_root_table_ele
    ; env
    mov al, PAIR_POINT_T
    mov ebx, [ENV_OFFSET]
    call fill_root_table_ele
    ; continue
    mov al, INTEGER_T
    mov ebx, [CONTINUE_OFFSET]
    call fill_root_table_ele
    ; argl
    mov al, PAIR_POINT_T
    mov ebx, [ARGL_OFFSET]
    call fill_root_table_ele
    ; uenv
    mov al, PAIR_POINT_T
    mov ebx, [UENV_OFFSET]
    call fill_root_table_ele
    ; stack
    mov al, PAIR_POINT_T
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
    pushad
    mov al, PAIR_POINT_T
    mov ebx, 0
    call put_1
    mov cl, PAIR_POINT_T
    mov edx, 0
    call put_2
    popad
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

;;; params: ecx -> address
;;; return: al -> type, ebx -> value
cadddr:
    push ecx
    call cdddr
    mov ecx, ebx
    call car
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

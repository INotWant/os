[bits 32]

global env_init, lookup_var, extend_env, define_var, set_var

extern ENV_OFFSET, ARGL_OFFSET
extern cons, car, cdr, caar, cadr, set_car, set_cdr, ELE_SIZE, PAIR_POINT_T, STRING_T
extern save_p, restore_ecx
extern str_cmp

;;; init environment
env_init:
    mov dword [ENV_OFFSET], 0
    ret

;;; params: eax --> address of variable name
;;; return: ecx --> address of special element (type[0] + data[1~3]), 0 show non exist
lookup_var:
    push ebx
    push edx
    push eax                ; 0 => var name point
    ; ecx -> env point
    mov ecx, [ENV_OFFSET]
    lookup_var_loop_0:
        cmp ecx, 0
        je lookup_var_end
        push ecx            ; 1 => env point
        call car
        ; ecx -> framework point
        mov ecx, ebx
        lookup_var_loop_1:
            cmp ecx, 0
            je lookup_var_loop_0_next
            push ecx        ; 2 => framework point
            call caar
            ; edx -> address of str(key)
            mov edx, ebx
            ; ebx -> address of variable name
            mov ebx, [esp + 8]
            ; cmp key & variable name
            call str_cmp
            cmp al, 0
            jne lookup_var_loop_1_next
            lookup_var_loop_end:
                pop ecx         ; 2 <=
                call car
                mov ecx, ebx
                add ecx, ELE_SIZE
                pop eax         ; 1 <=
                jmp lookup_var_end
            lookup_var_loop_1_next:
                pop ecx         ; 2 <=
                call cdr
                mov ecx, ebx
                jmp lookup_var_loop_1
        lookup_var_loop_0_next:
            pop ecx             ; 1 <=
            call cdr
            mov ecx, ebx
            jmp lookup_var_loop_0
    lookup_var_end:
        pop eax                 ; 0 <=
        pop edx
        pop ebx
        ret

;;; params: [ARGL_OFFSET]
;;;     1st -> address of var name's table;
;;;     2nd -> address of value's table.
;;; return: ecx --> address of new environment
;;; broke: all
extend_env:
    mov ecx, 0      ; ecx -> frame point 
    call save_p
    extend_env_loop:
        ; 1
        ; 1.1 get value table point
        mov ecx, [ARGL_OFFSET]
        call cadr
        ; ebx -> value table point
        push ebx
        ; 1.2 update value table point of argl table
        mov ecx, ebx
        call cdr
        ; ebx -> next
        push ebx
        mov ecx, [ARGL_OFFSET]
        call cdr
        mov ecx, ebx
        mov al, PAIR_POINT_T
        pop ebx
        call set_car
        ; 1.3 get key point
        mov ecx, [ARGL_OFFSET]
        call car
        ; ebx -> key table point
        push ebx
        ; 1.4 update var name point of argl table 
        mov ecx, ebx
        call cdr
        ; ebx -> next
        mov ecx, [ARGL_OFFSET]
        mov al, PAIR_POINT_T
        call set_car
        ; 1.5
        pop ebx
        pop edx       
        ; 2
        cmp ebx, 0
        je extend_env_1
        extend_env_0:
            mov ecx, ebx
            call car        ; key
            push ax
            push ebx
            mov ecx, edx
            call car        ; value
            mov cl, al
            mov edx, ebx
            pop ebx
            pop ax
            call cons
            ; eax -> kv point
            mov ebx, eax
            mov al, PAIR_POINT_T
            call restore_ecx
            mov edx, ecx
            mov cl, PAIR_POINT_T
            call cons
            ; eax -> frame point
            mov ecx, eax
            call save_p
            jmp extend_env_loop
        extend_env_1:
            ; 3
            mov al, PAIR_POINT_T
            call restore_ecx
            mov ebx, ecx
            mov cl, PAIR_POINT_T
            mov edx, [ENV_OFFSET]
            call cons
            ; eax -> env point
            mov ecx, eax
            mov [ENV_OFFSET], ecx
            ret

;;; params: al --> type
;;;         ebx --> data
;;;         ecx --> address of variable name
;;; return: al --> 1 show success, 0 show failure(environment unavailable)
;;; broke: ebx
define_var:
    push ecx
    mov ecx, [ENV_OFFSET]
    cmp ecx, 0
    je define_var_fail
    define_var_succ:
        mov cl, al
        mov edx, ebx
        mov al, STRING_T
        ; ebx -> address of variable name
        mov ebx, [esp]
        ; eax -> kv point
        call cons
        push eax
        mov ecx, [ENV_OFFSET]
        ; ebx -> frame point
        call car
        mov edx, ebx
        ; ebx -> kv point
        pop ebx
        mov cl, PAIR_POINT_T
        mov al, PAIR_POINT_T
        ; eax -> frame point
        call cons
        mov ebx, eax
        mov al, PAIR_POINT_T
        mov ecx, [ENV_OFFSET]
        call set_car
        pop ecx
        mov al, 1
        ret
    define_var_fail:
        pop ecx
        mov al, 0
        ret

;;; params: al --> type
;;;         ebx --> data
;;;         ecx --> address of variable name
;;; return: al --> 1 show success, 0 show failure(variable does not exist)
set_var:
    push ecx    
    push ebx
    push ax
    mov eax, ecx
    call lookup_var
    cmp ecx, 0
    je set_var_fail
    set_var_succ:
        pop ax
        pop ebx
        mov [ecx], al
        mov [ecx + 1], ebx
        pop ecx
        mov al, 1
        ret
    set_var_fail:
        pop ax
        mov al, 0
        pop ebx
        pop ecx
        ret


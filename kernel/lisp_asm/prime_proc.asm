[bits 32]

global primitive_procedure_names, primitive_procedure_objects
global TRUE_SYMBOL, FALSE_SYMBOL, OK_SYMBOL

extern ARGL_OFFSET
extern eval_error_handler
extern add_impl_c, sub_impl_c, mul_impl_c, div_impl_c, rem_impl_c, lt_impl_c, gt_impl_c, eq_impl_c, read_impl_c, display_impl_c, newline_impl_c
extern cons, car, cdr, cadr, set_car, set_cdr, INTEGER_T, FLOAT_T, STRING_T, PAIR_POINT_T
extern is_string, is_quote, is_symbol, text_of_quoted
extern make_primitive_procedure, is_primitive_procedure, primitive_procedure_impl
extern str_cmp
extern restore_ebx

;;; params: al -> type
;;; return: al
is_number:
    cmp al, INTEGER_T
    je is_number_true
    cmp al, FLOAT_T
    je is_number_true
    is_number_false:
        mov al, 0
        ret
    is_number_true:
        mov al, 1
        ret

;;; determine whether there is the correct number of parameters in [ARGL_OFFSET]
;;; params: ax -> number of targets
;;; return: al
args_is_correct_number:
    push ebx
    push ecx
    push edx
    push ax
    mov dx, 0
    mov ecx, [ARGL_OFFSET]
    args_number_loop:
        cmp ecx, 0
        je args_number_loop_e
        add dx, 1
        call cdr
        mov ecx, ebx
        jmp args_number_loop
        args_number_loop_e:
            pop ax
            cmp ax, dx
            je args_number_ture
            args_number_false:
                mov al, 0
                jmp args_number_end
            args_number_ture:
                mov al, 1
    args_number_end:
        pop edx
        pop ecx
        pop ebx
        ret

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data
add_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_ADD
    je add_impl_error
    ; 0.2 -> the type of args
    mov ecx, [ARGL_OFFSET]
    call car
    call is_number
    cmp al, 0
    mov ebx, UT_ADD
    je add_impl_error
    mov ecx, [ARGL_OFFSET]
    call cadr
    call is_number
    cmp al, 0
    mov ebx, UT_ADD
    je add_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call cdr
    ; cdecl call
    push ebx
    mov ecx, [ARGL_OFFSET]
    push ecx
    call add_impl_c
    pop ecx
    mov al, [ecx]
    mov ebx, [ecx + 1]
    pop ecx
    pop edx
    pop ecx
    ret
    add_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler

WNP_ADD db "wrong number of parameters -- +", 0
UT_ADD db "unsupport type -- +", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data
sub_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_SUB
    je sub_impl_error
    ; 0.2 -> the type of args
    mov ecx, [ARGL_OFFSET]
    call car
    call is_number
    cmp al, 0
    mov ebx, UT_SUB
    je sub_impl_error
    mov ecx, [ARGL_OFFSET]
    call cadr
    call is_number
    cmp al, 0
    mov ebx, UT_SUB
    je sub_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call cdr
    ; cdecl call
    push ebx
    mov ecx, [ARGL_OFFSET]
    push ecx
    call sub_impl_c
    pop ecx
    mov al, [ecx]
    mov ebx, [ecx + 1]
    pop ecx
    pop edx
    pop ecx
    ret
    sub_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_SUB db "wrong number of parameters -- -", 0
UT_SUB db "unsupport type -- -", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data
mul_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_MUL
    je mul_impl_error
    ; 0.2 -> the type of args
    mov ecx, [ARGL_OFFSET]
    call car
    call is_number
    cmp al, 0
    mov ebx, UT_MUL
    je mul_impl_error
    mov ecx, [ARGL_OFFSET]
    call cadr
    call is_number
    cmp al, 0
    mov ebx, UT_MUL
    je mul_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call cdr
    ; cdecl call
    push ebx
    mov ecx, [ARGL_OFFSET]
    push ecx
    call mul_impl_c
    pop ecx
    mov al, [ecx]
    mov ebx, [ecx + 1]
    pop ecx
    pop edx
    pop ecx
    ret
    mul_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_MUL db "wrong number of parameters -- *", 0
UT_MUL db "unsupport type -- *", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data
div_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_DIV
    je div_impl_error
    ; 0.2 -> the type of args
    mov ecx, [ARGL_OFFSET]
    call car
    call is_number
    cmp al, 0
    mov ebx, UT_DIV
    je div_impl_error
    mov ecx, [ARGL_OFFSET]
    call cadr
    call is_number
    cmp al, 0
    mov ebx, UT_DIV
    je div_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call cdr
    ; cdecl call
    push ebx
    mov ecx, [ARGL_OFFSET]
    push ecx
    call div_impl_c
    pop ecx
    mov al, [ecx]
    mov ebx, [ecx + 1]
    pop ecx
    pop edx
    pop ecx
    ret
    div_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_DIV db "wrong number of parameters -- /", 0
UT_DIV db "unsupport type -- /", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data
rem_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_REM
    je rem_impl_error
    ; 0.2 -> the type of args
    mov ecx, [ARGL_OFFSET]
    call car
    cmp al, INTEGER_T
    mov ebx, UT_REM
    jne rem_impl_error
    mov ecx, [ARGL_OFFSET]
    call cadr
    cmp al, INTEGER_T
    mov ebx, UT_REM
    jne rem_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call cdr
    ; cdecl call
    push ebx
    mov ecx, [ARGL_OFFSET]
    push ecx
    call rem_impl_c
    pop ecx
    mov al, [ecx]
    mov ebx, [ecx + 1]
    pop ecx
    pop edx
    pop ecx
    ret
    rem_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_REM db "wrong number of parameters -- %", 0
UT_REM db "unsupport type -- %", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type(PAIR_POINT_T), ebx -> address
;;; broke: all
cons_impl:
    ; 0 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_CONS
    je cons_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call car
    push ax
    push ebx
    call cadr
    mov cl, al
    mov edx, ebx
    pop ebx
    pop ax
    call cons
    mov ebx, eax
    mov al, PAIR_POINT_T
    ret
    cons_impl_error:
        jmp eval_error_handler 

WNP_CONS db "wrong number of parameters -- cons", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data
car_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 1
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_CAR
    je car_impl_error
    ; 0.2 -> the type of args
    mov ecx, [ARGL_OFFSET]
    call car
    cmp al, PAIR_POINT_T
    mov ebx, UT_CAR
    jne car_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call car
    mov ecx, ebx
    call car
    pop edx
    pop ecx
    ret
    car_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_CAR db "wrong number of parameters -- car", 0
UT_CAR db "unsupport type -- car", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data
cdr_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 1
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_CDR
    je cdr_impl_error
    ; 0.2 -> the type of args
    mov ecx, [ARGL_OFFSET]
    call car
    cmp al, PAIR_POINT_T
    mov ebx, UT_CDR
    jne cdr_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call car
    mov ecx, ebx
    call cdr
    pop edx
    pop ecx
    ret
    cdr_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_CDR db "wrong number of parameters -- cdr", 0
UT_CDR db "unsupport type -- cdr", 0

;;; params: [ARGL_OFFSET]
;;;         1st -> pair point
;;;         2nd -> element
;;; return: al -> type, ebx -> data ("ok")
set_car_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_SET_CAR
    je set_car_impl_error
    ; 0.2 --> the type of args
    mov ecx, [ARGL_OFFSET]
    call car
    cmp al, PAIR_POINT_T
    mov ebx, UT_SET_CAR
    jne set_car_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call car
    push ebx    
    call cadr
    pop ecx
    call set_car
    mov al, STRING_T
    mov ebx, OK_SYMBOL
    pop edx
    pop ecx
    ret
    set_car_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_SET_CAR db "wrong number of parameters -- set-car!", 0
UT_SET_CAR db "unsupport type -- set-car!", 0
OK_SYMBOL db "ok", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data ("ok")
set_cdr_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_SET_CDR
    je set_cdr_impl_error
    ; 0.2 --> the type of args
    mov ecx, [ARGL_OFFSET]
    call car
    cmp al, PAIR_POINT_T
    mov ebx, UT_SET_CDR
    jne set_cdr_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call car
    push ebx    
    call cadr
    pop ecx
    call set_cdr
    mov al, STRING_T
    mov ebx, OK_SYMBOL
    pop edx
    pop ecx
    ret
    set_cdr_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_SET_CDR db "wrong number of parameters -- set-cdr!", 0
UT_SET_CDR db "unsupport type -- set-cdr!", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data ("true" or "false")
is_null_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 1
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_IS_NULL
    je is_null_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call car
    cmp al, PAIR_POINT_T
    jne is_null_impl_false
    cmp ebx, 0
    jne is_null_impl_false
    is_null_impl_true:
        mov al, STRING_T
        mov ebx, TRUE_SYMBOL
        jmp is_null_impl_end
    is_null_impl_false:
        mov al, STRING_T
        mov ebx, FALSE_SYMBOL
    is_null_impl_end:
        pop edx
        pop ecx
        ret
    is_null_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_IS_NULL db "wrong number of parameters -- null?", 0
TRUE_SYMBOL db "true", 0
FALSE_SYMBOL db "false", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type(PAIR_POINT_T), ebx -> data (0)
display_impl:
    push ecx
    push edx
    ; 0 -> the number of args
    mov ax, 1
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_DISPLAY
    je display_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    push ecx
    ; cdecl call
    call display_impl_c
    pop edx
    pop edx
    pop ecx  
    mov al, PAIR_POINT_T
    mov ebx, 0
    ret
    display_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler

WNP_DISPLAY db "wrong number of parameters -- display", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data ("true" or "false")
lt_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_LT
    je lt_impl_error
    ; 0.2 -> the type of args
    mov ecx, [ARGL_OFFSET]
    call car
    call is_number
    cmp al, 0
    mov ebx, UT_LT
    je lt_impl_error
    mov ecx, [ARGL_OFFSET]
    call cadr
    call is_number
    cmp al, 0
    mov ebx, UT_LT
    je lt_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call cdr
    ; cdecl call
    push ebx
    mov ecx, [ARGL_OFFSET]
    push ecx
    call lt_impl_c
    pop ecx
    pop ecx
    cmp al, 1
    jne lt_impl_false
    lt_impl_true:
        mov al, STRING_T
        mov ebx, TRUE_SYMBOL
        jmp lt_impl_end
    lt_impl_false:
        mov al, STRING_T
        mov ebx, FALSE_SYMBOL
    lt_impl_end:
        pop edx
        pop ecx
        ret
    lt_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_LT db "wrong number of parameters -- <", 0
UT_LT db "unsupport type -- <", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data ("true" or "false")
gt_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_GT
    je gt_impl_error
    ; 0.2 -> the type of args
    mov ecx, [ARGL_OFFSET]
    call car
    call is_number
    cmp al, 0
    mov ebx, UT_GT
    je gt_impl_error
    mov ecx, [ARGL_OFFSET]
    call cadr
    call is_number
    cmp al, 0
    mov ebx, UT_GT
    je gt_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call cdr
    ; cdecl call
    push ebx
    mov ecx, [ARGL_OFFSET]
    push ecx
    call gt_impl_c
    pop ecx
    pop ecx
    cmp al, 1
    jne gt_impl_false
    gt_impl_true:
        mov al, STRING_T
        mov ebx, TRUE_SYMBOL
        jmp gt_impl_end
    gt_impl_false:
        mov al, STRING_T
        mov ebx, FALSE_SYMBOL
    gt_impl_end:
        pop edx
        pop ecx
        ret
    gt_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_GT db "wrong number of parameters -- >", 0
UT_GT db "unsupport type -- >", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data ("true" or "false")
eq_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_EQ
    je eq_impl_error
    ; 0.2 -> the type of args
    ; 0.2.1 -> for pair point
    mov ecx, [ARGL_OFFSET]
    call car
    cmp al, PAIR_POINT_T
    jne eq_impl_1
    mov edx, ebx
    call cadr
    cmp al, PAIR_POINT_T
    jne eq_impl_1
    eq_impl_0:
        cmp ebx, edx
        je eq_impl_true
        jmp eq_impl_false
    eq_impl_1:
    ; 0.2.2
        mov ecx, [ARGL_OFFSET]
        call car
        call is_number
        cmp al, 0
        mov ebx, UT_EQ
        je eq_impl_error
        mov ecx, [ARGL_OFFSET]
        call cadr
        call is_number
        cmp al, 0
        mov ebx, UT_EQ
        je eq_impl_error
        ; 1
        mov ecx, [ARGL_OFFSET]
        call cdr
        ; cdecl call
        push ebx
        mov ecx, [ARGL_OFFSET]
        push ecx
        call eq_impl_c
        pop ecx
        pop ecx
        cmp al, 1
        jne eq_impl_false
    eq_impl_true:
        mov al, STRING_T
        mov ebx, TRUE_SYMBOL
        jmp eq_impl_end
    eq_impl_false:
        mov al, STRING_T
        mov ebx, FALSE_SYMBOL
    eq_impl_end:
        pop edx
        pop ecx
        ret
    eq_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_EQ db "wrong number of parameters -- =", 0
UT_EQ db "unsupport type -- =", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data ("true" or "false")
ne_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_NE
    je ne_impl_error
    ; 0.2 -> the type of args
    ; 0.2.1 -> for pair point
    mov ecx, [ARGL_OFFSET]
    call car
    cmp al, PAIR_POINT_T
    jne nq_impl_1
    mov edx, ebx
    call cadr
    cmp al, PAIR_POINT_T
    jne nq_impl_1
    nq_impl_0:
        cmp ebx, edx
        jne eq_impl_true
        jmp eq_impl_false
    nq_impl_1:
    ; 0.2.2
    mov ecx, [ARGL_OFFSET]
    call car
    call is_number
    cmp al, 0
    mov ebx, UT_NE
    je ne_impl_error
    mov ecx, [ARGL_OFFSET]
    call cadr
    call is_number
    cmp al, 0
    mov ebx, UT_NE
    je ne_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call cdr
    ; cdecl call
    push ebx
    mov ecx, [ARGL_OFFSET]
    push ecx
    call eq_impl_c
    pop ecx
    pop ecx
    cmp al, 0
    jne ne_impl_false
    ne_impl_true:
        mov al, STRING_T
        mov ebx, TRUE_SYMBOL
        jmp ne_impl_end
    ne_impl_false:
        mov al, STRING_T
        mov ebx, FALSE_SYMBOL
    ne_impl_end:
        pop edx
        pop ecx
        ret
    ne_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler

WNP_NE db "wrong number of parameters -- !=", 0
UT_NE db "unsupport type -- !=", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data ("true" or "false")
le_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_LE
    je le_impl_error
    ; 0.2 -> the type of args
    mov ecx, [ARGL_OFFSET]
    call car
    call is_number
    cmp al, 0
    mov ebx, UT_LE
    je le_impl_error
    mov ecx, [ARGL_OFFSET]
    call cadr
    call is_number
    cmp al, 0
    mov ebx, UT_LE
    je le_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call cdr
    ; cdecl call
    push ebx
    mov ecx, [ARGL_OFFSET]
    push ecx
    call gt_impl_c
    pop ecx
    pop ecx
    cmp al, 0
    jne le_impl_false
    le_impl_true:
        mov al, STRING_T
        mov ebx, TRUE_SYMBOL
        jmp le_impl_end
    le_impl_false:
        mov al, STRING_T
        mov ebx, FALSE_SYMBOL
    le_impl_end:
        pop edx
        pop ecx
        ret
    le_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_LE db "wrong number of parameters -- <=", 0
UT_LE db "unsupport type -- <=", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data ("true" or "false")
ge_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_GE
    je ge_impl_error
    ; 0.2 -> the type of args
    mov ecx, [ARGL_OFFSET]
    call car
    call is_number
    cmp al, 0
    mov ebx, UT_GE
    je ge_impl_error
    mov ecx, [ARGL_OFFSET]
    call cadr
    call is_number
    cmp al, 0
    mov ebx, UT_GE
    je ge_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call cdr
    ; cdecl call
    push ebx
    mov ecx, [ARGL_OFFSET]
    push ecx
    call lt_impl_c
    pop ecx
    pop ecx
    cmp al, 0
    jne ge_impl_false
    ge_impl_true:
        mov al, STRING_T
        mov ebx, TRUE_SYMBOL
        jmp ge_impl_end
    ge_impl_false:
        mov al, STRING_T
        mov ebx, FALSE_SYMBOL
    ge_impl_end:
        pop edx
        pop ecx
        ret
    ge_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_GE db "wrong number of parameters -- >=", 0
UT_GE db "unsupport type -- >=", 0

;;; params: non
;;; return: al -> type, ebx -> data
;;; broke: all
read_impl:
    ; 0 -> the number of args
    mov ax, 0
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_READ
    je eval_error_handler
    ; 1
    call read_impl_c
    mov al, PAIR_POINT_T
    call restore_ebx
    ret

WNP_READ db "wrong number of parameters -- read", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data ("true" or "false")
is_number_impl:
    push ecx
    push edx
    ; 0 -> the number of args
    mov ax, 1
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_IS_NUMBER
    je is_number_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call car
    call is_number
    cmp al, 1
    je is_number_impl_true
    is_number_impl_false:
        mov al, STRING_T
        mov ebx, FALSE_SYMBOL
        jmp is_number_impl_end
    is_number_impl_true:
        mov al, STRING_T
        mov ebx, TRUE_SYMBOL
    is_number_impl_end:
        pop edx
        pop ecx
        ret
    is_number_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_IS_NUMBER db "wrong number of parameters -- number?", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data ("true" or "false")
is_string_impl:
    push ecx
    push edx
    ; 0 -> the number of args
    mov ax, 1
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_IS_STRING
    je is_string_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call is_string
    cmp al, 1
    je is_string_impl_true
    is_string_impl_false:
        mov al, STRING_T
        mov ebx, FALSE_SYMBOL
        jmp is_string_impl_end
    is_string_impl_true:
        mov al, STRING_T
        mov ebx, TRUE_SYMBOL
    is_string_impl_end:
        pop edx 
        pop ecx
        ret
    is_string_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_IS_STRING db "wrong number of parameters -- string?", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data ("true" or "false")
is_symbol_impl:
    push ecx
    push edx
    ; 0 -> the number of args
    mov ax, 1
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_IS_SYMBOL
    je is_symbol_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call is_symbol
    cmp al, 1
    je is_symbol_impl_true
    is_symbol_impl_false:
        mov al, STRING_T
        mov ebx, FALSE_SYMBOL
        jmp is_symbol_impl_end
    is_symbol_impl_true:
        mov al, STRING_T
        mov ebx, TRUE_SYMBOL
    is_symbol_impl_end:
        pop edx 
        pop ecx
        ret
    is_symbol_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_IS_SYMBOL db "wrong number of parameters -- symbol?", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data ("true" or "false")
is_quoted_impl:
    push ecx
    push edx
    ; 0 -> the number of args
    mov ax, 1
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_IS_QUOTED
    je is_quoted_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call is_quote
    cmp al, 1
    je is_quoted_impl_true
    is_quoted_impl_false:
        mov al, STRING_T
        mov ebx, FALSE_SYMBOL
        jmp is_quoted_impl_end
    is_quoted_impl_true:
        mov al, STRING_T
        mov ebx, TRUE_SYMBOL
    is_quoted_impl_end:
        pop edx 
        pop ecx
        ret
    is_quoted_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_IS_QUOTED db "wrong number of parameters -- quoted?", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data ("true" or "false")
is_pair_impl:
    push ecx
    push edx
    ; 0 -> the number of args
    mov ax, 1
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_IS_PAIR
    je is_pair_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call car
    cmp al, PAIR_POINT_T
    je is_pair_impl_true
    is_pair_impl_false:
        mov al, STRING_T
        mov ebx, FALSE_SYMBOL
        jmp is_pair_impl_end
    is_pair_impl_true:
        mov al, STRING_T
        mov ebx, TRUE_SYMBOL
    is_pair_impl_end:
        pop edx
        pop ecx
        ret
    is_pair_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_IS_PAIR db "wrong number of parameters -- pair?", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data ("true" or "false")
is_eq_impl:
    push ecx
    push edx
    ; 0 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_IS_EQ
    je is_eq_impl_error
    ; 1
    mov ecx, [ARGL_OFFSET]
    call car
    mov dl, al
    call cadr
    cmp al, dl
    jne is_eq_impl_false
    cmp al, STRING_T
    je is_eq_impl_str
    is_eq_impl_other:
        call eq_impl
        pop edx
        pop ecx
        ret
    is_eq_impl_str:
        mov edx, ebx
        mov ecx, [ARGL_OFFSET]
        call car
        call str_cmp
        cmp al, 0
        je is_eq_impl_true
    is_eq_impl_false:
        mov al, STRING_T
        mov ebx, FALSE_SYMBOL
        jmp is_eq_impl_end
    is_eq_impl_true:
        mov al, STRING_T
        mov ebx, TRUE_SYMBOL
    is_eq_impl_end:
        pop edx
        pop ecx
        ret
    is_eq_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_IS_EQ db "wrong number of parameters -- eq?", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data
;;; broke: all
apply_impl:
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_APPLY
    je eval_error_handler
    ; 0.2 -> the type of args
    mov ecx, [ARGL_OFFSET]
    call car
    mov ecx, ebx            ; ecx -> procedure point ?
    cmp al, PAIR_POINT_T
    mov ebx, UT_APPLY
    jne eval_error_handler
    cmp ecx, 0
    je eval_error_handler
    mov ecx, [ARGL_OFFSET]
    call cadr
    mov ecx, ebx            ; ecx -> params point ?
    cmp al, PAIR_POINT_T
    mov ebx, UT_APPLY
    jne eval_error_handler
    mov ecx, [ARGL_OFFSET]
    call car
    mov ecx, ebx            ; ecx -> procedure point ?
    call is_primitive_procedure
    cmp al, 0
    mov ebx, UT_APPLY
    je eval_error_handler
    ; 1
    call primitive_procedure_impl
    push ebx                ; ebx -> function point
    mov ecx, [ARGL_OFFSET]
    call cadr
    mov [ARGL_OFFSET], ebx ; ebx -> params
    pop ebx
    call ebx
    ret

WNP_APPLY db "wrong number of parameters -- apply", 0
UT_APPLY db "unsupport type -- apply", 0

;;; params: [ARGL_OFFSET]
;;; return: al -> type, ebx -> data
quoted_text_impl:
    push ecx
    push edx
    ; 0.1 -> the number of args
    mov ax, 2
    call args_is_correct_number
    cmp al, 0
    mov ebx, WNP_QUOTED_TEXT
    je quoted_text_impl_error
    ; 0.2 -> the type of args
    mov ecx, [ARGL_OFFSET]
    call is_quote
    cmp al, 1
    mov ebx, UT_QUOTED_TEXT
    jne quoted_text_impl_error
    ; 1
    call text_of_quoted
    pop edx
    pop ecx
    ret
    quoted_text_impl_error:
        pop edx
        pop ecx
        jmp eval_error_handler 

WNP_QUOTED_TEXT db "wrong number of parameters -- quotation-text", 0
UT_QUOTED_TEXT db "unsupport type -- quotation-text", 0

;;; params: non
;;; return: al -> type(PAIR_POINT_T), ebx -> data (0)
newline_impl:
    call newline_impl_c
    mov al, PAIR_POINT_T
    mov ebx, 0
    ret

;;; note: assume that there is no GC
;;; params: non
;;; return: eax
primitive_procedure_names:
    push ebx
    push ecx
    push edx

    mov al, STRING_T
    mov ebx, NEWLINE_SYMBOL
    mov cl, PAIR_POINT_T
    mov edx, 0
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, APPLY_SYMBOL

    call cons
    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, DISPLAY_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, READ_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, QUOTED_TEXT_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, IS_NULL_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, IS_EQ_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, IS_PAIR_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, IS_QUOTED_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, IS_SYMBOL_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, IS_STRING_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, IS_NUMBER_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, GE_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, LE_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, NE_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, EQ_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, GT_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, LT_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, SET_CDR_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, SET_CAR_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, CDR_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, CAR_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, CONS_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, REM_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, DIV_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, MUL_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, SUB_SYMBOL
    call cons

    mov cl, PAIR_POINT_T
    mov edx, eax
    mov al, STRING_T
    mov ebx, ADD_SYMBOL
    call cons

    pop edx
    pop ecx
    pop ebx
    ret
    
ADD_SYMBOL db "+", 0
SUB_SYMBOL db "-", 0
MUL_SYMBOL db "*", 0
DIV_SYMBOL db "/", 0
REM_SYMBOL db "%", 0
CONS_SYMBOL db "cons", 0
CAR_SYMBOL db "car", 0
CDR_SYMBOL db "cdr", 0
SET_CAR_SYMBOL db "set-car!", 0
SET_CDR_SYMBOL db "set-cdr!", 0
LT_SYMBOL db "<", 0
GT_SYMBOL db ">", 0
EQ_SYMBOL db "=", 0
NE_SYMBOL db "!=", 0
LE_SYMBOL db "<=", 0
GE_SYMBOL db ">=", 0
IS_NUMBER_SYMBOL db "number?", 0
IS_STRING_SYMBOL db "string?", 0
IS_SYMBOL_SYMBOL db "symbol?", 0
IS_QUOTED_SYMBOL db "quoted?", 0
IS_PAIR_SYMBOL db "pair?", 0
IS_EQ_SYMBOL db "eq?", 0
IS_NULL_SYMBOL db "null?", 0
QUOTED_TEXT_SYMBOL db "quoted-text", 0
READ_SYMBOL db "read", 0
DISPLAY_SYMBOL db "display", 0
APPLY_SYMBOL db "apply", 0
NEWLINE_SYMBOL db "newline", 0

;;; note: assume that there is no GC
;;; params: non
;;; return: eax
primitive_procedure_objects:
    push ebx
    push ecx
    push edx

    mov ecx, newline_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    mov edx, 0
    call cons

    push eax
    mov ecx, apply_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, display_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, read_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, quoted_text_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, is_null_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, is_eq_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, is_pair_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, is_quoted_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, is_symbol_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, is_string_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, is_number_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, ge_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, le_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, ne_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, eq_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, gt_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, lt_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, set_cdr_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, set_car_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, cdr_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, car_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, cons_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, rem_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, div_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, mul_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, sub_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    push eax
    mov ecx, add_impl
    call make_primitive_procedure
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    pop edx
    call cons

    pop edx
    pop ecx
    pop ebx
    ret
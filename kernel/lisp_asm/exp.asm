[bits 32]
global is_self_eval, is_variable
global is_quoted, text_of_quoted
global is_assignment, assignment_variable, assignment_value
global is_definition, definition_variable, definition_value
global is_lambda, lambda_params, lambda_body, make_lambda
global is_if, if_predicate, if_consequent, if_alternative, make_if
global is_begin, begin_actions, is_last_exp, first_exp, rest_exps, make_begin
global is_application, operator, operands, is_no_operands, first_operand, rest_operands

extern ARGL_OFFSET
extern str_cmp
extern save_p, restore_ebx, restore_ecx, STACK_OFFSET
extern cons, car, cdr, caar, cadr, cddr, cdadr, caadr, caddr, cdddr, set_car, set_cdr, INTEGER_T, FLOAT_T, STRING_T, PAIR_POINT_T

;;; params: ecx -> address of element
;;; return: al -> 1 show element is a number; 0 show it's not a number
is_number:
    cmp byte [ecx], INTEGER_T
    je is_number_true
    cmp byte [ecx], FLOAT_T
    je is_number_true
    is_number_false:
        mov al, 0
        ret
    is_number_true:
        mov al, 1
        ret

;;; params: ecx -> address of element
;;; return: al -> 1 show it's a string; 0 show it's not a string 
is_string:
    cmp byte [ecx], STRING_T
    jne is_string_false
    push ecx
    mov ecx, [ecx + 1]
    cmp byte [ecx], 0x22    ; "
    pop ecx 
    jne is_string_false
    is_string_true:
        mov al, 1
        ret
    is_string_false:
        mov al, 0
        ret

;;; params: ecx -> address of element
;;; return: al
is_quote:
    cmp byte [ecx], STRING_T
    jne is_quote_false
    push ecx
    mov ecx, [ecx + 1]
    cmp byte [ecx], 0x27    ; '
    pop ecx 
    jne is_quote_false
    is_quote_true:
        mov al, 1
        ret
    is_quote_false:
        mov al, 0
        ret

;;; params: ecx -> address of element
;;; return: al
is_symbol:
    call is_string
    cmp al, 1
    je is_symbol_false
    call is_quote
    cmp al, 1
    je is_symbol_false
    is_symbol_true:
        mov al, 1
        ret
    is_symbol_false:
        mov al, 0
        ret

;;; params: ecx -> address of exp
;;; return: al
is_self_eval:
    push ebx
    push ecx
    call car
    mov ecx, ebx
    call is_number
    cmp al, 1
    je is_self_eval_true
    call is_string
    cmp al, 1
    je is_self_eval_true
    is_self_eval_false:
        pop ecx
        pop ebx
        ret
    is_self_eval_true:
        pop ecx
        pop ebx
        ret

;;; params: ecx -> address of exp
;;; return: al
is_variable:
    push ebx
    push ecx
    call car
    mov ecx, ebx
    call is_symbol
    pop ecx
    pop ebx
    ret

;;; params: ecx -> address of exp
;;; return: al
is_quoted:
    push ebx
    push ecx
    call car
    mov ecx, ebx
    call is_quote
    pop ecx
    pop ebx
    ret

;;; param: eax -> address of exp
;;; return: al -> type, ebx -> data
text_of_quoted:
    push ecx
    push edx
    call car
    ; ebx -> str point
    mov edx, EMPTY_TABLE_SYMBOL
    call str_cmp
    cmp al, 0
    je text_of_quoted_et
    text_of_quoted_:
        mov al, STRING_T
        add ebx, 1
        jmp text_of_quoted_e
    text_of_quoted_et:
        mov al, PAIR_POINT_T
        mov ebx, 0
    text_of_quoted_e:
        pop edx
        pop ecx
        ret

EMPTY_TABLE_SYMBOL db "'()", 0

;;; params: ebx -> address of exp, edx -> address of str
;;; return: al
tagged:
    push ebx
    push ecx
    mov ecx, ebx
    call car
    cmp al, PAIR_POINT_T
    jne tagged_false
    mov ecx, ebx
    call car
    cmp al, STRING_T
    jne tagged_false
    call str_cmp
    jmp tagged_end
    tagged_false:
        mov al, 0
    tagged_end:
        pop ecx
        pop ebx
        ret

;;; params: ecx -> address of exp
;;; return: al
is_assignment:
    push ebx
    push edx
    mov ebx, ecx
    mov edx, SET_SYMBOL
    call tagged
    pop edx
    pop ebx
    ret

SET_SYMBOL db "set!", 0

;;; params: ecx -> address of exp
;;; return: al -> type, ebx -> data
assignment_variable:
    call caadr
    ret

;;; params: ecx -> address of exp
;;; return: ebx -> address of exp(special value)
assignment_value:
    push ax
    call caddr
    pop ax
    ret

;;; params: ecx -> address of exp
;;; return: al
is_definition:
    push ebx
    push edx
    mov ebx, ecx
    mov edx, DEFINE_SYMBOL
    call tagged
    pop edx
    pop ebx
    ret

DEFINE_SYMBOL db "define", 0

;;; params: ecx -> address of exp
;;; return: al, ebx
definition_variable:
    call caadr
    cmp al, STRING_T
    jne definition_variable_1
        ret
    definition_variable_1:
        push ecx
        mov ecx, ebx
        call car
        pop ecx
        ret

;;; params: ecx -> address of exp
;;; return: ebx -> address of exp
;;; broke: all
definition_value:
    call caadr
    cmp al, STRING_T
    je definition_value_1
    definition_value_0:
        call cddr
        ; edx -> body
        mov edx, ebx
        ; ebx -> params
        call cdadr
        call make_lambda
        ret
    definition_value_1:
        call caddr
        ret

;;; params: ecx -> address of exp
;;; return: al
is_lambda:
    push ebx
    push edx
    mov ebx, ecx
    mov edx, LAMBDA_SYMBOL
    call tagged
    pop edx
    pop ebx
    ret

;;; params: ecx -> address
;;; return: ebx
;;; broke: all
lambda_params_helper:
    cmp ecx, 0
    je lambda_params_helper_1
    lambda_params_helper_0:
        call save_p
        call caar
        mov cl, PAIR_POINT_T
        mov edx, 0
        call cons
        ; eax -> address of new params
        mov ecx, eax
        call save_p     ; (head) 
        call save_p     ; (last)

        mov ecx, [STACK_OFFSET]
        call caddr
        mov ecx, ebx
        call cdr
        ; ebx -> address of old params(next)
        push ebx
        mov ecx, [STACK_OFFSET]
        call cddr
        mov ecx, ebx
        mov al, PAIR_POINT_T
        mov ebx, [esp]
        call set_car
        pop ecx
        ; ecx -> address of old params(next)
        mov ecx, ebx
        lambda_params_helper_0_loop:
            cmp ecx, 0
            je lambda_params_helper_0_loop_end
            call caar
            mov cl, PAIR_POINT_T
            mov edx, 0
            call cons
            ; ebx -> address of new params
            mov ebx, eax
            call restore_ecx
            ; ecx -> address of new params(last)
            mov al, PAIR_POINT_T
            call set_cdr
            mov ecx, ebx
            call save_p

            mov ecx, [STACK_OFFSET]
            call caddr
            mov ecx, ebx
            call cdr
            ; ebx -> address of old params(next)
            push ebx
            mov ecx, [STACK_OFFSET]
            call cddr
            mov ecx, ebx
            mov al, PAIR_POINT_T
            mov ebx, [esp]
            call set_car
            pop ecx
            jmp lambda_params_helper_0_loop
            lambda_params_helper_0_loop_end:
                call restore_ecx
                call restore_ebx    ; head
                call restore_ecx
                ret

    lambda_params_helper_1:
        mov ebx, 0
        ret

;;; params: ecx -> address of exp
;;; return: ebx
;;; broke: all
lambda_params:
    call cadr
    mov ecx, ebx
    call lambda_params_helper
    ret

;;; params: ecx -> address of exp
;;; return: ebx -> address of exp
lambda_body:
    push ax
    call cddr
    pop ax
    ret

;;; params: ebx -> address of params, edx -> address of body
;;; return: ebx -> address of new lambda exp
;;; broke: all
make_lambda:
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    call cons
    mov ecx, eax
    call save_p

    mov al, STRING_T
    mov ebx, LAMBDA_SYMBOL
    mov cl, PAIR_POINT_T
    mov edx, 0
    call cons

    mov ebx, eax
    mov al, PAIR_POINT_T
    call restore_ecx
    mov edx, ecx
    mov cl, PAIR_POINT_T
    call cons
    mov ebx, eax
    ret

LAMBDA_SYMBOL db "lambda", 0

;;; params: ecx -> address of exp
;;; return: al
is_if:
    push ebx
    push edx
    mov ebx, ecx
    mov edx, IF_SYMBOL
    call tagged
    pop edx
    pop ebx
    ret

IF_SYMBOL db "if", 0

;;; params: ecx -> address of exp
;;; return: ebx
if_predicate:
    push ax
    call cadr
    pop ax
    ret

;;; params: ecx -> address of exp
;;; return: ebx
if_consequent:
    push ax
    call caddr
    pop ax
    ret

;;; params: ecx -> address of exp
;;; return: ebx
;;; broke: all
if_alternative:
    call cdddr
    cmp ebx, 0
    je if_alternative_1
        mov ecx, ebx
        call car
        ret
    if_alternative_1:
        mov al, STRING_T
        mov ebx, QUOTE_FALSE_SYMBOL
        mov cl, PAIR_POINT_T
        mov edx, 0
        call cons
        mov ebx, eax
        ret

QUOTE_FALSE_SYMBOL db "'false", 0

;;; params: [ARGL_OFFSET]
;;;         1st -> predicate
;;;         2nd -> consequent
;;;         3rd -> alternative
;;; return: ebx -> address of exp
;;; broke: all
make_if:
    mov al, STRING_T
    mov ebx, IF_SYMBOL
    mov cl, PAIR_POINT_T
    mov edx, 0
    call cons
    
    mov ebx, eax
    mov al, PAIR_POINT_T
    mov cl, PAIR_POINT_T
    mov edx, [ARGL_OFFSET]
    call cons
    mov ebx, eax
    ret

;;; params: ecx -> address of exp
;;; return: al
is_begin:
    push ebx
    push edx
    mov ebx, ecx
    mov edx, BEGIN_SYMBOL
    call tagged
    pop edx
    pop ebx
    ret

BEGIN_SYMBOL db "begin", 0

;;; params: ecx -> address of exp
;;; return: ebx
begin_actions:
    push ax
    call cdr
    pop ax
    ret

;;; params: ecx -> address of seq
;;; return: al
is_last_exp:
    push ebx
    call cdr
    cmp ebx, 0
    je is_last_exp_1
    is_last_exp_0:
        mov al, 0
        jmp is_last_exp_e
    is_last_exp_1:
        mov al, 1
    is_last_exp_e:
        pop ebx
        ret

;;; params: ecx -> address of seq
;;; return: ebx
first_exp:
    push ax
    call car
    pop ax
    ret

;;; params: ecx -> address of seq
;;; return: ebx
rest_exps:
    push ax
    call cdr
    pop ax
    ret

;;; params: ecx -> address of seq
;;; return: ebx
;;; broke: all
make_begin:
    call save_p
    mov al, STRING_T
    mov ebx, BEGIN_SYMBOL
    mov cl, PAIR_POINT_T
    mov edx, 0
    call cons

    mov ebx, eax
    mov al, PAIR_POINT_T
    call restore_ecx
    mov edx, ecx
    mov cl, PAIR_POINT_T
    call cons
    mov ebx, eax
    ret

;;; params: ecx -> address of exp
;;; return: al
is_application:
    push ebx
    call car
    cmp al, PAIR_POINT_T
    jne is_application_0
    cmp ebx, 0
    je is_application_0
    is_application_1:
        mov al, 1
        jmp is_application_e
    is_application_0:
        mov al, 0
    is_application_e:
        pop ebx
        ret

;;; params: ecx -> address of exp
;;; return: ebx
operator:
    push ax
    call car
    pop ax
    ret

;;; params: ecx -> address of exp
;;; return: ebx
operands:
    push ax
    call cdr
    pop ax
    ret

;;; params: ecx -> address of ops
;;; return: al
is_no_operands:
    cmp ecx, 0
    je is_no_operands_1
    is_no_operands_0:
        mov al, 0
        ret
    is_no_operands_1:
        mov al, 1
        ret

;;; params: ecx -> address of ops
;;; return: ebx
first_operand:
    push ax
    call car
    pop ax
    ret

;;; params: ecx -> address of ops
;;; return: ebx
rest_operands:
    push ax
    call cdr
    pop ax
    ret
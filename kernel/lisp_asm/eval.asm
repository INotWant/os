[bits 32]

global eval, eval_sequence

extern TRUE_SYMBOL, FALSE_SYMBOL, OK_SYMBOL
extern EXP_OFFSET, ENV_OFFSET, STACK_OFFSET, ARGL_OFFSET
extern is_self_eval, is_variable, is_quoted, text_of_quoted, is_assignment, assignment_variable, assignment_value, is_definition, definition_variable, definition_value, is_if, if_predicate, if_consequent, if_alternative, is_lambda, lambda_params, lambda_body, is_begin, begin_actions, is_last_exp, first_exp, rest_exps, is_application, is_no_operands, operator, operands
extern cons, list, car, cadr, cddr, caddr, set_car, set_cdr, STRING_T, PAIR_POINT_T
extern lookup_var, set_var, define_var
extern save_p, restore_ecx, restore_ebx
extern make_compound_procedure
extern str_cmp
extern apply
extern eval_error_handler

;;; params: non
;;; return: al -> type, ebx -> data
;;; broke: all
eval:
    mov ecx, [EXP_OFFSET]
    self_eval_exp:
        call is_self_eval
        cmp al, 1
        jne variable_exp
        call car
        jmp eval_normal_end
    variable_exp:
        call is_variable
        cmp al, 1
        jne quoted_exp
        call car
        ; eax -> address of var name
        mov eax, ebx
        call lookup_var
        cmp ecx, 0
        je variable_exp_non
        variable_exp_exist:
            mov al, [ecx]
            mov ebx, [ecx + 1]
            jmp eval_normal_end
        variable_exp_non:
            mov dword [EXP_OFFSET], 0
            mov ebx, VAR_NON_EXIST
            jmp eval_error_handler
    quoted_exp:
        call is_quoted
        cmp al, 1
        jne assignment_exp
        call text_of_quoted
        jmp eval_normal_end
    assignment_exp:
        call is_assignment
        cmp al, 1
        jne definition_exp
        call eval_assignment
        cmp al, 1
        jne assignment_exp_fail
        assignment_exp_succ:
            mov al, STRING_T
            mov ebx, OK_SYMBOL
            jmp eval_normal_end
        assignment_exp_fail:
            mov dword [EXP_OFFSET], 0
            mov ebx, ASSIGN_VAR_NON_EXIST
            jmp eval_error_handler
    definition_exp:
        call is_definition
        cmp al, 1
        jne if_exp
        call eval_definition
        cmp al, 1
        jne definition_exp_fail
        definition_exp_succ:
            mov al, STRING_T
            mov ebx, OK_SYMBOL
            jmp eval_normal_end
        definition_exp_fail:
            mov dword [EXP_OFFSET], 0
            mov ebx, DEFINE_ENV_NON_EXIST
            jmp eval_error_handler
    if_exp:
        call is_if
        cmp al, 1
        jne lambda_exp
        call eval_if
        jmp eval_normal_end
    lambda_exp:
        call is_lambda
        cmp al, 1
        jne begin_exp
        ; construct list(ARGL_OFFSET) -> (params, body, env)
        call lambda_params
        mov ax, PAIR_POINT_T
        push ax
        push ebx    ; ebx -> params
        mov ecx, [EXP_OFFSET]
        call lambda_body
        mov ax, PAIR_POINT_T
        push ax
        push ebx    ; ebx -> body
        mov ebx, [ENV_OFFSET]
        push ax
        push ebx    ; ebx -> env
        mov eax, 3
        push eax    ; eax -> list length
        call list   ; eax -> head pointer
        mov edx, eax
        call make_compound_procedure
        mov al, PAIR_POINT_T
        jmp eval_normal_end
    begin_exp:
        call is_begin
        cmp al, 1
        jne application_exp
        call begin_actions
        mov ecx, ebx
        call eval_sequence
        jmp eval_normal_end
    application_exp:
        call is_application
        cmp al, 1
        jne unknown_exp
        ; save exp
        call save_p
        ; exec operator
        call operator       ; ebx -> operator
        mov [EXP_OFFSET], ebx
        call eval           
        ; save procedure
        mov ecx, ebx        ; ecx -> procedure
        call save_p
        ; get ops
        mov ecx, [STACK_OFFSET]
        call cadr
        mov ecx, ebx        ; ecx -> exp
        call operands       ; ebx -> exps(ops)
        ; exec ops, and get real params
        mov ecx, ebx
        call eval_real_parameters
        mov edx, eax        ; edx -> real arguments
        call restore_ebx    ; ebx -> procedure
        call restore_ecx    ; ecx -> exp(non)
        ; apply
        call apply
        jmp eval_normal_end
    unknown_exp:
        mov dword [EXP_OFFSET], 0
        mov ebx, UNKNOWN_EXP_TYPE
        jmp eval_error_handler
    eval_normal_end:
        mov dword [EXP_OFFSET], 0
        ret

VAR_NON_EXIST db "variable non exist -- EVAL", 0
UNKNOWN_EXP_TYPE db "unknown expression type -- EVAL", 0
ASSIGN_VAR_NON_EXIST db "variable non exist -- set!", 0
DEFINE_ENV_NON_EXIST db "environment unavailable -- define", 0

;;; params: non
;;; return: al --> 1 show success, 0 show failure(variable does not exist)
;;; broke: all
eval_assignment:
    mov ecx, [EXP_OFFSET]
    call assignment_variable    ; ebx -> address of var name
    push ebx
    call assignment_value       ; ebx -> address of exp(special value)
    mov [EXP_OFFSET], ebx
    call eval                   ; al -> type of value, ebx -> value of data
    pop ecx                     ; ecx -> address of var name
    call set_var
    ret

;;; params: non
;;; return: al --> 1 show success, 0 show failure(environment unavailable)
;;; broke: all
eval_definition:
    mov ecx, [EXP_OFFSET]
    call definition_variable    ; ebx -> address of var name
    push ebx
    call definition_value       ; ebx -> address of exp(special value)
    mov [EXP_OFFSET], ebx
    call eval                   ; al -> type of value, ebx -> value of data
    pop ecx                     ; ecx -> address of var name
    call define_var
    ret

;;; params: non
;;; return: al --> type, ebx -> data
;;; broke: all
eval_if:
    ; 0 -> save exp
    mov ecx, [EXP_OFFSET]
    call save_p
    ; 1
    call if_predicate       ; ebx -> address of exp(predicate)
    mov [EXP_OFFSET], ebx
    call eval               ; al, ebx
    cmp al, STRING_T
    jne eval_if_alternative
    mov edx, TRUE_SYMBOL
    call str_cmp
    cmp al, 0
    jne eval_if_alternative
    eval_if_consequent:
        call restore_ecx
        call if_consequent
        jmp eval_if_end
    eval_if_alternative:
        call restore_ecx
        call if_alternative
    eval_if_end:
        mov [EXP_OFFSET], ebx
        call eval
        ret

;;; params: ecx -> exps
;;; return: al -> type, ebx -> data (of last exp)
;;; broke: all
eval_sequence:
    call save_p
    eval_sequence_loop:
        call restore_ecx
        call is_last_exp
        cmp al, 1
        jne eval_sequence_seq
        eval_sequence_last:
            call first_exp
            mov [EXP_OFFSET], ebx
            ;;; // TODO test
            ;; non tail recursion
            ; call eval
            ; ret
            ;; tail recursion
            jmp eval
        eval_sequence_seq:
            call first_exp
            mov [EXP_OFFSET], ebx
            call rest_exps
            mov ecx, ebx
            call save_p
            call eval
            jmp eval_sequence_loop

;;; params: ecx -> exps(ops)
;;; return: eax -> head address
;;; broke: all
eval_real_parameters:
    call is_no_operands
    cmp al, 1
    jne eval_real_parameters_1
    eval_real_parameters_0:
        mov eax, 0
        ret
    eval_real_parameters_1:
        call save_p             ; save ops
        call first_exp
        mov [EXP_OFFSET], ebx
        call eval
        mov cl, PAIR_POINT_T
        mov edx, 0
        call cons
        mov ecx, eax
        call save_p     ; (head) 
        call save_p     ; (last)
        eval_real_parameters_1_loop:
            mov ecx, [STACK_OFFSET]
            call caddr
            mov ecx, ebx
            call rest_exps
            mov edx, ebx
            cmp edx, 0
            je eval_real_parameters_1_end
            ; update ops in stack
            mov ecx, [STACK_OFFSET]
            call cddr
            mov ecx, ebx
            mov al, PAIR_POINT_T
            mov ebx, edx    ; ebx -> ops
            call set_car
            ; exec new exp
            mov ecx, ebx
            call first_exp
            mov [EXP_OFFSET], ebx
            call eval
            mov cl, PAIR_POINT_T
            mov edx, 0
            call cons
            mov edx, eax
            ; connect
            call restore_ecx
            mov al, PAIR_POINT_T
            mov ebx, edx     
            call set_cdr
            ; update (last)
            mov ecx, ebx
            call save_p
            jmp eval_real_parameters_1_loop
        eval_real_parameters_1_end:
            call restore_ebx
            call restore_ecx
            call restore_ebx
            mov eax, ecx
            ret

;;; // TODO 1) 处理 栈满问题
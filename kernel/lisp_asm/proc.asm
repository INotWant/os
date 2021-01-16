[bits 32]

global is_compound_procedure, compound_procedure_parameters, compound_procedure_body, compound_procedure_env, make_compound_procedure
global is_primitive_procedure, make_primitive_procedure, primitive_procedure_impl

extern str_cmp
extern cons, car, cadr, caddr, cadddr, STRING_T, PAIR_POINT_T, FUNC_POINT_T

;;; params: ebx -> address of exp, edx -> address of str
;;; return: al
tagged:
    push ecx
    call car
    cmp al, STRING_T
    jne tagged_false
    call str_cmp
    cmp al, 0
    jne tagged_false
    tagged_true:
        mov al, 1
    jmp tagged_end
    tagged_false:
        mov al, 0
    tagged_end:
        pop ecx
        ret

;;; params: ecx -> point
;;; return: al
is_compound_procedure:
    push ebx
    push edx
    mov ebx, ecx
    mov edx, COMPOUND_PROC_SYMBOL
    call tagged
    pop edx
    pop ebx
    ret

COMPOUND_PROC_SYMBOL db "procedure", 0

;;; params: ecx -> point
;;; return: ebx -> point
compound_procedure_parameters:
    push ax
    call cadr
    pop ax
    ret

;;; params: ecx -> point
;;; return: ebx -> point
compound_procedure_body:
    push ax
    call caddr
    pop ax
    ret

;;; params: ecx -> point
;;; return: ebx -> point
compound_procedure_env:
    push ax
    call cadddr
    pop ax
    ret

;;; params: edx -> head pointer of list(params, body, env)
;;; return: ebx
;;; broke: eax ,ecx
make_compound_procedure:
    mov al, STRING_T
    mov ebx, COMPOUND_PROC_SYMBOL
    mov cl, PAIR_POINT_T
    call cons
    mov ebx, eax
    ret

;;; params: ecx -> point
;;; return: al
is_primitive_procedure:
    push ebx
    push edx
    mov ebx, ecx
    mov edx, PRIMITIVE_PROC_SYMBOL
    call tagged
    pop edx
    pop ebx
    ret

PRIMITIVE_PROC_SYMBOL db "primitive", 0

;;; params: ecx -> point of function
;;; return: ebx
;;; broke: all
make_primitive_procedure:
    mov al, FUNC_POINT_T
    mov ebx, ecx
    mov cl, PAIR_POINT_T
    mov edx, 0
    call cons
    mov edx, eax
    mov cl, PAIR_POINT_T
    mov al, STRING_T
    mov ebx, PRIMITIVE_PROC_SYMBOL
    call cons
    mov ebx, eax
    ret

;;; params: ecx -> point
;;; return: ebx -> point
primitive_procedure_impl:
    push ax
    call cadr
    pop ax
    ret

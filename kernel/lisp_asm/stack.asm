[bits 32]

global stack_init, save_i, save_f, save_s, save_p, restore, restore_ebx, restore_ecx

extern INTEGER_T, FLOAT_T, STRING_T, PAIR_POINT_T
extern STACK_OFFSET
extern cons, car, cdr

;;; init stack
stack_init:
    mov dword [STACK_OFFSET], 0
    ret

;;; save integer to stack
;;; params: ebx -> value
;;; broke: eax, ecx, edx
save_i:
    mov al, INTEGER_T
    mov cl, PAIR_POINT_T
    mov edx, [STACK_OFFSET]
    call cons
    mov [STACK_OFFSET], eax
    ret

;;; save float to stack
;;; params: ebx -> value
;;; broke: eax, ecx, edx
save_f:
    mov al, FLOAT_T
    mov cl, PAIR_POINT_T
    mov edx, [STACK_OFFSET]
    call cons
    mov [STACK_OFFSET], eax
    ret

;;; save string to stack
;;; params: ecx -> value
;;; broke: eax, ebx, edx
save_s:
    mov al, STRING_T
    mov ebx, ecx
    mov cl, PAIR_POINT_T
    mov edx, [STACK_OFFSET]
    call cons
    mov [STACK_OFFSET], eax
    mov ecx, [eax + 1]
    ret

;;; save the pair point to stack
;;; params: ecx -> value
;;; broke: eax, ebx, edx
save_p:
    mov al, PAIR_POINT_T
    mov ebx, ecx
    mov cl, PAIR_POINT_T
    mov edx, [STACK_OFFSET]
    call cons
    mov [STACK_OFFSET], eax
    mov ecx, [eax + 1]
    ret

;;; get the top element of the stack
;;; return: al -> type, ebx -> value
restore:
    push ecx
    mov ecx, [STACK_OFFSET]
    call cdr
    mov [STACK_OFFSET], ebx
    call car
    pop ecx
    ret

;;; get the top element of the stack
;;; return: ebx -> value
restore_ebx:
    push ax
    push ecx
    mov ecx, [STACK_OFFSET]
    call cdr
    mov [STACK_OFFSET], ebx
    call car
    pop ecx
    pop ax
    ret

;;; get the top element of the stack
;;; return: ecx -> value
restore_ecx:
    push ax
    push ebx
    mov ecx, [STACK_OFFSET]
    call cdr
    mov [STACK_OFFSET], ebx
    call car
    mov ecx, ebx
    pop ebx
    pop ax
    ret
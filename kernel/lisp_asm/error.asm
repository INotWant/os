global eval_error_handler

extern kprint, clear_exp, clear_key_buffer
extern EXP_OFFSET, UENV_OFFSET, ENV_OFFSET, ARGL_OFFSET, STACK_OFFSET, CONTINUE_OFFSET
extern _loop

;;; params: ebx -> address of string about the cause
;;; return: non
;;; broke: all
eval_error_handler:
    ;; print reason
    push ebx
    call kprint
    pop ebx
    ;; print "\n> "
    mov ebx, NEW_START_SYMBOL
    push ebx
    call kprint
    pop ebx
    ;; clear exp str
    call clear_exp
    call clear_key_buffer
    ;; restore root table
    mov dword [EXP_OFFSET], 0
    mov ecx, [UENV_OFFSET]
    mov [ENV_OFFSET], ecx
    mov dword [CONTINUE_OFFSET], 0
    mov dword [ARGL_OFFSET], 0
    mov dword [STACK_OFFSET], 0
    ;; restore ebp esp esi edi
    mov eax, EBP_CONSTANT
    mov ebp, eax
    mov eax, ESP_CONSTANT
    mov esp, eax
    mov eax, 0x0
    mov esi, eax
    mov edi, eax
    ;; restore interupt
    sti
    jmp _loop

NEW_START_SYMBOL db 0x0A , "> ", 0
EBP_CONSTANT equ 0x9f000
ESP_CONSTANT equ 0x9eff8
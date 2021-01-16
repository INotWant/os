[bits 32]

global apply

extern extend_env
extern save_p, restore_ecx
extern ARGL_OFFSET, ENV_OFFSET
extern car, cadr, cdddr, list, PAIR_POINT_T
extern eval_sequence, eval_error_handler
extern is_primitive_procedure, primitive_procedure_impl, is_compound_procedure, compound_procedure_parameters, compound_procedure_body, compound_procedure_env

;;; params: ebx -> procedure, edx -> real arguments
;;; return: al -> type, ebx -> data
;;; broke: all
apply:
    mov ecx, ebx
    apply_primitive:
        call is_primitive_procedure
        cmp al, 1
        jne apply_compound
        call primitive_procedure_impl   ; ebx -> function point
        mov [ARGL_OFFSET], edx
        call ebx
        mov dword [ARGL_OFFSET], 0
        ret
    apply_compound:
        call is_compound_procedure
        cmp al, 1
        jne apply_error
        ; construct list(virtual_params, real_params, old_env, body, curr_env), use list because even if there is a GC, there is no error
        call compound_procedure_parameters  
        mov ax, PAIR_POINT_T
        push ax
        push ebx                        ; ebx -> params
        push ax
        push edx                        ; edx -> real params
        call compound_procedure_env         
        push ax
        push ebx                        ; ebx -> env
        call compound_procedure_body
        push ax
        push ebx                        ; ebx -> body
        mov ebx, [ENV_OFFSET]
        push ax
        push ebx                        ; ebx -> env(for restore)
        mov eax, 5
        push eax
        call list
        mov [ARGL_OFFSET], eax
        ; extend env
        call extend_env                 ; ecx -> new env
        mov [ENV_OFFSET], ecx
        ; restore [ARGL_OFFSET], and use stack to save body & env because of recursion(so don't use [ARGL_OFFSET])
        mov ecx, [ARGL_OFFSET]
        mov dword [ARGL_OFFSET], 0
        call cdddr
        mov ecx, ebx
        call save_p
        call car
        mov ecx, ebx                    ; ebx -> body(exps)
        ; exec exps
        call eval_sequence
        push ax
        push ebx
        ; restore env
        call restore_ecx
        call cadr
        mov [ENV_OFFSET], ebx
        pop ebx
        pop ax
        ret
    apply_error:
        mov ebx, UNKNOWN_PROC_TYPE
        jmp eval_error_handler

UNKNOWN_PROC_TYPE db "unknown procedure type -- APPLY", 0
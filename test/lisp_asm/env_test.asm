[bits 32]

%ifdef LISP_TEST

global env_test

extern ARGL_OFFSET
extern lookup_var, extend_env, define_var, set_var
extern print_string_pm
extern cons, INTEGER_T, FLOAT_T, STRING_T, PAIR_POINT_T

;;; suppose no gc
env_test:
    mov dword [ARGL_OFFSET], 0
    ; test lookup_var
    env_test_0:
        mov eax, V1
        call lookup_var
        cmp ecx, 0
        je env_test_1
        mov ebx, ENV_ERR_0
        call print_string_pm
        ret
    ; test extend_env & lookup_var
    env_test_1:
        mov al, INTEGER_T
        mov ebx, 1
        mov cl, PAIR_POINT_T
        mov edx, 0
        call cons
        ; eax -> value point
        mov ebx, eax
        mov al, PAIR_POINT_T
        mov cl, PAIR_POINT_T
        mov edx, [ARGL_OFFSET]
        call cons
        mov [ARGL_OFFSET], eax
        mov al, STRING_T
        mov ebx, V1
        mov cl, PAIR_POINT_T
        mov edx, 0
        call cons
        ; eax -> key point
        mov ebx, eax
        mov al, PAIR_POINT_T
        mov cl, PAIR_POINT_T
        mov edx, [ARGL_OFFSET]
        call cons
        mov [ARGL_OFFSET], eax
        call extend_env
        mov eax, V1
        call lookup_var
        cmp byte [ecx], INTEGER_T
        jne env_test_1_error
        cmp dword [ecx + 1], 1
        jne env_test_1_error
        jmp env_test_2     
        env_test_1_error:
            mov ebx, ENV_ERR_1
            call print_string_pm
            ret
    ; test lookup_var
    env_test_2:
        mov eax, V2
        call lookup_var
        cmp ecx, 0
        je env_test_3
        mov ebx, ENV_ERR_2
        call print_string_pm
        ret
    ; test define_var
    env_test_3:
        mov al, FLOAT_T
        mov ebx, 2
        mov ecx, V2
        call define_var
        mov eax, V2
        call lookup_var
        cmp byte [ecx], FLOAT_T
        jne env_test_3_error
        cmp dword [ecx + 1], 2
        jne env_test_3_error
        jmp env_test_4
        env_test_3_error:
            mov ebx, ENV_ERR_3
            call print_string_pm
            ret
    ; test set_var
    env_test_4:
        mov al, INTEGER_T
        mov ebx, 3
        mov ecx, V1
        call set_var
        mov eax, V1
        call lookup_var
        cmp byte [ecx], INTEGER_T
        jne env_test_4_error
        cmp dword [ecx + 1], 3
        jne env_test_4_error
        ret     
        env_test_4_error:
            mov ebx, ENV_ERR_4
            call print_string_pm
            ret

V1 db "v1", 0
V2 db "v2", 0

ENV_ERR_0 db "env error 0 -- lookup_var" , 0
ENV_ERR_1 db "env error 1 -- extend_env or lookup_var " , 0
ENV_ERR_2 db "env error 2 -- lookup_var" , 0
ENV_ERR_3 db "env error 3 -- define_var" , 0
ENV_ERR_4 db "env error 4 -- set_var" , 0

%endif

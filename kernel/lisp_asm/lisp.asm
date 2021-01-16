[bits 32]

global EXP_OFFSET, ENV_OFFSET, CONTINUE_OFFSET, ARGL_OFFSET, UENV_OFFSET, STACK_OFFSET
global lisp_init

extern pair_pool_init, stack_init, env_init
extern list, PAIR_POINT_T
extern extend_env
extern primitive_procedure_names, primitive_procedure_objects

; special register
EXP_OFFSET equ 0x9f004
ENV_OFFSET equ 0x9f008
CONTINUE_OFFSET equ 0x9f00c
ARGL_OFFSET equ 0x9f010
UENV_OFFSET equ 0x9f014     ; init env
STACK_OFFSET equ 0x9f018

lisp_init:
    mov dword [EXP_OFFSET], 0
    mov dword [ENV_OFFSET], 0
    mov dword [CONTINUE_OFFSET], 0
    mov dword [ARGL_OFFSET], 0
    mov dword [UENV_OFFSET], 0
    mov dword [STACK_OFFSET], 0

    call pair_pool_init

    ;; extend env
    mov ax, PAIR_POINT_T
    push ax
    call primitive_procedure_names
    push eax    ; eax -> var names
    mov ax, PAIR_POINT_T
    push ax
    call primitive_procedure_objects
    push eax    ; eax -> values
    mov ax, PAIR_POINT_T
    push ax
    mov eax, 0
    push eax    ; eax -> env
    mov eax, 3
    push eax
    call list   ; eax -> list(var_names, values, env)
    mov [ARGL_OFFSET], eax
    call extend_env
    mov [ENV_OFFSET], ecx
    mov [UENV_OFFSET], ecx
    mov dword [ARGL_OFFSET], 0

    ret
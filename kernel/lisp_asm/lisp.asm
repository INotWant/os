[bits 32]

global EXP_OFFSET, ENV_OFFSET, PROC_OFFSET, ARGL_OFFSET, UENV_OFFSET, STACK_OFFSET
global lisp_init

extern pair_pool_init, stack_init, env_init

; special register
EXP_OFFSET equ 0x9f004
ENV_OFFSET equ 0x9f008
PROC_OFFSET equ 0x9f00c
ARGL_OFFSET equ 0x9f010
UENV_OFFSET equ 0x9f014
STACK_OFFSET equ 0x9f018

lisp_init:
    call pair_pool_init
    call stack_init
    call env_init
    ret
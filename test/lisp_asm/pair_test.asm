[bits 32]

%ifdef LISP_TEST

global pair_test

extern EXP_OFFSET, ENV_OFFSET, CONTINUE_OFFSET, ARGL_OFFSET, UENV_OFFSET, STACK_OFFSET
extern print_string_pm
extern INTEGER_T, FLOAT_T, STRING_T, PAIR_POINT_T, FREE_OFFSET
extern cons, car, cdr, set_car, set_cdr

pair_test:
    ; test cons
    mov al, INTEGER_T
    mov ebx, 0x7e4
    mov cl, FLOAT_T
    mov edx, 0x7e5
    call cons
    ; test car
    push eax
    mov ecx, eax
    call car
    cmp al, INTEGER_T
    jne pair_err_0
    cmp ebx, 0x7e4
    jne pair_err_1
    ; test cdr
    pop ecx
    call cdr
    cmp al, FLOAT_T
    jne pair_err_2
    cmp ebx, 0x7e5  
    jne pair_err_3
    ; test set_cdr
    mov al, INTEGER_T
    mov ebx, 0x7e4
    call set_cdr
    call cdr
    cmp al, INTEGER_T
    jne pair_err_4
    cmp ebx, 0x7e4
    jne pair_err_5

    %ifdef GC_TEST
    ; test GC
    call pair_gc_test
    %endif

    jmp pair_test_end

    pair_err_0:
        mov ebx, PAIR_ERR_0
        call print_string_pm
        jmp pair_test_end
    pair_err_1:
        mov ebx, PAIR_ERR_1
        call print_string_pm
        jmp pair_test_end
    pair_err_2:
        mov ebx, PAIR_ERR_2
        call print_string_pm
        jmp pair_test_end
    pair_err_3:
        mov ebx, PAIR_ERR_3
        call print_string_pm
        jmp pair_test_end
    pair_err_4:
        mov ebx, PAIR_ERR_4
        call print_string_pm
        jmp pair_test_end
    pair_err_5:
        mov ebx, PAIR_ERR_5
        call print_string_pm
        jmp pair_test_end

    mov ebx, PAIR_SUCC
    call print_string_pm

    pair_test_end:
        ret

%ifdef GC_TEST
pair_gc_test:
    ; init root table
    mov dword [EXP_OFFSET], 0
    mov dword [ENV_OFFSET], 0
    mov dword [CONTINUE_OFFSET], 0
    mov dword [ARGL_OFFSET], 0
    mov dword [UENV_OFFSET], 0
    mov dword [STACK_OFFSET], 0

    mov al, INTEGER_T
    mov ebx, 0x7e4
    mov cl, FLOAT_T
    mov edx, 0x7e5
    ; trigger GC
    call cons
    cmp word [FREE_OFFSET], 0x9
    jne pair_gc_test_1
    pair_gc_test_0:
        ret
    pair_gc_test_1:
        mov ebx, PAIR_ERR_6
        call print_string_pm
        ret
%endif

PAIR_ERR_0 db "pair error 0 -- car type", 0
PAIR_ERR_1 db "pair error 1 -- car value", 0
PAIR_ERR_2 db "pair error 2 -- cdr type", 0
PAIR_ERR_3 db "pair error 3 -- cdr value", 0
PAIR_ERR_4 db "pair error 4 -- set-cdr type", 0
PAIR_ERR_5 db "pair error 5 -- set-cdr value", 0
PAIR_ERR_6 db "pair error 6 -- GC", 0
PAIR_SUCC db "pair succ", 0

%endif
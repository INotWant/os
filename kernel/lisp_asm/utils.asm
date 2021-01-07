[bits 32]

global str_cmp

;;; params: ebx -> address of str0, edx -> address of str1
;;; return: al ; 0 show str0 = str1, 1 show str0 > str1, -1 show str0 < str1
str_cmp:
    push ebx
    push edx
    str_cmp_loop:
        mov al, [ebx]
        cmp al, [edx]
        jne str_cmp_1
        str_cmp_0:  ; [ebx] = [edx]
            cmp byte [ebx], 0
            je str_cmp_0_1
            str_cmp_0_0:
                add ebx, 1
                add edx, 1
                jmp str_cmp_loop
            str_cmp_0_1:
                mov al, 0
                jmp str_cmp_end
        str_cmp_1:
            jb str_cmp_1_1
            str_cmp_1_0:    ; [ebx] > [edx]
                mov al, 1
                jmp str_cmp_end
            str_cmp_1_1:    ; [ebx] < [edx]
                mov al, -1
        str_cmp_end:
            pop edx
            pop ebx
            ret
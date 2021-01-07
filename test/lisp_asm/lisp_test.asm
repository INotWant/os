[bits 32]

%ifdef LISP_TEST

global lisp_test

extern pair_test, env_test

lisp_test:
    call pair_test
    call env_test
    ret

%endif
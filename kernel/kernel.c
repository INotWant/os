#include "kernel.h"
#include "welcome.h"
#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "lisp/pair.h"
#include "lisp/constant.h"
#include "lisp/stack.h"
#include "lisp/input.h"
#include "lisp/lisp.h"
#include "lisp/procedure.h"

#include "../test/test.h"

/** 表达式最大长度 **/
#define EXP_MAX_LEN 2048

/* 存放表达式 */
static char *exp_str;
static size_t exp_len = 0;

void kernel_main() {
    interrupt_install();
    irq_install();

    memory_init();
    pair_init();
    constant_init();
    lisp_init();

    /* 调用所有测试 */
    // test_main();

    welcome();
    /* 开启键盘 */
    enable_keyboard();

    exp_str = (char *)memory_malloc(EXP_MAX_LEN);
    kprint("Lisp is the best language!");
    kprint("\n> ");
}

void clear_exp() {
    exp_len = 0;
}

void user_input(char* input) {
    if (strcmp(input, "end") == 0){
        kprint("Stopping the CPU, Bye!\n");
        asm volatile("hlt");
    }
    int len = input_len();
    if (len == -1)
        kprint("too many characters in a line!");
    else if (len == 0) {
        if (exp_len == 0)
            kprint("> ");
        return;
    } else {
        exp_len += len;
        if (exp_len > EXP_MAX_LEN) {
            kprint("too many characters in a expression!");
            exp_len = 0;
        } else {
            memory_copy((uint8_t *)input, (uint8_t *)(exp_str + (exp_len - len)), len);
            int flag = is_legal(exp_str, exp_len);
            if (flag == 0) {            /* 表达式合法 */
                void *exp = save_str_to_pair(exp_str, exp_len);
                /* 执行表达式，并打印结果 */
                element_t ele = lisp_exec(exp);
                if (ele.type != POINT_PAIR_T || ele.val.point != 0)
                print_element(ele);
                exp_len = 0;
            } else if (flag == -1) {    /* 表达式不合法 */
                kprint(">> illegal expression!");
                exp_len = 0;
            } else if (flag == 1)       /* 表达式不完整继续输入 */
                return;
        }
    }
    kprint("\n> ");
}
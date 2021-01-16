#include "kernel.h"
#include "welcome.h"
#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "lisp_asm/_common.h"
#include "lisp_asm/_input.h"

#define EXP_MAX_LEN 2048

/* 存放表达式 */
static char *exp_str;
static size_t exp_len = 0;

extern void lisp_init();
#ifdef LISP_TEST
extern void lisp_test();
#endif

/* 函数声明 */
static void default_input_handler(char *input);

void kernel_main()
{
    interrupt_install();
    irq_install(default_input_handler);

    memory_init();

    welcome();
    /* 开启键盘 */
    enable_keyboard();

    exp_str = (char *)memory_malloc(EXP_MAX_LEN);
    lisp_init();

#ifdef LISP_TEST
    lisp_test();
#endif

    kprint("Lisp is the best language!");
    kprint("\n> ");
}

void clear_exp()
{
    exp_len = 0;
}

static void default_input_handler(char *input)
{
    if (strcmp(input, "end") == 0)
    {
        kprint("Stopping the CPU, Bye!\n");
        asm volatile("hlt");
    }
    int len = input_len();
    if (len == -1)
    {
        kprint("too many characters in a line!");
        exp_len = 0;
    }
    else if (len == 0)
    {
        if (exp_len == 0)
            kprint("> ");
        else
            exp_str[exp_len++] = '\n';
        return;
    }
    else
    {
        exp_len += len;
        if (exp_len >= EXP_MAX_LEN)
        {
            kprint("too many characters in a expression!");
            exp_len = 0;
        }
        else
        {
            memory_copy((uint8_t *)input, (uint8_t *)(exp_str + (exp_len - len)), len);
            int flag = is_legal(exp_str, exp_len);
            if (flag == 0)
            { /* 表达式合法 */
                void *exp = save_str_to_pair(exp_str, exp_len);
                uint32_t type;
                int32_t data;
                asm volatile(
                    "movl %2, %%eax\n\t"
                    "movl %%eax, (%3)\n\t"
                    "call eval\n\t"
                    "movl %%eax, %0\n\t"
                    "movl %%ebx, %1\n\t"
                    : "=m"(type), "=m"(data)
                    : "m"(exp), "r"(EXP_OFFSET)
                    : "eax", "ebx");
                element_t ele;
                ele.type = (uint8_t)(type & 0xff);
                memory_copy((uint8_t *)(&data), (uint8_t *)(&ele.val), 4);
                if (ele.type != PAIR_POINT_T || ele.val.point != 0)
                {
                    asm volatile(
                        "movl %0, %%ecx\n\t"
                        "push %%ecx\n\t"
                        "call display_impl_c\n\t"
                        "pop %%ecx\n\t"
                        :
                        : "r"(&ele)
                        : "ecx");
                }
                exp_len = 0;
            }
            else if (flag == -1)
            { /* 表达式不合法 */
                kprint(">> illegal expression!");
                exp_len = 0;
            }
            else if (flag == 1)
            { /* 表达式不完整继续输入 */
                exp_str[exp_len++] = '\n';
                return;
            }
        }
    }
    kprint("\n> ");
}
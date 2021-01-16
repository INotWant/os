#include <stdint.h>
#include "_common.h"
#include "_input.h"
#include "../../libc/string.h"
#include "../../libc/mem.h"
#include "../../drivers/screen.h"
#include "../../drivers/keyboard.h"

void add_impl_c(void *arg1_p, void *arg2_p)
{
    uint8_t type1 = *((uint8_t *)arg1_p);
    uint8_t type2 = *((uint8_t *)arg2_p);
    if (type1 == INTEGER_T && type2 == INTEGER_T)
    {
        int32_t data1 = *((int32_t *)((uint8_t *)arg1_p + 1));
        int32_t data2 = *((int32_t *)((uint8_t *)arg2_p + 1));
        int32_t ret = data1 + data2;
        *((int32_t *)((uint8_t *)arg1_p + 1)) = ret;
    }
    else
    {
        float data1, data2;
        if (type1 == INTEGER_T)
            data1 = *((int32_t *)((uint8_t *)arg1_p + 1));
        else
            data1 = *((float *)((uint8_t *)arg1_p + 1));
        if (type2 == INTEGER_T)
            data2 = *((int32_t *)((uint8_t *)arg2_p + 1));
        else
            data2 = *((float *)((uint8_t *)arg2_p + 1));
        float ret = data1 + data2;
        *((uint8_t *)arg1_p) = FLOAT_T;
        *((float *)((uint8_t *)arg1_p + 1)) = ret;
    }
}

void sub_impl_c(void *arg1_p, void *arg2_p)
{
    uint8_t type1 = *((uint8_t *)arg1_p);
    uint8_t type2 = *((uint8_t *)arg2_p);
    if (type1 == INTEGER_T && type2 == INTEGER_T)
    {
        int32_t data1 = *((int32_t *)((uint8_t *)arg1_p + 1));
        int32_t data2 = *((int32_t *)((uint8_t *)arg2_p + 1));
        int32_t ret = data1 - data2;
        *((int32_t *)((uint8_t *)arg1_p + 1)) = ret;
    }
    else
    {
        float data1, data2;
        if (type1 == INTEGER_T)
            data1 = *((int32_t *)((uint8_t *)arg1_p + 1));
        else
            data1 = *((float *)((uint8_t *)arg1_p + 1));
        if (type2 == INTEGER_T)
            data2 = *((int32_t *)((uint8_t *)arg2_p + 1));
        else
            data2 = *((float *)((uint8_t *)arg2_p + 1));
        float ret = data1 - data2;
        *((uint8_t *)arg1_p) = FLOAT_T;
        *((float *)((uint8_t *)arg1_p + 1)) = ret;
    }
}

void mul_impl_c(void *arg1_p, void *arg2_p)
{
    uint8_t type1 = *((uint8_t *)arg1_p);
    uint8_t type2 = *((uint8_t *)arg2_p);
    if (type1 == INTEGER_T && type2 == INTEGER_T)
    {
        int32_t data1 = *((int32_t *)((uint8_t *)arg1_p + 1));
        int32_t data2 = *((int32_t *)((uint8_t *)arg2_p + 1));
        int32_t ret = data1 * data2;
        *((int32_t *)((uint8_t *)arg1_p + 1)) = ret;
    }
    else
    {
        float data1, data2;
        if (type1 == INTEGER_T)
            data1 = *((int32_t *)((uint8_t *)arg1_p + 1));
        else
            data1 = *((float *)((uint8_t *)arg1_p + 1));
        if (type2 == INTEGER_T)
            data2 = *((int32_t *)((uint8_t *)arg2_p + 1));
        else
            data2 = *((float *)((uint8_t *)arg2_p + 1));
        float ret = data1 * data2;
        *((uint8_t *)arg1_p) = FLOAT_T;
        *((float *)((uint8_t *)arg1_p + 1)) = ret;
    }
}

void div_impl_c(void *arg1_p, void *arg2_p)
{
    uint8_t type1 = *((uint8_t *)arg1_p);
    uint8_t type2 = *((uint8_t *)arg2_p);
    if (type1 == INTEGER_T && type2 == INTEGER_T)
    {
        int32_t data1 = *((int32_t *)((uint8_t *)arg1_p + 1));
        int32_t data2 = *((int32_t *)((uint8_t *)arg2_p + 1));
        int32_t ret = data1 / data2;
        *((int32_t *)((uint8_t *)arg1_p + 1)) = ret;
    }
    else
    {
        float data1, data2;
        if (type1 == INTEGER_T)
            data1 = *((int32_t *)((uint8_t *)arg1_p + 1));
        else
            data1 = *((float *)((uint8_t *)arg1_p + 1));
        if (type2 == INTEGER_T)
            data2 = *((int32_t *)((uint8_t *)arg2_p + 1));
        else
            data2 = *((float *)((uint8_t *)arg2_p + 1));
        float ret = data1 / data2;
        *((uint8_t *)arg1_p) = FLOAT_T;
        *((float *)((uint8_t *)arg1_p + 1)) = ret;
    }
}

void rem_impl_c(void *arg1_p, void *arg2_p)
{
    int32_t data1 = *((int32_t *)((uint8_t *)arg1_p + 1));
    int32_t data2 = *((int32_t *)((uint8_t *)arg2_p + 1));
    int32_t ret = data1 % data2;
    *((int32_t *)((uint8_t *)arg1_p + 1)) = ret;
}

uint8_t lt_impl_c(void *arg1_p, void *arg2_p)
{
    uint8_t type1 = *((uint8_t *)arg1_p);
    uint8_t type2 = *((uint8_t *)arg2_p);
    float data1, data2;
    if (type1 == INTEGER_T)
        data1 = *((int32_t *)((uint8_t *)arg1_p + 1));
    else
        data1 = *((float *)((uint8_t *)arg1_p + 1));
    if (type2 == INTEGER_T)
        data2 = *((int32_t *)((uint8_t *)arg2_p + 1));
    else
        data2 = *((float *)((uint8_t *)arg2_p + 1));
    if (data1 < data2)
        return 1;
    return 0;
}

uint8_t gt_impl_c(void *arg1_p, void *arg2_p)
{
    uint8_t type1 = *((uint8_t *)arg1_p);
    uint8_t type2 = *((uint8_t *)arg2_p);
    float data1, data2;
    if (type1 == INTEGER_T)
        data1 = *((int32_t *)((uint8_t *)arg1_p + 1));
    else
        data1 = *((float *)((uint8_t *)arg1_p + 1));
    if (type2 == INTEGER_T)
        data2 = *((int32_t *)((uint8_t *)arg2_p + 1));
    else
        data2 = *((float *)((uint8_t *)arg2_p + 1));
    if (data1 > data2)
        return 1;
    return 0;
}

uint8_t eq_impl_c(void *arg1_p, void *arg2_p)
{
    uint8_t type1 = *((uint8_t *)arg1_p);
    uint8_t type2 = *((uint8_t *)arg2_p);
    float data1, data2;
    if (type1 == INTEGER_T)
        data1 = *((int32_t *)((uint8_t *)arg1_p + 1));
    else
        data1 = *((float *)((uint8_t *)arg1_p + 1));
    if (type2 == INTEGER_T)
        data2 = *((int32_t *)((uint8_t *)arg2_p + 1));
    else
        data2 = *((float *)((uint8_t *)arg2_p + 1));
    if (data1 == data2)
        return 1;
    return 0;
}

static char *exp_str;
static size_t exp_len = 0;
static int flag;

static void input_handler_for_read(char *input)
{
    int len = input_len();
    if (len == -1)
    {
        flag = -2;
    }
    else if (len == 0)
    {
        if (exp_len != 0)
            exp_str[exp_len++] = '\n';
    }
    else
    {
        exp_len += len;
        if (exp_len >= EXP_MAX_LEN)
        {
            flag = -3;
        }
        else
        {
            memory_copy((uint8_t *)input, (uint8_t *)(exp_str + (exp_len - len)), len);
            flag = is_legal(exp_str, exp_len);
        }
    }
}

static char *ERR_ENL_READ = "expression not legal -- read";
static char *ERR_TMCL_READ = "too many characters in a line -- read";
static char *ERR_TMCE_READ = "too many characters in a expression -- read";

void read_impl_c()
{
    exp_len = 0;
    exp_str = (char *)memory_malloc(EXP_MAX_LEN);
    input_handler_fp default_input_handler = input_handler;
    input_handler = input_handler_for_read;
    clear_key_buffer();
    flag = 1;

    while (flag == 1)
    {
        asm volatile(
            "sti\n\t"
            "hlt\n\t");
    }

    input_handler = default_input_handler;
    if (flag == 0)
    {
        void *p = save_str_to_pair(exp_str, exp_len);
        memory_free(exp_str);
        asm volatile(
            "movl %0, %%ecx\n\t"
            "call save_p\n\t"
            :
            : "r"(p)
            : "ecx");
        return;
    }
    memory_free(exp_str);

    if (flag == -1)
    {
        asm volatile(
            "movl %0, %%ebx\n\t"
            "call eval_error_handler"
            :
            : "r"(ERR_ENL_READ)
            : "ebx");
    }
    else if (flag == -2)
    {
        asm volatile(
            "movl %0, %%ebx\n\t"
            "call eval_error_handler"
            :
            : "r"(ERR_TMCL_READ)
            : "ebx");
    }
    else if (flag == -3)
    {
        asm volatile(
            "movl %0, %%ebx\n\t"
            "call eval_error_handler"
            :
            : "r"(ERR_TMCE_READ)
            : "ebx");
    }
}

void display_impl_c(void *arg_p)
{
    uint8_t type = *((uint8_t *)arg_p);
    if (type == INTEGER_T)
    {
        char str[13];
        int32_t data = *((int32_t *)((uint8_t *)arg_p + 1));
        int2str(data, str);
        kprint(str);
    }
    else if (type == FLOAT_T)
    {
        char str[20];
        float data = *((float *)((uint8_t *)arg_p + 1));
        float2str(data, str, 6);
        kprint(str);
    }
    else if (type == STRING_T)
    {
        char *str = *((char **)((uint8_t *)arg_p + 1));
        if (str[0] == '\'')
            kprint(str + 1);
        else
            kprint(str);
    }
    else if (type == PAIR_POINT_T)
    {
        void *point = *((void **)((uint8_t *)arg_p + 1));
        uint32_t ret = 0;
        asm volatile(
            "push %%ecx\n\t"
            "movl %1, %%ecx\n\t"
            "movl $0x0, %%eax\n\t"
            "call is_compound_procedure\n\t"
            "movl %%eax, %0\n\t"
            "pop %%ecx\n\t"
            : "=m"(ret)
            : "r"(point)
            : "al");
        if (ret == 1)
        {
            kprint("#compound -- ");
        }
        else
        {
            asm volatile(
                "push %%ecx\n\t"
                "movl %1, %%ecx\n\t"
                "movl $0x0, %%eax\n\t"
                "call is_primitive_procedure\n\t"
                "movl %%eax, %0\n\t"
                "pop %%ecx\n\t"
                : "=m"(ret)
                : "r"(point)
                : "al");
            if (ret == 1)
                kprint("#primitive -- ");
        }
        char str[11];
        int2hex_str((uint32_t)point, str);
        kprint(str);
    }
}

void newline_impl_c()
{
    kprint("\n");
}
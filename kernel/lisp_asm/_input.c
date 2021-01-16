#include <stdint.h>
#include "_common.h"
#include "_input.h"
#include "../../libc/mem.h"
#include "../../libc/string.h"

// Locate the quotation mark expression. Returns the end position of the quotation mark expression.
// If there is a problem with the quotation mark expression format, it returns -1
static int locate_quote(char *str, size_t start, size_t len)
{
    size_t i = start + 1;
    if (start == len)
        return -1;
    if (str[i] == '(')
    { /* for '() */
        ++i;
        if (i == len || str[i] != ')')
            return -1;
        return ++i;
    }
    while (i < len && (is_alphabet(str[i]) || is_digital(str[i]) ||
                       str[i] == '!' || str[i] == '?' || str[i] == '=' ||
                       str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/'))
        ++i;
    return i;
}

// Locate string constants. Returns the end position of the string constant, and return -2 if the string is incomplete
static int locate_string(char *str, size_t start, size_t len)
{
    size_t i = start + 1;
    while (i < len && (str[i] != '"'))
        ++i;
    if (i == len)
        return -2;
    return ++i;
}

static uint8_t is_available_char(char c)
{
    return is_alphabet(c) || is_digital(c) ||
           c == '!' || c == '_' || c == '.' || c == '=' || c == '?' ||
           c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '<' || c == '>';
}

// Positioning symbols or numbers. Returns the end position of a symbol or number
static int locate_symbol_or_number(char *str, size_t start, size_t len)
{
    size_t i = start + 1;
    while (i < len && is_available_char(str[i]))
        ++i;
    return i;
}

static uint8_t is_quote_or_string_or_symbol_number_start(char c)
{
    if (c == '\'')
        return 1;
    if (c == '"')
        return 1;
    if (is_available_char(c))
        return 1;
    return 0;
}

static int get_end_next(char *str, size_t start, size_t len)
{
    size_t i = start;
    if (str[i] == '\'')
        return locate_quote(str, i, len);
    else if (str[i] == '"')
        return locate_string(str, i, len);
    else
        return locate_symbol_or_number(str, i, len);
}

int is_legal(char *str, size_t len)
{
    size_t i = 0;
    while (i < len && is_space(str[i]))
        ++i;
    if (i == len) /* remove the white space character to get the empty string */
        return 0;
    if (is_quote_or_string_or_symbol_number_start(str[i]))
    { /* start with ' or " or character */
        int ret = get_end_next(str, i, len);
        if (ret == -1)
            return -1;
        if (ret == -2)
            return 1;
        i = ret;
        while (i < len && is_space(str[i]))
            ++i;
        return i == len ? 0 : -1;
    }
    else if (str[i] == '(')
    { /* start with ( */
        size_t brackets_num = 1;
        ++i;
        while (i < len)
        {
            char c = str[i++];
            if (is_space(c))
                continue;
            else if (c == '(')
                ++brackets_num;
            else if (c == ')')
            {
                --brackets_num;
                if (brackets_num == 0)
                {
                    while (i < len && is_space(str[i]))
                        ++i;
                    return i == len ? 0 : -1;
                }
            }
            else if (is_quote_or_string_or_symbol_number_start(c))
            {
                int ret = get_end_next(str, i - 1, len);
                if (ret == -1)
                    return -1;
                if (ret == -2)
                    return 1;
                i = ret;
            }
            else /* unknown character */
                return -1;
        }
        return brackets_num == 0 ? 0 : 1;
    }
    else /* unknown character */
        return -1;
}

/* ===================================================================== */

static element_t construct_integer_element(int32_t val)
{
    element_t element;
    element.type = INTEGER_T;
    element.val.ival = val;
    return element;
}

static element_t construct_float_element(float val)
{
    element_t element;
    element.type = FLOAT_T;
    element.val.fval = val;
    return element;
}

static element_t construct_point_element(void *point)
{
    element_t element;
    element.type = PAIR_POINT_T;
    element.val.point = point;
    return element;
}

static element_t constructe_element(char *str, size_t curr, size_t end, uint8_t is_arrive_end)
{
    size_t i = curr;
    char *substr = str + i;
    i = get_end_next(str, i, end + 1);
    size_t substr_len = i - curr;

    if (is_integer(substr, substr_len))
    {
        int32_t num = str2int32(substr, substr_len);
        return construct_integer_element(num);
    }
    else if (is_float(substr, substr_len))
    {
        float num = str2float(substr, substr_len);
        return construct_float_element(num);
    }
    else
    {
        element_t element;
        element.type = STRING_T;
        if (is_arrive_end)
        { /* make the code as far as possible not to go to this branch */
            char *p = (char *)memory_malloc(substr_len + 1);
            memory_copy((uint8_t *)substr, (uint8_t *)p, substr_len);
            p[substr_len] = '\0';
            element.val.point = put_string_to_constant_pool(p);
            memory_free(p);
        }
        else
        {
            char tmp = substr[substr_len];
            substr[substr_len] = '\0';
            element.val.point = put_string_to_constant_pool(substr);
            substr[substr_len] = tmp;
        }
        return element;
    }
}

static void conn_pair(void *new_pair_p)
{
    void *ret_p = 0;
    asm volatile(
        "movl (%1), %%ecx\n\t"
        "call cadr\n\t"
        "movl %%ebx, %0\n\t"
        : "=m"(ret_p)
        : "r"(STACK_OFFSET)
        : "al", "ebx", "ecx");
    if (ret_p == 0)
    {
        asm volatile(
            "movl (%0), %%ecx\n\t"
            "call cdr\n\t"
            "movl %%ebx, %%ecx\n\t"
            "movb %1, %%al\n\t"
            "movl %2, %%ebx\n\t"
            "call set_car\n\t"
            "movl (%0), %%ecx\n\t"
            "movb %1, %%al\n\t"
            "movl %2, %%ebx\n\t"
            "call set_car\n\t"
            :
            : "r"(STACK_OFFSET), "i"(PAIR_POINT_T), "m"(new_pair_p)
            : "al", "ebx", "ecx");
        return;
    }
    else
    {
        asm volatile(
            "movl (%0), %%ecx\n\t"
            "call car\n\t"
            "movl %%ebx, %%ecx\n\t"
            "movb %1, %%al\n\t"
            "movl %2, %%ebx\n\t"
            "call set_cdr\n\t"
            "movl (%0), %%ecx\n\t"
            "movb %1, %%al\n\t"
            "movl %2, %%ebx\n\t"
            "call set_car\n\t"
            :
            : "r"(STACK_OFFSET), "i"(PAIR_POINT_T), "m"(new_pair_p)
            : "al", "ebx", "ecx");
        return;
    }
}

static void *cons_inner(element_t *car_ele, element_t *cdr_ele)
{
    uint32_t type1 = car_ele->type;
    void *data1 = &car_ele->val;
    uint32_t type2 = cdr_ele->type;
    void *data2 = &cdr_ele->val;
    void *ret = 0;
    asm volatile(
        "movl %1, %%eax\n\t"
        "movl (%2), %%ebx\n\t"
        "movl %3, %%ecx\n\t"
        "movl (%4), %%edx\n\t"
        "call cons\n\t"
        "movl %%eax, %0\n\t"
        : "=m"(ret)
        : "m"(type1), "r"(data1), "m"(type2), "r"(data2)
        : "eax", "ebx", "ecx", "edx");
    return ret;
}

static void *save_str_to_pair_helper(char *str, size_t start, size_t end)
{
    element_t end_element = construct_point_element(0);
    size_t i = start;
    while (i <= end && is_space(str[i]))
        ++i;
    if (i - 1 == end) /* remove the white space character to get the empty string */
        return 0;
    if (is_quote_or_string_or_symbol_number_start(str[i]))
    { /* start with ' or " or character */
        element_t element = constructe_element(str, i, end, i - 1 == end ? 1 : 0);
        return cons_inner(&element, &end_element);
    }
    else
    { /* start with ( */
        ++i;
        while (is_space(str[i]))
            ++i;
        size_t right_bracket_pos = end;
        while (str[right_bracket_pos] != ')')
            --right_bracket_pos;
        if (i == right_bracket_pos) /* for () or (     ) */
            return cons_inner(&end_element, &end_element);
        asm volatile(
            "movl $0x0, %%ecx\n\t"
            "call save_p\n\t"
            "call save_p\n\t"
            :
            :
            : "ecx");
        while (i < right_bracket_pos)
        {
            char c = str[i];
            if (c == '(')
            {
                /* find the right bracket that matches it */
                size_t new_start = i++;
                uint8_t left_bracket_num = 0;
                while (i < right_bracket_pos)
                {
                    char cc = str[i];
                    if (cc == '(')
                    {
                        ++left_bracket_num;
                        ++i;
                    }
                    else if (cc == ')')
                    {
                        if (left_bracket_num == 0)
                            break;
                        else
                            --left_bracket_num;
                        ++i;
                    }
                    else if (is_quote_or_string_or_symbol_number_start(cc))
                        i = get_end_next(str, i, end + 1);
                    else
                        ++i;
                }
                void *sub_ret = save_str_to_pair_helper(str, new_start, i);
                if (sub_ret == 0)
                    sub_ret = cons_inner(&end_element, &end_element); /* for () or (    ) */
                element_t element = construct_point_element(sub_ret);
                void *new_pair_p = cons_inner(&element, &end_element);
                conn_pair(new_pair_p);
                ++i;
            }
            else if (!is_space(c))
            {
                element_t element = constructe_element(str, i, end, 0);
                element = construct_point_element(cons_inner(&element, &end_element));
                void *new_pair_p = cons_inner(&element, &end_element);
                conn_pair(new_pair_p);
                i = get_end_next(str, i, end + 1);
            }
            else /* white space character */
                ++i;
        }
        void *ret = 0;
        asm volatile(
            "call restore_ecx\n\t"
            "call restore_ecx\n\t"
            "movl %%ecx, %0\n\t"
            : "=m"(ret)
            :
            : "ecx");
        return ret;
    }
}

void *save_str_to_pair(char *str, size_t len)
{
    return save_str_to_pair_helper(str, 0, len - 1);
}

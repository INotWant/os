#include "input.h"
#include "pair.h"
#include "stack.h"
#include "constant.h"
#include "../../libc/string.h"
#include "../../libc/mem.h"

// 定位引号表达式。返回引号表达式的结束位置，若判定引号表达式格式存在问题则返回 -1
static int locate_quote(char *str, size_t start, size_t len) {
    size_t i = start + 1;
    if (start == len)
        return -1;
    if (str[i] == '(') {    /* 针对 '() */
        ++i;
        if (i == len || str[i] != ')')
            return -1;
        return ++i;
    }
    while(i < len && (is_alphabet(str[i]) || is_digital(str[i]) ||
        str[i] == '!' || str[i] == '?' || str[i] == '=' ||
        str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/'))
        ++i;
    return i;
}

// 定位字符串常量。返回字符串常量的结束位置，若判定字符串不完整则返回 -2
static int locate_string(char *str, size_t start, size_t len) {
    size_t i = start + 1;
    while (i < len && (str[i] != '"'))
        ++i;
    if (i == len)
        return -2;
    return ++i;
}

static uint8_t is_available_char(char c) {
    return is_alphabet(c) || is_digital(c) ||
           c == '!' || c == '_' || c == '.' || c == '=' || c == '?' ||
           c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '<' || c == '>';
}

// 定位符号或数字。返回符号或数字的结束位置
static int locate_symbol_or_number(char *str, size_t start, size_t len) {
    size_t i = start + 1;
    while(i < len && is_available_char(str[i]))
        ++i;
    return i;
}

static uint8_t is_quote_or_string_or_symbol_number_start(char c) {
    if (c == '\'')
        return 1;
    if (c == '"')
        return 1;
    if (is_available_char(c))
        return 1;
    return 0;
}

static int get_end_next(char *str, size_t start, size_t len) {
    size_t i = start;
    if (str[i] == '\'')
        return locate_quote(str, i, len);
    else if (str[i] == '"')
        return locate_string(str, i, len);
    else
        return locate_symbol_or_number(str, i, len);
}

int is_legal(char *str, size_t len) {
    size_t i = 0;
    while (i < len && is_space(str[i]))
        ++i;
    if (i == len)   /* 去除空白字符，得到空串 */
        return 0;
    if (is_quote_or_string_or_symbol_number_start(str[i])) {   /* 以 引号 双引号 符号 开始 */
        int ret = get_end_next(str, i, len);
        if (ret == -1)
            return -1;
        if (ret == -2)
            return 1;
        i = ret;
        while (i < len && is_space(str[i]))
            ++i;
        return i == len ? 0 : -1;
    } else if (str[i] == '(') {                                 /* 以左括号开始 */
        size_t brackets_num = 1;
        ++i;
        while(i < len) {
            char c = str[i++];
            if (is_space(c))
                continue;
            else if (c == '(')
                ++brackets_num;
            else if (c == ')') {
                --brackets_num;
                if (brackets_num == 0) {
                    while(i < len && is_space(str[i]))      /* 涵盖了 < 0 的情况 */
                        ++i;
                    return i == len ? 0 : -1;
                }
            } else if (is_quote_or_string_or_symbol_number_start(c)) {
                int ret = get_end_next(str, i - 1, len);
                if (ret == -1)
                    return -1;
                if (ret == -2)
                    return 1;
                i = ret;
            } else                  /* 未知字符 */
                return -1;
        }
        return brackets_num == 0 ? 0 : 1;
    } else                          /* 未知字符 */
        return -1;
}

static element_t constructe_element(char *str, size_t curr, size_t end, uint8_t is_arrive_end) {
    size_t i = curr;
    char *substr = str + i;
    i = get_end_next(str, i, end + 1);
    size_t substr_len = i - curr;

    if (is_integer(substr, substr_len)) {       /* 整数 */
        int32_t num = str2int32(substr, substr_len);
        return construct_integer_element(num);
    } else if (is_float(substr, substr_len)) {  /* 浮点数 */
        float num = str2float(substr, substr_len);
        return construct_float_element(num);
    } else {                                    /* 字符串 */
        element_t element;
        element.type = STRING_T;
        if (is_arrive_end) {         /* 已到最后（使代码尽可能不走该分支） */
            char *p = (char *)memory_malloc(substr_len + 1);
            memory_copy((uint8_t *)substr, (uint8_t *)p, substr_len);
            p[substr_len] = '\0';
            element.val.point = put_string_to_constant_pool(p);
            memory_free(p);
        } else {
            char tmp = substr[substr_len];
            substr[substr_len] = '\0';
            element.val.point = put_string_to_constant_pool(substr);
            substr[substr_len] = tmp;
        }
        return element;
    }
}

static void conn_pair(void **ret_p, void **last_pair_pp, void *new_pair_p) {
    if (*ret_p == 0) {
        *ret_p = new_pair_p;
        /* [在建] 保存根指针 -- for GC of pair */
        if (new_pair_p != 0) {
            element_t ele = construct_point_element(new_pair_p);
            push(&ele);
        }
    } else {
        element_t conn_element = construct_point_element(new_pair_p);
        set_cdr(*last_pair_pp, &conn_element);
    }
    *last_pair_pp = new_pair_p;
}

static void *save_str_to_pair_helper(char *str, size_t start, size_t end) {
    element_t end_element = ZERO_POINT;
    size_t i = start;
    while(i <= end && is_space(str[i]))
        ++i;
    if (i - 1 == end)   /* 去除空白字符为空串 */
        return 0;       /* 表示什么都没有 */

    if (is_quote_or_string_or_symbol_number_start(str[i])) {   /* 以 引号 双引号 符号 开始 */
        element_t element = constructe_element(str, i, end, i - 1 == end ? 1 : 0);
        return cons(&element, &end_element);
    } else {                                            /* 以括号开始 */
        ++i;
        while(is_space(str[i]))
            ++i;
        size_t right_bracket_pos = end;
        while(str[right_bracket_pos] != ')')
            --right_bracket_pos;
        if (i == right_bracket_pos)     /* 针对 () or (     ) */
            return cons(&end_element, &end_element);    /* 表示空表 */

        void *ret = 0;
        void *last_pair_p = 0;
        while (i < right_bracket_pos) {
            char c = str[i];
            if (c == '(') {
                /* 寻找与它匹配的右括号 */
                size_t new_start = i++;
                uint8_t left_bracket_num = 0;
                while (i < right_bracket_pos) {
                    char cc = str[i];
                    if (cc == '(') {
                        ++left_bracket_num;
                        ++i;
                    } else if (cc == ')') {
                        if (left_bracket_num == 0)
                            break;
                        else
                            --left_bracket_num;
                        ++i;
                    } else if (is_quote_or_string_or_symbol_number_start(cc))
                        i = get_end_next(str, i, end + 1);
                    else
                        ++i;
                }
                void *sub_ret = save_str_to_pair_helper(str, new_start, i);
                if (sub_ret == 0) 
                    sub_ret = cons(&end_element, &end_element); /* 表示空表，() 或 (    ) 得此 */
                element_t element = construct_point_element(sub_ret);
                void *new_pair_p = cons(&element, &end_element);
                conn_pair(&ret, &last_pair_p, new_pair_p);
                ++i;
            } else if (!is_space(c)) {
                element_t element = constructe_element(str, i, end, 0);
                element = construct_point_element(cons(&element, &end_element));
                void *new_pair_p = cons(&element, &end_element);
                conn_pair(&ret, &last_pair_p, new_pair_p);
                i = get_end_next(str, i, end + 1);
            } else      /* 空白字符 */
                ++i;
        }
        /* [在建] 恢复 */
        if (ret != 0)
            pop();
        return ret;
    }
}

void *save_str_to_pair_no_update_exp(char *str, size_t len) {
    return save_str_to_pair_helper(str, 0, len - 1);
}

void *save_str_to_pair(char *str, size_t len) {
    /* 清楚 root 表中的 exp 指针 */
    update_exp_point(0);
    void *exp = save_str_to_pair_helper(str, 0, len - 1);
    /* 更新 root 表中的 exp 指针 -- for GC of pair */
    update_exp_point(exp);
    return exp;
}
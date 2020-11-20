#include "input.h"
#include "pair.h"
#include "../../libc/string.h"
#include "../../libc/mem.h"

int is_legal(char *str, size_t len) {
    size_t i = 0;
    while(i < len && isspace(str[i]))
        ++i;
    if (i == len)   /* 去除空白字符，得到空串 */
        return 0;

    if (str[i] != '(') {        /* 以非左括号开始 */
        uint8_t has_white_space = 0;
        while(i < len) {
            char c = str[i];
            if (c == '(' || c == ')')
                return -1;
            if (has_white_space && !isspace(c))
                return -1;
            if (isspace(c))
                has_white_space = 1;
            ++i;
        }
        return 0;
    }

    size_t brackets_num = 1;    /* 以左括号开始 */
    ++i;
    while(i < len) {
        char c = str[i++];
        if (isspace(c))
            continue;
        if (c == '(')
            ++brackets_num;
        else if(c == ')') {
            --brackets_num;
            if (brackets_num == 0) {
                while(i < len && isspace(str[i]))
                    ++i;
                return i == len ? 0 : -1;
            }
        }
    }
    return brackets_num == 0 ? 0 : 1;
}

static element_t constructe_element(char *substr, size_t substr_len, uint8_t is_arrive_end) {
    if (is_integer(substr, substr_len)) {      /* 整数 */
        int32_t num = str2int32(substr, substr_len);
        return construct_integer_element(num);
    } else if (is_float(substr, substr_len)) { /* 浮点数 */
        float num = str2float(substr, substr_len);
        return construct_float_element(num);
    } else {                                /* 字符串 */
        element_t element;
        if (substr_len + 1 <= 7) {  /* 字符串放于序对中 */
            element.type = STRING_SHORT_T;
            memory_copy((uint8_t *)substr, (uint8_t *)element.val.short_string, substr_len);
            element.val.short_string[substr_len] = '\0';
        } else {                    /* 字符串放于字符串常量池 */
            element.type = STRING_LONG_T;
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
        }
        return element;
    }
}

static void conn_pair(void **ret_p, void **last_pair_pp, void *new_pair_p) {
    if (*ret_p == 0) {
        *ret_p = new_pair_p;
    } else {
        element_t conn_element = construct_point_element(new_pair_p);
        set_cdr(*last_pair_pp, &conn_element);
    }
    *last_pair_pp = new_pair_p;
}

static void *save_str_to_pair_helper(char *str, size_t start, size_t end) {
    size_t i = start;
    while(i <= end && isspace(str[i]))
        ++i;
    if (i - 1 == end)   /* 去除空白字符为空串 */
        return 0;       /* 表空表 */

    element_t end_element = construct_point_element(0);
    if (str[i] != '(') {    /* 字母 or 数字 */
        size_t substr_len = 0;
        char *substr = str + i;
        while (i <= end && !isspace(str[i])) {
            ++substr_len;
            ++i;
        }
        element_t element = constructe_element(substr, substr_len, i - 1 == end ? 1 : 0);
        return cons(&element, &end_element);
    } else {                /* 以左括号开始 */
        ++i;
        size_t right_bracket_pos = end;
        while(str[right_bracket_pos] != ')')
            --right_bracket_pos;
        
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
                    if (cc == '(')
                        ++left_bracket_num;
                    else if (cc == ')') {
                        if (left_bracket_num == 0)
                            break;
                        else
                            --left_bracket_num;
                    }
                    ++i;
                }
                element_t element = construct_point_element(save_str_to_pair_helper(str, new_start, i));
                void *new_pair_p = cons(&element, &end_element);
                conn_pair(&ret, &last_pair_p, new_pair_p);
                ++i;
            } else if (!isspace(c)) {
                size_t substr_len = 0;
                char *substr = str + i;
                while (!isspace(str[i]) && str[i] != ')' && str[i] != '(') {
                    ++substr_len;
                    ++i;
                }
                element_t element = constructe_element(substr, substr_len, 0);
                void *new_pair_p = cons(&element, &end_element);
                conn_pair(&ret, &last_pair_p, new_pair_p);
            } else  /* 空白字符 */
                ++i;
        }
        return ret;     /* () or (    ) 等情况，应得到空表 */
    }
}

void *save_str_to_pair(char *str, size_t len) {
    return save_str_to_pair_helper(str, 0, len - 1);
}
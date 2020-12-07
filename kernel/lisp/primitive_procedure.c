#include "primitive_procedure.h"
#include "procedure.h"
#include "eval.h"
#include "constant.h"
#include "input.h"
#include "lisp.h"
#include "../../drivers/screen.h"
#include "../../drivers/keyboard.h"
#include "../../libc/string.h"
#include "../../libc/mem.h"
#include "../../libc/utils.h"

static uint8_t is_number(element_t *ep) {
    uint8_t type = ep->type;
    return type == INTEGER_T || type == FLOAT_T;
}

static float get_number(element_t *ep) {
    if (ep->type == INTEGER_T)
        return (float)ep->val.ival;
    else if (ep->type == FLOAT_T)
        return ep->val.fval;
    return 0.0f;
}

// 获取参数的个数
static uint8_t get_args_number(void *args) {
    uint8_t ret = 0;
    void *p = args;
    while(p != 0) {
        ++ret;
        if (cdr(p).type == POINT_PAIR_T)
            p = cdr(p).val.point;
        else
            break;
    }
    return ret;
}

// '+'
static element_t add_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_ADD);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (!is_number(&ele1) || !is_number(&ele2))
        eval_error_handler(UT_ADD);
    if (ele1.type == INTEGER_T && ele2.type == INTEGER_T)
        return construct_integer_element(ele1.val.ival + ele2.val.ival);
    float sum = get_number(&ele1);
    sum += get_number(&ele2);
    return construct_float_element(sum);
}

// '-'
static element_t sub_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_SUB);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (!is_number(&ele1) || !is_number(&ele2))
        eval_error_handler(UT_SUB);
    if (ele1.type == INTEGER_T && ele2.type == INTEGER_T)
        return construct_integer_element(ele1.val.ival - ele2.val.ival);
    float ret = get_number(&ele1);
    ret -= get_number(&ele2);
    return construct_float_element(ret);
}

// '*'
static element_t mul_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_MUL);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (!is_number(&ele1) || !is_number(&ele2))
        eval_error_handler(UT_MUL);
    if (ele1.type == INTEGER_T && ele2.type == INTEGER_T)
        return construct_integer_element(ele1.val.ival * ele2.val.ival);
    float ret = get_number(&ele1);
    ret *= get_number(&ele2);
    return construct_float_element(ret);
}

// '/'
static element_t div_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_DIV);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (!is_number(&ele1) || !is_number(&ele2))
        eval_error_handler(UT_DIV);
    if (ele1.type == INTEGER_T && ele2.type == INTEGER_T)
        return construct_integer_element(ele1.val.ival / ele2.val.ival);
    float ret = get_number(&ele1);
    ret /= get_number(&ele2);
    return construct_float_element(ret);
}

// '%'
static element_t rem_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_REM);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (ele1.type != INTEGER_T || ele2.type != INTEGER_T)
        eval_error_handler(UT_REM);
    return construct_integer_element(ele1.val.ival % ele2.val.ival);
}

// 'cons'
static element_t cons_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_CONS);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    return construct_point_element(cons(&ele1, &ele2));
}

// 'car'
static element_t car_impl(void *args) {
    if (get_args_number(args) != 1)
        eval_error_handler(WNP_CAR);
    element_t ele = car(args);
    if (ele.type != POINT_PAIR_T)
        eval_error_handler(UT_CAR);
    return car(ele.val.point);
}

// set-car!
static element_t set_car_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_SET_CAR);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (ele1.type != POINT_PAIR_T || ele1.val.point == 0)
        eval_error_handler(UT_SET_CAR);
    set_car(ele1.val.point, &ele2);
    return OK;
}

// 'cdr'
static element_t cdr_impl(void *args) {
    if (get_args_number(args) != 1)
        eval_error_handler(WNP_CDR);
    element_t ele = car(args);
    if (ele.type != POINT_PAIR_T)
        eval_error_handler(UT_CDR);
    return cdr(ele.val.point);
}

// set-cdr!
static element_t set_cdr_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_SET_CDR);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (ele1.type != POINT_PAIR_T || ele1.val.point == 0)
        eval_error_handler(UT_SET_CDR);
    set_cdr(ele1.val.point, &ele2);
    return OK;
}

// 'null?'
static element_t is_null_impl(void *args) {
    if (get_args_number(args) != 1)
        eval_error_handler(WNP_IS_NULL);
    element_t ele = car(args);
    if (ele.type != POINT_PAIR_T)
        eval_error_handler(UT_IS_NULL);
    if (ele.val.point == 0)
        return TRUE;
    else
        return FALSE;
}

// 'display'
static element_t display_impl(void *args) {
    if (get_args_number(args) != 1)
        eval_error_handler(WNP_DISPLAY);
    print_element(car(args));
    return ZERO_POINT;  /* 代表无返回结果 */
}

// '<' 实现
static element_t lt_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_LT);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (!is_number(&ele1) || !is_number(&ele2))
        eval_error_handler(UT_LT);
    if (ele1.type == INTEGER_T && ele2.type == INTEGER_T)
        return ele1.val.ival < ele2.val.ival ? TRUE : FALSE;
    float v1, v2;
    if (ele1.type == INTEGER_T)
        v1 = (float) ele1.val.ival;
    else
        v1 = ele1.val.fval;
    if (ele2.type == INTEGER_T)
        v2 = (float) ele2.val.ival;
    else
        v2 = ele2.val.fval;
    return v1 < v2 ? TRUE : FALSE;
}

// '>' 实现
static element_t gt_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_GT);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (!is_number(&ele1) || !is_number(&ele2))
        eval_error_handler(UT_GT);
    if (ele1.type == INTEGER_T && ele2.type == INTEGER_T)
        return ele1.val.ival > ele2.val.ival ? TRUE : FALSE;
    float v1, v2;
    if (ele1.type == INTEGER_T)
        v1 = (float) ele1.val.ival;
    else
        v1 = ele1.val.fval;
    if (ele2.type == INTEGER_T)
        v2 = (float) ele2.val.ival;
    else
        v2 = ele2.val.fval;
    return v1 > v2 ? TRUE : FALSE;
}

// '=' 实现
static element_t eq_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_EQ);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (ele1.type == POINT_PAIR_T && ele2.type == POINT_PAIR_T)
        return ele1.val.point == ele2.val.point ? TRUE : FALSE;
    if (!is_number(&ele1) || !is_number(&ele2))
        eval_error_handler(UT_EQ);
    if (ele1.type == INTEGER_T && ele2.type == INTEGER_T)
        return ele1.val.ival == ele2.val.ival ? TRUE : FALSE;
    float v1, v2;
    if (ele1.type == INTEGER_T)
        v1 = (float) ele1.val.ival;
    else
        v1 = ele1.val.fval;
    if (ele2.type == INTEGER_T)
        v2 = (float) ele2.val.ival;
    else
        v2 = ele2.val.fval;
    return v1 == v2 ? TRUE : FALSE;
}

// '!=' 实现
static element_t ne_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_NE);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (ele1.type == POINT_PAIR_T && ele2.type == POINT_PAIR_T)
        return ele1.val.point != ele2.val.point ? TRUE : FALSE;
    if (!is_number(&ele1) || !is_number(&ele2))
        eval_error_handler(UT_NE);
    element_t eq_ret_ele = eq_impl(args);
    char *p = (char *)eq_ret_ele.val.point;
    if (strcmp(p, "true") == 0)
        return FALSE;
    return TRUE;
}

// '<=' 实现
static element_t le_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_LE);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (!is_number(&ele1) || !is_number(&ele2))
        eval_error_handler(UT_LE);
    element_t lt_ret_ele = lt_impl(args);
    char *p = (char *)lt_ret_ele.val.point;
    if (strcmp(p, "true") == 0)
        return TRUE;
    return eq_impl(args);
}

// '>=' 实现
static element_t ge_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_GE);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (!is_number(&ele1) || !is_number(&ele2))
        eval_error_handler(UT_GE);
    element_t gt_ret_ele = gt_impl(args);
    char *p = (char *)gt_ret_ele.val.point;
    if (strcmp(p, "true") == 0)
        return TRUE;
    return eq_impl(args);
}

static char *exp_str;
static size_t exp_len = 0;
static int flag;

static void input_handler_for_read(char *input) {
    int len = input_len();
    if (len == -1) {
        flag = -2;
    } else if (len == 0) {
        if (exp_len != 0)
            exp_str[exp_len++] = '\n';
    } else {
        exp_len += len;
        if (exp_len >= EXP_MAX_LEN) {
            flag = -3;
        } else {
            memory_copy((uint8_t *)input, (uint8_t *)(exp_str + (exp_len - len)), len);
            flag = is_legal(exp_str, exp_len);
        }
    }
}

static element_t read_impl(void *args) {
    if (get_args_number(args) != 0)
        eval_error_handler(WNP_READ);
    exp_len = 0;
    exp_str = (char *)memory_malloc(EXP_MAX_LEN);
    input_handler_fp default_input_handler = input_handler;
    input_handler = input_handler_for_read;
    clear_key_buffer();
    flag = 1;

    while (flag == 1) {
        asm volatile("sti");
        asm volatile("hlt");
    }

    input_handler = default_input_handler;
    if (flag == 0) {
        element_t ele = construct_point_element(save_str_to_pair(exp_str, exp_len));
        memory_free(exp_str);
        return ele;
    }

    memory_free(exp_str);
    if (flag == -1)
        eval_error_handler(ERR_ENL_READ);
    else if (flag == -2)
        eval_error_handler(ERR_TMCL_READ);
    else if (flag == -3)
        eval_error_handler(ERR_TMCE_READ);
}

// number?
static element_t is_number_impl(void *args) {
    if (get_args_number(args) != 1)
        eval_error_handler(WNP_IS_NUMBER);
    element_t ele = car(args);
    if (ele.type == INTEGER_T || ele.type == FLOAT_T)
        return TRUE;
    return FALSE;
}

// string?
static element_t is_string_impl(void *args) {
    if (get_args_number(args) != 1)
        eval_error_handler(WNP_IS_STRING);
    element_t ele = car(args);
    if (ele.type == STRING_T && *(char *)(ele.val.point) == '"')
        return TRUE;
    return FALSE;
}

// symbol?
static element_t is_symbol_impl(void *args) {
    if (get_args_number(args) != 1)
        eval_error_handler(WNP_IS_SYMBOL);
    element_t ele = car(args);
    if (ele.type == STRING_T) {
        char c = *(char *)(ele.val.point);
        if (c != '"' && c != '\'')
            return TRUE;
    }
    return FALSE;
}

// quoted?
static element_t is_quoted_impl(void *args) {
    if (get_args_number(args) != 1)
        eval_error_handler(WNP_IS_QUOTED);
    element_t ele = car(args);
    if (ele.type == STRING_T) {
        char c = *(char *)(ele.val.point);
        if (c == '\'')
            return TRUE;
    }
    return FALSE;
}

static element_t quotation_text_impl(void *args) {
    if (get_args_number(args) != 1)
        eval_error_handler(WNP_QUOTATION_TEXT);
    element_t ele = car(args);
    if (ele.type != STRING_T || *((char *)ele.val.point) != '\'')
        eval_error_handler(UT_QUOTATION_TEXT);
    char *p = (char *)ele.val.point + 1;
    element_t ret_ele;
    ret_ele.val.point = p;
    int i = 0;
    if (p[i] == '(' && p[i + 1] == ')')     /* '() 表空表 */
        return ZERO_POINT;
    return ret_ele;
}

// pair?
static element_t is_pair_impl(void *args) {
    if (get_args_number(args) != 1)
        eval_error_handler(WNP_IS_PAIR);
    element_t ele = car(args);
    if (ele.type == POINT_PAIR_T)
        return TRUE;
    return FALSE;
}

// eq?
static element_t is_eq_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_IS_EQ);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (ele1.type != ele2.type)
        return FALSE;
    if (ele1.type == STRING_T)
        return strcmp((char *)ele1.val.point, (char *)ele2.val.point) == 0 ? TRUE : FALSE;
    return eq_impl(args);
}

// apply -- 注：其第二个参数应为一个表，表示参数
static element_t apply_impl(void *args) {
    if (get_args_number(args) != 2)
        eval_error_handler(WNP_APPLY);
    element_t ele1 = car(args);
    element_t ele2 = cadr(args);
    if (ele1.type != POINT_PAIR_T || ele1.val.point == 0 || ele2.type != POINT_PAIR_T)
        eval_error_handler(UT_APPLY);
    void *procedure = ele1.val.point;
    if (!is_primitive_procedure(procedure))
        eval_error_handler(UT_APPLY);
    return primitive_procedure_impl(procedure)(ele2.val.point);
}

// newline
static element_t newline_impl(void *args) {
    kprint("\n");
    UNUSED(args);
    return ZERO_POINT;  /* 代表无返回结果 */
}

void *primitive_procedure_names() {
    element_t ele1 = construct_string_element("+");
    element_t ele2 = construct_string_element("-");
    element_t ele3 = construct_string_element("*");
    element_t ele4 = construct_string_element("/");
    element_t ele5 = construct_string_element("%");
    element_t ele6 = construct_string_element("cons");
    element_t ele7 = construct_string_element("car");
    element_t ele8 = construct_string_element("cdr");
    element_t ele9 = construct_string_element("null?");
    element_t ele10 = construct_string_element("display");
    element_t ele11 = construct_string_element("<");
    element_t ele12 = construct_string_element(">");
    element_t ele13 = construct_string_element("=");
    element_t ele14 = construct_string_element("!=");
    element_t ele15 = construct_string_element("<=");
    element_t ele16 = construct_string_element(">=");
    element_t ele17 = construct_string_element("read");
    element_t ele18 = construct_string_element("number?");
    element_t ele19 = construct_string_element("string?");
    element_t ele20 = construct_string_element("symbol?");
    element_t ele21 = construct_string_element("quoted?");
    element_t ele22 = construct_string_element("quotation-text");   /* 由于涉及引号表达式的底层存储，所以提供此基础操作 */
    element_t ele23 = construct_string_element("pair?");
    element_t ele24 = construct_string_element("eq?");              /* 先判类型是否一致，若不一致直接 FALSE ，再去判是否相等 */
    element_t ele25 = construct_string_element("set-car!");
    element_t ele26 = construct_string_element("set-cdr!");
    element_t ele27 = construct_string_element("apply");
    element_t ele28 = construct_string_element("newline");
    element_t elements[] = {ele1, ele2, ele3, ele4, ele5, ele6, ele7, ele8, ele9, ele10, ele11, ele12, ele13, ele14,
                            ele15, ele16, ele17, ele18, ele19, ele20, ele21, ele22, ele23, ele24, ele25, ele26, ele27, ele28};
    /* 注：此处 list 第二参数已超 18。但因在初始化时运行故认为运行期间不会引起 GC */
    return list(28, elements);
}

void *primitive_procedure_objects() {
    element_t ele1 = construct_point_element(make_primitive_procedure((void *)add_impl));
    element_t ele2 = construct_point_element(make_primitive_procedure((void *)sub_impl));
    element_t ele3 = construct_point_element(make_primitive_procedure((void *)mul_impl));
    element_t ele4 = construct_point_element(make_primitive_procedure((void *)div_impl));
    element_t ele5 = construct_point_element(make_primitive_procedure((void *)rem_impl));
    element_t ele6 = construct_point_element(make_primitive_procedure((void *)cons_impl));
    element_t ele7 = construct_point_element(make_primitive_procedure((void *)car_impl));
    element_t ele8 = construct_point_element(make_primitive_procedure((void *)cdr_impl));
    element_t ele9 = construct_point_element(make_primitive_procedure((void *)is_null_impl));
    element_t ele10 = construct_point_element(make_primitive_procedure((void *)display_impl));
    element_t ele11 = construct_point_element(make_primitive_procedure((void *)lt_impl));
    element_t ele12 = construct_point_element(make_primitive_procedure((void *)gt_impl));
    element_t ele13 = construct_point_element(make_primitive_procedure((void *)eq_impl));
    element_t ele14 = construct_point_element(make_primitive_procedure((void *)ne_impl));
    element_t ele15 = construct_point_element(make_primitive_procedure((void *)le_impl));
    element_t ele16 = construct_point_element(make_primitive_procedure((void *)ge_impl));
    element_t ele17 = construct_point_element(make_primitive_procedure((void *)read_impl));
    element_t ele18 = construct_point_element(make_primitive_procedure((void *)is_number_impl));
    element_t ele19 = construct_point_element(make_primitive_procedure((void *)is_string_impl));
    element_t ele20 = construct_point_element(make_primitive_procedure((void *)is_symbol_impl));
    element_t ele21 = construct_point_element(make_primitive_procedure((void *)is_quoted_impl));
    element_t ele22 = construct_point_element(make_primitive_procedure((void *)quotation_text_impl));
    element_t ele23 = construct_point_element(make_primitive_procedure((void *)is_pair_impl));
    element_t ele24 = construct_point_element(make_primitive_procedure((void *)is_eq_impl));
    element_t ele25 = construct_point_element(make_primitive_procedure((void *)set_car_impl));
    element_t ele26 = construct_point_element(make_primitive_procedure((void *)set_cdr_impl));
    element_t ele27 = construct_point_element(make_primitive_procedure((void *)apply_impl));
    element_t ele28 = construct_point_element(make_primitive_procedure((void *)newline_impl));
    element_t elements[] = {ele1, ele2, ele3, ele4, ele5, ele6, ele7, ele8, ele9, ele10, ele11, ele12, ele13, ele14,
                            ele15, ele16, ele17, ele18, ele19, ele20, ele21, ele22, ele23, ele24, ele25, ele26, ele27, ele28};
    /* 注：此处 list 第二参数已超 18；而且此函数中存在 [在建] 指针。但因在初始化时运行故认为运行期间不会引起 GC */
    return list(28, elements);
}

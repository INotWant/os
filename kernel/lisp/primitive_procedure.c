#include "primitive_procedure.h"
#include "procedure.h"
#include "eval.h"
#include "constant.h"
#include "../../drivers/screen.h"
#include "../../libc/string.h"
#include "../../libc/mem.h"

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

// 'cdr'
static element_t cdr_impl(void *args) {
    if (get_args_number(args) != 1)
        eval_error_handler(WNP_CDR);
    element_t ele = car(args);
    if (ele.type != POINT_PAIR_T)
        eval_error_handler(UT_CDR);
    return cdr(ele.val.point);
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
    element_t ele = car(args);
    if (ele.type == STRING_T)
        kprint(ele.val.point);
    else if (ele.type == INTEGER_T){
        char *tp = (char *)memory_malloc(12);
        int2str(ele.val.ival, tp);
        kprint(tp);
        memory_free(tp);
    } else if (ele.type == FLOAT_T) {
        char *tp = (char *)memory_malloc(20);
        float2str(ele.val.fval, tp, 6);
        kprint(tp);
        memory_free(tp);
    } else if (ele.type == POINT_PAIR_T) {
        char *tp = (char *)memory_malloc(11);
        int2hex_str((uint32_t)ele.val.point, tp);
        kprint(tp);
        memory_free(tp);
    }
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
    element_t elements[] = {ele1, ele2, ele3, ele4, ele5, ele6, ele7, ele8, ele9, ele10, ele11, ele12, ele13, ele14, ele15, ele16};
    return list(16, elements);
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
    element_t elements[] = {ele1, ele2, ele3, ele4, ele5, ele6, ele7, ele8, ele9, ele10, ele11, ele12, ele13, ele14, ele15, ele16};
    return list(16, elements);
}

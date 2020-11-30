#include "expression.h"
#include "constant.h"
#include "../../libc/mem.h"
#include "../../libc/string.h"

static uint8_t is_number(element_t *ep) {
    return ep->type == INTEGER_T || ep->type == FLOAT_T;
}

/**
 * 注：在使用时字符串被分为三种类型：1）字符串常量 2）引号表达式 3）符号
 *    以下三个函数用于判断具体是哪种类型。
 */

// 字符串常量以 双引号 开头
static uint8_t is_string(element_t *ep) {
    if (ep->type == STRING_T)
        return *(char *)(ep->val.point) == '\"' ? 1 : 0;
    return 0;
}

// 引号表达式以 引号 开头
static uint8_t is_quote(element_t *ep) {
    if (ep->type == STRING_T)
        return *(char *)ep->val.point == '\'' ? 1 : 0;
    return 0;
}

// 除字符串常量、引号表达式外的字符串为 符号
static uint8_t is_symbol(element_t *ep) {
    if (is_string(ep) || is_quote(ep))
        return 0;
    uint8_t type = ep->type;
    return type == STRING_T;
}

uint8_t is_self_evaluating(void *exp) {
    element_t element = car(exp);    
    return is_number(&element) || is_string(&element);
}

uint8_t is_variable(void *exp) {
    element_t element = car(exp);
    return is_symbol(&element);
}

uint8_t is_quoted(void *exp) {
    element_t element = car(exp);
    return is_quote(&element);
}

element_t text_of_quotation(void *exp) {
    element_t ret_ele;
    element_t ele = car(exp);
    ret_ele.type = STRING_T;
    char *p = (char *)ele.val.point + 1;
    ret_ele.val.point = p;
    int i = 0;
    if (p[i] == '(' && p[i + 1] == ')')     /* '() 表空表 */
        return ZERO_POINT;
    return ret_ele;
}

static uint8_t tagged(void *exp, char *str) {
    element_t element = car(exp);
    if(element.type == POINT_PAIR_T) {
        void *pp = element.val.point;
        element = car(pp);
        if (element.type == STRING_T)
            return strcmp(element.val.point, str) == 0;
    }
    return 0;
}

uint8_t is_assignment(void *exp) {
    return tagged(exp, "set!");
}

element_t assignment_variable(void *exp) {
    return caadr(exp);
}

void *assignment_value(void *exp) {
    return caddr(exp).val.point;
}

uint8_t is_definition(void *exp) {
    return tagged(exp, "define");
}

element_t definition_variable(void *exp) {
    element_t element = caadr(exp);
    if (element.type == STRING_T)
        return element;
    else
        return car(element.val.point);
}

void *definition_value(void *exp) {
    element_t element = caadr(exp);
    if (element.type == STRING_T) 
        return caddr(exp).val.point;
    else {
        void *parameters = cdadr(exp).val.point;
        void *body = cddr(exp).val.point;
        return make_lambda(parameters, body);
    }
}

uint8_t is_lambda(void *exp) {
    return tagged(exp, "lambda");
}

// 扒皮 -- 之前的输入中一个基本元素是由一个序对表示
static void *lambda_parameters_helper(void *parameters) {
    if (parameters == 0)
        return 0;
    element_t car_ele = caar(parameters);
    element_t cdr_ele = construct_point_element(lambda_parameters_helper(cdr(parameters).val.point));
    return cons(&car_ele, &cdr_ele);
}

void *lambda_parameters(void *exp) {
    return lambda_parameters_helper(cadr(exp).val.point);
}

void *lambda_body(void *exp) {
    return cddr(exp).val.point;
}

void *make_lambda(void *parameters, void *body) {
    element_t ele1 = LAMBDA;
    element_t end_ele = ZERO_POINT;
    ele1 = construct_point_element(cons(&ele1, &end_ele));
    element_t ele2 = construct_point_element(parameters);
    element_t ele3 = construct_point_element(body);
    element_t elements[] = {ele1, ele2};
    void *ret = list(2, elements); /* 注：此处参数 body 可从 exp 到达，故不用担心它被误 GC */
    set_cdr(cdr(ret).val.point, &ele3);
    return ret;
}

uint8_t is_if(void *exp) {
    return tagged(exp, "if");
}

void *if_predicate(void *exp) {
    return cadr(exp).val.point;
}

void *if_consequent(void *exp) {
    return caddr(exp).val.point;
}

void *if_alternative(void *exp) {
    void *pp = cdddr(exp).val.point;
    if (pp != 0)
        return car(pp).val.point;
    else {
        element_t ele1 = QUOTE_FALSE;
        element_t ele2 = ZERO_POINT;
        return cons(&ele1, &ele2);
    }
}

void *make_if(void *predicate, void *consequent, void *alternative) {
    element_t ele1 = IF;
    element_t end_ele = ZERO_POINT;
    ele1 = construct_point_element(cons(&ele1, &end_ele));
    element_t ele2 = construct_point_element(predicate);
    element_t ele3 = construct_point_element(consequent);
    element_t ele4 = construct_point_element(alternative);
    element_t elements[] = {ele1, ele2, ele3, ele4};
    return list(4, elements);
}

uint8_t is_begin(void *exp) {
    return tagged(exp, "begin");
}

void *begin_actions(void *exp) {
    return cdr(exp).val.point;
}

uint8_t is_last_exp(void *seq) {
    return cdr(seq).val.point == 0;
}

void* first_exp(void *seq) {
    return car(seq).val.point;
}

void *rest_exps(void *seq) {
    return cdr(seq).val.point;
}

void *make_begin(void *seq) {
    element_t ele1 = BEGIN;
    element_t end = ZERO_POINT;
    ele1 = construct_point_element(cons(&ele1, &end));
    element_t ele2 = construct_point_element(seq);
    return cons(&ele1, &ele2);
}

uint8_t is_application(void *exp) {
    element_t ele = car(exp);
    return ele.type == POINT_PAIR_T && ele.val.point != 0;
}

void *operator(void *exp) {
    return car(exp).val.point;
}

void *operands(void *exp) {
    return cdr(exp).val.point;
}

uint8_t is_no_operands(void *ops) {
    return ops == 0;
}

void* first_operand(void *ops) {
    return car(ops).val.point;
}

void *rest_operands(void *ops) {
    return cdr(ops).val.point;
}

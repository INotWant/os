#include "eval.h"
#include "expression.h"
#include "env.h"
#include "pair.h"
#include "procedure.h"
#include "apply.h"
#include "constant.h"
#include "../kernel.h"
#include "../../drivers/screen.h"
#include "../../drivers/keyboard.h"
#include "../../libc/string.h"

static element_t eval_assignment(void *exp, void *env) {
    char *var_name = assignment_variable(exp).val.point;
    element_t value_ele = eval(assignment_value(exp), env);
    set_var_value(var_name, &value_ele, env);
    return OK;
}

static element_t eval_definition(void *exp, void *env) {
    char *var_name = definition_variable(exp).val.point;
    element_t value_ele = eval(definition_value(exp), env);
    define_var(var_name, &value_ele, env);
    return OK;
}

static element_t eval_if(void *exp, void *env) {
    element_t ret_ele = eval(if_predicate(exp), env);
    if (ret_ele.type == STRING_T)
        if (strcmp(ret_ele.val.point, "true") == 0)
            return eval(if_consequent(exp), env);
    return eval(if_alternative(exp), env); /* 只要返回的结果不是 true 就执行这一分支 */
}

element_t eval_sequence(void *exps, void *env) {
    if (is_last_exp(exps))
        return eval(first_exp(exps), env);
    else {
        eval(first_exp(exps), env);
        return eval_sequence(rest_exps(exps), env);
    }
}

static void *eval_real_parameters(void *ops, void *env) {
    if (is_no_operands(ops))
        return 0;   /* 表空表 */
    element_t car_ele = eval(first_exp(ops), env);
    element_t cdr_ele = construct_point_element(eval_real_parameters(rest_exps(ops), env));
    return cons(&car_ele, &cdr_ele);
}

element_t eval(void *exp, void *env) {
    if (is_self_evaluating(exp))
        return car(exp);
    else if (is_variable(exp)) {
        element_t ret_ele = lookup_variable_value((char *)car(exp).val.point, env);
        if (ret_ele.type == NON_EXIST_T)
            eval_error_handler(VAR_NON_EXIST);
        return ret_ele;
    } else if (is_quoted(exp))
        return text_of_quotation(exp);
    else if (is_assignment(exp))
        return eval_assignment(exp, env);
    else if (is_definition(exp))
        return eval_definition(exp, env);
    else if (is_if(exp))
        return eval_if(exp, env);
    else if (is_lambda(exp)) 
        return construct_point_element(make_procedure(lambda_parameters(exp), lambda_body(exp), env));
    else if (is_begin(exp))
        return eval_sequence(begin_actions(exp), env);
    else if (is_application(exp)){
        element_t op_ele = eval(operator(exp), env);
        if (op_ele.type != POINT_PAIR_T || op_ele.val.point == 0)
            eval_error_handler(PROC_NON_EXIST);
        void *procedure = op_ele.val.point;
        void *real_parameters = eval_real_parameters(operands(exp), env);
        return apply(procedure, real_parameters);
    } else 
        eval_error_handler(UNKNOWN_EXP_TYPE);
}

void eval_error_handler(element_t reasion) {
    /* 打印错误原因 */
    print_element(reasion);
    kprint("\n> ");
    /* 清空 exp 键盘输入缓存 */
    clear_exp();
    clear_key_buffer();
    /* 恢复 root 表，无需管理 env ，其将在 lisp_exec 中恢复 */
    update_exp_point(0);
    update_stack_top_point(0);
    /* 恢复 ebp esp esi edi */
    asm volatile("movl %0, %%ebp" ::"r"(EBP_CONSTANT));
    asm volatile("movl %0, %%esp" ::"r"(ESP_CONSTANT));
    asm volatile("movl $0x0, %esi");
    asm volatile("movl $0x0, %edi");
    /* 恢复中断，跳转 */
    asm volatile("sti");
    asm volatile("jmp _loop");
}
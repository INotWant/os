#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "stdint.h"
#include "pair.h"

/**
 * 表达式的表示（把表示抽象出来，使表达式的语法和语义实现相隔离）
 * 
 * 注：以下 void * 指针指的是序对
 */

/**
 * 判断是否是自求值表达式（数值 or 字符串）
 */
uint8_t is_self_evaluating(void *exp);

/**
 * 判断是否是变量
 */
uint8_t is_variable(void *exp);

/**
 * 判断是否是引号表达式
 * 注：其中 '() 表空表
 * 
 * 引号表达式：’xxx
 */
uint8_t is_quoted(void *exp);

/**
 * 获取引号表达式中的文本
 */
element_t text_of_quotation(void *exp);

/**
 * 判断是否是赋值表达式
 * 
 * 赋值表达式：(set! <var> <value>)
 */
uint8_t is_assignment(void *exp);

/**
 * 获取赋值表达式中的变量名
 */
element_t assignment_variable(void *exp);

/**
 * 获取赋值表达式中的值
 */
void *assignment_value(void *exp);

/**
 * 判断是否是定义表达式
 * 
 * 定义表达式：
 *   1) (define <var> <value>)
 *   2) (define (<var> <parameter_1> ... <parameter_n>) <body>)
 */
uint8_t is_definition(void *exp);

/**
 * 获取定义表达式的变量名
 */
element_t definition_variable(void *exp);

/**
 * 获取定义表达式的值
 */
void *definition_value(void *exp);

/**
 * 判断是否是 lambda 表达式
 * 
 * lambda: (lambda (<parameter_1> ... <parameter_n>) <body>)
 */
uint8_t is_lambda(void *exp);

/**
 * 获取 lambda 表达式的参数
 */
void *lambda_parameters(void *exp);

/**
 * 获取 lambda 表达式的体
 */
void *lambda_body(void *exp);

/**
 * 由 参数 和 体 构造 lambda 表达式
 */
void *make_lambda(void *parameters, void *body);

/**
 * 判断是否是 if 表达式
 * 
 * if 表达式：(if predicate consequent <alternative>)
 */
uint8_t is_if(void *exp);

/**
 * 获取 if 表达式的谓词
 */
void *if_predicate(void *exp);

/**
 * 获取 if 表达式的推论
 */
void *if_consequent(void *exp);

/**
 * 获取 if 表达式的替代
 * 注：若空缺，使用 'false 替代
 */
void *if_alternative(void *exp);

/**
 * 由 谓词 推论 替代 构建 if 表达式
 */
void *make_if(void *predicate, void *consequent, void *alternative);

/**
 * 判断是否是 begin 表达式
 * 
 * begin 表达式：(begin action_1 ... <action_n>)
 */
uint8_t is_begin(void *exp);

/**
 * 获取 begin 表达式的表达式序列
 */
void *begin_actions(void *exp);

/**
 * 判断是否是表达式序列的最后一个
 */
uint8_t is_last_exp(void *seq);

/**
 * 获取表达式序列的第一个表达式
 */
void* first_exp(void *seq);

/**
 * 获取表达式序列的除第一个外的表达式
 */
void *rest_exps(void *seq);

/**
 * 构建 begin 表达式
 */
void *make_begin(void *seq);

/**
 * 判断是否是 过程应用
 * 
 * 过程应用 (运算符 <运算对象1> ... <运算对象n>)
 */
uint8_t is_application(void *exp);

/**
 * 获取过程应用的 “运算符”
 */
void *operator(void *exp);

/**
 * 获取过程应用的 “运算对象”表
 */
void *operands(void *exp);

/**
 * 判断 “运算对象”表 是否为空
 */
uint8_t is_no_operands(void *ops);

/**
 * 获取第一个 “运算对象”
 */
void *first_operand(void *ops);

/**
 * 获取除第一个外的其他运算对象
 */
void *rest_operands(void *ops);

#endif

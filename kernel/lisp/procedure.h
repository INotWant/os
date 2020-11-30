#ifndef PROCEDURE_H
#define PROCEDURE_H

/**
 * 过程的表示 -- 过程对象
 */

#include <stdint.h>
#include "pair.h"

/**
 * 构造描述复合过程的过程对象
 * 
 * @param: parameters --> 参数
 * @param: body       --> 函数体
 * @param: env        --> 环境
 */
void *make_procedure(void *parameters, void *body, void *env);

/**
 * 判断过程对象描述的是否是 “复合过程”
 */
uint8_t is_compound_procedure(void *p);

/**
 * 获取复合过程对象的参数
 */
void *procedure_parameters(void *p);

/**
 * 获取复合过程对象的函数体
 */
void *procedure_body(void *p);

/**
 * 获取复合过程对象的环境
 */
void *procedure_env(void *p);

/**
 * 判断过程对象描述的是否是 “基本过程”
 */
uint8_t is_primitive_procedure(void *p);

/**
 * 构造描述基本过程的过程对象
 * 
 * @param: pf --> 基本过程对应 C 中的函数指针
 */
void *make_primitive_procedure(void *pf);

/** 定义实现基本过程的函数指针 **/
typedef element_t (*primitive_pf)(void *);

/**
 * 获取实现基本过程的函数指针
 */
primitive_pf primitive_procedure_impl(void *p);

#endif

#ifndef LISP_H
#define LISP_H

#include "pair.h"

/** 表达式最大长度 **/
#define EXP_MAX_LEN 2048

/**
 * 初始化 lisp 解释器
 */
void lisp_init();

/**
 * 执行 lisp 表达式
 */
element_t lisp_exec(void *exp);

#endif

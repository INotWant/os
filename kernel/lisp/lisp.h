#ifndef LISP_H
#define LISP_H

#include "pair.h"

/**
 * 初始化 lisp 解释器
 */
void lisp_init();

/**
 * 执行 lisp 表达式
 */
element_t lisp_exec(void *exp);

#endif

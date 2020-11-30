#ifndef EVAL_H
#define EVAL_H

#include "pair.h"

/**
 * 求值表达式
 * 
 * @param: exp --> 使用序对表示的表达式
 * @param: env --> 当前环境指针
 * @return: 表达式的求值结果
 */
element_t eval(void *exp, void *env);

/**
 * 求值表达式序列
 * 
 * @param: exps --> 表达式序列
 * @param: env  --> 当前环境指针
 */
element_t eval_sequence(void *exps, void *env);

/**
 * eval 的错误机制 —— 在执行表达式遇到错误时调用此
 * 此方法逻辑如下：
 *   1）打印错误原因
 *   2）清空表达式 exp 以及键盘输入缓存
 *   3）恢复 root 表 -- for GC of pair
 *   4）恢复 ebp esp（即栈）esi edi -- 与此同时，也把 continue 做了修改
 *   5）恢复中断，调至 `jmp $`
 */
void eval_error_handler(element_t reasion);

#endif

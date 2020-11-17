#ifndef STACK_H
#define STACK_H

/**
 * 基于 “cons”(序对) 实现栈。
 * 
 * 基于序对实现的原因：堆栈中的一些元素可能为某些应存活序对的根，序对的 GC 算法需要看到这些元素。
 */

#include "pair.h"

/** 栈顶指针 **/
void *stack_top_point;

/**
 * 初始化栈
 * @return: 失败返回 0，成功返回非 0
 */
uint8_t stack_init(); 

/**
 * 入栈
 * @return: 失败返回 0，成功返回非 0
 */
uint8_t push(element_t *element_point);

/**
 * 出栈
 * @return: 若返回指针为 0 的序对指针，则栈已空
 */
element_t pop();

#endif
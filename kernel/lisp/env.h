#ifndef ENV_H
#define ENV_H

/**
 * 基于 “序对” 实现环境。
 * 这里基于序对的原因同堆栈基于序对实现的原因，对环境的 GC 直接依靠序对的 GC。
 * 
 * 环境定义：框架组成的序列
 * 框架定义：键值对组成的序列
 * 键值对：键 --> 变量名，值 --> 变量值
 * 
 * 实现原理图示：
 * 
 * env_top_point
 *    |
 *    v
 *  +-+-+---+   +---+---+
 *  |   |   +-->+   |   |   （ 框 架 序 列 ）
 *  +-+-+---+   +---+---+
 *    |
 *    v
 *  +-+-+---+   +---+---+   +---+---+
 *  |   |   +-->+   |   +-->+   |   |    （ 具 体 某 一 框 架 ）
 *  +-+-+---+   +---+---+   +---+---+
 *    |
 *    v
 *  +-+-+---+
 *  | K | V |
 *  +---+---+
 */

#include "pair.h"

/**
 * 查找指定变量（约束）在环境中的值
 * @param: var_name --> 变量名
 * @return: 相应值
 */
element_t lookup_variable_value(char *var_name);

/**
 * 扩展环境
 * @param: vars     --> 变量名表（用序对表示的表）
 * @param: values   --> 变量值表（用序对表示的表）
 * @return: 新的 env_top_point
 */
void *extend_env(void *var_names, void *values);

/**
 * 在环境（最新的框架）中添加一新约束，若已存在则覆盖
 * @param: var_name --> 变量名
 * @param: value_p  --> 值指针
 */
void define_var(char *var_name, element_t *value_p);

/**
 * 修改环境中指定约束
 * @param: var_name --> 变量名
 * @param: value_p  --> 值指针
 * @return: 若不存在 var_name 指定的变量则返回 0，成功则返回 1
 */
uint8_t set_var_value(char *var_name, element_t *value_p);

#endif

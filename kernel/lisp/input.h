#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>

/**
 * 判断字符串是否符合 lisp 语法
 * 
 * @param: str --> 字符串首地址
 * @param: len --> 字符串长度
 * 
 * @return: -1 非法，0 合法且完整，1 不完整
 */
int is_legal(char *str, size_t len);

/**
 * 把字符串按照括号['(' or ')']切分，然后以表形式（序对）存储
 * 
 * @param: str --> 字符串首地址
 * @param: len --> 字符串长度
 * 
 * @return: 表首地址
 */
void *save_str_to_pair(char *str, size_t len);

#endif


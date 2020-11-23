#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stddef.h>

/** 整数转为字符串 **/
void int_to_ascii(int n, char str[]);
/** 把数值转为十六进制表示的字符串 **/
void hex_to_ascii(int n, char str[]);
/** 反转字符串 **/
void reverse(char s[]);
/** 求字符串长度 **/
int strlen(char s[]);
/** 清除字符串最后一位 **/
void backspace(char s[]);
/** 在字符串末尾添加指定字符 **/
void append(char s[], char n);
/** 字符串比较 **/
int strcmp(char s1[], char s2[]);
/** 判断字符是否是空白字符 **/
int is_space(char c);
/** 判断字符是否是数字 **/
int is_digital(char c);
/** 判断字符是否是字母 **/
int is_alphabet(char c);
/** 判断字符串是不是一个整数 **/
uint8_t is_integer(char *str, size_t len);
/** 判断字符串是不是浮点数 **/
uint8_t is_float(char *str, size_t len);
/** 字符串转为整数 **/
int32_t str2int32(char *str, size_t len);
/** 字符串转为浮点数 **/
float str2float(char *str, size_t len);

/**
 * 把字符串放入“字符串常量池”
 * 
 * 设计字符串常量池的目的（或效果）：   --> 主要是为了节约空间
 * 1）视字符串为不可变
 * 2）同一字符串全局只存一次，节约空间
 * 
 * @param: str -> 待放入字符串的首地址
 * @return: 字符串在常量池中的地址，若返回 0 则表示无足够的存储空间
 * 
 * 注：切记勿使用指针来修改字符串，字符串是不可变的
 */
char *put_string_to_constant_pool(char *str);

/**
 * 从“字符串常量池”中删除某字符串
 * 
 * @param: str --> 字符串在常量池的首地址
 * 
 * 注：最后一个使用者才有权删除
 */
void remove_string_in_constant_pool(char *str);

#endif
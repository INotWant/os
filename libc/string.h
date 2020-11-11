#ifndef STRING_H
#define STRING_H

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

#endif
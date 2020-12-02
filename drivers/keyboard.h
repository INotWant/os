#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef void (*input_handler_fp)(char *);

void init_keyboard(input_handler_fp default_input_handler);

/**
 * 返回当前输入缓冲区已保存的字符数量
 * @return  输入缓冲区已满返回 -1
 */
int input_len();

/**
 * 清楚键盘输入缓存
 */
void clear_key_buffer();

/**
 * 开启键盘
 */
void enable_keyboard();

/**
 * 关闭键盘
 */
void disable_keyboard();

/**
 * 对输入字符串进行处理的函数指针
 */
extern input_handler_fp input_handler;

#endif
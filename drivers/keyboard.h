#ifndef KEYBOARD_H
#define KEYBOARD_H

void init_keyboard();

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

#endif
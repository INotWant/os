#ifndef KEYBOARD_H
#define KEYBOARD_H

void init_keyboard();

/**
 * 返回当前输入缓冲区已保存的字符数量
 * @return  输入缓冲区已满返回 -1
 */
int input_len();

#endif
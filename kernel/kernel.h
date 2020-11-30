#ifndef KERNEL_H
#define KERNEL_H

/**
 * 进入 _start() 函数后，ebp esp 寄存器的初始值，被用于错误机制
 * 注：boot 中若修改 ebp esp 的初始值，此处要做对应修改
 */
#define EBP_CONSTANT    0x7b00
#define ESP_CONSTANT    0x7af8   

void user_input(char *input);

void clear_exp();

#endif
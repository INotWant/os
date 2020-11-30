#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define TIMER_FREQ 1000 /* 定时器频率，每秒振动次数 */

/**
 * 初始化定时器
 */
void init_timer();

/**
 * 睡眠指定时间
 * @param: time --> 以毫秒为单位
 */
void sleep(uint32_t time);

#endif
#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

/** 选取低 16位 **/
#define low_16(value) (uint16_t)((value) & 0xFFFF)
/** 选取高 16位 **/
#define high_16(value) (uint16_t)(((value) >> 16) & 0xFFFF)
/** 为避免编译器 'unused parameter' 警告 **/
#define UNUSED(x) (void)(x)



#endif
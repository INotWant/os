#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <stddef.h>

/**
 * 内存拷贝
 * @param: source --> 源地址
 * @param: dest   --> 目的地址
 * @param: nbytes --> 拷贝的字节数
 */
void memory_copy(uint8_t *source, uint8_t *dest, size_t nbytes);

/**
 * 为内存中指定地址赋指定值指定次数次
 * @param: dest --> 指定地址
 * @param: val  --> 指定值
 * @param: len  --> 指定次数
 */
void memory_set(uint8_t *dest, uint8_t val, size_t len);

#endif
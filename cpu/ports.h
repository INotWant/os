#ifndef PORTS_H
#define PORTS_H

#include <stdint.h>

/**
 * 从指定端口读取 1byte 
 */
uint8_t port_byte_in(uint16_t port);

/**
 * 往指定端口写 1byte 
 */
void port_byte_out(uint16_t port, uint8_t data);

/**
 * 从指定端口读取 2byte
 */
uint16_t port_word_in(uint16_t port);

/**
 * 往指定端口写 2byte
 */
void port_word_out(uint16_t port, uint16_t data);

#endif
#ifndef _INPUT_H
#define _INPUT_H

#include <stddef.h>

/**
 * determine whether the string conforms to lisp syntax
 * note:
 *   - for string constants, use double quotation marks, and can span lines
 *   - for quotation mark expression, start with single quotation mark, followed by one or more letters or numbers or (!, ?, +, -, *, /, =) 
 *          In addition, '() shows the empty table. And it can't span lines
 *   - for symbols (including numeric values), they can be composed of letters, numbers, _, !, %, ., +, -, *, /
 *
 * @param: str -- > first address of string
 * @param: len -- > string length
 *
 * @return: - 1 is illegal, 0 is legal and complete, 1 is incomplete
 */
int is_legal(char *str, size_t len);

/**
 * the string is divided into brackets ['('or') '] and stored in table(use pair)
 * 
 * note: the following describes the representation of a, (a), ((a) b)
 * 
 *  +---+---+           +---+---+   +---+---+
 *  | a | / | --> a     |   |   +--->   | / |    --> ((a) b)
 *  +---+---+           +-+-+---+   +-+-+---+
 *                        |           |
 *  +---+---+           +-v-+---+   +-v-+---+
 *  |   | / | --> (a)   |   | / |   | b | / |
 *  +-+-+---+           +-+-+---+   +---+---+
 *    |                   |
 *  +-v-+---+           +-v-+---+
 *  | a | / |           | a | / |
 *  +---+---+           +---+---+
 * 
 * @param: str --> first address of string
 * @param: len --> string length
 * 
 * @return: address
 */
void *save_str_to_pair(char *str, size_t len);

#endif
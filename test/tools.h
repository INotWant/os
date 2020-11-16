#ifndef TOOLS
#define TOOLS

#include "stdint.h"
#include "../drivers/screen.h"

/** 判断 v1 与 v2 是否相等，若不相等则打印 p 指向的字符串 **/
#define assert_eq(v1, v2, p) ((uint32_t)(v1) == (uint32_t)(v2) ? (void)(0) : kprint(p))

/** 判断 v1 与 v2 是否相等，若相等则打印 p 指向的字符串 **/
#define assert_neq(v1, v2, p) ((uint32_t)(v1) != (uint32_t)(v2) ? (void)(0) : kprint(p))

#endif
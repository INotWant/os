#ifndef APPLY_H
#define APPLY_H

#include "pair.h"

/**
 * 求解过程应用
 * 
 * @param: procedure --> 过程对象
 * @param: arguments --> 实参
 */
element_t apply(void *procedure, void *arguments);

#endif
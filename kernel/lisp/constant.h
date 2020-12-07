#ifndef CONSTANT_H
#define CONSTANT_H

#include "pair.h"

/** 定义一些 primitive_procedure.c 模块使用的 element_t 常量 **/
extern element_t WNP_ADD;
extern element_t WNP_SUB;
extern element_t WNP_MUL;
extern element_t WNP_DIV;
extern element_t WNP_REM;
extern element_t WNP_CONS;
extern element_t WNP_CAR;
extern element_t WNP_CDR;
extern element_t WNP_IS_NULL;
extern element_t WNP_DISPLAY;
extern element_t WNP_LT;
extern element_t WNP_GT;
extern element_t WNP_EQ;
extern element_t WNP_NE;
extern element_t WNP_LE;
extern element_t WNP_GE;
extern element_t WNP_READ;
extern element_t WNP_IS_NUMBER;
extern element_t WNP_IS_STRING;
extern element_t WNP_IS_SYMBOL;
extern element_t WNP_IS_PAIR;
extern element_t WNP_IS_EQ;
extern element_t WNP_IS_QUOTED;
extern element_t WNP_QUOTATION_TEXT;
extern element_t WNP_SET_CAR;
extern element_t WNP_SET_CDR;
extern element_t WNP_APPLY;
extern element_t UT_ADD;
extern element_t UT_SUB;
extern element_t UT_MUL;
extern element_t UT_DIV;
extern element_t UT_REM;
extern element_t UT_CAR;
extern element_t UT_CDR;
extern element_t UT_IS_NULL;
extern element_t UT_LT;
extern element_t UT_GT;
extern element_t UT_EQ;
extern element_t UT_NE;
extern element_t UT_LE;
extern element_t UT_GE;
extern element_t UT_SET_CAR;
extern element_t UT_SET_CDR;
extern element_t UT_APPLY;
extern element_t UT_QUOTATION_TEXT;
extern element_t ERR_ENL_READ;
extern element_t ERR_TMCL_READ;
extern element_t ERR_TMCE_READ;

/** 定义一些 eval.c 模块使用的 element_t 常量 **/
extern element_t VAR_NON_EXIST;
extern element_t PROC_NON_EXIST;
extern element_t UNKNOWN_EXP_TYPE;

/** 定义一些 expression.c 模块使用的 element_t 常量 **/
extern element_t QUOTE_FALSE;
extern element_t LAMBDA;
extern element_t IF;
extern element_t BEGIN;
extern element_t COND_ELSE_ERR;

/** 定义一些 procedure.c 模块使用的 element_t 常量 **/
extern element_t PROCEDURE;
extern element_t PRIMITIVE;

/** 定义 apply.c 模块中使用的 element_t 常量 **/
extern element_t UNKNOWN_PROC_TYPE;

/** 定义一些公用 element_t 常量 **/
extern element_t FALSE;
extern element_t TRUE;
extern element_t OK;
extern element_t ZERO_POINT;
extern element_t NON_EXIST;

/**
 * 初始化常量
 */
void constant_init();

#endif
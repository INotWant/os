#include "constant.h"

element_t WNP_ADD;
element_t WNP_SUB;
element_t WNP_MUL;
element_t WNP_DIV;
element_t WNP_REM;
element_t WNP_CONS;
element_t WNP_CAR;
element_t WNP_CDR;
element_t WNP_IS_NULL;
element_t WNP_DISPLAY;
element_t WNP_LT;
element_t WNP_GT;
element_t WNP_EQ;
element_t WNP_NE;
element_t WNP_LE;
element_t WNP_GE;
element_t UT_ADD;
element_t UT_SUB;
element_t UT_MUL;
element_t UT_DIV;
element_t UT_REM;
element_t UT_CAR;
element_t UT_CDR;
element_t UT_IS_NULL;
element_t UT_LT;
element_t UT_GT;
element_t UT_EQ;
element_t UT_NE;
element_t UT_LE;
element_t UT_GE;

element_t VAR_NON_EXIST;
element_t PROC_NON_EXIST;
element_t UNKNOWN_EXP_TYPE;

element_t QUOTE_FALSE;
element_t LAMBDA;
element_t IF;
element_t BEGIN;
element_t COND_ELSE_ERR;

element_t PROCEDURE;
element_t PRIMITIVE;

element_t UNKNOWN_PROC_TYPE;

element_t FALSE;
element_t TRUE;
element_t OK;
element_t ZERO_POINT;
element_t NON_EXIST;

void constant_init() {
    WNP_ADD = construct_string_element("wrong number of parameters -- +");
    WNP_SUB = construct_string_element("wrong number of parameters -- -");
    WNP_MUL = construct_string_element("wrong number of parameters -- *");
    WNP_DIV = construct_string_element("wrong number of parameters -- /");
    WNP_REM = construct_string_element("wrong number of parameters -- rem");
    WNP_CONS = construct_string_element("wrong number of parameters -- cons");
    WNP_CAR = construct_string_element("wrong number of parameters -- car");
    WNP_CDR = construct_string_element("wrong number of parameters -- cdr");
    WNP_IS_NULL = construct_string_element("wrong number of parameters -- null?");
    WNP_DISPLAY = construct_string_element("wrong number of parameters -- display");
    WNP_LT = construct_string_element("wrong number of parameters -- <");
    WNP_GT = construct_string_element("wrong number of parameters -- >");
    WNP_EQ = construct_string_element("wrong number of parameters -- =");
    WNP_NE = construct_string_element("wrong number of parameters -- !=");
    WNP_LE = construct_string_element("wrong number of parameters -- <=");
    WNP_GE = construct_string_element("wrong number of parameters -- >=");
    UT_ADD = construct_string_element("unsupport type -- +");
    UT_SUB = construct_string_element("unsupport type -- -");
    UT_MUL = construct_string_element("unsupport type -- *");
    UT_DIV = construct_string_element("unsupport type -- /");
    UT_REM = construct_string_element("unsupport type -- rem");
    UT_CAR = construct_string_element("unsupport type -- car");
    UT_CDR = construct_string_element("unsupport type -- cdr");
    UT_IS_NULL = construct_string_element("unsupport type -- null?");
    UT_LT = construct_string_element("unsupport type -- <");
    UT_GT = construct_string_element("unsupport type -- >");
    UT_EQ = construct_string_element("unsupport type -- =");
    UT_NE = construct_string_element("unsupport type -- !=");
    UT_LE = construct_string_element("unsupport type -- <=");
    UT_GE = construct_string_element("unsupport type -- >=");

    VAR_NON_EXIST = construct_string_element("variable non exist -- EVAL");
    PROC_NON_EXIST = construct_string_element("procedure non exist -- EVAL");
    UNKNOWN_EXP_TYPE = construct_string_element("unknown expression type -- EVAL");

    QUOTE_FALSE = construct_string_element("'false");
    LAMBDA = construct_string_element("lambda");
    IF = construct_string_element("if");
    BEGIN = construct_string_element("begin");
    COND_ELSE_ERR = construct_string_element("else clause isn't last -- COND->if");

    PROCEDURE = construct_string_element("procedure");
    PRIMITIVE = construct_string_element("primitive");

    UNKNOWN_PROC_TYPE = construct_string_element("unknown procedure type -- APPLY");

    OK = construct_string_element("ok");
    TRUE = construct_string_element("true");
    FALSE = construct_string_element("false");
    ZERO_POINT = construct_point_element(0);
    NON_EXIST = construct_non_exist_element();
}
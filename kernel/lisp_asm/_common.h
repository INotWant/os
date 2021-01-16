#ifndef _COMMON_H
#define _COMMON_H

#define EXP_OFFSET 0x9f004
#define STACK_OFFSET 0x9f018

#define INTEGER_T 0x0
#define FLOAT_T 0x1
#define STRING_T 0x2
#define PAIR_POINT_T 0x3

#define EXP_MAX_LEN 2048

typedef struct
{
    uint8_t type; /* type */
    union
    {
        int32_t ival; /* integer */
        float fval;   /* float */
        void *point;  /* point of string or pair */
    } val;
} __attribute__((packed)) element_t;

#endif
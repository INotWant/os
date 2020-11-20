/**
 * 基于连续存储，实现 “序对”（即表存储结构）
 * 
 * 下图为一种实现方式（参考，SICP P375）：
 * 
 *     Index  0   1   2   3   4   5
 *          +---+---+---+---+---+------+
 *  the-cars|   |   |   |   |   |   |...
 *          +--------------------------+
 *          +--------------------------+
 *  the-cdrs|   |   |   |   |   |   |...
 *          +---+---+---+---+---+------+
 * 
 * 其中，`the-cars` 与 `the-cdrs` 分别是两个连续的存储空间。
 * the-cars 存储的是对应索引的 car 部分，而 the-cdrs 存储的是对应索引的 cdr 部分。
 * 
 * 下图为另一种实现方式（下面将采用该实现方式✅）：
 * 
 *  car cdr
 *  +--+------+------+------+------+------+---
 *  |  |  ||  |  ||  |  ||  |  ||  |  ||  |...
 *  +--+------+------+------+------+------+---
 * 
 * 其中，只需一个连续的存储空间。一个序对的 car 与 cdr 部分相连。
 * 
 * 无论采用哪种方式，图中的一小块代表序对中的一元素。由于元素的类型有多种，所以采用 “类型 + 数据” 的存储方式。
 * 下图描述了一块：
 * 
 *  0     7 8                        31 32                                   64
 *  +------+---------------------------+--------------------------------------+
 *  |      |                           |                                      |
 *  +------+---------------------------+--------------------------------------+
 *   类 型          保          留                   数            据
 * 
 * 一块占 8个字节，前 1 字节代表数据类型，后 4 字节代表数据，中间 3 字节用于对齐或它用。
 * 
 * 目前，一共考虑有 5 中类型：
 *     [类型]                           [范围]
 *    - 整数                            4 字节
 *    - 浮点数                          4 字节
 *    - 字符串（短，<= 7字节，含结束符）    7 字节（直接存放字符串，3 字节的保留位置亦被用）
 *    - 字符串（长，> 7 字节）            4 字节（存放的是地址，指向实际存放位置）
 *    - 指针（指向序对，即嵌套）           4 字节
 * 
 * GC —— 采用停止复制算法
 *   注: 这里假设（由最前面的几个序对组成的表）是目前所有仍存活序对的根
 *       所以，在使用时存储的第一个表中应包含且只应包含所有的根
 *       （表的概念：把几个序对通过 cdr 连接起来便是表）
 * 
 * 注：序对实现在 内存管理 之上
 */

#ifndef PAIR
#define PAIR

#include <stdint.h>

/** 最大序对数目（一个序对包含两个元素：car + cdr） **/
#define PAIR_MAX_NUMBER     10240
/** 调小最大序对数，方便调试 **/
// #define PAIR_MAX_NUMBER     6

/** 定义序对中元素类型对应的标号 **/
#define INTEGER_T       0x0   /* 整型 */
#define FLOAT_T         0x1   /* 浮点型 */
#define STRING_SHORT_T  0x2   /* 短字符串 */
#define STRING_LONG_T   0x3   /* 长字符串 */
#define POINT_PAIR_T    0x4   /* 指针 */
#define NON_EXIST       0xe   /* 表不存在的元素 */
#define BROKEN_HEART_T  0xf   /* 破碎的心，用于 GC */

/** 序对中元素（类型 + 值） **/
typedef struct {
    uint8_t type;                   /* 元素类型 */
    union {
        int32_t ival;               /* 整型 */
        float fval;                 /* 浮点型 */
        char short_string[7];       /* 短字符串 */
        void *point;                /* 指向 长字符串 or 序对，为 0 时表空表或空串 */
    } val;
} element_t;

/**
 * 初始化序对
 * @return: 失败返回 0，成功返回非 0
 */
uint8_t pair_init();

/**
 * 销毁序对
 */
void pair_destory();

/**
 * 构建一个新序对
 * @param: car_element_point --> 指定 car 部分元素
 * @param: cdr_element_point --> 指定 cdr 部分元素
 * @return: 返回执行新序对首地址的指针，若无足够空间返回 0
 */
void *cons(element_t *car_element_point, element_t *cdr_element_point);

/**
 * 返回序对的 car 部分
 * @param: pair_point 序对指针
 */
element_t car(void *pair_point);

/**
 * 返回序对的 cdr 部分
 * @param: pair_point 序对指针
 */
element_t cdr(void *pair_point);

/**
 * 修改 序对 的 car 部分
 * @param: pair_point 序对指针
 * @param: element_point 元素指针
 */
void set_car(void *pair_point, element_t *element_point);

/**
 * 修改 序对 的 cdr 部分
 * @param: pair_point 序对指针
 * @param: element_point 元素指针
 */
void set_cdr(void *pair_point, element_t *element_point);

/**
 * 构建表述整型的 element_t
 */
element_t construct_integer_element(int32_t val);

/**
 * 构建表述浮点型的 element_t
 */
element_t construct_float_element(float val);


/**
 * 构建表述字符串的 element_t
 */
element_t construct_string_element(char *str);

/**
 * 构建表述指针的 element_t
 */
element_t construct_point_element(void *point);

/**
 * 构建表述非存在元素的 element_t
 */
element_t construct_non_exist_element();

#endif

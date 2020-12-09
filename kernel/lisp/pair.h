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
 *  0     7 8                        39
 *  +------+---------------------------+
 *  |      |                           |
 *  +------+---------------------------+
 *   类  型      数              据                   
 * 
 * 一块占 5 个字节，前 1 字节代表数据类型，后 4 字节代表数据
 * 
 * 目前，一共考虑有 5 中类型：
 *     [类型]                           [范围]
 *    - 整数                             8 - 39
 *    - 浮点数                           8 - 39
 *    - 字符串                           8 - 39（存放的是地址，指向实际存放位置）
 *    - 指针（指向序对，即嵌套）            8 - 39
 * 
 * GC —— 采用停止复制算法
 *   说明1：这里假设（由最前面的几个序对组成的表）是目前所有仍存活序对的根
 *         所以，在使用时存储的第一个表中应包含且只应包含所有的根（此表被称为 root 表）
 *        （表的概念：把几个序对通过 cdr 连接起来便是表）
 *   说明2：当前设计的 root 表中元素依次为：
 *      - exp_p     -->     指向描述表达式的表
 *      - env_p     -->     指向描述环境的表
 *      - stack_tp  -->     指向栈顶指针（注：栈使用序对实现），栈被用来保存调用 cons 前需要被标为存活序对的指针。指针被分为两种（[在建] or [参数]）：
 *        - [在建] input.c 中的 conn_pair() 方法
 *        - [参数] env.c 中的 extend_env() 方法， pair.c 中的 cons() 和 list() 方法
 *   说明3：stack 专门为 GC 而设计，其中存放部分需存活序对的指针（实现见 stack.c）
 *         在 stack 的 push 方法中将调用 cons_for_stack 方法，此方法类似 cons 方法，但不进行 GC
 *         是 push 方法的功能决定了 cons_for_stack 方法不能进行 GC
 *         为了保证 push 的正常功能，在 “序对池” 中保留了 STACK_RESERVE_NUMBER 大小的空间
 * 
 * 注：序对实现在 内存管理 之上
 */

#ifndef PAIR_H
#define PAIR_H

#include <stddef.h>
#include <stdint.h>

/**
 * 最大序对数目（一个序对包含两个元素：car + cdr）
 * 此值至少为 24 ((18(list方法最大18次push)+2(cons方法最大2次push))(预留的push空间)+3(root表)+1(可用数目))
 * 注：push 指的是 stack.c 中的 push 方法
 */
#define PAIR_MAX_NUMBER         4096000
#define STACK_RESERVE_NUMBER    20

/** 定义序对中元素类型对应的标号 **/
#define INTEGER_T       0x0   /* 整型 */
#define FLOAT_T         0x1   /* 浮点型 */
#define STRING_T        0x2   /* 字符串 */
#define POINT_PAIR_T    0x3   /* 指针 */
#define NON_EXIST_T     0xe   /* 表不存在的元素 */
#define BROKEN_HEART_T  0xf   /* 破碎的心，用于 GC */

/** 序对中元素（类型 + 值） **/
typedef struct {
    uint8_t type;                   /* 元素类型 */
    union {
        int32_t ival;               /* 整型 */
        float fval;                 /* 浮点型 */
        void *point;                /* 指向 长字符串 or 序对，为 0 时表空表或空串 */
    } val;
} element_t;

/**
 * 初始化序对
 * @return: 失败返回 0，成功返回非 0
 */
uint8_t pair_init();

/**
 * 更新 root 表中的 exp 指针
 */
void update_exp_point(void *exp);

/**
 * 更新 root 表中的 env 指针
 */
void update_env_point(void *env);

/**
 * 更新 root 表中的 栈顶指针
 */
void update_stack_top_point(void *stack_top_point);

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
 * 构建一个新序对，专门为堆栈的 push 定制 -- for GC of pair
 * 最大不同：无足够空间时不进行 GC
 * 
 * @param: car_element_point --> 指定 car 部分元素
 * @param: cdr_element_point --> 指定 cdr 部分元素
 * @return: 返回执行新序对首地址的指针，若无足够空间直接返回 0
 */
void *cons_for_stack(element_t *car_element_point, element_t *cdr_element_point);

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
 * 根据输入的一系列元素使用序对构建表
 * @param: num 元素个数
 * @param: ep 元素数组的头指针（注：受 GC 机制所限，元素个数不要超过 18）
 */
void *list(size_t num, element_t *ep);

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

/**
 * 打印 element_t
 */
void print_element(element_t ele);

/** 一些有用的宏定义 **/
#define caar(p) car(car(p).val.point)
#define cadr(p) car(cdr(p).val.point)
#define cddr(p) cdr(cdr(p).val.point)
#define caadr(p) car(cadr(p).val.point)
#define caddr(p) car(cddr(p).val.point)
#define cdadr(p) cdr(cadr(p).val.point)
#define cdddr(p) cdr(cddr(p).val.point)
#define cadddr(p) car(cdddr(p).val.point)

#endif

#include "pair.h"
#include "../../libc/mem.h"
#include "../../libc/string.h"
#include <stddef.h>

/** 每个元素所占存储空间 **/
#define ELEMENT_SIZE 5
/** 一个序对所占的存储空间 **/
#define PAIR_SIZE (ELEMENT_SIZE * 2)

/** 由起始地址和索引，求索引所向的序对地址 **/
#define GET_PAIR_POINT(sp, index) ((void *)((char *)(sp) + (index)*PAIR_SIZE))

#define GET_CAR_EP(pp) ((char *)(pp))
#define GET_CDR_EP(pp) ((char *)(pp) + ELEMENT_SIZE)

/** 由元素指针，获取数据类型 **/
#define GET_TYPE(ep) (*(uint8_t *)(ep))
/** 由元素指针，设置数据类型 **/
#define PUT_TYPE(ep, t) (*(uint8_t *)(ep) = (uint8_t)(t))
/** 由元素指针，获取整型数值 **/
#define GET_INT_VAL(ep) (*((int32_t *)((char *)(ep) + 1)))
/** 由元素指针，设置整型数值 **/
#define PUT_INT_VAL(ep, val) (*((int32_t *)((char *)(ep) + 1)) = (int32_t)(val))
/** 由元素指针，获取浮点型数值 **/
#define GET_FLOAT_VAL(ep) (*((float *)((char *)(ep) + 1)))
/** 由元素指针，设置浮点型数值 **/
#define PUT_FLOAT_VAL(ep, val) (*((float *)((char *)(ep) + 1)) = (float)(val))
/** 由元素指针，获取指针数值 **/
#define GET_POINT_VAL(ep) ((void *)(*((uint32_t *)((char *)(ep) + 1))))
/** 由元素指针，设置指针数值 **/
#define PUT_POINT_VAL(ep, val) (*((uint32_t *)((char *)(ep) + 1)) = (uint32_t)(val))

static char *old_start_point;   /* 永远指向当前在用的连续存储空间的首地址 */
static char *new_start_point;   /* 永远指向下次被用的连续存储空间的首地址 */

static char *root;              /* 指向第一个序对，用于 GC 的存活对象分析 */

static size_t index = 0;        /* 当前可用索引 */

uint8_t pair_init() {
    size_t size = PAIR_MAX_NUMBER * PAIR_SIZE;
    char *p = (char *)memory_malloc(size);
    if (p != 0)
        old_start_point = p;
    else
        return 0;
    p = (char *)memory_malloc(size);
    if (p != 0) {
        new_start_point = p;
        return 1;
    }else {
        memory_free(old_start_point);
        root = 0;
        index = 0;
        return 0;
    }
}

void pair_destory() {
    memory_free(old_start_point);
    memory_free(new_start_point);
    old_start_point = 0;
    new_start_point = 0;
    root = 0;
    index = 0;
}

static void assign_ep(char *ep, element_t *element_point) {
    uint8_t type = element_point->type;
    PUT_TYPE(ep, type);
    if (type == INTEGER_T)
        PUT_INT_VAL(ep, element_point->val.ival);
    else if (type == FLOAT_T)
        PUT_FLOAT_VAL(ep, element_point->val.fval);
    else if (type == STRING_T || type == POINT_PAIR_T)
        PUT_POINT_VAL(ep, element_point->val.point);
}

static void gc_helper(char *ep, char **root_p) {
    uint8_t type = GET_TYPE(ep);
    if (type == POINT_PAIR_T && GET_POINT_VAL(ep) != 0) {  /* 指向序对，指针为 0 时表空表 */
        void *pp = GET_POINT_VAL(ep);
        char *pp_car_ep = GET_CAR_EP(pp);
        char *pp_cdr_ep = GET_CDR_EP(pp);
        uint8_t pp_car_type = GET_TYPE(pp_car_ep);
        if (pp_car_type == BROKEN_HEART_T) {
            PUT_POINT_VAL(ep, GET_POINT_VAL(pp_cdr_ep));
        } else {
            char *free = *root_p;
            memory_copy((uint8_t *)pp, (uint8_t *)free, PAIR_SIZE);
            PUT_TYPE(pp_car_ep, BROKEN_HEART_T);
            PUT_POINT_VAL(pp_cdr_ep, free);
            PUT_POINT_VAL(ep, free);
            *root_p = free + PAIR_SIZE;
        }
    }
}

// 采用停止复制算法
static void garbage_collection() {
    memory_copy((uint8_t *)root, (uint8_t *)new_start_point, PAIR_SIZE);
    root = new_start_point;

    char *scan = root;
    char *free = new_start_point + PAIR_SIZE;
    while(scan != free) {
        gc_helper(GET_CAR_EP(scan), &free);
        gc_helper(GET_CDR_EP(scan), &free);
        scan += PAIR_SIZE;
    }
    /* 维护不变 */
    new_start_point = old_start_point;
    old_start_point = root;
    index = (scan - old_start_point) / PAIR_SIZE;
}

void *cons(element_t *car_element_point, element_t *cdr_element_point) {
    if (index == PAIR_MAX_NUMBER) {
        garbage_collection();
        if (index == PAIR_MAX_NUMBER)   /* GC 后仍无空间 */
            return 0;
    }
    void *pp = GET_PAIR_POINT(old_start_point, index);
    assign_ep(GET_CAR_EP(pp), car_element_point);
    assign_ep(GET_CDR_EP(pp), cdr_element_point);
    ++index;
    if (root == 0)
        root = pp;
    return pp;
}

static void assign_element(element_t *element_point, char *ep){
    uint8_t type = GET_TYPE(ep);
    element_point->type = type;
    if (type == INTEGER_T)
        element_point->val.ival = GET_INT_VAL(ep);
    else if (type == FLOAT_T)
        element_point->val.fval = GET_FLOAT_VAL(ep);
    else if (type == STRING_T || type == POINT_PAIR_T)
        element_point->val.point = GET_POINT_VAL(ep);
}

element_t car(void *pair_point) {
    element_t element;
    assign_element(&element, GET_CAR_EP(pair_point));
    return element;
}

element_t cdr(void *pair_point) {
    element_t element;
    assign_element(&element, GET_CDR_EP(pair_point));
    return element;
}

void set_car(void *pair_point, element_t *element_point) {
    assign_ep(GET_CAR_EP(pair_point), element_point);
}

void set_cdr(void *pair_point, element_t *element_point) {
    assign_ep(GET_CDR_EP(pair_point), element_point);
}

element_t construct_integer_element(int32_t val) {
    element_t element;
    element.type = INTEGER_T;
    element.val.ival = val;
    return element;
}

element_t construct_float_element(float val) {
    element_t element;
    element.type = FLOAT_T;
    element.val.fval = val;
    return element;
}

element_t construct_string_element(char *str) {
    size_t str_len = strlen(str) + 1;  /* key 字符串总长度（含结束符） */
    element_t element;
    element.type = STRING_T;
    element.val.point = put_string_to_constant_pool(str);
    return element;
}

element_t construct_point_element(void *point) {
    element_t element;
    element.type = POINT_PAIR_T;
    element.val.point = point;
    return element;
}

element_t construct_non_exist_element() {
    element_t element;
    element.type = NON_EXIST;
    return element;
}

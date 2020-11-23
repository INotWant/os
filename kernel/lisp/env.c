#include "env.h"
#include "../../libc/mem.h"
#include "../../libc/string.h"

#define GET_NEXT_PAIR_POINT(pp) (cdr(pp).val.point)
#define GET_VAR_NAME_POINT(ep) (char *)((ep)->val.point)

void *env_top_point = 0;    /* 环境头指针，即指向最新创建的框架 */

// 从框架中查找某约束（键值对）
static void *lookup_kv_form_frame(void *frame_point, char *var_name) {
    void *p = frame_point;
    while (p != 0) {
        void *kv_point = car(p).val.point;
        element_t element = car(kv_point);
        char *curr_var_name = GET_VAR_NAME_POINT(&element);
        if (strcmp(curr_var_name, var_name) == 0)
            return kv_point;
        p = GET_NEXT_PAIR_POINT(p);
    }
    return 0;
}

// 从框架中查找某变量的值
static element_t lookup_variable_value_form_frame(void *frame_point, char *var_name) {
    void *p = lookup_kv_form_frame(frame_point, var_name);
    if (p == 0)
        return construct_non_exist_element();
    else
        return cdr(p);
}

element_t lookup_variable_value(char *var_name) {
    void *p = env_top_point;
    while (p != 0) {
        void *frame_point = car(p).val.point;
        element_t element = lookup_variable_value_form_frame(frame_point, var_name);
        if (element.type != NON_EXIST)
            return element;
        p = GET_NEXT_PAIR_POINT(p);
    }
    return construct_non_exist_element();
}

// 头插
static void *insert_kv_to_frame(void *frame_point, void *kv_point) {
    element_t car_element = construct_point_element(kv_point);
    element_t cdr_element = construct_point_element(frame_point);
    return cons(&car_element, &cdr_element);
}

void *extend_env(void *var_names, void *values) {
    void *var_name_p = var_names;
    void *value_p = values;
    void *frame_point = 0;
    void *kv_point;
    while (var_name_p != 0 && value_p != 0) {
        element_t key_ele = car(var_name_p);
        element_t value_ele = car(value_p);
        kv_point = cons(&key_ele, &value_ele);
        frame_point = insert_kv_to_frame(frame_point, kv_point);
        var_name_p = GET_NEXT_PAIR_POINT(var_name_p);
        value_p = GET_NEXT_PAIR_POINT(value_p);
    }
    if (frame_point == 0)   /* var_names or values 为空表 */
        return env_top_point;
    else {
        element_t car_ele = construct_point_element(frame_point);
        element_t cdr_ele = construct_point_element(env_top_point);
        env_top_point = cons(&car_ele, &cdr_ele);
        return env_top_point;
    }
}

void define_var(char *var_name, element_t *value_p) {
    if (env_top_point != 0) {
        void *frame_point = car(env_top_point).val.point;
        void *kv_point = lookup_kv_form_frame(frame_point, var_name);
        if (kv_point != 0) {    /* 原本存在则覆盖 */
            set_cdr(kv_point, value_p);
        } else {                /* 新增 */
            element_t key_element = construct_string_element(var_name);
            kv_point = cons(&key_element, value_p);
            frame_point = insert_kv_to_frame(frame_point, kv_point);
            element_t car_element = construct_point_element(frame_point);
            set_car(env_top_point, &car_element);      
        }  
    }
}

uint8_t set_var_value(char *var_name, element_t *value_p) {
    void *p = env_top_point;
    while (p != 0) {
        void *frame_point = car(p).val.point;
        void *kv_point = lookup_kv_form_frame(frame_point, var_name);
        if (kv_point != 0) {
            set_cdr(kv_point, value_p);
            return 1;
        }
        p = GET_NEXT_PAIR_POINT(p);
    }
    return 0;
}

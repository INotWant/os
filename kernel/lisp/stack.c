#include "stack.h"
#include "constant.h"

/** 栈顶指针 **/
void *stack_top_point;

uint8_t push(element_t *element_point) {
    element_t element = construct_point_element(stack_top_point);
    void *pp = cons_for_stack(element_point, &element);
    if (pp == 0)
        return 0;
    stack_top_point = pp;
    /* 更新 root 表中的 stack_top_point -- for GC of pair */
    update_stack_top_point(stack_top_point);
    return 1;
}

element_t pop() {
    if (stack_top_point == 0)
        return NON_EXIST;

    element_t element = car(stack_top_point);
    stack_top_point = cdr(stack_top_point).val.point;
    /* 更新 root 表中的 stack_top_point -- for GC of pair */
    update_stack_top_point(stack_top_point);
    return element;
}

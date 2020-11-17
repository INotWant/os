#include "stack.h"

/* 栈底指针 */
static void *stack_bottom_point;

uint8_t stack_init() {
    element_t element;
    element.type = POINT_PAIR_T;
    element.val.point = 0;
    void *pp = cons(&element, &element);
    if (pp == 0)
        return 0;
    stack_top_point = pp;
    stack_bottom_point = pp;
    return 1;
}

uint8_t push(element_t *element_point) {
    element_t element;
    element.type = POINT_PAIR_T;
    element.val.point = stack_top_point;
    void *pp = cons(element_point, &element);
    if (pp == 0)
        return 0;
    stack_top_point = pp;
    return 1;
}

element_t pop() {
    element_t element;
    if (stack_top_point == stack_bottom_point){
        element.type = POINT_PAIR_T;
        element.val.point = 0;
        return element;
    }
    element = car(stack_top_point);
    stack_top_point = cdr(stack_top_point).val.point;
    return element;
}

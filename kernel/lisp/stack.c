#include "stack.h"

void stack_init() {
    stack_top_point = 0;
}

uint8_t push(element_t *element_point) {
    element_t element = construct_point_element(stack_top_point);
    void *pp = cons(element_point, &element);
    if (pp == 0)
        return 0;
    stack_top_point = pp;
    return 1;
}

element_t pop() {
    if (stack_top_point == 0)
        return construct_non_exist_element();

    element_t element = car(stack_top_point);
    stack_top_point = cdr(stack_top_point).val.point;
    return element;
}

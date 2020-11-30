#include "procedure.h"
#include "pair.h"
#include "constant.h"
#include "../../libc/string.h"

void *make_procedure(void *parameters, void *body, void *env) {
    /* ("procedure" parameters body env) */
    element_t ele1 = PROCEDURE;
    element_t ele2 = construct_point_element(parameters);
    element_t ele3 = construct_point_element(body);
    element_t ele4 = construct_point_element(env);
    element_t elements[] = {ele1, ele2, ele3, ele4};
    return list(4, elements);
}

static uint8_t tagged(void *exp, char *str) {
    element_t element = car(exp);
    if(element.type == STRING_T)
        return strcmp(element.val.point, str) == 0;
    return 0;
}

uint8_t is_compound_procedure(void *p) {
    return tagged(p, "procedure");
}

void *procedure_parameters(void *p) {
    return cadr(p).val.point;
}

void *procedure_body(void *p) {
    return caddr(p).val.point;
}

void *procedure_env(void *p) {
    return cadddr(p).val.point;
}

uint8_t is_primitive_procedure(void *p) {
    return tagged(p, "primitive");
}

void *make_primitive_procedure(void *pf) {
    /* ("primitive" pf) */
    element_t ele1 = PRIMITIVE;
    element_t ele2 = construct_point_element(pf);   /* 注：此处指针指向的是函数，而不是序对 */
    element_t elements[] = {ele1, ele2};
    return list(2, elements);
}

primitive_pf primitive_procedure_impl(void *p) {
    return cadr(p).val.point;
}

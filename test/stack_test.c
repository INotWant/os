#include "stack_test.h"
#include "tools.h"
#include "../kernel/lisp/stack.h"

// 测试正常操作
static void test1(){
    element_t element1;
    element1.type = FLOAT_T;
    element1.val.fval = 10.1f;
    push(&element1);

    element_t element2;
    element2.type = INTEGER_T;
    element2.val.ival = 10;
    push(&element2);

    element_t new_element = pop();
    assert_eq(element2.type, new_element.type, "err: test1-1\n");
    assert_eq(element2.val.ival, new_element.val.ival, "err: test1-2\n");

    new_element = pop();
    assert_eq(element1.type, new_element.type, "err: test1-3\n");
    assert_eq(element1.val.fval, new_element.val.fval, "err: test1-4\n");
}

// 测试空栈 pop
static void test2(){
    element_t element = pop();
    assert_eq(element.type, POINT_PAIR_T, "err: test2-1\n");
    assert_eq(element.val.point, 0, "err: test2-2\n");
}

void stack_test() {
    kprint("stack_test start->\n");
    test1();
    test2();
    kprint("stack_test end<-\n");
}

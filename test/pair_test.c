#include "pair_test.h"
#include "tools.h"
#include "../kernel/lisp/pair.h"

// 测试初始化
static void test1() {
    element_t car_ep;
    car_ep.type = INTEGER_T;
    car_ep.val.ival = 100;

    element_t cdr_ep;
    cdr_ep.type = FLOAT_T;
    cdr_ep.val.fval = 10.f;
    void *pp = cons(&car_ep, &cdr_ep);
    assert_neq(pp, 0, "err: test1\n");
}

// 整型 浮点型
static void test2() {
    element_t car_ep;
    car_ep.type = INTEGER_T;
    car_ep.val.ival = 100;

    element_t cdr_ep;
    cdr_ep.type = FLOAT_T;
    cdr_ep.val.fval = 10.f;
    void *pp = cons(&car_ep, &cdr_ep);

    element_t car_ep_ = car(pp);
    assert_eq(car_ep.type, car_ep_.type, "err: test2-1\n");    
    assert_eq(car_ep.val.ival, car_ep_.val.ival, "err: test2-2\n");    

    element_t cdr_ep_ = cdr(pp);
    assert_eq(cdr_ep.type, cdr_ep_.type, "err: test2-3\n");
    assert_eq(cdr_ep.val.fval, cdr_ep_.val.fval, "err: test2-4\n");
}

// 字符串
static void test3() {
    element_t car_ep;
    car_ep.type = STRING_T;
    char *src_p = "abc";
    car_ep.val.point = src_p;

    char *chars = "def";
    element_t cdr_ep;
    cdr_ep.type = STRING_T;
    cdr_ep.val.point = chars;
    void *pp = cons(&car_ep, &cdr_ep);

    element_t car_ep_ = car(pp);
    assert_eq(car_ep.type, car_ep_.type, "err: test3-1\n");
    char *dest_p = car_ep_.val.point;
    int i = 0;
    while(src_p[i] != '\0') {
        if (src_p[i] != dest_p[i]){
            kprint("err: test3-2\n");
            break;
        }
        ++i;
    }
    if (dest_p[i] != '\0')
        kprint("err: test3-2\n");

    element_t cdr_ep_ = cdr(pp);
    assert_eq(cdr_ep.type, cdr_ep_.type, "err: test3-3\n");
    assert_eq(chars, cdr_ep_.val.point, "err: test3-4\n");
}

// 套娃
static void test4() {
    element_t ep1;
    ep1.type = INTEGER_T;
    ep1.val.ival = 1;

    element_t ep2;
    ep1.type = POINT_PAIR_T;
    ep1.val.point = 0;

    void *pp1 = cons(&ep1, &ep2);
    
    element_t ep_1;
    ep_1.type = INTEGER_T;
    ep_1.val.ival = 2;

    element_t ep_2;
    ep_2.type = POINT_PAIR_T;
    ep_2.val.point = pp1;

    void *pp2 = cons(&ep_1, &ep_2);

    element_t ep_2_ = cdr(pp2);
    assert_eq(ep_2.type, ep_2_.type, "err: test4-1\n");
    assert_eq(pp1, ep_2_.val.point, "err: test4-2\n");
}

// 测试 GC 1
static void test5() {
    pair_destory();
    pair_init();
    element_t ep;
    ep.type = INTEGER_T;
    ep.val.ival = 2;

    void *p;
    for (int i = 0; i < PAIR_MAX_NUMBER + 2; i++) {
        p = cons(&ep, &ep);
        assert_neq(p, 0, "err: test5\n");
    }
}

// 测试 GC 2
static void test6() {
    pair_destory();
    pair_init();

    element_t ep1;
    ep1.type = INTEGER_T;
    ep1.val.ival = 2;
    element_t ep2;
    ep2.type = POINT_PAIR_T;
    ep2.val.point = 0;
    void *p1 = cons(&ep1, &ep2);
    void *p2 = cons(&ep1, &ep2);
    void *p3 = cons(&ep1, &ep2);
    void *p4 = cons(&ep1, &ep2);
    void *p5 = cons(&ep1, &ep2);

    element_t ep;
    ep.type = POINT_PAIR_T;
    ep.val.point = p2;
    set_cdr(p1, &ep);
    ep.val.point = p3;
    set_cdr(p2, &ep);
    ep.val.point = p4;
    set_cdr(p3, &ep);
    ep.val.point = p5;
    set_cdr(p4, &ep);

    void *p = cons(&ep1, &ep1);
    assert_neq(p, 0, "err: test6-1\n");

    p = cons(&ep1, &ep1);
    assert_neq(p, 0, "err: test6-2\n");
}

// 测试 GC 3
static void test7() {
    pair_destory();
    pair_init();

    element_t ep1;
    ep1.type = INTEGER_T;
    ep1.val.ival = 2;
    element_t ep2;
    ep2.type = POINT_PAIR_T;
    ep2.val.point = 0;
    void *p1 = cons(&ep1, &ep2);
    void *p2 = cons(&ep1, &ep2);
    void *p3 = cons(&ep1, &ep2);
    void *p4 = cons(&ep1, &ep2);
    void *p5 = cons(&ep1, &ep2);
    void *p6 = cons(&ep1, &ep2);

    element_t ep;
    ep.type = POINT_PAIR_T;
    ep.val.point = p2;
    set_cdr(p1, &ep);
    ep.val.point = p3;
    set_cdr(p2, &ep);
    ep.val.point = p4;
    set_cdr(p3, &ep);
    ep.val.point = p5;
    set_cdr(p4, &ep);
    ep.val.point = p6;
    set_cdr(p5, &ep);

    void *p = cons(&ep1, &ep1);
    assert_eq(p, 0, "err: test7-1\n");

    p = cons(&ep1, &ep1);
    assert_eq(p, 0, "err: test7-2\n");
}

void pair_test() {
    kprint("pair_test start->\n");
    test1();
    test2();
    test3();
    test4();
    test5();
    // test6();
    // test7();
    kprint("pair_test end<-\n");
}
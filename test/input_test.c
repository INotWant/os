#include "input_test.h"
#include "tools.h"
#include "../kernel/lisp/input.h"
#include "../kernel/lisp/pair.h"

// 测试 is_legal
static void test1() {
    assert_eq(is_legal("", 0), 0, "err: test1-1\n");
    assert_eq(is_legal(" ", 1), 0, "err: test1-2\n");
    assert_eq(is_legal("  ", 2), 0, "err: test1-3\n");
    assert_eq(is_legal("a", 1), 0, "err: test1-4\n");
    assert_eq(is_legal(" a", 2), 0, "err: test1-5\n");
    assert_eq(is_legal(" a ", 3), 0, "err: test1-6\n");
    assert_eq(is_legal(" a a", 4), -1, "err: test1-7\n");
    assert_eq(is_legal("a a", 3), -1, "err: test1-8\n");
    assert_eq(is_legal(")", 1), -1, "err: test1-9\n");
    assert_eq(is_legal(" ) ", 3), -1, "err: test1-10\n");
    assert_eq(is_legal("(", 1), 1, "err: test1-11\n");
    assert_eq(is_legal(" (", 2), 1, "err: test1-12\n");
    assert_eq(is_legal(" (a", 3), 1, "err: test1-13\n");
    assert_eq(is_legal(" ( a", 4), 1, "err: test1-14\n");
    assert_eq(is_legal("(a)", 3), 0, "err: test1-15\n");
    assert_eq(is_legal(" (a)", 4), 0, "err: test1-16\n");
    assert_eq(is_legal(" ( a ) ", 7), 0, "err: test1-17\n");
    assert_eq(is_legal(" ((a)", 5), 1, "err: test1-18\n");
    assert_eq(is_legal(" ()a)", 5), -1, "err: test1-19\n");
    assert_eq(is_legal(" (  a ))", 8), -1, "err: test1-20\n");
    assert_eq(is_legal(" (  a )a", 8), -1, "err: test1-21\n");
    assert_eq(is_legal("\n(\ta ))", 8), -1, "err: test1-22\n");
    assert_eq(is_legal("(\n(\ta ))", 8), 0, "err: test1-23\n");
}

// 测试 save_str_to_pair 1
static void test2() {
    assert_eq(save_str_to_pair("", 0), 0, "err: test2-1\n");
    assert_eq(save_str_to_pair("  ", 2), 0, "err: test2-2\n");
    assert_eq(save_str_to_pair("()", 2), 0, "err: test2-3\n");
    assert_eq(save_str_to_pair(" () ", 4), 0, "err: test2-4\n");
}

// 测试 save_str_to_pair 2
static void test3() {
    void *p = save_str_to_pair(" a ", 3);
    element_t element = car(p);
    assert_eq(element.type, STRING_SHORT_T, "err: test3-1\n");
    assert_eq(element.val.short_string[0], 'a', "err: test3-2\n");
    assert_eq(element.val.short_string[1], '\0', "err: test3-3\n");
    element = cdr(p);
    assert_eq(element.type, POINT_PAIR_T, "err: test3-4\n");
    assert_eq(element.val.point, 0, "err: test3-5\n");
}

// 测试 save_str_to_pair 3
static void test4() {
    void *p1 = save_str_to_pair(" abcdefg ", 9);
    element_t element = car(p1);
    assert_eq(element.type, STRING_LONG_T, "err: test4-1\n");
    assert_eq(((char *)element.val.point)[0], 'a', "err: test4-2\n");
    assert_eq(((char *)element.val.point)[7], '\0', "err: test4-3\n");

    void *p2 = save_str_to_pair(" abcdefg ", 9);
    element_t element_ = car(p2);
    assert_eq(element.val.point, element_.val.point, "err: test4-4\n");
}

// 测试 save_str_to_pair 4
static void test5() {
    void *p1 = save_str_to_pair(" (a b) ", 7);
    element_t element = car(p1);
    assert_eq(element.val.short_string[0], 'a', "err: test5-1\n");
    element = car(cdr(p1).val.point);
    assert_eq(element.val.short_string[0], 'b', "err: test5-2\n");

    void *p2 = save_str_to_pair(" ( a b) ", 8);
    element = car(p2);
    assert_eq(element.val.short_string[0], 'a', "err: test5-3\n");
    element = car(cdr(p2).val.point);
    assert_eq(element.val.short_string[0], 'b', "err: test5-4\n");

    void *p3 = save_str_to_pair(" ( a b ) ", 9);
    element = car(p3);
    assert_eq(element.val.short_string[0], 'a', "err: test5-5\n");
    element = car(cdr(p3).val.point);
    assert_eq(element.val.short_string[0], 'b', "err: test5-6\n");
}

// 测试 save_str_to_pair 4
static void test6() {
    void *p1 = save_str_to_pair(" ((a) b) ", 9);
    element_t element = car(car(p1).val.point);
    assert_eq(element.val.short_string[0], 'a', "err: test6-1\n");
    element = car(cdr(p1).val.point);
    assert_eq(element.val.short_string[0], 'b', "err: test6-2\n");

    void *p2 = save_str_to_pair(" (a (b) ) ", 10);
    element = car(car(cdr(p2).val.point).val.point);
    assert_eq(element.val.short_string[0], 'b', "err: test6-3\n");
    element = cdr(car(cdr(p2).val.point).val.point);
    assert_eq(element.type, POINT_PAIR_T, "err: test6-4\n");
    assert_eq(element.val.point, 0, "err: test6-5\n");

    void *p3 = save_str_to_pair(" (() a) ", 8);
    element = car(p3);
    assert_eq(element.type, POINT_PAIR_T, "err: test6-6\n");
    assert_eq(element.val.point, 0, "err: test6-7\n");
}

void input_test() {
    kprint("input_test start->\n");
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    kprint("input_test end<-\n");
}

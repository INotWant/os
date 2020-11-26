#include "string_test.h"
#include "tools.h"
#include "../libc/string.h"

// 测试字符串常量池
static void test1() {
    char *str = "HELLO";
    char *str_pool1 = put_string_to_constant_pool(str);
    char *str_pool2 = put_string_to_constant_pool(str);
    assert_eq(str_pool1, str_pool2, "err: test1\n");
}

// 测试 str2int32
static void test2() {
    assert_eq(str2int32("-11", 3), -11, "err: test2-1\n");
    assert_eq(str2int32("11", 2), 11, "err: test2-2\n");
    assert_eq(str2int32("0", 1), 0, "err: test2-3\n");

    assert_eq(str2int32("-2147483648", 11), -2147483648, "err: test2-4\n");
    assert_eq(str2int32("2147483647", 10), 2147483647, "err: test2-5\n");
}

// 测试 str2float
static void test3() {
    assert_eq(str2float("0.0", 3), 0.0, "err: test3-1\n");
    assert_eq(str2float("-.0", 3), -0.0, "err: test3-2\n");
    assert_eq(str2float("1.0", 3), 1.0, "err: test3-3\n");
    assert_eq(str2float(".02", 3), .02, "err: test3-4\n");
    assert_eq(str2float("20.02", 5), 20.02, "err: test3-5\n");
    if (str2float("-20.02", 6) - (-20.02) > 1e-6)
        kprint("err: test3-6\n");
}

// 测试 int2str
static void test4() {
    char str[12];
    int2str(0, str);
    assert_eq(strcmp(str, "0"), 0, "err: test4-1\n");
    int2str(INT32_MIN, str);
    assert_eq(strcmp(str, "-2147483648"), 0, "err: test4-2\n");
    int2str(-INT32_MAX, str);
    assert_eq(strcmp(str, "-2147483647"), 0, "err: test4-3\n");
    int2str(INT32_MAX, str);
    assert_eq(strcmp(str, "2147483647"), 0, "err: test4-4\n");
}

// 测试 float2str
static void test5() {
    char str[20];
    float2str(-5.6f, str, 0);
    assert_eq(strcmp(str, "-5"), 0, "err: test5-1\n");
    float2str(-5.6f, str, 1);
    assert_eq(strcmp(str, "-5.5"), 0, "err: test5-2\n");
    float2str(5.6f, str, 1);
    assert_eq(strcmp(str, "5.5"), 0, "err: test5-3\n");
    float2str(5.6f, str, 2);
    assert_eq(strcmp(str, "5.59"), 0, "err: test5-4\n");
}

// 测试 int2hex_str
static void test6() {
    char str[11];
    int2hex_str(0, str);
    assert_eq(strcmp(str, "0x00000000"), 0, "err: test6-1\n");
    int2hex_str(0x3, str);
    assert_eq(strcmp(str, "0x00000003"), 0, "err: test6-2\n");
    int2hex_str(0xffffffff, str);
    assert_eq(strcmp(str, "0xffffffff"), 0, "err: test6-3\n");
}

void string_test() {
    kprint("string_test start->\n");
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    kprint("string_test end<-\n");
}
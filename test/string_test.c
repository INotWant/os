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

void string_test() {
    kprint("string_test start->\n");
    test1();
    test2();
    test3();
    kprint("string_test end<-\n");
}
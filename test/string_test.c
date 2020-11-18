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

void string_test() {
    kprint("hash_table_test start->\n");
    test1();
    kprint("hash_table_test end<-\n");
}
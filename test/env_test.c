#include "env_test.h"
#include "tools.h"
#include "../kernel/lisp/env.h"
#include "../kernel/lisp/pair.h"
#include "../kernel/lisp/constant.h"
#include "../libc/string.h"

static void *env;

// 测试 lookup_variable_value 查找不存在元素
static void test1() {
    element_t element = lookup_variable_value("0-0", env);
    assert_eq(element.type, NON_EXIST_T, "err: test1\n");
}

// 测试 extend_env & lookup_variable_value & define_var
static void test2() {
    element_t ele1 = construct_string_element("0-0");
    element_t ele2 = ZERO_POINT;
    void *var_names = cons(&ele1, &ele2);
    ele1 = construct_string_element("v1");
    void *values = cons(&ele1, &ele2);
    env = extend_env(var_names, values, 0);
    element_t ret = lookup_variable_value("0-0", env);
    assert_eq(ele1.type, ret.type, "err: test2-1\n");
    assert_eq(strcmp(ele1.val.point, ret.val.point), 0, "err: test2-2\n");

    element_t ele1_new = construct_string_element("v2     ");
    define_var("0-0", &ele1_new, env);  /* define，已存在则覆盖 */
    ret = lookup_variable_value("0-0", env);
    assert_eq(ele1_new.type, ret.type, "err: test2-3\n");
    assert_eq(strcmp(ele1_new.val.point, ret.val.point), 0, "err: test2-4\n");

    ele1_new = construct_string_element("v3     ");
    define_var("0-1", &ele1_new, env);  /* define，不存在的 */
    ret = lookup_variable_value("0-1", env);
    assert_eq(ele1_new.type, ret.type, "err: test2-5\n");
    assert_eq(strcmp(ele1_new.val.point, ret.val.point), 0, "err: test2-6\n");
}

// 测试 set_var_value
static void test3() {
    uint8_t flag = set_var_value("0-2", 0, env);    /* set，不存在的 */
    assert_eq(flag, 0, "err: test3-1\n");

    element_t value = construct_string_element("v4     ");
    set_var_value("0-1", &value, env);              /* set，已存在的 */
    element_t ret = lookup_variable_value("0-1", env);
    assert_eq(value.type, ret.type, "err: test3-2\n");
    assert_eq(strcmp(value.val.point, ret.val.point), 0, "err: test3-3\n");
}

// 测试 extend_env
static void test4() {
    element_t ele1 = construct_string_element("1-0");
    element_t ele2 = ZERO_POINT;
    void *var_names = cons(&ele1, &ele2);
    ele1 = construct_string_element("v5     ");
    void *values = cons(&ele1, &ele2); 
    env = extend_env(var_names, values, env);

    element_t ret = lookup_variable_value("1-0", env);
    assert_eq(ele1.type, ret.type, "err: test4-1\n");
    assert_eq(strcmp(ele1.val.point, ret.val.point), 0, "err: test4-2\n");

    ret = lookup_variable_value("0-0", env);    /* 查找之前环境的 */
    assert_eq(ret.type, STRING_T, "err: test4-3\n");
    assert_eq(strcmp(ret.val.point, "v2     "), 0,"err: test4-4\n");
    
    ele1 = construct_string_element("v6     ");
    set_var_value("0-0", &ele1, env);           /* 设置之前环境的 */
    ret = lookup_variable_value("0-0", env);
    assert_eq(ret.type, STRING_T, "err: test4-5\n");
    assert_eq(strcmp(ret.val.point, ele1.val.point), 0, "err: test4-6\n");

    ele1 = construct_string_element("v7     ");
    define_var("0-0", &ele1, env);              /* 在新框架中定义之前已有的 */
    ret = lookup_variable_value("0-0", env);
    assert_eq(ret.type, STRING_T, "err: test4-7\n");
    assert_eq(strcmp(ret.val.point, ele1.val.point), 0, "err: test4-8\n");
}

void env_test() {
    kprint("env_test start->\n");
    test1();
    test2();
    test3();
    test4();
    kprint("env_test end<-\n");
}

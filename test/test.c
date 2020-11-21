#include "test.h"
#include "mem_test.h"
#include "pair_test.h"
#include "stack_test.h"
#include "hash_table_test.h"
#include "string_test.h"
#include "input_test.h"
#include "env_test.h"

void test_main() {
    /* 有关内存管理的测试 */
    mem_test();
    /* 有关序对的测试 */
    pair_test();
    /* 有关栈的测试 */
    stack_test();
    /* 有关哈希表的测试 */
    hash_table_test();
    /* 有关字符串的测试 */
    string_test();
    /* 有关 input 的测试 */
    input_test();
    /* 有关 env 的测试 */
    env_test();
}

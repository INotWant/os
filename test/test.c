#include "test.h"
#include "mem_test.h"
#include "pair_test.h"
#include "stack_test.h"
#include "hash_table_test.h"

void test_main() {
    /* 有关内存管理的测试 */
    mem_test();
    /* 有关序对的测试 */
    pair_test();
    /* 有关栈的测试 */
    stack_test();
    /* 有关哈希表的测试 */
    hash_table_test();
}

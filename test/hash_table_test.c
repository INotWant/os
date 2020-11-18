#include "hash_table_test.h"
#include "tools.h"
#include "../libc/hash_table.h"
#include "../drivers/screen.h"

static void test1() {
    hash_table ht;
    new_hash_table(&ht, 0);
    char *key1 = "gao";
    void *val1 = (void *)1;
    hash_table_put(&ht, key1, val1);
    kv* kv = hash_table_get(&ht, key1);
    assert_eq(val1, kv->val, "err: test1-1\n");

    hash_table_remove(&ht, key1);
    kv = hash_table_get(&ht, key1);
    assert_eq(kv, 0, "err: test2-1\n");
    free_hash_table(&ht);
}

void hash_table_test() {
    kprint("hash_table_test start->\n");
    test1();
    kprint("hash_table_test end<-\n");
}
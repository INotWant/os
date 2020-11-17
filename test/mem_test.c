#include "mem_test.h"
#include "tools.h"
#include "../libc/mem.h"
#include "../drivers/screen.h"

#define WSIZE 4
#define GET(p) (*(uint32_t *)(p))
/** 获取块大小 **/
#define GET_SIZE(p) (GET(p) & ~0x7)
/** 根据申请返回指针，求对应块的头部指针 **/
#define HDRP(bp) ((char *)(bp) - WSIZE)

// 测试内存是否正常释放
static void test1() {
    memory_init();

    void *p1 = memory_malloc(1);
    memory_free(p1);
    void *p2 = memory_malloc(12);
    assert_eq(p1, p2, "err: test1\n");
    memory_free(p1);
}

// 测试合并1
static void test2() {
    memory_init();

    void *p1 = memory_malloc(1);
    void *p2 = memory_malloc(1);
    memory_free(p1);
    memory_free(p2);

    p1 = (void *)START_MEMORY_ADDRESS;
    size_t size = GET_SIZE(p1);
    assert_eq(size, 32, "err: test2-1\n");

    p1 = memory_malloc(1);
    assert_eq(HDRP(p1), START_MEMORY_ADDRESS, "err: test2-2\n");
    memory_free(p1);
}

// 测试合并2
static void test3() {
    memory_init();

    void *p1 = memory_malloc(1);
    void *p2 = memory_malloc(1);
    void *p3 = memory_malloc(1);
    memory_free(p1);
    memory_free(p3);

    void *p4 = memory_malloc(1);
    assert_eq(p3, p4, "err: test3-1\n");
    memory_free(p4);
    
    memory_free(p2);
    size_t size = GET_SIZE(HDRP(p1));
    assert_eq(size, 48, "err: test3-2\n");

    p1 = memory_malloc(37);
    assert_eq(HDRP(p1), START_MEMORY_ADDRESS, "err: test3-3\n");
    memory_free(p1);
}

// 测试合并&分割
static void test4() {
    memory_init();

    void *p1 = memory_malloc(1);
    void *p2 = memory_malloc(13);
    void *p3 = memory_malloc(1);
    void *p4 = memory_malloc(1);

    memory_free(p1);
    memory_free(p2);
    memory_free(p3);
    memory_free(p4);

    size_t size = GET_SIZE(HDRP(p1));
    assert_eq(size, 72, "err: test4-1\n");

    p1 = memory_malloc(1);
    assert_eq(HDRP(p1), START_MEMORY_ADDRESS, "err: test4-2\n");

    p2 = memory_malloc(32);
    size = GET_SIZE(HDRP(p2));
    assert_eq(size, 40, "err: test4-3\n");
    
    memory_free(p1);
    memory_free(p2);
}

// 测试分配边界1
static void test5() {
    memory_init();

    size_t max_size = (END_MEMORY_ADDRESS - START_MEMORY_ADDRESS) / 8 * 8 - 4;  /* 最大可分配空间 */
    void *p1 = memory_malloc(max_size);
    assert_eq(GET_SIZE(HDRP(p1)), max_size + 4, "err: test5-1\n");

    void *p2 = memory_malloc(1);
    assert_eq(0, p2, "err: test5-2\n");
    
    memory_free(p1);
    p2 = memory_malloc(1);
    assert_eq(START_MEMORY_ADDRESS, HDRP(p2), "err: test5-3\n");

    memory_free(p2);
}

// 测试分配边界2
static void test6() {
    memory_init();

    size_t unable_size = (END_MEMORY_ADDRESS - START_MEMORY_ADDRESS) / 8 * 8;
    void *p = memory_malloc(unable_size);
    assert_eq(p, 0, "err: test6\n");
}

void mem_test() {
    kprint("mem_test start->\n");
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    kprint("mem_test end<-\n");
}
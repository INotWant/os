#include "mem.h"

void memory_copy(uint8_t *source, uint8_t *dest, size_t nbytes) {
    for (size_t i = 0; i < nbytes; i++) {
        dest[i] = source[i];
    }
}

void memory_set(uint8_t *dest, uint8_t val, size_t len) {
    for (size_t i = 0; i < len; i++){
        dest[i] = val;
    }
}

/**********************************************************
 *                  memory management                     *
 **********************************************************/

static char *mem_start;             /* 指向被管理内存的初始位置 */
static char *mem_brk;               /* 指向当前可管理内存的最大位置加 1 */
static char *mem_max_addr;          /* 指向可被管理内存的最大位置加 1 */
static uint32_t *last_chunk_point;  /* 指向最后一个块 */

/** 空闲链表数组 **/
static uint32_t *free_chunk_points[10];

void memory_init() {
    mem_start = (char *)START_MEMORY_ADDRESS;
    mem_brk = (char *)START_MEMORY_ADDRESS;
    mem_max_addr = (char *)END_MEMORY_ADDRESS;  /* 使得最后一字永不可用 */
}

// 获取所申请内存大小位于空闲链表数组的档次
static uint8_t compute_memory_level(size_t size) {
    uint8_t level = 0;
    uint32_t acc = 12;
    while (size > acc) {
        acc = 2 * acc - 4;
        ++level;
    }
    return level;
}

// 从空闲链表中获取第一个满足大小的块
static uint32_t *find_first_appro_chunk(uint32_t *fcp, size_t size) {
    uint32_t *curr_fcp = fcp;
    while (curr_fcp != 0){
        size_t usable_size = GET_USABLE_SIZE(curr_fcp);
        if (usable_size >= size) 
            return curr_fcp;
        curr_fcp = SUCCP_FROM_HDRP(curr_fcp);
    }
    return 0;
}

// 计算满足待申请内存的块的最小大小
static size_t compute_chunk_size(size_t size) {
    size_t chunk_size = ((size + 3) / 8 + 1) * 8;
    chunk_size = chunk_size < 16 ? 16 : chunk_size; /* 需要扩展的大小（带头部） */
    return chunk_size;
}

// 扩展可管理的内存大小
static void *extend(size_t size){
    size_t extend_size = compute_chunk_size(size);
    if ((size_t)(mem_max_addr - mem_brk) < extend_size) {         /* out of memory */
        // TODO 错误码
        return 0;
    }

    uint32_t *hp = (uint32_t *)mem_brk;
    uint8_t flags = mem_start != mem_brk && GET_ALLOC(last_chunk_point) == 1 ? 0x3 : 0x1;

    PUT(hp, PACK(extend_size, flags));
    PUT(hp + 1, last_chunk_point);
    PUT(hp + 2, 0);
    if (last_chunk_point != 0)
        PUT(last_chunk_point + 2, hp);

    last_chunk_point = hp;
    mem_brk += extend_size;
    return hp;
}

// 从空闲链表中删除将要被分配的块
static void delete_free_chunks(uint32_t **root_pp, uint32_t *delete_fcp) {
    uint32_t *pre_chunk = PREP_FROM_HDRP(delete_fcp);
    uint32_t *succ_fcp = SUCCP_FROM_HDRP(delete_fcp);
    if (*root_pp == delete_fcp) {       /* 删除头结点 */ 
        PUT(root_pp, succ_fcp);
    }else {
        PUT(pre_chunk + 2, succ_fcp);
    }
    if (succ_fcp != 0)
        PUT(succ_fcp + 1, pre_chunk);
}

// 把指定块插入到合适 “档” 的空闲链表中
static void insert_free_chunks(uint32_t *chunk_point) {
    size_t size = GET_SIZE(chunk_point) - WSIZE;    /* 减去头部大小 */
    uint8_t level = compute_memory_level(size);
    uint32_t *root = free_chunk_points[level];

    free_chunk_points[level] = chunk_point;
    PUT(PREP_FROM_HDRP(chunk_point), 0);
    PUT(SUCCP_FROM_HDRP(chunk_point), root);
    if (root != 0) 
        PUT(PREP_FROM_HDRP(root), chunk_point);
}

// 分割 --> 把一整块都分配出去造成浪费，故把整块分成两块（分配块+剩余块）。最后返回指向剩余块的指针
static uint32_t *segment(uint32_t *chunk_point, size_t remain_chunk_size) {
    size_t total_size = GET_SIZE(chunk_point);
    size_t num = (total_size - remain_chunk_size) / 4;
    uint32_t *remain_chunk_point = chunk_point + num;

    PUT(remain_chunk_point, PACK(remain_chunk_size, 0x2));                  /* 设置头部 --> flags：上一块已分配，当前块未分配 */
    PUT(FTRP_FROM_HDRP(remain_chunk_point), PACK(remain_chunk_size, 0x20)); /* 设置脚部 */
    return remain_chunk_point;
}

void *memory_malloc(size_t size) {
    if (size == 0) 
        return 0;
    uint8_t level = compute_memory_level(size);
    int i = level;
    uint32_t *appro_fcp = 0;
    for(; i < 10; i++) {
        uint32_t *fcp = free_chunk_points[i];
        if (fcp == 0) 
            continue;
        else {
            appro_fcp = find_first_appro_chunk(fcp, size);
            if (appro_fcp != 0)
                break;
        }
    }
    if (appro_fcp == 0) {   /* 无可用块，去扩展 */
        uint32_t *hp = extend(size);
        if (hp == 0)
            return 0;
        else
            return hp + 1;
    } else {                /* 已找到可用块，进行分割 */
        if (appro_fcp != last_chunk_point) {                            /* 非最后一块，告诉下一块 “已分配” */
            uint32_t *next_cp = (uint32_t *) NEXT_HDRP(appro_fcp + 1);  /* 下一块指针（无论是否已分配） */
            SET_PRE_ALLOCATED(next_cp);
        }
        size_t return_chunk_size;
        size_t min_chunk_size = compute_chunk_size(size);
        size_t appro_chunk_size = GET_SIZE(appro_fcp);
        size_t remain_chunk_size = appro_chunk_size - min_chunk_size;
        if (remain_chunk_size >= 16) {              /* 剩余空间超过 16，可进行分割 */
            return_chunk_size = min_chunk_size;
            /* 切断空闲链表 */
            delete_free_chunks(&(free_chunk_points[i]), appro_fcp);
            /* 分割: 整块 -> (分配块+剩余块) */
            uint32_t *remain_chunk_point = segment(appro_fcp, remain_chunk_size);
            /* 把剩余块插入合适的空闲链表 */
            insert_free_chunks(remain_chunk_point);
        } else {
            return_chunk_size = appro_chunk_size;
            /* 切断空闲链表 */
            delete_free_chunks(&(free_chunk_points[i]), appro_fcp);
        }
        uint8_t flags = 0x1;
        if (GET_PRE_ALLOC(appro_fcp))               /* 根据之前的标志判断上一块是否已分配 */
            flags = 0x3;
        PUT(appro_fcp, PACK(return_chunk_size, flags));
        return appro_fcp + 1;
    }
}

static uint32_t *merge(uint32_t *cp1, uint32_t *cp2) {
    uint32_t *new_chunk_point = cp1;
    size_t new_chunk_size = GET_SIZE(cp1) + GET_SIZE(cp2);
    uint32_t val = PACK(new_chunk_size, GET_FLAGS(cp1));
    PUT(new_chunk_point, val);                 /* 设置合并后的头部 */
    PUT(FTRP_FROM_HDRP(new_chunk_point), val); /* 设置合并后的脚部 */
    return new_chunk_point;
}

void memory_free(void *bp) {
    if (bp < (void *)mem_start || bp >= (void *)mem_brk)    /* bp 不在管理的内存空间内 */
        return;

    uint32_t *chunk_point = (uint32_t *)HDRP(bp);
    uint32_t *prev_cp;
    uint32_t *new_chunk_point = chunk_point;
    uint8_t hasMerge = 0;   /* 是否存在合并 */
    if (chunk_point != (uint32_t *)mem_start) { /* 非第一块 */
        prev_cp = (uint32_t *)PREV_HDRP(bp);
        if (!GET_ALLOC(prev_cp)) {          /* 且前一块未分配 */
            hasMerge = 1;
            /* 从空闲链表中删除前一块 */
            uint8_t level = compute_memory_level(GET_SIZE(prev_cp) - WSIZE);
            delete_free_chunks(&(free_chunk_points[level]), prev_cp);
            /* 合并 */
            new_chunk_point = prev_cp;
            new_chunk_point = merge(new_chunk_point, chunk_point);
        }
    }

    uint32_t *next_cp = 0;
    if (chunk_point != last_chunk_point) {      /* 非最后一块 */
        next_cp = (uint32_t *) NEXT_HDRP(bp);
        if (!GET_ALLOC(next_cp)) {          /* 且后一块未分配 */
            hasMerge = 1;
            /* 从空闲链表中删除后一块 */
            uint8_t level = compute_memory_level(GET_SIZE(next_cp) - WSIZE);
            delete_free_chunks(&(free_chunk_points[level]), next_cp);
            /* 合并 */
            new_chunk_point = merge(new_chunk_point, next_cp);
            /* 维护 last_chunk_point */
            if (next_cp == last_chunk_point)
                last_chunk_point = new_chunk_point;
        }
    }

    if (hasMerge)
        insert_free_chunks(new_chunk_point);    /* 把合并得到的新块插入到恰当的空闲链表 */
}
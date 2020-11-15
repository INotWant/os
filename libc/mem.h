#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <stddef.h>

/**
 * 内存拷贝
 * @param: source --> 源地址
 * @param: dest   --> 目的地址
 * @param: nbytes --> 拷贝的字节数
 */
void memory_copy(uint8_t *source, uint8_t *dest, size_t nbytes);

/**
 * 为内存中指定地址赋指定值指定次数次
 * @param: dest --> 指定地址
 * @param: val  --> 指定值
 * @param: len  --> 指定次数
 */
void memory_set(uint8_t *dest, uint8_t val, size_t len);


/**********************************************************
 *                  memory management                     *
 **********************************************************/

/**
 * 内存管理实现原理：分离的显示空闲链表（CSAPP，p604）
 * 
 * 块结构
 *  31                  3     0
 *  +-------------------+-----+
 *  |       块 大 小     | a/f | 头 部 --> 标志（保留、上一块是否已分配、当前块是否已分配）
 *  +-------------------+-----+
 *  |           pre           |
 *  +-------------------------+
 *  |           succ          |
 *  +-------------------------+
 *  |                         |
 *  |                         |
 *  +-------------------+-----+
 *  |       块 大 小     | a/f | 脚 部 --> 仅存在于空闲块
 *  +-------------------+-----+
 * 注：
 *  1) 块大小是 8(DSIZE) 的倍数，故块地址的末尾3个0
 * 
 * 空闲链表数组，共分为 10 档。
 * 分别为（注：有效载荷，即不含头部）：
 *  第 0 档，   1-12
 *  第 1 档，   13-20
 *  第 2 档，   21-36
 *  第 3 档，   37-68
 *  第 4 档，   69-132
 *  第 5 档，   133-516
 *  第 6 档，   517-1028
 *  第 7 档，   1029-2052
 *  第 8 档，   2053-4000
 *  第 9 档，   4100-...
 * 
 * [初始化]
 *    所有空闲链表为空
 * [内存申请流程]
 *    1）从最小可满足的申请大小的那档空闲链表开始，去搜索最先适配的块
 *    2.1）若未找到，去扩展 --> 扩展时采用 “拿恰好满足申请大小” 的策略
 *    2.2) 若已找到，则利用它去分配 --> 此时可能存在块分割情况，整块分为（分配块+剩余块）
 * [内存释放流程]
 *    主要思路：查看 “前一块” 与 “后一块” (指连读的内存块) 是否是未分配的，若是则合并
 */

/** 内存管理的起始地址，考虑 内部布局 **/
#define START_MEMORY_ADDRESS 0x100000
/** 内存管理的结束地址 **/
#define END_MEMORY_ADDRESS 0xffffffff

/** 单字 **/
#define WSIZE 4
/** 双字 **/
#define DSIZE (WSIZE * 2)

/** 把块大小和标志组合起放于头部 **/
#define PACK(size, flags) ((size) | (flags))

#define GET(p) (*(uint32_t *)(p))
#define PUT(p, val) (*(uint32_t *)(p) = (uint32_t)val)

/** 获取块大小 **/
#define GET_SIZE(p) (GET(p) & ~0x7)
/** 获取块可放最大负载的大小 **/
#define GET_USABLE_SIZE(p) (GET_SIZE(p) - WSIZE)
/** 获取头部标志 **/
#define GET_FLAGS(p) (GET(p) & 0x7)
/** 获取当前是否已分配 **/
#define GET_ALLOC(p) (GET(p) & 0x1)
/** 获取上一块是否已分配 **/
#define GET_PRE_ALLOC(p) (GET(p) & 0x2)
/** 设置标志位标识上一块已被分配 **/
#define SET_PRE_ALLOCATED(p) PUT(p, PACK(GET_SIZE(p), ((*(uint32_t *)(p) & 0x7) | 0x2)))
/** 设置标志位标识上一块未被分配 **/
#define SET_PRE_UNALLOCATED(p) PUT(p, PACK(GET_SIZE(p), (*(uint32_t *)(p) & 0x5))

/** 根据申请返回指针，求对应块的头部指针 **/
#define HDRP(bp) ((char *)(bp) - WSIZE)
/** 根据头部指针，求脚部指针 **/
#define FTRP_FROM_HDRP(hp) ((char *)(hp) + GET_SIZE(hp) - WSIZE)

/** 根据申请返回指针，求下一块的头部指针 **/
#define NEXT_HDRP(bp) HDRP((char *)(bp) + GET_SIZE(HDRP(bp)))
/** 根据申请返回指针，求上一块的头部指针 **/
#define PREV_HDRP(bp) HDRP((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))

/** 根据头部指针，求 pre 指针 **/
#define PREP_FROM_HDRP(hp) ((uint32_t *)(*((uint32_t *)(hp) + 1)))
/** 根据头部指针，求 succ 指针 **/
#define SUCCP_FROM_HDRP(hp) ((uint32_t *)(*((uint32_t *)(hp) + 2)))

/**
 * 初始化内存管理
 */
void memory_init();

/**
 * 申请内存
 * @param: size --> 指定申请内存大小
 */
void *memory_malloc(size_t size);

/**
 * 释放内存
 */
void memory_free(void *bp);

#endif
#ifndef HASH_TABLE_H
#define HASH_TABLE_H

/**
 * “哈希表” 实现
 * 
 * 注：基于 内存管理
 */

#include <stdint.h>
#include <stddef.h>

typedef struct _kv{
    struct _kv *p_next;
    char *key;
    void *val;
} kv;

typedef struct {
    kv **table;
    size_t table_size;              /* 哈希表大小 */
    size_t element_size;            /* 哈希表当前元素个数 */

    void (*free_val_hf)(void *);    /* 释放 value 时回调的函数 */
} hash_table;

/**
 * 构建一个哈希表
 * @return: 返回 0 表构建内存失败，非 0 表构建成功
 */
uint8_t new_hash_table(hash_table *htp, void (*free_value_hf)(void *));

/**
 * 释放哈希表
 */
void free_hash_table(hash_table *htp);

/**
 * put 新 kv
 * @return: 返回 0 表 put 失败，非 0 表成功
 */
uint8_t hash_table_put(hash_table *htp, char *key, void *value);

/**
 * 根据 key 查找 value
 * @return: 返回 0 表未找到
 */
void *hash_table_get(hash_table *htp, char *key);

/**
 * 依据 key 删除对应键值对
 */
void hash_table_remove(hash_table *htp, char *key);

#endif
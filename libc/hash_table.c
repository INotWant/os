#include "hash_table.h"
#include "mem.h"
#include "string.h"

/** 默认哈希表大小 **/
#define DEFAULT_TABLE_SIZE 16
/** 装载因子 **/
#define LOAD_FACTOR 0.75

uint8_t new_hash_table(hash_table *htp, void (*free_value_hf)(void *)) {
    htp->table_size = DEFAULT_TABLE_SIZE;
    htp->element_size = 0;
    size_t size = sizeof(kv *) * DEFAULT_TABLE_SIZE;
    htp->table = (kv **)memory_malloc(size);
    if (htp->table == 0)
        return 0;
    memory_set((uint8_t *)htp->table, 0, size);
    htp->free_val_hf = free_value_hf;
    return 1;
}

static void kv_free(kv *p_kv, void (*free_value_hf)(void *)) {
    memory_free(p_kv->key);
    if (free_value_hf != 0)
        free_value_hf(p_kv->val);
    memory_free(p_kv);
}

// BKDR Hash --> 来自 The C Programming Language
static uint32_t hash_function(char *key) {
    uint32_t hash = 0;
    int i = 0;
    while(key[i] != '\0')
        hash = hash * 131 + key[i++];
    return hash;
}

static kv *kv_new(char *key, void *val) {
    kv *p_kv = memory_malloc(sizeof(kv));
    if (p_kv == 0)
        return 0;
    size_t key_len = strlen(key);
    char *p_new_key = memory_malloc(key_len + 1);
    if (p_new_key == 0){
        memory_free(p_kv);
        return 0;
    }
    memory_copy((uint8_t *)key, (uint8_t *)p_new_key, key_len + 1);
    p_kv->key = p_new_key;
    p_kv->p_next = 0;
    p_kv->val = val;
    return p_kv;
}

void free_hash_table(hash_table *htp) {
    if (htp != 0) {
        for (size_t i = 0; i < htp->table_size; i++) {
            if (htp->table[i] != 0) {
                kv *p_curr = htp->table[i];
                kv *p_next;
                while (p_curr != 0){
                    p_next = p_curr->p_next;
                    kv_free(p_curr, htp->free_val_hf);
                    p_curr = p_next;
                }
            }
        }
        memory_free(htp->table);
    }
}

static void insert(kv** root_p, kv *curr){
    if (*root_p == 0){
        *root_p = curr;
        curr->p_next = 0;
    } else {
        curr->p_next = *root_p;
        *root_p = curr;
    }
}

static void rehash(hash_table *htp) {
    size_t new_table_size = htp->table_size * 2;
    size_t apply_size = new_table_size * sizeof(kv *);
    kv **new_tp = (kv **)memory_malloc(apply_size);
    if (new_tp == 0)     /* 无可用空间 */
        return;
    memory_set((uint8_t *)new_tp, 0, apply_size);
    memory_copy((uint8_t *)htp->table, (uint8_t *)new_tp, htp->table_size * sizeof(kv *));

    for (size_t i = 0; i < htp->table_size; i++) {
        if (htp->table[i] != 0) {
            kv *p_curr = htp->table[i];
            kv *p_next;
            while (p_curr != 0) {
                p_next = p_curr->p_next;
                int index = hash_function(p_curr->key) % new_table_size;
                insert(&(new_tp[index]), p_curr);
                p_curr = p_next;
            }
        }
    }
    
    memory_free(htp->table);
    htp->table = new_tp;
    htp->table_size = new_table_size;
}

uint8_t hash_table_put(hash_table *htp, char *key, void *value) {
    uint32_t hash = hash_function(key);
    int table_index = hash % htp->table_size;
    if (htp->table[table_index] == 0) {
        htp->table[table_index] = kv_new(key, value);
        if (htp->table[table_index] == 0)
            return 0;
    }else {
        kv *p_curr = htp->table[table_index];
        kv *p_pre;
        while (0 != p_curr) {
            if (strcmp(key, p_curr->key) == 0) {
                if (0 != htp->free_val_hf)
                    htp->free_val_hf(p_curr->val);
                p_curr->val = value;
                return 1;
            }
            p_pre = p_curr;
            p_curr = p_curr->p_next;
        }
        p_pre->p_next = kv_new(key, value);
        if (p_pre->p_next == 0)
            return 0;
    }
    htp->element_size += 1;
    if (htp->element_size > (size_t)(htp->table_size * LOAD_FACTOR))    /* 再哈希 */
        rehash(htp);
    return 1;
}

void *hash_table_get(hash_table *htp, char *key) {
    uint32_t hash = hash_function(key);
    int table_index = hash % htp->table_size;
    kv *p_curr = htp->table[table_index];
    while (0 != p_curr) {
        if (strcmp(key, p_curr->key) == 0) 
            return p_curr->val;
        p_curr = p_curr->p_next;
    }
    return 0;
}

void hash_table_remove(hash_table *htp, char *key) {
    uint32_t hash = hash_function(key);
    int table_index = hash % htp->table_size;
    if (0 != htp->table[table_index]) {
        kv *p_curr = htp->table[table_index];
        kv *p_pre = 0;
        while (0 != p_curr) {
            if (strcmp(key, p_curr->key) == 0)
                break;
            p_pre = p_curr;
            p_curr = p_curr->p_next;
        }
        if (0 != p_curr) {
            if (0 == p_pre) {
                htp->table[table_index] = p_curr->p_next;
            } else {
                p_pre->p_next = p_curr->p_next;
            }
            kv_free(p_curr, htp->free_val_hf);
            htp->element_size -= 1;
        }
    }
}

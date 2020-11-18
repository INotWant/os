#include "string.h"
#include "hash_table.h"

#include <stdint.h>

void int_to_ascii(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) {
        n = -n;
    }
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0)
        str[i++] = '-';
    str[i] = '\0';
    reverse(str);
}

void hex_to_ascii(int n, char str[]) {
    append(str, '0');
    append(str, 'x');
    char zeros = 0;

    uint32_t tmp;
    int i;
    for (i = 28; i > 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0)
            continue;
        zeros = 1;
        if (tmp > 0xA)
            append(str, tmp - 0xA + 'a');
        else
            append(str, tmp + '0');
    }

    tmp = n & 0xF;
    if (tmp >= 0xA)
        append(str, tmp - 0xA + 'a');
    else
        append(str, tmp + '0');
}

void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

int strlen(char s[]) {
    int i = 0;
    while (s[i] != '\0')
        ++i;
    return i;
}

void append(char s[], char n){
    int len = strlen(s);
    s[len] = n;
    s[len + 1] = '\0';
}

void backspace(char s[]){
    int len = strlen(s);
    s[len - 1] = '\0';
}

int strcmp(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0')
            return 0;
    }
    return s1[i] - s2[i];
}

/** 使用哈希表实现 “字符串常量池” **/
static hash_table ht_for_constant_pool;

char *put_string_to_constant_pool(char *str) {
    kv** table= ht_for_constant_pool.table;
    if (table == 0)
        new_hash_table(&ht_for_constant_pool, 0);
    kv *kvp = hash_table_get(&ht_for_constant_pool, str);
    if (kvp == 0) {
        kvp = hash_table_put(&ht_for_constant_pool, str, 0);
        return kvp == 0 ? 0 : kvp->key;
    }
    return kvp->key;
}

void remove_string_in_constant_pool(char *str) {
    kv** table= ht_for_constant_pool.table;
    if (table == 0)
        return;
    hash_table_remove(&ht_for_constant_pool, str);
}
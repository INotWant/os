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

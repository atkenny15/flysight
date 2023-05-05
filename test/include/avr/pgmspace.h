#pragma once

#include <stdint.h>
#include <stddef.h>

inline uint8_t pgm_read_byte(const void * addr) {
    (void) addr;
    return 0;
}

inline uint16_t pgm_read_word(const void *addr) {
    (void) addr;
    return 0;
}

inline void eeprom_read_block (void *dst, const void *src, size_t n) {
    (void) dst;
    (void) src;
    (void) n;
}

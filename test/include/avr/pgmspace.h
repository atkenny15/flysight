#pragma once

#include <cstdint>
#include <cstddef>

inline uint8_t pgm_read_byte(const char * addr) {
    return 0;
}

inline void eeprom_read_block (void *dst, const void *src, size_t n) {
}

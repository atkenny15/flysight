#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

uint8_t pgm_read_byte(const void* addr);

uint16_t pgm_read_word(const void* addr);

void eeprom_read_block(void* dst, const void* src, size_t n);

#ifdef __cplusplus
}
#endif

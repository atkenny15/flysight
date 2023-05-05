#include <avr/pgmspace.h>

uint8_t pgm_read_byte(const void* addr) {
    return 0;
}

uint16_t pgm_read_word(const void* addr) {
    return 0;
}

void eeprom_read_block(void* dst, const void* src, size_t n) {
}

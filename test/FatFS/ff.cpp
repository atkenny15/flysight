#include "FatFS/ff.h"

void f_putc(char c, FIL *file) {
}

FRESULT f_mkdir(const char * dir) {
    return FR_OK;
}

FRESULT f_chdir(const char * dir) {
    return FR_OK;
}

FRESULT f_open(FIL *file, const char *filename, const int perm) {
    return FR_OK;
}

void f_close(FIL *file) {
}

bool f_eof(FIL *file) {
    return true;
}

void f_gets(const void *buffer, size_t size, FIL *file) {
}

void f_puts(const void *buffer, FIL *file) {
}

void f_read(FIL *file, const void *buffer, size_t size, unsigned int *p) {
    if (p != NULL) {
        *p = 0;
    }
}

void f_lseek(FIL *file, unsigned int offset) {
}

void f_sync(FIL *file) {
}

void f_sync_1(FIL *file) {
}

void f_sync_2(FIL *file) {
}

void f_sync_3(FIL *file) {
}

int disk_is_ready (void) {
    return 1;
}

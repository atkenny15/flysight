#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DWORD int

#define FRESULT int
#define FR_OK 0

#define FA_READ (1 << 0)
#define FA_WRITE (1 << 1)
#define FA_CREATE_ALWAYS (1 << 2)

typedef struct {
    int x;
} FIL;

inline void f_putc(char c, FIL *file) {
    (void) c;
    (void) file;
}

inline FRESULT f_mkdir(const char * dir) {
    (void) dir;
    return FR_OK;
}

inline FRESULT f_chdir(const char * dir) {
    (void) dir;
    return FR_OK;
}

inline FRESULT f_open(FIL *file, const char *filename, const int perm) {
    (void) file;
    (void) filename;
    (void) perm;
    return FR_OK;
}

inline void f_close(FIL *file) {
    (void) file;
}

inline bool f_eof(FIL *file) {
    (void) file;
    return true;
}

inline void f_gets(const void *buffer, size_t size, FIL *file) {
    (void) buffer;
    (void) size;
    (void) file;
}

inline void f_puts(const void *buffer, FIL *file) {
    (void) buffer;
    (void) file;
}

inline void f_read(FIL *file, const void *buffer, size_t size, unsigned int *p) {
    (void) file;
    (void) buffer;
    (void) size;
    (void) p;
    if (p != NULL) {
        *p = 0;
    }
}

inline void f_lseek(FIL *file, unsigned int offset) {
    (void) file;
    (void) offset;
}

inline void f_sync(FIL *file) {
}

inline void f_sync_1(FIL *file) {
    (void) file;
}

inline void f_sync_2(FIL *file) {
    (void) file;
}

inline void f_sync_3(FIL *file) {
    (void) file;
}

inline int disk_is_ready (void) {
    return 1;
}

#ifdef __cplusplus
}
#endif

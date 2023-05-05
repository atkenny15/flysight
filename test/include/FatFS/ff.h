#pragma once

#define FRESULT int
#define FR_OK 0

#define FA_READ (1 << 0)
#define FA_WRITE (1 << 1)
#define FA_CREATE_ALWAYS (1 << 2)

struct FIL {};

inline void f_putc(char c, FIL *file) {
}

inline FRESULT f_chdir(const char * dir) {
    return FR_OK;
}

inline FRESULT f_open(FIL *file, const char *filename, const int perm) {
    return FR_OK;
}

inline void f_close(FIL *file) {
}

inline bool f_eof(FIL *file) {
    return true;
}

inline void f_gets(const char *buffer, size_t size, FIL *file) {
}

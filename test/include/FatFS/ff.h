#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define DWORD int

#define FRESULT int
#define FR_OK 0
#define FR_ERR 0

#define FA_READ (1 << 0)
#define FA_WRITE (1 << 1)
#define FA_CREATE_ALWAYS (1 << 2)

typedef struct {
    FILE* handle;
} FIL;

void f_putc(char c, FIL* file);

FRESULT f_mkdir(const char* dir);

FRESULT f_chdir(const char* dir);

FRESULT f_open(FIL* file, const char* filename, const int perm);

void f_close(FIL* file);

bool f_eof(FIL* file);

char * f_gets(void* buffer, size_t size, FIL* file);

void f_puts(const void* buffer, FIL* file);

void f_read(FIL* file, const void* buffer, size_t size, unsigned int* p);

void f_lseek(FIL* file, unsigned int offset);

void f_sync(FIL* file);

void f_sync_1(FIL* file);

void f_sync_2(FIL* file);

void f_sync_3(FIL* file);

int disk_is_ready(void);

#ifdef __cplusplus
}
#endif

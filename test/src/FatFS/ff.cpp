#include "FatFS/ff.h"

#include <filesystem>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <optional>
#include <string_view>

#define TODO()                \
    throw std::runtime_error( \
        fmt::format("TODO: {}:{} - {}"_cf, __FILE__, __LINE__, __PRETTY_FUNCTION__))

using fmt::literals::operator""_cf;

namespace {

std::string_view g_cwd;

std::string to_lower(const std::string_view s) {
    std::string ret;
    ret.reserve(s.size());
    for (const auto &c : s) {
        ret.push_back(std::tolower(c));
    }
    return ret;
}

}

void f_putc(char c, FIL* file) {
    TODO();
}

FRESULT f_mkdir(const char* dir) {
    TODO();
    return FR_OK;
}

FRESULT f_chdir(const char* dir) {
    std::string_view dir_sv(dir, strlen(dir));
    if (dir_sv == "\\") {
        g_cwd = dir_sv;
        return FR_OK;
    }

    fmt::print(stderr, "Unexpected directory: '{}'\n"_cf, dir_sv);
    return FR_ERR;
}

FRESULT f_open(FIL* file, const char* filename, const int perms) {
    const std::string_view filename_sv(filename, strlen(filename));
    const auto filename_lc = to_lower(filename_sv);

    if (g_cwd != "\\") {
        fmt::print(stderr, "Bad working directory: '{}'\n"_cf, g_cwd);
        return FR_ERR;
    }

    const auto path = std::filesystem::path(TEST_DATA_PATH);
    std::optional<std::filesystem::path> found;

    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (to_lower(entry.path().filename().string()) == filename_lc) {
            found = entry.path();
        }
    }

    if (!found || !std::filesystem::exists(*found)) {
        fmt::print(stderr, "Path does not exist: {}/{}\n"_cf, path.string(), filename);
        return FR_ERR;
    }

    const char* mode;
    if ((perms & FA_READ) != 0) {
        mode = "r";
    }
    else if ((perms & FA_WRITE) != 0) {
        mode = "w";
    }
    else {
        fmt::print(stderr, "Unsupported permissions: {}\n"_cf, perms);
        return FR_ERR;
    }

    file->handle = fopen(found->string().c_str(), mode);
    if (file->handle == NULL) {
        fmt::print(stderr, "Could not open file '{}': {}\n"_cf, found->string(), strerror(errno));
        return FR_ERR;
    }

    return FR_OK;
}

void f_close(FIL* file) {
    if (fclose(file->handle) != 0) {
        fmt::print(stderr, "Could not close file: {}\n"_cf, strerror(errno));
    }
    file->handle = NULL;
}

bool f_eof(FIL* file) {
    return feof(file->handle) != 0;
}

char * f_gets(void* buffer, size_t size, FIL* file) {
    return fgets(reinterpret_cast<char *>(buffer), size, file->handle);
}

void f_puts(const void* buffer, FIL* file) {
    TODO();
}

void f_read(FIL* file, const void* buffer, size_t size, unsigned int* p) {
    TODO();
}

void f_lseek(FIL* file, unsigned int offset) {
    TODO();
}

void f_sync(FIL* file) {
    TODO();
}

void f_sync_1(FIL* file) {
    TODO();
}

void f_sync_2(FIL* file) {
    TODO();
}

void f_sync_3(FIL* file) {
    TODO();
}

int disk_is_ready(void) {
    TODO();
    return 1;
}

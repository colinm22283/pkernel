#pragma once

#include <stdint.h>

#include <pkos/types.h>

#include <syscall_number.h>
#include <error_number.h>

#include <defs.h>

static inline fd_t open(const char * path, open_options_t options) {
    int64_t ret;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_OPEN), "S" ((uint64_t) path), "d" (options) : "memory", "cc");

    return ret;
}

static inline int64_t close(fd_t fd) {
    int64_t ret;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_CLOSE), "S" ((uint64_t) fd) : "memory", "cc");

    return ret;
}

static inline int64_t write(fd_t fd, const char * buffer, uint64_t size) {
    int64_t ret;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_WRITE), "S" (fd), "d" ((uint64_t) buffer), "c" (size) : "memory", "cc");

    return ret;
}

static inline int64_t read(fd_t fd, char * buffer, uint64_t size) {
    int64_t ret;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_READ), "S" (fd), "d" ((uint64_t) buffer), "c" (size) : "memory", "cc");

    return ret;
}

static inline int64_t seek(fd_t fd, int64_t offset, seek_origin_t origin) {
    int64_t ret;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_SEEK), "S" (fd), "d" (offset), "c" ((uint64_t) origin) : "memory", "cc");

    return ret;
}

static inline __NORETURN void exit(uint64_t code) {
    uint64_t ret;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_EXIT), "S" (code) : "memory", "cc");

    __UNREACHABLE();
}

static inline int64_t readdir(fd_t fd, directory_entry_t * entries, uint64_t size) {
    int64_t ret;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_READDIR), "S" (fd), "d" ((uint64_t) entries), "c" (size) : "memory", "cc");

    return ret;
}

static inline int64_t chdir(const char * path) {
    int64_t ret;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_CHDIR), "S" ((uint64_t) path) : "memory", "cc");

    return ret;
}

static inline pid_t fork(void) {
    int64_t ret;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_FORK) : "memory", "cc");

    return ret;
}

static inline int64_t exec(const char * path, const char ** argv, uint64_t argc) {
    int64_t ret;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_EXEC), "S" ((uint64_t) path), "d" ((uint64_t) argv), "c" ((uint64_t) argc) : "memory", "cc");

    return ret;
}

static inline void * map(fd_t fd, void * map_address, uint64_t size, uint64_t offset) {
    int64_t ret;

    register uint64_t r8 asm("r8") = offset;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_MAP), "S" (fd), "d" ((uint64_t) map_address), "c" (size), "r" (r8) : "memory", "cc");

    return (void *) ret;
}

static inline void * pipe(fd_t fds[2], open_options_t options) {
    int64_t ret;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_PIPE), "S" ((uint64_t) fds), "d" ((uint64_t) options) : "memory", "cc");

    return (void *) ret;
}

static inline void * dup(fd_t dst, fd_t src) {
    int64_t ret;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_DUP), "S" ((uint64_t) dst), "d" ((uint64_t) src) : "memory", "cc");

    return (void *) ret;
}

static inline error_number_t mount(const char * dst, const char * src, const char * fs, mount_options_t options, const char * data) {
    int64_t ret;

    register uint64_t r8 asm("r8") = options;
    register uint64_t r9 asm("r9") = (uint64_t) data;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_MOUNT), "S" ((uint64_t) dst), "d" ((uint64_t) src), "c" ((uint64_t) fs), "r" (r8), "r" (r9) : "memory", "cc");

    return ret;
}

static inline error_number_t unmount(const char * mount_point) {
    int64_t ret;

    asm volatile ("int $0x30" : "=a" (ret) : "a" (SYSCALL_UNMOUNT), "S" ((uint64_t) mount_point) : "memory", "cc");

    return ret;
}

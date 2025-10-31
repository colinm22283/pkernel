#pragma once

#include <stdint.h>

typedef int64_t fd_t;

typedef int64_t pid_t;

typedef enum {
    SO_BEG = 0,
    SO_END = 1,
    SO_CUR = 2,
} seek_origin_t;

typedef enum {
    FS_REGULAR,
    FS_DIRECTORY,
    FS_DEVICE,
    FS_PIPE,
    FS_SOCKET,
} fs_file_type_t;

typedef struct {
    fs_file_type_t file_type;
    uint64_t struct_size;

    char name[];
} directory_entry_t;

typedef uint64_t open_options_t;
#define OPEN_READ     ((open_options_t) 0b000000000000000000000000000000000000000000000000000001)
#define OPEN_WRITE    ((open_options_t) 0b000000000000000000000000000000000000000000000000000010)
#define OPEN_CREATE   ((open_options_t) 0b000000000000000000000000000000000000000000000000000100)
#define OPEN_NONBLOCK ((open_options_t) 0b000000000000000000000000000000000000000000000000001000)

typedef uint64_t map_options_t;
#define MAP_WRITE    ((map_options_t) 0b000000000000000000000000000000000000000000000000000001)
#define MAP_EXECUTE  ((map_options_t) 0b000000000000000000000000000000000000000000000000000010)
#define MAP_ANON     ((map_options_t) 0b000000000000000000000000000000000000000000000000000100)

typedef uint64_t mount_options_t;

typedef enum {
    SOCKET_UNIX,
} socket_domain_t;

typedef enum {
    SOCKET_STREAM,
    SOCKET_DGRAM,
} socket_type_t;

typedef struct sockaddr_s sockaddr_t;

typedef struct {
    char path[];
} sockaddr_unix_t;

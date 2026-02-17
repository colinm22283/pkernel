#pragma once

#include <stddef.h>

#include <device/device.h>

#include <devfs/devfs.h>

struct tty_s;

typedef uint64_t tty_write_handler_t(struct tty_s * tty, const char * buffer, size_t size);

typedef struct tty_s {
    void * cookie;

    size_t tty_num;

    device_t * device;
    devfs_entry_t * devfs_entry;

    event_t * read_ready;

    bool buffer_ready;
    size_t buffer_size, buffer_capacity;
    size_t buffer_offset;
    pman_mapping_t * buffer_mapping;

    char last_char;

    bool echo;

    tty_write_handler_t * write_handler;

    struct tty_s * next;
    struct tty_s * prev;
} tty_t;

void ttys_init(void);

tty_t * tty_init(tty_write_handler_t * write_handler, void * cookie);
void tty_free(tty_t * tty);

size_t tty_provide_char(tty_t * tty, char buffer);


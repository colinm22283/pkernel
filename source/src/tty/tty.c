#include <stdbool.h>

#include <tty/tty.h>

#include <scheduler/scheduler.h>

#include <util/heap/heap.h>
#include <util/string/writestr.h>
#include <util/memory/memcpy.h>

#include <config/tty.h>

tty_t tty_head, tty_tail;

uint64_t tty_write(device_t * dev, const char * buffer, uint64_t size) {
    tty_t * tty = dev->private;

    for (uint64_t i = 0; i < size; i++) {
        if (buffer[i] == '\n') {
            tty->write_handler(tty, "\r\n", 2);
        }
        else {
            tty->write_handler(tty, &buffer[i], 1);
        }
    }

    return size;
}

uint64_t tty_read(device_t * dev, char * buffer, uint64_t size) {
    tty_t * tty = dev->private;

    if (!tty->buffer_ready) {
        scheduler_await(tty->read_ready);
    }

    size_t amnt = size > tty->buffer_size ? tty->buffer_size : size;

    memcpy(buffer, tty->buffer_mapping->vaddr + tty->buffer_offset, amnt);

    tty->buffer_offset += amnt;

    if (tty->buffer_size == tty->buffer_offset) {
        tty->buffer_ready = false;
        tty->buffer_size = 0;
        tty->buffer_offset = 0;
    }

    return amnt;
}

void ttys_init(void) {
    tty_head.next = &tty_tail;
    tty_head.prev = NULL;
    tty_tail.next = NULL;
    tty_tail.prev = &tty_head;
}

tty_t * tty_init(tty_write_handler_t * write_handler, void * cookie) {
    device_char_operations_t dev_ops = {
        .write = tty_write,
        .read = tty_read,
    };
    device_char_data_t dev_data = { };

    size_t tty_num = 0;

    for (
        tty_t * tty = tty_head.next;
        tty != &tty_tail;
        tty = tty->next
    ) {
        if (tty->tty_num == tty_num) {
            tty_num++;
            tty = &tty_head;
        }
    }

    tty_t * tty = heap_alloc(sizeof(tty_t));

    tty->cookie = cookie;

    tty->tty_num = tty_num;

    char name[100] = "tty";
    name[writestr(name + 3, 99 - 3, 0, tty->tty_num) + 3] = '\0';

    tty->device = device_create_char(name, tty, &dev_ops, &dev_data);
    tty->devfs_entry = devfs_register(tty->device);

    tty->read_ready = event_init();

    tty->buffer_ready = false;
    tty->buffer_size = 0;
    tty->buffer_capacity = TTY_BUFFER_SIZE;
    tty->buffer_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, TTY_BUFFER_SIZE);

    tty->last_char = '\0';

    tty->echo = true;

    tty->write_handler = write_handler;

    tty->next = tty_head.next;
    tty->prev = &tty_head;

    tty_head.next->prev = tty;
    tty_head.next = tty;

    return tty;
}

void tty_free(tty_t * tty) {
    // TODO
}

size_t tty_provide_char(tty_t * tty, char buffer) {
    if (tty->echo) {
        if (buffer == 0x08) {
            if (tty->buffer_size > 0) tty->write_handler(tty, "\x08 \x08", 3);
        }
        else if (buffer == '\n' || buffer == '\r') {
            tty->write_handler(tty, "\r\n", 2);
        }
        else {
            if (tty->buffer_size < tty->buffer_capacity) tty->write_handler(tty, &buffer, 1);
        }
    }

    switch (buffer) {
        case 0x08: {
            if (tty->buffer_size > 0) tty->buffer_size--;
        } break;

        case '\r': {
            if (tty->last_char != '\n') {
                tty->buffer_ready = true;
                ((char *) tty->buffer_mapping->vaddr)[tty->buffer_size++] = '\n';

                event_invoke(tty->read_ready);
            }
        } break;

        case '\n': {
            if (tty->last_char != '\r') {
                tty->buffer_ready = true;
                ((char *) tty->buffer_mapping->vaddr)[tty->buffer_size++] = '\n';

                event_invoke(tty->read_ready);
            }
        } break;

        default: {
            if (tty->buffer_size < tty->buffer_capacity) {
                ((char *) tty->buffer_mapping->vaddr)[tty->buffer_size++] = buffer;
            }
        } break;
    }

    tty->last_char = buffer;

    return 1;
}


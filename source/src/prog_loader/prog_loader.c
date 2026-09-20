#include <prog_loader/prog_loader.h>

#include <filesystem/superblock.h>

#include <pman/pman.h>

#include <util/heap/heap.h>

#include <elf/elf.h>

#include <config/prog_loader.h>

#ifdef PROG_LOADER_DEBUG
    #define DEBUG_LOGGER_ENABLED
#endif
#include <debug/debug_logger.h>

DEFINE_KERNEL_PRINTF("prog loader");

size_t prog_read_handler(void * cookie, char * buffer, size_t size, size_t offset) {
    fs_directory_entry_t * dirent = cookie;

    fs_size_t read_bytes;

    dirent->superblock->superblock_ops->read(dirent, buffer, size, offset, &read_bytes);

    return read_bytes;
}

void * prog_loader_alloc(size_t size) {
    return heap_alloc_debug(size, "elf alloc");
}
void prog_loader_free(void * ptr) {
    heap_free(ptr);
}

void prog_loader_init(void) {
    elf_init(prog_loader_alloc, prog_loader_free);
}

int load_program(process_t * process, fs_directory_entry_t * dirent) {
    if (dirent == NULL) {
        return -ENOENT;
    }

    if (dirent->type == FS_DIRECTORY) {
        fs_directory_entry_release(dirent);

        return -EISDIR;
    }

    elf_t elf;
    if (elf_load(&elf, dirent, prog_read_handler) != ELF_ERROR_OK) {
        fs_directory_entry_release(dirent);

        return -ENOEXEC;
    }

    kprintf("Loading ELF headers");

    kprintf("  Entry: %p", (void *) elf.header.entry);

    for (size_t i = 0; i < elf.pheader_count; i++) {
        elf_pheader_t * header = &elf.pheaders[i];

        if (header->type == ELF_PH_TYPE_LOAD) {
            kprintf("  ELF_PH_TYPE_LOAD: vaddr = %p, memsz = %i", (void *) header->vaddr, header->memsz);

            void * aligned_vaddr = (void *) (((intptr_t) header->vaddr / PAGE_SIZE) * PAGE_SIZE);
            size_t extra_size = ((intptr_t) header->vaddr - (intptr_t) aligned_vaddr);
            kprintf("  aligned_vaddr = %p, extra = %i", aligned_vaddr, extra_size);

            pman_range_t * range = pman_get_range(process->paging_context, (void *) header->vaddr, header->memsz);

            void * user_vaddr = NULL;

            if (range->size == 0 || !range->complete) {
                kprintf("    Unmapping possible old segments");
                pman_range_unmap(range);

                if (header->memsz != 0) {
                    kprintf("    Mapping as new segment");

                    pman_protection_flags_t prot = 0;

                    if (header->flags & ELF_PH_FLAGS_W) prot |= PMAN_WRITE;
                    if (header->flags & ELF_PH_FLAGS_X) prot |= PMAN_EXECUTE;

                    kprintf("    Mapping new kernel segment of size %i", header->memsz + extra_size);
                    pman_range_t * range = pman_add_anon_map(process->paging_context, aligned_vaddr, header->memsz + extra_size, 0, prot);

                    if (range == NULL) {
                        return -ENOMEM;
                    }

                    user_vaddr = range->vaddr + extra_size;
                    pman_range_free(range);
                }
            }
            else {
                if (header->memsz == 0) {
                    kprintf("    Unmapping possible old segments");
                    pman_range_unmap(range);

                    user_vaddr = NULL;
                }
            }

            if (user_vaddr != NULL) {
                char * segment_buffer = heap_alloc(header->memsz);

                elf_load_segment(&elf, header, segment_buffer);

                kprintf("    First 10 bytes:");
                for (size_t j = 0; j < 10; j++) kprintf("      %i", ((unsigned char *) segment_buffer)[j]);

                process_copy_to_user(process, user_vaddr, segment_buffer, header->memsz);

                heap_free(segment_buffer);
            }
        }
    }

    thread_load_pc(process->threads[0], (void *) elf.header.entry);

    elf_release(&elf);

    return 0;
}

#include <prog_loader/prog_loader.h>

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

int load_program(process_t * process, fs_directory_entry_t * dirent) {
    if (dirent == NULL) {
        return ERROR_FS_NO_ENT;
    }

    if (dirent->type != FS_REGULAR) {
        fs_directory_entry_release(dirent);

        return ERROR_NOT_REG;
    }

    elf_t elf;
    if (elf_load(&elf, dirent, prog_read_handler) != ELF_ERROR_OK) {
        fs_directory_entry_release(dirent);

        return ERROR_BAD_ELF;
    }

    kprintf("Loading ELF headers");

    kprintf("  Entry: %p", (void *) elf.header.entry);

    for (size_t i = 0; i < elf.pheader_count; i++) {
        elf_pheader_t * header = &elf.pheaders[i];

        if (header->type == ELF_PH_TYPE_LOAD) {
            kprintf("  ELF_PH_TYPE_LOAD: vaddr = %p, memsz = %i", (void *) header->vaddr, header->memsz);

            void * aligned_vaddr = (void *) (((intptr_t) header->vaddr / PAGE_SIZE) * PAGE_SIZE);
            size_t extra_size = (header->vaddr - (intptr_t) aligned_vaddr);
            kprintf("  aligned_vaddr = %p, extra = %i", aligned_vaddr, extra_size);

            pman_mapping_t * mapping = pman_context_get_vaddr(process->paging_context, (void *) header->vaddr);

            void * process_mapping = NULL;

            if (mapping == NULL) {
                if (header->memsz != 0) {
                    kprintf("    Mapping as new segment");

                    pman_protection_flags_t prot = 0;

                    if (header->flags & ELF_PH_FLAGS_W) prot |= PMAN_PROT_WRITE;
                    if (header->flags & ELF_PH_FLAGS_X) prot |= PMAN_PROT_EXECUTE;

                    kprintf("    Mapping new kernel segment");
                    pman_mapping_t * kernel_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, header->memsz + extra_size);

                    if (kernel_mapping == NULL) {
                        return ERROR_BAD_MAP;
                    }

                    kprintf("    Mapping new user segment");
                    mapping = pman_context_add_shared(process->paging_context, prot, kernel_mapping, aligned_vaddr);

                    if (mapping == NULL) {
                        return ERROR_BAD_MAP;
                    }

                    process_mapping = kernel_mapping->vaddr + extra_size;
                    pman_context_unmap(kernel_mapping);
                }
            }
            else {
                if (header->memsz == 0) {
                    kprintf("    Unmapping old segment");

                    pman_context_unmap(mapping);

                    process_mapping = NULL;
                }
                else {
                    kprintf("    Remapping old segment");

                    mapping = pman_context_resize(mapping, header->memsz);

                    process_mapping = get_root_mapping(mapping)->vaddr;
                }
            }

            if (process_mapping != NULL) {
                elf_load_segment(&elf, header, process_mapping);

                kprintf("    First 10 bytes:");
                for (size_t j = 0; j < 10; j++) kprintf("      %i", ((unsigned char *) process_mapping)[j]);
            }
        }
    }

    thread_load_pc(process->threads[0], (void *) elf.header.entry);

    elf_release(&elf);

    return ERROR_OK;
}

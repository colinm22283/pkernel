#include <filesystem/superblock.h>

#include <util/heap/heap.h>
#include <util/memory/memcpy.h>

fs_superblock_t * superblock_alloc(const fs_superblock_ops_t * superblock_ops) {
    fs_superblock_t * superblock = heap_alloc_debug(sizeof(fs_superblock_t), "superblock");

    superblock->superblock_ops = superblock_ops;

    return superblock;
}

void superblock_free(fs_superblock_t * superblock) {
    heap_free(superblock);
}

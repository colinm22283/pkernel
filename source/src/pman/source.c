#include <stddef.h>

#include <pman/types.h>
#include <pman/pman.h>

#include <filesystem/file.h>

#include <util/heap/heap.h>

#include <sys/panic.h>

static inline pman_source_t * pman_source_init(void) {
    pman_source_t * source = heap_alloc(sizeof(pman_source_t));

    source->references = 1;

    source->file        = NULL;
    source->file_offset = 0;

    source->initialized = false;
    source->dirty       = false;
    source->evicted     = false;
    source->anchored    = false;
    source->mapped      = false;

    return source;
}

static inline void pman_source_free(pman_source_t * source) {
    heap_free(source);
}

pman_source_t * pman_source_init_file(fs_file_t * file, size_t offset) {
    pman_source_t * source = pman_source_init();

    source->file        = file;
    source->file_offset = offset;
    
    source->initialized = true;
    source->dirty       = false;
    source->evicted     = true;
    source->anchored    = false;
    source->mapped      = false;

    return source;
}

pman_source_t * pman_source_init_anon(void) {
    pman_source_t * source = pman_source_init();

    source->initialized = false;
    source->dirty       = false;
    source->evicted     = true;
    source->anchored    = false;
    source->mapped      = false;

    return source;
}

pman_source_t * pman_source_init_anon_kernel(size_t paddr) {
    pman_source_t * source = pman_source_init();

    source->initialized = true;
    source->dirty       = false;
    source->evicted     = false;
    source->anchored    = true;
    source->mapped      = false;

    palloc_alloc_contiguous(&source->palloc, PAGE_SIZE, paddr);

    return source;
}

void pman_source_add_ref(pman_source_t * source) {
    source->references++;
}

void pman_source_sub_ref(pman_source_t * source) {
    source->references--;

    if (source->references == 0) {
        pman_source_free(source);
    }
}

void pman_source_page_in(pman_source_t * source) {
    if (!source->evicted) return;

    if (source->file == NULL) {
        palloc_alloc(&source->palloc, PAGE_SIZE);
    }
    else {
        // TODO
        panic0("pman_source_page_in unimp");
    }

    source->initialized = true;
    source->evicted = false;
}

void * pman_source_map_to_kernel(pman_source_t * source) {
    if (source->mapped) return source->mapping.vaddr;

    pman_source_page_in(source);

    void * vaddr = valloc_alloc(&pman_kernel_context()->valloc, PAGE_SIZE);

    paging_map_ex(
        pman_kernel_context()->tlt,
        &source->mapping,
        source->palloc.paddrs[0],
        vaddr,
        1,
        true,
        false,
        false
    );

    source->mapped = true;

    return source->mapping.vaddr;
}


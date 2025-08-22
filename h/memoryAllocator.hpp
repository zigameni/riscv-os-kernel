
#ifndef MEMORY_ALLOCATOR_HPP
#define MEMORY_ALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {
private:
    struct FreeBlock {
        size_t size;
        FreeBlock* next;
    };

    static const size_t ALIGNMENT = 8;
    static const size_t ALIGN_MASK = ALIGNMENT - 1;
    static const size_t HEADER_SIZE = sizeof(size_t);

    static char* heap_start;
    static char* heap_end;
    static FreeBlock* free_list;

    static size_t align(size_t n);
    static FreeBlock* find_free_block(size_t size);
    static void split_block(FreeBlock* block, size_t size);
    static void coalesce();

public:
    static void* mem_alloc(size_t size);
    static int mem_free(void* ptr);
    static void initFreeBlock();
};

#endif // MEMORY_ALLOCATOR_HPP
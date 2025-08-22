

#include "../h/memoryAllocator.hpp"

// Static member initialization
char* MemoryAllocator::heap_start = 0;
char* MemoryAllocator::heap_end = 0;
MemoryAllocator::FreeBlock* MemoryAllocator::free_list = 0;

size_t MemoryAllocator::align(size_t n) {
    return (n + ALIGN_MASK) & ~ALIGN_MASK;
}

MemoryAllocator::FreeBlock* MemoryAllocator::find_free_block(size_t size) {
    FreeBlock* prev = 0;
    FreeBlock* curr = free_list;

    while (curr != 0) {
        if (curr->size >= size) {
            if (prev != 0) {
                prev->next = curr->next;
            } else {
                free_list = curr->next;
            }
            return curr;
        }
        prev = curr;
        curr = curr->next;
    }
    return 0;
}

void MemoryAllocator::split_block(FreeBlock* block, size_t size) {
    if (block->size >= size + HEADER_SIZE + ALIGNMENT) {
        FreeBlock* new_block = (FreeBlock*)((char*)block + size + HEADER_SIZE);
        new_block->size = block->size - size - HEADER_SIZE;
        new_block->next = free_list;
        free_list = new_block;

        block->size = size;
    }
}

void MemoryAllocator::coalesce() {
    FreeBlock* curr = free_list;
    while (curr != 0 && curr->next != 0) {
        if ((char*)curr + curr->size + HEADER_SIZE == (char*)curr->next) {
            curr->size += curr->next->size + HEADER_SIZE;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

void* MemoryAllocator::mem_alloc(size_t size) {
    if (size == 0) return 0;

    size = align(size + HEADER_SIZE);

    FreeBlock* block = find_free_block(size);
    if (block == 0) return 0;

    split_block(block, size);

    size_t* header = (size_t*)block;
    *header = size;

    return (void*)(header + 1);
}

int MemoryAllocator::mem_free(void* ptr) {
    if (ptr == 0 || (char*)ptr < heap_start || (char*)ptr >= heap_end) return -1;

    // Check alignment
    if (((size_t)ptr & ALIGN_MASK) != 0) return -1;

    size_t* header = (size_t*)ptr - 1;
    size_t size = *header;

    // Basic sanity check on the size
    if (size < HEADER_SIZE || (char*)ptr + size > heap_end) return -1;

    FreeBlock* block = (FreeBlock*)header;

    // Check if this block is already in the free list
    for (FreeBlock* curr = free_list; curr != 0; curr = curr->next) {
        if (curr == block) return -1; // Already freed
    }

    // Insert at the correct position to maintain address order
    FreeBlock** curr = &free_list;
    while (*curr != 0 && *curr < block) {
        curr = &((*curr)->next);
    }
    block->next = *curr;
    *curr = block;

    // Coalesce with next block if adjacent
    if (block->next && (char*)block + block->size == (char*)block->next) {
        block->size += block->next->size;
        block->next = block->next->next;
    }

    // Coalesce with previous block if adjacent
    if (free_list != block) {
        FreeBlock* prev = free_list;
        while (prev->next != block) prev = prev->next;
        if ((char*)prev + prev->size == (char*)block) {
            prev->size += block->size;
            prev->next = block->next;
        }
    }

    return 0;
}

void MemoryAllocator::initFreeBlock() {
    heap_start = (char*)HEAP_START_ADDR;
    heap_end = (char*)HEAP_END_ADDR;

    free_list = (FreeBlock*)heap_start;
    free_list->size = (size_t)(heap_end - heap_start);
    free_list->next = 0;
}
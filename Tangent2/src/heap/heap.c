/*
 * heap.c
 * Implementation of heap memory management
 *  Created on: Jan 5, 2026
 *      Author: harma
 */

#include "heap.h"

// Get user data pointer from block header
static void *hdata(block_t *b) {
    return (void *)((byte *)b + sizeof(block_t));
}

// Get block header from user data pointer
static block_t *hblock(void *ptr) {
    return (block_t *)((byte *)ptr - sizeof(block_t));
}

// Align size to heap block alignment
static word halign(word size) {
    if (size % HEAP_BLOCK_ALIGN == 0) {
        return size;
    }
    return size + (HEAP_BLOCK_ALIGN - (size % HEAP_BLOCK_ALIGN));
}

// Initialize heap with single free block
void hinit(void) {
    block_t *initial_block = (block_t *)HEAP_START_ADDR;
    initial_block->size = HEAP_MAX_SIZE - sizeof(block_t);
    initial_block->next = 0;
    initial_block->free = 1;
}

// Split block into two if larger than needed
static void hsplit(block_t *b, word size) {
    block_t *new_block = (block_t *)((byte *)hdata(b) + size);
    new_block->size = b->size - size - sizeof(block_t);
    new_block->next = b->next;
    new_block->free = 1;
    b->size = size;
    b->next = new_block;
}

// Merge free blocks
void hmerge(void) {
    block_t *current = (block_t *)HEAP_START_ADDR;
    while (current && current->next) {
        if (current->free && current->next->free) {
            current->size += sizeof(block_t) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

// Find memory block (helper for halloc)
static block_t *hfind(word size) {
    block_t *current = (block_t *)HEAP_START_ADDR;
    while (current) {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return 0;
}

// Allocate memory block
void *halloc(word size) {
    size = halign(size);
    block_t *b = hfind(size);

    if (!b) {
        hmerge();
        b = hfind(size);
        if (!b) return 0;
    }

    if (b->size >= size + sizeof(block_t) + HEAP_BLOCK_ALIGN)
        hsplit(b, size);

    b->free = 0;
    return hdata(b);
}

// Allocate and zero-initialize memory block
void *hcalloc(word num, word size) {
    word total_size = num * size;
    total_size = halign(total_size);
    void *ptr = halloc(total_size);
    if (ptr) {
        byte *bptr = (byte *)ptr;
        for (word i = 0; i < total_size; i++) {
            bptr[i] = 0;
        }
    }
    return ptr;
}

// Reallocate memory block for new size
void *hrealloc(void *ptr, word size) {
    size = halign(size);
    if (!ptr) return halloc(size);
    block_t *old = hblock(ptr);
    word old_size = old->size;

    if (size <= old_size) return ptr;

    void *new_ptr = halloc(size);
    if (!new_ptr) return 0;

    byte *src = (byte *)ptr;
    byte *dst = (byte *)new_ptr;
    for (word i = 0; i < old_size; i++) dst[i] = src[i];

    old->free = 1;

    return new_ptr;
}

// Free memory block
void hfree(void *ptr) {
    if (!ptr) return;
    block_t *b = hblock(ptr);
    b->free = 1;
}

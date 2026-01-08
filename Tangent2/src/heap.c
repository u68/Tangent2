/*
 * heap.c
 * Implementation of heap memory management
 *  Created on: Jan 5, 2026
 *      Author: harma
 */

#include "heap.h"
#include "base.h"

#define HEAP_START_ADDR 0x9C04 // Start address of heap memory
#define HEAP_MAX_SIZE   0x2000 // Maximum size of heap memory (8 KB)
#define HEAP_BLOCK_ALIGN 2 // Align blocks to 2-byte boundaries (word aligned)

// Block header structure
typedef struct block {
    word size; // Size of block excluding header
    struct block *next; // Pointer to next block
    byte free; // Whether block is free (1) or used (0)
} block_t;

// Get user data pointer from block header
static void *hdata(block_t *b) {
    return (void *)((byte *)b + sizeof(block_t));
}

// Get block header from user data pointer
static block_t *hblock(void *ptr) {
    return (block_t *)((byte *)ptr - sizeof(block_t));
}

// Align size
static word halign(word size) {
    // If already aligned, return size
    if (size % HEAP_BLOCK_ALIGN == 0) {
        return size;
    }
    // Otherwise, align size (subtract remainder from next multiple)
    return size + (HEAP_BLOCK_ALIGN - (size % HEAP_BLOCK_ALIGN));
}

// Initialize heap
void hinit() {
    // Create a big fat block of free memory
    block_t *initial_block = (block_t *)HEAP_START_ADDR;
    // Set initial block properties
    initial_block->size = HEAP_MAX_SIZE - sizeof(block_t);
    initial_block->next = 0;
    initial_block->free = 1;
}

// Split block into two if larger than needed
static void hsplit(block_t *b, word size) {
    block_t *new_block = (block_t *)((byte *)hdata(b) + size);
    // Set new block properties
    new_block->size = b->size - size - sizeof(block_t);
    new_block->next = b->next;
    new_block->free = 1;
    // Update original block properties
    b->size = size;
    b->next = new_block;
}

// Merge free blocks
void hmerge() {
    block_t *current = (block_t *)HEAP_START_ADDR;
    while (current && current->next) {
        if (current->free && current->next->free) {
            current->size += sizeof(block_t) + current->next->size; // merge sizes
            current->next = current->next->next; // skip next block
        } else {
            current = current->next; // move to next block
        }
    }
}

// Find memory block (helper for halloc)
static block_t *hfind(word size) {
    block_t *current = (block_t *)HEAP_START_ADDR;
    while (current) {
        if (current->free && current->size >= size) {
            return current; // return header pointer
        }
        current = current->next;
    }
    return 0; // nothing found
}

// Allocate memory block
void *halloc(word size) {
    size = halign(size); // Align size
    block_t *b = hfind(size);

    if (!b) {
        hmerge(); // Try merging free blocks
        b = hfind(size); // Try finding again
        if (!b) return 0; // still nothing
    }

    if (b->size >= size + sizeof(block_t) + HEAP_BLOCK_ALIGN)
        hsplit(b, size); // split if block too large

    b->free = 0;
    return hdata(b);
}

// Allocate and zero-initialize memory block
void *hcalloc(word num, word size) {
    word total_size = num * size;
    total_size = halign(total_size); // Align size
    void *ptr = halloc(total_size);
    if (ptr) {
        // Zero-initialize memory
        byte *bptr = (byte *)ptr;
        for (word i = 0; i < total_size; i++) {
            bptr[i] = 0;
        }
    }
    return ptr;
}

// Reallocate memory block for new size
void *hrealloc(void *ptr, word size) {
    size = halign(size); // Align size

    if (!ptr) return halloc(size); // If null just allocate new block
    block_t *old = hblock(ptr);
    word old_size = old->size;

    // If new size fits in current block, just return same pointer
    if (size <= old_size) return ptr;

    // Try to allocate new block (keep old block allocated!)
    void *new_ptr = halloc(size);
    if (!new_ptr) return 0; // allocation failed, old data still intact

    // Copy old data to new location
    byte *src = (byte *)ptr;
    byte *dst = (byte *)new_ptr;
    for (word i = 0; i < old_size; i++) dst[i] = src[i];

    // Now safe to free old block
    old->free = 1;

    return new_ptr;
}

void hfree(void *ptr) {
    if (!ptr) return; // ignore null pointers
    block_t *b = hblock(ptr);
    b->free = 1; // mark block as free
}

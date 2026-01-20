# Heap Management Module

## Purpose
This document describes the heap memory management system implemented in `src/heap/heap.c` and its interface in `heap.h`. It explains the heap design, memory layout, allocation algorithms, block structure, API functions, and usage patterns.
---

## Table of contents
- [Overview](#overview)
- [Heap layout and block structure](#heap-layout-and-block-structure)
- [Initialization](#initialization)
- [Allocation and free](#allocation-and-free)
- [Block splitting and merging](#block-splitting-and-merging)
- [API reference](#api-reference)
- [Examples](#examples)

---

## Overview
The Tangent2 heap manager provides dynamic memory allocation for the system. It uses a simple block-based allocator with support for splitting, merging, and alignment. The heap is a contiguous region of memory starting at a fixed address, divided into blocks with headers.

Key properties:
- Heap starts at `HEAP_START_ADDR` (0x9C04) and is `HEAP_MAX_SIZE` (0x2000) bytes long.
- Each block has a header with size, next pointer, and free flag.
- Allocations are aligned to `HEAP_BLOCK_ALIGN` (2 bytes).
- The heap is managed as a singly linked list of blocks.

Source reference: `src/heap/heap.c`, `src/heap/heap.h`.

---

## Heap layout and block structure
The heap consists of a sequence of memory blocks. Each block contains:
- `size`: number of bytes in the block (excluding header)
- `next`: pointer to the next block
- `free`: flag (1 if free, 0 if allocated)

Block header structure:
```c
typedef struct block {
    word size;
    struct block *next;
    byte free;
} block_t;
```

User data immediately follows the block header. Helper functions convert between block pointers and user pointers.

---

## Initialization
The heap is initialized with a single free block covering the entire heap region:
```c
void hinit();
```
This sets up the initial block at `HEAP_START_ADDR` with `size = HEAP_MAX_SIZE - sizeof(block_t)`.

---

## Allocation and free
Memory is allocated using:
```c
void *halloc(word size);
```
- Finds a free block large enough for the requested size (aligned).
- Splits the block if possible.
- Marks the block as allocated and returns a pointer to user data.

Zero-initialized allocation:
```c
void *hcalloc(word num, word size);
```
- Allocates and zeroes `num * size` bytes.

Reallocation:
```c
void *hrealloc(void *ptr, word size);
```
- Allocates a new block if the requested size is larger than the current block.
- Copies old data and frees the old block.

Freeing memory:
```c
void hfree(void *ptr);
```
- Marks the block as free.

---

## Block splitting and merging
- Blocks are split when a free block is larger than needed for an allocation.
- Adjacent free blocks are merged to reduce fragmentation:
```c
void hmerge();
```
- Called automatically during allocation if no suitable block is found.

---

## API reference
- `void hinit();` — Initialize the heap.
- `void hmerge();` — Merge adjacent free blocks.
- `void *halloc(word size);` — Allocate memory.
- `void *hcalloc(word num, word size);` — Allocate and zero memory.
- `void *hrealloc(void *ptr, word size);` — Reallocate memory.
- `void hfree(void *ptr);` — Free memory.

---

## Examples
Allocate 32 bytes:
```c
void *ptr = halloc(32);
```
Free memory:
```c
hfree(ptr);
```
Allocate and zero 10 words:
```c
word *arr = hcalloc(10, sizeof(word));
```
Reallocate to 64 bytes:
```c
ptr = hrealloc(ptr, 64);
```

---

## Notes
- All allocations are aligned to 2 bytes.
- The heap manager does not compact memory; merging only combines adjacent free blocks.
- If allocation fails, `halloc` and `hrealloc` return 0.

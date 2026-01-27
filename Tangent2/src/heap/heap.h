/*
 * heap.h
 * Interface for heap memory management
 *  Created on: Jan 5, 2026
 *      Author: harma
 */

#ifndef HEAP_H_
#define HEAP_H_

#include "../base.h"

#define HEAP_START_ADDR 0x9C04
#define HEAP_MAX_SIZE   0x2000
#define HEAP_BLOCK_ALIGN 2

typedef struct block {
    word size;
    struct block *next;
    byte free;
} block_t;

void hinit(void);
void hmerge(void);
void *halloc(word size);
void *hcalloc(word num, word size);
void *hrealloc(void *ptr, word size);
void hfree(void *ptr);

#endif /* HEAP_H_ */
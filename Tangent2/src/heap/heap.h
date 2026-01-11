/*
 * heap.h
 * Interface for heap memory management
 *  Created on: Jan 5, 2026
 *      Author: harma
 */

#ifndef HEAP_H_
#define HEAP_H_

#include "../base.h"

void hinit();
void hmerge();
void *halloc(word size);
void *hcalloc(word num, word size);
void *hrealloc(void *ptr, word size);
void hfree(void *ptr);

#endif /* HEAP_H_ */
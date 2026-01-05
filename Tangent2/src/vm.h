/*
 * vm.h
 * Interface for virtual machine
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#ifndef VM_H_
#define VM_H_

#include "base.h"

byte* vm_spawn(word code_size, word ram_size);
void vm_destroy(byte* vm_id);
void vm_step(byte* vm_id);

#endif /* VM_H_ */
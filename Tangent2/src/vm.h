/*
 * vm.h
 * Interface for virtual machine
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#ifndef VM_H_
#define VM_H_

#include "base.h"

#define VM_MAX_VM_START 16 // Default maximum number of VMs that can be started,
//if more are needed, it will allocate more, but it will having to shift everything over 
#define VM_MANAGER_START_ADDR 0x9C04
#define VM_MACHINES_ADDR VM_MANAGER_START_ADDR + (VM_MAX_VM_START * 2) // Each VM uses 2 bytes for its ID pointer
#define VM_REGISTERS_COUNT 16

byte* vm_spawn(word code_size, word ram_size);
void vm_destroy(byte* vm_id);
void vm_step(byte* vm_id);

#endif /* VM_H_ */
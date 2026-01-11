/*
 * vm.h
 * Interface for virtual machine
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#ifndef VM_H_
#define VM_H_

#include "../base.h"

#define MAX_VMS 16 // Default max, dymanically expands when full

typedef union {
    word ern[8]; // Access as 16-bit registers
    byte rn[16]; // Access as 8-bit registers
} registers_t;

typedef union {
    byte raw;
    struct {
        byte zero : 1;
        byte neg : 1;
        byte carry : 1;
        byte overflow : 1;
    } field;
} psw_flags_t;

typedef struct __attribute__((aligned(2))) {
    struct {
        byte code_in_rom : 1; // If set, code is in ROM, else in RAM
        byte uses_ram : 1; // If set, VM uses RAM (else no RAM, eg program only uses registers etc)
        byte running : 1; // If set, VM is currently running
        word code_size;
        word ram_size;
    } vm_properties;
    registers_t registers;
    psw_flags_t psw;
    word pc; // Program counter
    word lr; // Link register
    word sp; // Stack pointer
    byte* code;
    byte* ram;
} TangentMachine;

void vm_init_system();
void vm_init(TangentMachine* vm, byte* code);
TangentMachine* vm_spawn(byte* code);
void vm_destroy(TangentMachine* vm);
void vm_step(TangentMachine* vm);
void vm_step_all();

#endif /* VM_H_ */
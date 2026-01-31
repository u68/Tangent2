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

typedef enum opcode_t {
    OP_MOV8_REG_REG, // Data movement
    OP_MOV16_REG_REG,
    OP_MOV16_REG_IMM,
    OP_LOAD8_REG_MREG,
    OP_LOAD8_REG_MIMM,
    OP_LOAD16_REG_MREG,
    OP_LOAD16_REG_MIMM,
    OP_STORE8_MREG_REG,
    OP_STORE8_MREG_IMM,
    OP_STORE8_MIMM_REG,
    OP_STORE8_MIMM_IMM,
    OP_STORE16_MREG_REG,
    OP_STORE16_MREG_IMM,
    OP_STORE16_MIMM_REG,
    OP_STORE16_MIMM_IMM,

    OP_PUSH8_REG, // Stack operations
    OP_PUSH8_IMM,
    OP_PUSH16_REG,
    OP_PUSH16_IMM,
    OP_POP8_REG,
    OP_POP16_REG,
    OP_PUSH_LR,
    OP_POP_PC,
    OP_PUSH_SP,
    OP_POP_SP,

    OP_ADD8_REG_REG, // ALU operations
    OP_ADD16_REG_REG,
    OP_ADD16_REG_IMM,
    OP_SUB8_REG_REG,
    OP_SUB16_REG_REG,
    OP_SUB16_REG_IMM,
    OP_INC8_REG,
    OP_INC16_REG,
    OP_DEC8_REG,
    OP_DEC16_REG,
    OP_MUL8_REG_REG,
    OP_MUL16_REG_REG,
    OP_MUL16_REG_IMM,
    OP_DIV8_REG_REG,
    OP_DIV16_REG_REG,
    OP_DIV16_REG_IMM,
    OP_MOD8_REG_REG,
    OP_MOD8_REG_IMM,
    OP_MOD16_REG_REG,
    OP_MOD16_REG_IMM,

    OP_AND8_REG_REG, // Logical operations
    OP_AND16_REG_REG,
    OP_AND16_REG_IMM,
    OP_OR8_REG_REG,
    OP_OR16_REG_REG,
    OP_OR16_REG_IMM,
    OP_XOR8_REG_REG,
    OP_XOR16_REG_REG,
    OP_XOR16_REG_IMM,
    OP_NOT8_REG,
    OP_NOT16_REG,

    OP_CMP8_REG_REG, // Comparison operations
    OP_CMP16_REG_REG,
    OP_CMP16_REG_IMM,
    OP_SLL8_REG_REG, // Shift operations
    OP_SLL8_REG_IMM,
    OP_SLL16_REG_REG,
    OP_SLL16_REG_IMM,
    OP_SRL8_REG_REG,
    OP_SRL8_REG_IMM,
    OP_SRL16_REG_REG,
    OP_SRL16_REG_IMM,
    OP_SRA8_REG_REG,
    OP_SRA8_REG_IMM,
    OP_SRA16_REG_REG,
    OP_SRA16_REG_IMM,

    OP_B_REG, // Control flow
    OP_B_IMM,
    OP_BEQ_IMM,
    OP_BNE_IMM,
    OP_BLT_IMM,
    OP_BLE_IMM,
    OP_BGT_IMM,
    OP_BGE_IMM,
    OP_BL_REG,
    OP_BL_IMM,

    // The 16-aligned blocks below hold opcodes that modify a register and take an 8-bit immediate.
    // Each listed constant is the base of a 16-entry block (dest register encoded in low nibble).
    OP_MOV8_REG_IMM = 0x60,
    OP_ADD8_REG_IMM = OP_MOV8_REG_IMM + 0x10,
    OP_SUB8_REG_IMM = OP_ADD8_REG_IMM + 0x10,
    OP_MUL8_REG_IMM = OP_SUB8_REG_IMM + 0x10,
    OP_DIV8_REG_IMM = OP_MUL8_REG_IMM + 0x10,
    OP_AND8_REG_IMM = OP_DIV8_REG_IMM + 0x10,
    OP_OR8_REG_IMM  = OP_AND8_REG_IMM + 0x10,
    OP_XOR8_REG_IMM = OP_OR8_REG_IMM + 0x10,
    OP_CMP8_REG_IMM = OP_XOR8_REG_IMM + 0x10,

    OP_SYSCALL = 0xF0,
	OP_END	 = 0xFF

} opcode_t;

void vm_init_system(void);
void vm_init(TangentMachine* vm, byte* code);
TangentMachine* vm_spawn(const byte* code);
TangentMachine *vm_run_file(fs_node_t* parent, const char *filename);
void vm_destroy(TangentMachine* vm);
void vm_step(TangentMachine* vm);
void vm_step_all(void);

#endif /* VM_H_ */

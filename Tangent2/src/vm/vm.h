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
    OP_MOV8_REG_REG, // Data movement // 2 bytes // mov r1, r8
    OP_MOV16_REG_REG, // 2 bytes // mov er0, er4
    OP_MOV16_REG_IMM, // 4 bytes // mov er2, #0x1234
    OP_LOAD8_REG_MREG, // 2 bytes // l r3, [er0]
    OP_LOAD8_REG_MIMM, // 4 bytes // l r5, [0x1234]
    OP_LOAD16_REG_MREG, // 2 bytes // l er0, [er2]
    OP_LOAD16_REG_MIMM, // 4 bytes // l er4, [0x1234]
    OP_STORE8_MREG_REG, // 2 bytes // st [er0], r3
    OP_STORE8_MREG_IMM, // 4 bytes // st [er2], #07
    OP_STORE8_MIMM_REG, // 4 bytes // st [0x1234], r5
    OP_STORE8_MIMM_IMM, // 4 bytes // st [0x1234], #0b101010
    OP_STORE16_MREG_REG, // 2 bytes // st [er0], er6
    OP_STORE16_MREG_IMM, // 4 bytes // st [er2], adr_of(value)
    OP_STORE16_MIMM_REG, // 4 bytes // st [0x1234], er1
    OP_STORE16_MIMM_IMM, // 6 bytes // st [0x1234], #-1234 

    OP_PUSH8_REG, // Stack operations // 2 bytes // push r4
    OP_PUSH8_IMM, // 2 bytes // push #'a'
    OP_PUSH16_REG, // 2 bytes // push er3
    OP_PUSH16_IMM, // 4 bytes // push #"ab"
    OP_POP8_REG, // 2 bytes // pop r7
    OP_POP16_REG, // 2 bytes // pop er2
    OP_PUSH_LR, // 2 bytes // push lr
    OP_POP_PC, // 2 bytes // pop pc
    OP_PUSH_SP, // 2 bytes // push sp
    OP_POP_SP, // 2 bytes // pop sp

    OP_ADD8_REG_REG, // ALU operations // 2 bytes // add r1, r2
    OP_ADD16_REG_REG, // 2 bytes // add er0, er2
    OP_ADD16_REG_IMM, // 4 bytes // add er3, adr_of(loop_start)
    OP_SUB8_REG_REG, // 2 bytes // sub r4, r5
    OP_SUB16_REG_REG, // 2 bytes // sub er6, er8
    OP_SUB16_REG_IMM, // 4 bytes // sub er1, adr_of(jump_tabel)
    OP_INC8_REG, // 2 bytes // inc r0
    OP_INC16_REG, // 2 bytes // inc er6
    OP_DEC8_REG, // 2 bytes // dec r3
    OP_DEC16_REG, // 2 bytes // dec er4
    OP_MUL8_REG_REG, // 2 bytes // mul r2, r7
    OP_MUL16_REG_REG, // 2 bytes // mul er0, er4
    OP_MUL16_REG_IMM, // 4 bytes // mul er5, #0x0020
    OP_DIV8_REG_REG, // 2 bytes // div r1, r3
    OP_DIV16_REG_REG, // 2 bytes // div er2, er6
    OP_DIV16_REG_IMM, // 4 bytes // div er4, adr_of(divisor)
    OP_MOD8_REG_REG, // 2 bytes // mod r5, r2
    OP_MOD8_REG_IMM, // 4 bytes // mod r7, #10
    OP_MOD16_REG_REG, // 2 bytes // mod er2, er0
    OP_MOD16_REG_IMM, // 4 bytes // mod er2, #0x0003

    OP_AND8_REG_REG, // Logical operations // 2 bytes // and r1, r4
    OP_AND16_REG_REG, // 2 bytes // and er0, er4
    OP_AND16_REG_IMM, // 4 bytes // and er2, #0x00FF
    OP_OR8_REG_REG, // 2 bytes // or r3, r5
    OP_OR16_REG_REG, // 2 bytes // or er0, er6
    OP_OR16_REG_IMM, // 4 bytes // or er2, #0x0F0F
    OP_XOR8_REG_REG, // 2 bytes // xor r7, r2
    OP_XOR16_REG_REG, // 2 bytes // xor er0, er10
    OP_XOR16_REG_IMM, // 4 bytes // xor er4, #0xAAAA
    OP_NOT8_REG, // 2 bytes // not r5
    OP_NOT16_REG, // 2 bytes // not er2

    OP_CMP8_REG_REG, // Comparison operations // 2 bytes // cmp r1, r3
    OP_CMP16_REG_REG, // 2 bytes // cmp er0, er4
    OP_CMP16_REG_IMM, // 4 bytes // cmp er2, adr_of(pivot)
    OP_SLL8_REG_REG, // Shift operations // 2 bytes // Shifting is only 2 bytes since for 16 bit regs, max shift is 15, and that fits in 4 bits // sll r1, r2
    OP_SLL8_REG_IMM, // 2 bytes // sll r3, #4
    OP_SLL16_REG_REG, // 2 bytes // sll er0, er2
    OP_SLL16_REG_IMM, // 2 bytes // sll er4, #8
    OP_SRL8_REG_REG, // 2 bytes // srl r5, r1
    OP_SRL8_REG_IMM, // 2 bytes // srl r7, #3
    OP_SRL16_REG_REG, // 2 bytes // srl er2, er0
    OP_SRL16_REG_IMM, // 2 bytes // srl er6, #12
    OP_SRA8_REG_REG, // 2 bytes // sra r4, r2
    OP_SRA8_REG_IMM, // 2 bytes // sra r6, #1
    OP_SRA16_REG_REG, // 2 bytes // sra er0, er4
    OP_SRA16_REG_IMM, // 2 bytes // sra er8, #5

    OP_B_REG, // Control flow // 2 bytes // b er2
    OP_B_IMM, // 4 bytes // b loop_start (can also be b #0x1234)
    OP_BEQ_IMM, // 2 bytes // beq loop_start
    OP_BNE_IMM, // 2 bytes // bne #-0x2
    OP_BLT_IMM, // 2 bytes // blt loop_end
    OP_BLE_IMM, // 2 bytes // ble increase_counter
    OP_BGT_IMM, // 2 bytes // bgt check_limit
    OP_BGE_IMM, // 2 bytes // bge start_processing
    OP_BL_REG, // 2 bytes // bl er4
    OP_BL_IMM, // 4 bytes // bl function_start
    OP_RT, // 2 bytes // rt

    // The 16-aligned blocks below hold opcodes that modify a register and take an 8-bit immediate.
    // Each listed constant is the base of a 16-entry block (dest register encoded in low nibble).
    // All are 2 bytes long
    OP_MOV8_REG_IMM = 0x60, // mov r0, #0x12
    OP_ADD8_REG_IMM = OP_MOV8_REG_IMM + 0x10, // add r3, #0x05
    OP_SUB8_REG_IMM = OP_ADD8_REG_IMM + 0x10, // sub r7, #0x01
    OP_MUL8_REG_IMM = OP_SUB8_REG_IMM + 0x10, // mul r2, #0x03
    OP_DIV8_REG_IMM = OP_MUL8_REG_IMM + 0x10, // div r4, #0x02
    OP_AND8_REG_IMM = OP_DIV8_REG_IMM + 0x10, // and r5, #0x0F
    OP_OR8_REG_IMM  = OP_AND8_REG_IMM + 0x10, // or r1, #0xF0
    OP_XOR8_REG_IMM = OP_OR8_REG_IMM + 0x10, // xor r6, #0xAA
    OP_CMP8_REG_IMM = OP_XOR8_REG_IMM + 0x10, // cmp r0, #0x10

    OP_SYSCALL = 0xF0,
	OP_END	 = 0xFF

} opcode_t;

// Syscall
typedef enum {
    // Syscall numbers
    COPY_TO_RAM,
    GET_ELEMENT_FIELD,
    SET_ELEMENT_FIELD,
    RENDER_ELEMENT,
    DRAW_LINE,
    SET_PIXEL,
    GET_PIXEL,
    DRAW_IMAGE,
    DRAW_RECT,
    DRAW_TEXT,
    SLEEP,
    GET_TIME_MS,
    GET_TIME_S,
    FS_READ,
    FS_WRITE,
    FS_MKDIR,
    FS_DELETE,
    FS_RENAME,
    FS_MOVE,
    STOP,
    END,
} syscall_t;
// rn[x] are byte registers
// ern[x] are word registers
// element and transform addresses are offsets into vm->code (+4 .. code_size + 4)
// strings and buffers are offsets into vm->ram
// invalid element, field, type, or out-of-bounds access triggers BSOD

// GET_ELEMENT_FIELD
// Inputs:
//   ern[0] element address
//   rn[2]  field id
// Outputs:
//   rn[4]  byte value
//   ern[2] word value or pointer depending on field
// Notes:
//   id and rotation return in ern[2]
//   text returns a pointer in ern[2]
//   most numeric fields return in rn[4]
//   field validity depends on element type

// SET_ELEMENT_FIELD
// Inputs:
//   ern[0] element address
//   rn[2]  field id
//   rn[4]  byte value
//   ern[2] word value or pointer for text and rotation
// Notes:
//   text fields expect a pointer into vm->ram
//   rotation is stored as value modulo 360
//   colour is set via tml_set_colour

// RENDER_ELEMENT
// Inputs:
//   ern[0] element address
//   ern[1] transform address or 0
// Notes:
//   rendering currently stubbed

// DRAW_LINE
// rn[0] x0
// rn[1] y0
// rn[2] x1
// rn[3] y1
// rn[4] colour
// rn[5] thickness
// rn[6] style

// SET_PIXEL
// rn[0] x
// rn[1] y
// rn[2] colour
// rn[3] buffer

// GET_PIXEL
// rn[0] x
// rn[1] y
// rn[2] buffer
// rn[0] returns pixel value

// DRAW_IMAGE
// rn[0] x
// rn[1] y
// rn[2] width
// rn[3] height
// ern[2] image pointer
// rn[6] source x
// rn[7] source y
// ern[4] flags
// rn[10] buffer

// DRAW_RECT
// rn[0] x
// rn[1] y
// rn[2] width
// rn[3] height
// rn[6] radius x
// rn[7] radius y
// ern[4] colour
// rn[10] thickness
// rn[11] style
// rn[12] buffer

// DRAW_TEXT
// rn[0] x
// rn[1] y
// ern[1] text pointer
// rn[4] font
// rn[5] scale
// rn[6] align
// ern[4] colour
// rn[7] buffer

// SLEEP
// ern[2] milliseconds
// currently no-op

// GET_TIME_MS
// ern[0] returns milliseconds

// GET_TIME_S
// ern[0] returns seconds

// FS_READ
// ern[0] path pointer
// ern[1] destination pointer
// ern[2] length

// FS_WRITE
// ern[0] path pointer
// ern[1] source pointer
// ern[2] length

// FS_MKDIR
// ern[0] path pointer

// FS_DELETE
// ern[0] path pointer
// rn[2] 0xFF enables recursive delete

// FS_RENAME
// ern[0] old path pointer
// ern[1] new name pointer

// FS_MOVE
// ern[0] path pointer
// moves node to FS_ROOT

// STOP
// END
// stops VM execution

void vm_init_system(void);
void vm_init(TangentMachine* vm, byte* code);
TangentMachine* vm_spawn(const byte* code);
TangentMachine *vm_run_file(fs_node_t* parent, const char *filename);
void vm_destroy(TangentMachine* vm);
void vm_step(TangentMachine* vm);
void vm_step_all(void);

#endif /* VM_H_ */

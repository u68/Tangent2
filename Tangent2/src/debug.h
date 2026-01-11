/*
 * debug.h
 * Interface for debugging utilities
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "base.h"

typedef enum error_t {
    ERROR_UNKNOWN,
    ERROR_HEAP_OUT_OF_MEMORY,
    ERROR_VM_INVALID_INSTRUCTION,
    ERROR_VM_STACK_OVERFLOW,
    ERROR_VM_STACK_UNDERFLOW,
    ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS,
    ERROR_VM_INVALID_REGISTER,
    ERROR_VM_INVALID_ALU_OPERATION,
    ERROR_VM_INVALID_SYSCALL,
    ERROR_TUI_INVALID_ELEMENT,
    ERROR_TUI_INVALID_ELEMENT_FIELD,
    ERROR_TUI_INVALID_COLOUR,
    ERROR_TUI_INVALID_FONT_SIZE,
    ERROR_TUI_INVALID_PIXEL_SIZE,
    ERROR_TUI_INVALID_LINE_STYLE,
    ERROR_TUI_INVALID_TEXT_ALIGNMENT,
} error_t;

extern void test_regs(); // Function is stored in startup.asm, it stores all registers to 0x9100

void print_error(error_t error_code);
void trigger_bsod(error_t error_code);
void get_regs_snapshot(byte* buffer); // Will trip operation, since snapshot is hardcoded to 0x9100 (see startup.asm)

const char* error_to_string(error_t error_code);

const char ERROR_UNKNOWN_STR[] = "Unknown error";
const char ERROR_HEAP_OUT_OF_MEMORY_STR[] = "Heap out of memory";
const char ERROR_VM_INVALID_INSTRUCTION_STR[] = "VM invalid instruction";
const char ERROR_VM_STACK_OVERFLOW_STR[] = "VM stack overflow";
const char ERROR_VM_STACK_UNDERFLOW_STR[] = "VM stack underflow";
const char ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS_STR[] = "VM out of bounds memory access";
const char ERROR_VM_INVALID_REGISTER_STR[] = "VM invalid register";
const char ERROR_VM_INVALID_ALU_OPERATION_STR[] = "VM invalid ALU operation";
const char ERROR_VM_INVALID_SYSCALL_STR[] = "VM invalid syscall";
const char ERROR_TUI_INVALID_ELEMENT_STR[] = "TUI invalid element";
const char ERROR_TUI_INVALID_ELEMENT_FIELD_STR[] = "TUI invalid element field";
const char ERROR_TUI_INVALID_COLOUR_STR[] = "TUI invalid colour";
const char ERROR_TUI_INVALID_FONT_SIZE_STR[] = "TUI invalid font size";
const char ERROR_TUI_INVALID_PIXEL_SIZE_STR[] = "TUI invalid pixel size";
const char ERROR_TUI_INVALID_LINE_STYLE_STR[] = "TUI invalid line style";
const char ERROR_TUI_INVALID_TEXT_ALIGNMENT_STR[] = "TUI invalid text alignment";

#endif /* DEBUG_H_ */
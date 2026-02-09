/*
 * debug.h
 * Interface for debugging utilities
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "../libcw.h"

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

extern void test_regs(void); // Function is stored in startup.asm, it stores all registers to 0x9100

void print_error(error_t error_code);
void trigger_bsod(error_t error_code);
void get_regs_snapshot(byte *buffer); // Will trip operation, since snapshot is hardcoded to 0x9100 (see startup.asm)

const char* error_to_string(error_t error_code);

extern const char ERROR_UNKNOWN_STR[];
extern const char ERROR_HEAP_OUT_OF_MEMORY_STR[];
extern const char ERROR_VM_INVALID_INSTRUCTION_STR[];
extern const char ERROR_VM_STACK_OVERFLOW_STR[];
extern const char ERROR_VM_STACK_UNDERFLOW_STR[];
extern const char ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS_STR[];
extern const char ERROR_VM_INVALID_REGISTER_STR[];
extern const char ERROR_VM_INVALID_ALU_OPERATION_STR[];
extern const char ERROR_VM_INVALID_SYSCALL_STR[];
extern const char ERROR_TUI_INVALID_ELEMENT_STR[];
extern const char ERROR_TUI_INVALID_ELEMENT_FIELD_STR[];
extern const char ERROR_TUI_INVALID_COLOUR_STR[];
extern const char ERROR_TUI_INVALID_FONT_SIZE_STR[];
extern const char ERROR_TUI_INVALID_PIXEL_SIZE_STR[];
extern const char ERROR_TUI_INVALID_LINE_STYLE_STR[];
extern const char ERROR_TUI_INVALID_TEXT_ALIGNMENT_STR[];

#endif /* DEBUG_H_ */

/*
 * debug.c
 * Implementation of debugging utilities
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#include "debug.h"
#include "bsod.h"
#include "heap.h"
#include "Tui/glib.h"

// Convert error code to string and print
void print_error(error_t error_code) {
    const char* error_str = error_to_string(error_code);
    tui_draw_text(0, 0, error_str, TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
}

// Black screen of death wrapper
void trigger_bsod(error_t error_code) {
    test_regs(); // Store registers to 0x9100
    byte* regs_snapshot = (byte*)0x9100;
    bsod_show(error_code, regs_snapshot);
}

// Get registers snapshot into provided buffer
void get_regs_snapshot(byte* buffer) {
    if (!buffer) return;
    test_regs(); // Store registers to 0x9100
    byte* regs_snapshot = (byte*)0x9100;
    for (int i = 0; i < 24; i++) {
        buffer[i] = regs_snapshot[i];
    }
}

// Convert error code to string
const char* error_to_string(error_t error_code) {
    switch (error_code) {
    case ERROR_UNKNOWN:
        return ERROR_UNKNOWN_STR;
    case ERROR_HEAP_OUT_OF_MEMORY:
        return ERROR_HEAP_OUT_OF_MEMORY_STR;
    case ERROR_VM_INVALID_INSTRUCTION:
        return ERROR_VM_INVALID_INSTRUCTION_STR;
    case ERROR_VM_STACK_OVERFLOW:
        return ERROR_VM_STACK_OVERFLOW_STR;
    case ERROR_VM_STACK_UNDERFLOW:
        return ERROR_VM_STACK_UNDERFLOW_STR;
    case ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS:
        return ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS_STR;
    case ERROR_VM_INVALID_REGISTER:
        return ERROR_VM_INVALID_REGISTER_STR;
    case ERROR_VM_INVALID_ALU_OPERATION:
        return ERROR_VM_INVALID_ALU_OPERATION_STR;
    case ERROR_VM_INVALID_SYSCALL:
        return ERROR_VM_INVALID_SYSCALL_STR;
    case ERROR_TUI_INVALID_ELEMENT:
        return ERROR_TUI_INVALID_ELEMENT_STR;
    case ERROR_TUI_INVALID_ELEMENT_FIELD:
        return ERROR_TUI_INVALID_ELEMENT_FIELD_STR;
    case ERROR_TUI_INVALID_COLOUR:
        return ERROR_TUI_INVALID_COLOUR_STR;
    case ERROR_TUI_INVALID_FONT_SIZE:
        return ERROR_TUI_INVALID_FONT_SIZE_STR;
    case ERROR_TUI_INVALID_PIXEL_SIZE:
        return ERROR_TUI_INVALID_PIXEL_SIZE_STR;
    case ERROR_TUI_INVALID_LINE_STYLE:
        return ERROR_TUI_INVALID_LINE_STYLE_STR;
    case ERROR_TUI_INVALID_TEXT_ALIGNMENT:
        return ERROR_TUI_INVALID_TEXT_ALIGNMENT_STR;
    default:
        return ERROR_UNKNOWN_STR;
    }
}
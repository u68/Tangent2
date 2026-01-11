/*
 * debug.c
 * Implementation of debugging utilities
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#include "debug.h"
#include "bsod.h"
#include "../tui/glib.h"
#include "../heap/heap.h"

// Error strings
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
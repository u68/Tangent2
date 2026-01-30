/*
 * vm.c
 * Implementation of virtual machine
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#include "vm.h"
#include "../tui/glib.h"
#include "../heap/heap.h"
#include "../debug/debug.h"
#include "../time/time.h"
#include "../fs/fs.h"

// Internals

#define SIGNED_LT(psw)  ((psw).field.neg != (psw).field.overflow)
#define SIGNED_GT(psw)  (!((psw).field.zero) && ((psw).field.neg == (psw).field.overflow))
#define SIGNED_LE(psw)  ((psw).field.zero || ((psw).field.neg != (psw).field.overflow))
#define SIGNED_GE(psw)  ((psw).field.neg == (psw).field.overflow)
#define SIGNED_EQ(psw)  ((psw).field.zero)
#define SIGNED_NE(psw)  (!((psw).field.zero))

#define UNSIGNED_LT(psw)  ((psw).field.carry)
#define UNSIGNED_GE(psw)  (!((psw).field.carry))
#define UNSIGNED_EQ(psw)  ((psw).field.zero)
#define UNSIGNED_NE(psw)  (!((psw).field.zero))
#define UNSIGNED_GT(psw)  (!((psw).field.zero) && !((psw).field.carry))
#define UNSIGNED_LE(psw)  ((psw).field.zero || ((psw).field.carry))

static TangentMachine** vm_pool = 0;
static word vm_capacity = 0;

// Opcode definitions


typedef enum {
    ALU_OP_ADD,
    ALU_OP_SUB,
    ALU_OP_AND,
    ALU_OP_OR,
    ALU_OP_XOR,
    ALU_OP_NOT,
    ALU_OP_INC,
    ALU_OP_DEC,
    ALU_OP_SLL,
    ALU_OP_SRL,
    ALU_OP_SRA,
    ALU_OP_MUL,
    ALU_OP_DIV,
    ALU_OP_MOD,
} alu_op_t;

// ALU operation declarations
static psw_flags_t alu_operation_byte(alu_op_t op, byte a, byte b, byte* result);
static psw_flags_t alu_operation_word(alu_op_t op, word a, word b, word* result);

// Memory access and stack operations
static void write_memory_byte(TangentMachine* vm, word address, byte value);
static void write_memory_word(TangentMachine* vm, word address, word value);
static byte read_memory_byte(TangentMachine* vm, word address);
static word read_memory_word(TangentMachine* vm, word address);
static void push_stack_byte(TangentMachine* vm, byte value);
static void push_stack_word(TangentMachine* vm, word value);
static byte pop_stack_byte(TangentMachine* vm);
static word pop_stack_word(TangentMachine* vm);

// Comparison operations
static psw_flags_t cmp_bytes(byte a, byte b);
static psw_flags_t cmp_words(word a, word b);

// Syscall
typedef enum {
    // Syscall numbers
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
    STOP,
    END,
} syscall_t;

static void vm_syscall(TangentMachine* vm, syscall_t syscall_number);

// ALU operation implementations
static psw_flags_t alu_operation_byte(alu_op_t op, byte a, byte b, byte* result) {
    psw_flags_t flags = {0};
    byte res = 0;

    switch (op) {
        case ALU_OP_ADD: {
            dword temp = (dword)a + (dword)b;
            res = (byte)(temp & 0xFF);
            flags.field.carry = (temp > 0xFF);
            flags.field.overflow = (((a ^ res) & (b ^ res) & 0x80) != 0);
            break;
        }
        case ALU_OP_SUB: {
            dword temp = (dword)a - (dword)b;
            res = (byte)(temp & 0xFF);
            flags.field.carry = (a < b); // borrow
            flags.field.overflow = (((a ^ b) & (a ^ res) & 0x80) != 0);
            break;
        }
        case ALU_OP_AND:
            res = a & b;
            break;
        case ALU_OP_OR:
            res = a | b;
            break;
        case ALU_OP_XOR:
            res = a ^ b;
            break;
        case ALU_OP_NOT:
            res = ~a;
            break;
        case ALU_OP_INC:
            res = a + 1;
            flags.field.overflow = (a == 0x7F);
            break;
        case ALU_OP_DEC:
            res = a - 1;
            flags.field.overflow = (a == 0x80);
            break;
        case ALU_OP_SLL: {
            byte n = (byte)(b & 7);
            if (n == 0) {
                res = a;
                flags.field.carry = 0;
            } else {
                res = (byte)(a << n);
                flags.field.carry = (byte)((a >> (8 - n)) & 0x01);
            }
            break;
        }
        case ALU_OP_SRL: {
            byte n = (byte)(b & 7);
            if (n == 0) {
                res = a;
                flags.field.carry = 0;
            } else {
                res = (byte)(a >> n);
                flags.field.carry = (byte)((a >> (n - 1)) & 0x01);
            }
            break;
        }
        case ALU_OP_SRA: {
            byte n = (byte)(b & 7);
            if (n == 0) {
                res = a;
                flags.field.carry = 0;
            } else {
                sbyte sa = (sbyte)a;
                res = (byte)((sa >> n) & 0xFF);
                flags.field.carry = (byte)((a >> (n - 1)) & 0x01);
            }
            break;
        }
        case ALU_OP_MUL: {
            dword temp = (dword)a * (dword)b;
            res = (byte)(temp & 0xFF);
            flags.field.carry = (temp > 0xFF);
            break;
        }
        case ALU_OP_DIV:
            if (b == 0) res = 0;
            else res = (byte)(a / b);
            break;
        case ALU_OP_MOD:
            if (b == 0) res = 0;
            else res = (byte)(a % b);
            break;
        default:
            trigger_bsod(ERROR_VM_INVALID_ALU_OPERATION);
            break;
    }

    flags.field.zero = (res == 0);
    flags.field.neg = (res & 0x80) != 0;

    if (result) {
        *result = res;
    }
    return flags;
}

static psw_flags_t alu_operation_word(alu_op_t op, word a, word b, word* result) {
    psw_flags_t flags = {0};
    word res = 0;

    switch (op) {
        case ALU_OP_ADD: {
            dword temp = (dword)a + (dword)b;
            res = (word)(temp & 0xFFFF);
            flags.field.carry = (temp > 0xFFFF);
            flags.field.overflow = (((a ^ res) & (b ^ res) & 0x8000) != 0);
            break;
        }
        case ALU_OP_SUB: {
            dword temp = (dword)a - (dword)b;
            res = (word)(temp & 0xFFFF);
            flags.field.carry = (a < b); // borrow
            flags.field.overflow = (((a ^ b) & (a ^ res) & 0x8000) != 0);
            break;
        }
        case ALU_OP_AND:
            res = a & b;
            break;
        case ALU_OP_OR:
            res = a | b;
            break;
        case ALU_OP_XOR:
            res = a ^ b;
            break;
        case ALU_OP_NOT:
            res = ~a;
            break;
        case ALU_OP_INC:
            res = a + 1;
            flags.field.overflow = (a == 0x7FFF);
            break;
        case ALU_OP_DEC:
            res = a - 1;
            flags.field.overflow = (a == 0x8000);
            break;
        case ALU_OP_SLL: {
            byte n = (byte)(b & 15);
            if (n == 0) {
                res = a;
                flags.field.carry = 0;
            } else {
                res = (word)(a << n);
                flags.field.carry = (word)((a >> (16 - n)) & 0x0001);
            }
            break;
        }
        case ALU_OP_SRL: {
            byte n = (byte)(b & 15);
            if (n == 0) {
                res = a;
                flags.field.carry = 0;
            } else {
                res = (word)(a >> n);
                flags.field.carry = (word)((a >> (n - 1)) & 0x0001);
            }
            break;
        }
        case ALU_OP_SRA: {
            byte n = (byte)(b & 15);
            if (n == 0) {
                res = a;
                flags.field.carry = 0;
            } else {
                sword sa = (sword)a;
                res = (word)((sa >> n) & 0xFFFF);
                flags.field.carry = (word)((a >> (n - 1)) & 0x0001);
            }
            break;
        }
        case ALU_OP_MUL: {
            dword temp = (dword)a * (dword)b;
            res = (word)(temp & 0xFFFF);
            flags.field.carry = (temp > 0xFFFF);
            break;
        }
        case ALU_OP_DIV:
            if (b == 0) res = 0;
            else res = (word)(a / b);
            break;
        case ALU_OP_MOD:
            if (b == 0) res = 0;
            else res = (word)(a % b);
            break;
        default:
            trigger_bsod(ERROR_VM_INVALID_ALU_OPERATION);
            break;
    }

    flags.field.zero = (res == 0);
    flags.field.neg = (res & 0x8000) != 0;

    if (result) {
        *result = res;
    }
    return flags;
}

// Memory access implementations
// Pretty much self explanatory

static void write_memory_byte(TangentMachine* vm, word address, byte value) {
    if (vm->vm_properties.uses_ram && address < vm->vm_properties.ram_size) {
        vm->ram[address] = value;
    } else {
        trigger_bsod(ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS);
    }
}

static void write_memory_word(TangentMachine* vm, word address, word value) {
    if (vm->vm_properties.uses_ram && address + 1 < vm->vm_properties.ram_size) {
        vm->ram[address] = value & 0xFF;
        vm->ram[address + 1] = (value >> 8) & 0xFF;
    } else {
        trigger_bsod(ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS);
    }
}

static byte read_memory_byte(TangentMachine* vm, word address) {
    if (vm->vm_properties.uses_ram && address < vm->vm_properties.ram_size) {
        return vm->ram[address];
    } else {
        trigger_bsod(ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS);
        return 0;
    }
}

static word read_memory_word(TangentMachine* vm, word address) {
    if (vm->vm_properties.uses_ram && address + 1 < vm->vm_properties.ram_size) {
        return vm->ram[address] | (vm->ram[address + 1] << 8);
    } else {
        trigger_bsod(ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS);
        return 0;
    }
}

static void push_stack_byte(TangentMachine* vm, byte value) {
    if (vm->sp == 0) {
        trigger_bsod(ERROR_VM_STACK_OVERFLOW);
        return;
    }
    vm->sp--;
    write_memory_byte(vm, vm->sp, value);
}

static void push_stack_word(TangentMachine* vm, word value) {
    if (vm->sp < 2) {
        trigger_bsod(ERROR_VM_STACK_OVERFLOW);
        return;
    }
    vm->sp -= 2;
    write_memory_word(vm, vm->sp, value);
}

static byte pop_stack_byte(TangentMachine* vm) {
    if (vm->sp >= vm->vm_properties.ram_size) {
        trigger_bsod(ERROR_VM_STACK_UNDERFLOW);
        return 0;
    }
    byte value = read_memory_byte(vm, vm->sp);
    vm->sp++;
    return value;
}

static word pop_stack_word(TangentMachine* vm) {
    if (vm->sp + 1 >= vm->vm_properties.ram_size) {
        trigger_bsod(ERROR_VM_STACK_UNDERFLOW);
        return 0;
    }
    word value = read_memory_word(vm, vm->sp);
    vm->sp += 2;
    return value;
}

static psw_flags_t cmp_bytes(byte a, byte b) {
    psw_flags_t flags = {0};
    byte res = a - b;

    flags.field.zero  = (res == 0);
    flags.field.neg   = (res & 0x80) != 0;
    flags.field.carry = (a < b); // borrow
    flags.field.overflow = ((a ^ b) & (a ^ res) & 0x80) != 0;

    return flags;
}

static psw_flags_t cmp_words(word a, word b) {
    psw_flags_t flags = {0};
    word res = a - b;

    flags.field.zero  = (res == 0);
    flags.field.neg   = (res & 0x8000) != 0;
    flags.field.carry = (a < b); // borrow
    flags.field.overflow = ((a ^ b) & (a ^ res) & 0x8000) != 0;

    return flags;
}

// Syscall handler
static void vm_syscall(TangentMachine* vm, syscall_t syscall_number) {
    switch (syscall_number) {
        case GET_ELEMENT_FIELD: {
            word elem_addr = vm->registers.ern[0];
            if (elem_addr < 4 || elem_addr >= (word)(vm->vm_properties.code_size + 4)) { trigger_bsod(ERROR_TUI_INVALID_ELEMENT); return; }
            TmlElement* elem = (TmlElement*)(vm->code + elem_addr);
            byte field = vm->registers.rn[2];
            switch (field) {
                case FIELD_ID:
                    vm->registers.ern[2] = elem->id;
                    break;
                case FIELD_X:
                    vm->registers.rn[4] = elem->x;
                    break;
                case FIELD_Y:
                    vm->registers.rn[4] = elem->y;
                    break;
                case FIELD_WIDTH:
                    if (elem->type == TML_TYPE_BUTTON) vm->registers.rn[4] = elem->data.button.width;
                    else if (elem->type == TML_TYPE_DIV) vm->registers.rn[4] = elem->data.div.width;
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_HEIGHT:
                    if (elem->type == TML_TYPE_BUTTON) vm->registers.rn[4] = elem->data.button.height;
                    else if (elem->type == TML_TYPE_DIV) vm->registers.rn[4] = elem->data.div.height;
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_COLOUR:
                    vm->registers.rn[4] = elem->colour;
                    break;
                case FIELD_THICKNESS:
                    if (elem->type == TML_TYPE_BUTTON) vm->registers.rn[4] = elem->data.button.border_thickness;
                    else if (elem->type == TML_TYPE_DIV) vm->registers.rn[4] = elem->data.div.border_thickness;
                    else if (elem->type == TML_TYPE_LINE) vm->registers.rn[4] = elem->data.line.thickness;
                    else if (elem->type == TML_TYPE_CHECKBOX) vm->registers.rn[4] = elem->data.checkbox.border_thickness;
                    else if (elem->type == TML_TYPE_RADIO) vm->registers.rn[4] = elem->data.radio.border_thickness;
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_STYLE:
                    if (elem->type == TML_TYPE_BUTTON) vm->registers.rn[4] = elem->data.button.border_style;
                    else if (elem->type == TML_TYPE_DIV) vm->registers.rn[4] = elem->data.div.border_style;
                    else if (elem->type == TML_TYPE_LINE) vm->registers.rn[4] = elem->data.line.style;
                    else if (elem->type == TML_TYPE_CHECKBOX) vm->registers.rn[4] = elem->data.checkbox.border_style;
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_FONT:
                    if (elem->type == TML_TYPE_TEXT) vm->registers.rn[4] = elem->data.text.font_size;
                    else if (elem->type == TML_TYPE_BUTTON) vm->registers.rn[4] = elem->data.button.font_size;
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_TEXT:
                    if (elem->type == TML_TYPE_TEXT) vm->registers.ern[2] = (word)elem->data.text.text;
                    else if (elem->type == TML_TYPE_BUTTON) vm->registers.ern[2] = (word)elem->data.button.text;
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_ALIGN:
                    if (elem->type == TML_TYPE_BUTTON) vm->registers.rn[4] = elem->data.button.text_align;
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_CHECKED:
                    if (elem->type == TML_TYPE_CHECKBOX) vm->registers.rn[4] = elem->select.field.selected;
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_SELECTED:
                    if (elem->type == TML_TYPE_RADIO) vm->registers.rn[4] = elem->select.field.selected;
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_SIZE:
                    if (elem->type == TML_TYPE_CHECKBOX) vm->registers.rn[4] = elem->data.checkbox.size;
                    else if (elem->type == TML_TYPE_RADIO) vm->registers.rn[4] = elem->data.radio.size;
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_END_X:
                    if (elem->type == TML_TYPE_LINE) vm->registers.rn[4] = elem->data.line.x1;
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_END_Y:
                    if (elem->type == TML_TYPE_LINE) vm->registers.rn[4] = elem->data.line.y1;
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_ROTATION:
                    vm->registers.ern[2] = elem->rotation;
                    break;
                case FIELD_ANCHOR_X:
                    vm->registers.rn[4] = elem->anchor_x;
                    break;
                case FIELD_ANCHOR_Y:
                    vm->registers.rn[4] = elem->anchor_y;
                    break;
                default:
                    trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD);
                    break;
            }
            break;
        }
        case SET_ELEMENT_FIELD: {
            word elem_addr = vm->registers.ern[0];
            if (elem_addr < 4 || elem_addr >= (word)(vm->vm_properties.code_size + 4)) { trigger_bsod(ERROR_TUI_INVALID_ELEMENT); return; }
            TmlElement* elem = (TmlElement*)(vm->code + elem_addr);
            byte field = vm->registers.rn[2];
            switch (field) {
                case FIELD_ID:
                    elem->id = (word)vm->registers.ern[2];
                    break;
                case FIELD_X:
                    elem->x = (byte)vm->registers.rn[4];
                    break;
                case FIELD_Y:
                    elem->y = (byte)vm->registers.rn[4];
                    break;
                case FIELD_WIDTH:
                    if (elem->type == TML_TYPE_BUTTON) elem->data.button.width = (byte)vm->registers.rn[4];
                    else if (elem->type == TML_TYPE_DIV) elem->data.div.width = (byte)vm->registers.rn[4];
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_HEIGHT:
                    if (elem->type == TML_TYPE_BUTTON) elem->data.button.height = (byte)vm->registers.rn[4];
                    else if (elem->type == TML_TYPE_DIV) elem->data.div.height = (byte)vm->registers.rn[4];
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_COLOUR:
                    tml_set_colour(elem, (byte)vm->registers.rn[4]);
                    break;
                case FIELD_THICKNESS:
                    if (elem->type == TML_TYPE_BUTTON) elem->data.button.border_thickness = (byte)vm->registers.rn[4];
                    else if (elem->type == TML_TYPE_DIV) elem->data.div.border_thickness = (byte)vm->registers.rn[4];
                    else if (elem->type == TML_TYPE_LINE) elem->data.line.thickness = (byte)vm->registers.rn[4];
                    else if (elem->type == TML_TYPE_CHECKBOX) elem->data.checkbox.border_thickness = (byte)vm->registers.rn[4];
                    else if (elem->type == TML_TYPE_RADIO) elem->data.radio.border_thickness = (byte)vm->registers.rn[4];
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_STYLE:
                    if (elem->type == TML_TYPE_BUTTON) elem->data.button.border_style = (byte)vm->registers.rn[4];
                    else if (elem->type == TML_TYPE_DIV) elem->data.div.border_style = (byte)vm->registers.rn[4];
                    else if (elem->type == TML_TYPE_LINE) elem->data.line.style = (byte)vm->registers.rn[4];
                    else if (elem->type == TML_TYPE_CHECKBOX) elem->data.checkbox.border_style = (byte)vm->registers.rn[4];
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_FONT:
                    if (elem->type == TML_TYPE_TEXT) elem->data.text.font_size = (byte)vm->registers.rn[4];
                    else if (elem->type == TML_TYPE_BUTTON) elem->data.button.font_size = (byte)vm->registers.rn[4];
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_TEXT:
                    if (elem->type == TML_TYPE_TEXT) elem->data.text.text = (const char*)vm->registers.ern[2];
                    else if (elem->type == TML_TYPE_BUTTON) elem->data.button.text = (const char*)vm->registers.ern[2];
                    else if (elem->type == TML_TYPE_INPUT) elem->data.input.text = (char*)vm->registers.ern[2];
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_ALIGN:
                    if (elem->type == TML_TYPE_BUTTON) elem->data.button.text_align = (byte)vm->registers.rn[4];
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_CHECKED:
                    if (elem->type == TML_TYPE_CHECKBOX) elem->select.field.selected = (byte)vm->registers.rn[4];
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_SELECTED:
                    if (elem->type == TML_TYPE_RADIO) elem->select.field.selected = (byte)vm->registers.rn[4];
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_SIZE:
                    if (elem->type == TML_TYPE_CHECKBOX) elem->data.checkbox.size = (byte)vm->registers.rn[4];
                    else if (elem->type == TML_TYPE_RADIO) elem->data.radio.size = (byte)vm->registers.rn[4];
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_END_X:
                    if (elem->type == TML_TYPE_LINE) elem->data.line.x1 = (byte)vm->registers.rn[4];
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_END_Y:
                    if (elem->type == TML_TYPE_LINE) elem->data.line.y1 = (byte)vm->registers.rn[4];
                    else { trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD); }
                    break;
                case FIELD_ROTATION:
                    elem->rotation = vm->registers.ern[2] % 360;
                    break;
                case FIELD_ANCHOR_X:
                    elem->anchor_x = (byte)vm->registers.rn[4];
                    break;
                case FIELD_ANCHOR_Y:
                    elem->anchor_y = (byte)vm->registers.rn[4];
                    break;
                default:
                    trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD);
                    break;
            }
            break;
        }
        case RENDER_ELEMENT: {
            word elem_addr = vm->registers.ern[0];
            if (elem_addr < 4 || elem_addr >= (word)(vm->vm_properties.code_size + 4)) { trigger_bsod(ERROR_TUI_INVALID_ELEMENT); return; }
            TmlElement* elem = (TmlElement*)(vm->code + elem_addr);
            TmlTransform* transform = 0;
            word tr_addr = vm->registers.ern[1];
            if (tr_addr) {
                if (tr_addr < 4 || tr_addr >= (word)(vm->vm_properties.code_size + 4)) { trigger_bsod(ERROR_TUI_INVALID_ELEMENT); return; }
                transform = (TmlTransform*)(vm->code + tr_addr);
            }
            //tml_render_element(elem, transform);
            break;
        }
        case DRAW_LINE:
            tui_draw_line(vm->registers.rn[0],vm->registers.rn[1],vm->registers.rn[2],vm->registers.rn[3],vm->registers.rn[4],vm->registers.rn[5],vm->registers.rn[6]);
            break;
        case SET_PIXEL:
            tui_set_pixel(vm->registers.rn[0],vm->registers.rn[1],vm->registers.rn[2],vm->registers.rn[3]);
            break;
        case GET_PIXEL: {
            byte x = vm->registers.rn[0];
            byte y = vm->registers.rn[1];
            byte buf = vm->registers.rn[2];
            byte val = buf ? tui_get_pixel_b(x, y, buf) : tui_get_pixel(x, y);
            vm->registers.rn[0] = val;
            break;
        }
        case DRAW_IMAGE:
            tui_draw_image(vm->registers.rn[0],vm->registers.rn[1],vm->registers.rn[2],vm->registers.rn[3],(const byte*)vm->registers.ern[2],vm->registers.rn[6],vm->registers.rn[7],vm->registers.ern[4],vm->registers.rn[10]);
            break;
        case DRAW_RECT:
            tui_draw_rectangle(vm->registers.rn[0], vm->registers.rn[1], vm->registers.rn[2], vm->registers.rn[3], (sbyte)vm->registers.rn[6], (sbyte)vm->registers.rn[7], vm->registers.ern[4], vm->registers.rn[10], vm->registers.rn[11], vm->registers.rn[12]);
            break;
        case DRAW_TEXT:
            tui_draw_text(vm->registers.rn[0],vm->registers.rn[1],(const char*)vm->registers.ern[1],vm->registers.rn[4],vm->registers.rn[5],vm->registers.rn[6],vm->registers.ern[4],vm->registers.rn[7]);
            break;
        case SLEEP: {
            // I will implement this later to only pause the VM instead of the whole system
            //delay_ms(vm->registers.ern[2]);
            break;
        }
        case GET_TIME_MS:
            vm->registers.ern[0] = (Timer0Counter >> 3);
            break;
        // Unreliable ish
        case GET_TIME_S:
            vm->registers.ern[0] = (Timer0Counter >> 3) / 1000;
            break;
        case FS_READ: {
            // Check if file exists
            fs_node_t *fs_chk = fs_touch(FS_ROOT, (const char*)vm->registers.ern[0], PERMS_RW);
            if (fs_chk) {
                fs_read_file(fs_chk, (byte*)vm->registers.ern[1], vm->registers.rn[2]);
            }
            break;
        }
        case FS_WRITE: {
            // Check if file exists
            fs_node_t *fs_chk = fs_touch(FS_ROOT, (const char*)vm->registers.ern[0], PERMS_RW);
            if (fs_chk) {
                fs_write_file(fs_chk, (const byte*)vm->registers.ern[1], vm->registers.rn[2]);
            }
            break;
        }
        case FS_MKDIR:
            fs_mkdir(FS_ROOT, (const char*)vm->registers.ern[0], PERMS_RW);
            break;
        case FS_DELETE: {
            fs_node_t *fs_chk = fs_touch(FS_ROOT, (const char*)vm->registers.ern[0], PERMS_RW);
            if (fs_chk) {
                fs_delete_node(fs_chk);
            }
            break;

        }
        case STOP:
            vm->vm_properties.running = 0;
            break;
        case END:
            vm->vm_properties.running = 0;
            break;
        default:
            trigger_bsod(ERROR_VM_INVALID_SYSCALL);
            break;
    }
}

// Initialize VM system with heap-allocated pool of NULL pointers
void vm_init_system(void) {
    vm_capacity = MAX_VMS;
    vm_pool = (TangentMachine**)hcalloc(vm_capacity, sizeof(TangentMachine*));
}

// Initialize individual VM structure with external code pointer and RAM pointers
void vm_init(TangentMachine* vm, byte* code) {
    if (!vm || !code) return;

    // RAM is allocated immediately after the VM structure
    vm->ram = (byte*)vm + sizeof(TangentMachine);

    // Code pointer is external
    vm->code = code;

    // Ensure uses_ram reflects ram_size (vm_spawn sets ram_size and code_size)
    vm->vm_properties.uses_ram = (vm->vm_properties.ram_size > 0) ? 1 : 0;
    vm->vm_properties.running = 1;
    vm->sp = vm->vm_properties.ram_size; // Stack pointer starts at top of RAM
    vm->pc = 4; // code starts after the 2-word (4-byte) header
} 

// Create new VM from bytecode, add to pool, expand pool if needed
TangentMachine* vm_spawn(const byte* code) {
    if (!code) return 0;
    
    // Find first NULL slot in pool
    word slot = vm_capacity;
    for (word i = 0; i < vm_capacity; i++) {
        if (vm_pool[i] == 0) {
            slot = i;
            break;
        }
    }

    // If no free slot, expand pool
    if (slot == vm_capacity) {
        word new_capacity = vm_capacity + MAX_VMS;
        vm_pool = (TangentMachine**)hrealloc(vm_pool, new_capacity * sizeof(TangentMachine*));
        for (word i = vm_capacity; i < new_capacity; i++) {
            vm_pool[i] = 0;
        }
        vm_capacity = new_capacity;
        slot = vm_capacity - MAX_VMS;
    }

    // Get code_size and RAM size from code header (explicit little-endian)
    word code_size = (word)(code[0] | (code[1] << 8));
    word ram_size = (word)(code[2] | (code[3] << 8));
    word total_size = sizeof(TangentMachine) + ram_size;

    // Allocate VM structure + RAM from heap (no code)
    TangentMachine* vm = (TangentMachine*)hcalloc(1, total_size);
    if (!vm) return 0;

    // Store header info and initialize fields
    vm->vm_properties.code_size = code_size;
    vm->vm_properties.ram_size = ram_size;
    vm->vm_properties.uses_ram = (ram_size > 0) ? 1 : 0;
    vm->vm_properties.running = 1;
    vm->code = code;

    // Initialize VM with external code pointer and per-instance fields
    vm_init(vm, code);
    vm_pool[slot] = vm;
    return vm; 
}

// Free VM from heap and set its pool slot to NULL
void vm_destroy(TangentMachine* vm) {
    if (vm) {
        // Find and free the slot
        for (word i = 0; i < vm_capacity; i++) {
            if (vm_pool[i] == vm) {
                hfree(vm);
                vm_pool[i] = 0;
                break;
            }
        }
    }
}

// Execute single instruction for given VM
void vm_step(TangentMachine* vm) {
    if (!vm || !vm->vm_properties.running) return;

    /* Code bounds: code bytes are located starting at offset 4, length vm->vm_properties.code_size */
    word code_start = 4;
    word code_end = (word)(vm->vm_properties.code_size + code_start); /* exclusive */

    /* Ensure we have at least opcode+operand available */
    if (vm->pc < code_start || vm->pc + 1 >= code_end) {
        trigger_bsod(ERROR_VM_INVALID_INSTRUCTION);
        return;
    }

    // Fetch instruction
    byte opcode = vm->code[vm->pc];
    vm->pc++;
    byte operand_whole = vm->code[vm->pc];
    byte operand1 = (operand_whole >> 4) & 0x0F;
    byte operand2 = operand_whole & 0x0F;

    switch(opcode) {
        case OP_MOV8_REG_REG:
            {
                vm->registers.rn[operand1] = vm->registers.rn[operand2];
                vm->pc += 1;
                vm->psw.raw  = 0;
                vm->psw.field.zero = (vm->registers.rn[operand1] == 0);
                break;
            }
        case OP_MOV16_REG_IMM:
            {
                /* need operand + 2 bytes */
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte dest = operand1 & 0x07;
                word imm = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                vm->registers.ern[dest] = imm;
                vm->pc += 3;
                vm->psw.raw = 0;
                vm->psw.field.zero = (imm == 0);
                break;
            }
        case OP_MOV16_REG_REG:
            {
                byte dest = operand1 & 0x07;
                byte src = operand2 & 0x07;
                vm->registers.ern[dest] = vm->registers.ern[src];
                vm->pc += 1;
                vm->psw.raw = 0;
                vm->psw.field.zero = (vm->registers.ern[dest] == 0);
                break;
            }
        case OP_LOAD8_REG_MREG:
            {
                byte dest = operand1;
                byte addr_reg = operand2 & 0x07;
                word address = vm->registers.ern[addr_reg];
                byte value = read_memory_byte(vm, address);
                vm->registers.rn[dest] = value;
                vm->pc += 1;
                vm->psw.raw = 0;
                vm->psw.field.zero = (value == 0);
                break;
            }
        case OP_LOAD8_REG_MIMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte dest = operand1;
                word address = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                byte value = read_memory_byte(vm, address);
                vm->registers.rn[dest] = value;
                vm->pc += 3;
                vm->psw.raw = 0;
                vm->psw.field.zero = (value == 0);
                break;
            }
        case OP_LOAD16_REG_MREG:
            {
                byte dest = operand1 & 0x07;
                byte addr_reg = operand2 & 0x07;
                word address = vm->registers.ern[addr_reg];
                word value = read_memory_word(vm, address);
                vm->registers.ern[dest] = value;
                vm->pc += 1;
                vm->psw.raw = 0;
                vm->psw.field.zero = (value == 0);
                break;
            }
        case OP_LOAD16_REG_MIMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte dest = operand1 & 0x07;
                word address = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                word value = read_memory_word(vm, address);
                vm->registers.ern[dest] = value;
                vm->pc += 3;
                vm->psw.raw = 0;
                vm->psw.field.zero = (value == 0);
                break;
            }
        case OP_STORE8_MREG_REG:
            {
                byte addr_reg = operand1 & 0x07;
                byte src = operand2;
                word address = vm->registers.ern[addr_reg];
                byte value = vm->registers.rn[src];
                write_memory_byte(vm, address, value);
                vm->pc += 1;
                break;
            }
        case OP_STORE8_MREG_IMM:
            {
                if (vm->pc + 1 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte addr_reg = operand1 & 0x07;
                word address = vm->registers.ern[addr_reg];
                byte value = vm->code[vm->pc + 1];
                write_memory_byte(vm, address, value);
                vm->pc += 3;
                break;
            }
        case OP_STORE8_MIMM_REG:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte src = operand2;
                word address = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                byte value = vm->registers.rn[src];
                write_memory_byte(vm, address, value);
                vm->pc += 3;
                break;
            }
        case OP_STORE8_MIMM_IMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                word address = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                byte value = operand_whole;
                write_memory_byte(vm, address, value);
                vm->pc += 3;
                break;
            }
        case OP_STORE16_MREG_REG:
            {
                byte src = operand2 & 0x07;
                byte addr_reg = operand1 & 0x07;
                word address = vm->registers.ern[addr_reg];
                word value = vm->registers.ern[src];
                write_memory_word(vm, address, value);
                vm->pc += 1;
                break;
            }
        case OP_STORE16_MREG_IMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte src = operand2 & 0x07;
                word address = vm->registers.ern[src];
                word value = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                write_memory_word(vm, address, value);
                vm->pc += 3;
                break;
            }
        case OP_STORE16_MIMM_REG:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte src = operand2 & 0x07;
                word address = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                word value = vm->registers.ern[src];
                write_memory_word(vm, address, value);
                vm->pc += 3;
                break;
            }
        case OP_STORE16_MIMM_IMM:
            {
                if (vm->pc + 4 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                word address = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                word value = (word)(vm->code[vm->pc + 3] | (vm->code[vm->pc + 4] << 8));
                write_memory_word(vm, address, value);
                vm->pc += 5;
                break;
            }

        case OP_PUSH8_REG:
            {
                byte src = operand1;
                byte value = vm->registers.rn[src];
                push_stack_byte(vm, value);
                vm->pc += 1;
                break;
            }
        case OP_PUSH8_IMM:
            {
                byte value = operand_whole;
                push_stack_byte(vm, value);
                vm->pc += 1;
                break;
            }
        case OP_PUSH16_REG:
            {
                byte src = operand1 & 0x07;
                word value = vm->registers.ern[src];
                push_stack_word(vm, value);
                vm->pc += 1;
                break;
            }
        case OP_PUSH16_IMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                word value = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                push_stack_word(vm, value);
                vm->pc += 3;
                break;
            }
        case OP_POP8_REG:
            {
                byte dest = operand1;
                byte value = pop_stack_byte(vm);
                vm->registers.rn[dest] = value;
                vm->pc += 1;
                vm->psw.raw = 0;
                vm->psw.field.zero = (value == 0);
                break;
            }
        case OP_POP16_REG:
            {
                byte dest = operand1 & 0x07;
                word value = pop_stack_word(vm);
                vm->registers.ern[dest] = value;
                vm->pc += 1;
                vm->psw.raw = 0;
                vm->psw.field.zero = (value == 0);
                break;
            }
        case OP_PUSH_LR:
            {
                push_stack_word(vm, vm->lr);
                vm->pc += 1;
                break;
            }
        case OP_POP_PC:
            {
                word value = pop_stack_word(vm);
                /* validate target address */
                if (value < code_start || value + 1 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                vm->pc = value;
                break;
            }
        case OP_PUSH_SP:
            {
                push_stack_word(vm, vm->sp);
                vm->pc += 1;
                break;
            }
        case OP_POP_SP:
            {
                word value = pop_stack_word(vm);
                vm->sp = value;
                vm->pc += 1;
                break;
            }

        case OP_ADD8_REG_REG:
            {
                byte dest = operand1;
                byte src = operand2;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_ADD, vm->registers.rn[dest], vm->registers.rn[src], &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_ADD16_REG_REG:
            {
                byte dest = operand1 & 0x07;
                byte src = operand2 & 0x07;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_ADD, vm->registers.ern[dest], vm->registers.ern[src], &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_ADD16_REG_IMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte dest = operand1 & 0x07;
                word imm = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_ADD, vm->registers.ern[dest], imm, &result);
                vm->registers.ern[dest] = result;
                vm->pc += 3;
                vm->psw = flags;
                break;
            }
        case OP_SUB8_REG_REG:
            {
                byte dest = operand1;
                byte src = operand2;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_SUB, vm->registers.rn[dest], vm->registers.rn[src], &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_SUB16_REG_REG:
            {
                byte dest = operand1 & 0x07;
                byte src = operand2 & 0x07;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_SUB, vm->registers.ern[dest], vm->registers.ern[src], &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_SUB16_REG_IMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte dest = operand1 & 0x07;
                word imm = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_SUB, vm->registers.ern[dest], imm, &result);
                vm->registers.ern[dest] = result;
                vm->pc += 3;
                vm->psw = flags;
                break;
            }
        case OP_INC8_REG:
            {
                byte dest = operand1;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_INC, vm->registers.rn[dest], 0, &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_INC16_REG:
            {
                byte dest = operand1 & 0x07;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_INC, vm->registers.ern[dest], 0, &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_DEC8_REG:
            {
                byte dest = operand1;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_DEC, vm->registers.rn[dest], 0, &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_DEC16_REG:
            {
                byte dest = operand1 & 0x07;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_DEC, vm->registers.ern[dest], 0, &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_MUL8_REG_REG:
            {
                byte dest = operand1;
                byte src = operand2;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_MUL, vm->registers.rn[dest], vm->registers.rn[src], &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_MUL16_REG_REG:
            {
                byte dest = operand1 & 0x07;
                byte src = operand2 & 0x07;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_MUL, vm->registers.ern[dest], vm->registers.ern[src], &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_MUL16_REG_IMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte dest = operand1 & 0x07;
                word imm = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_MUL, vm->registers.ern[dest], imm, &result);
                vm->registers.ern[dest] = result;
                vm->pc += 3;
                vm->psw = flags;
                break;
            }
        case OP_DIV8_REG_REG:
            {
                byte dest = operand1;
                byte src = operand2;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_DIV, vm->registers.rn[dest], vm->registers.rn[src], &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_DIV16_REG_REG:
            {
                byte dest = operand1 & 0x07;
                byte src = operand2 & 0x07;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_DIV, vm->registers.ern[dest], vm->registers.ern[src], &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_DIV16_REG_IMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte dest = operand1 & 0x07;
                word imm = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_DIV, vm->registers.ern[dest], imm, &result);
                vm->registers.ern[dest] = result;
                vm->pc += 3;
                vm->psw = flags;
                break;
            }
        case OP_MOD8_REG_REG:
            {
                byte dest = operand1;
                byte src = operand2;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_MOD, vm->registers.rn[dest], vm->registers.rn[src], &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_MOD8_REG_IMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte dest = operand1;
                byte imm = vm->code[vm->pc+1];
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_MOD, vm->registers.rn[dest], imm, &result);
                vm->registers.rn[dest] = result;
                vm->pc += 3;
                vm->psw = flags;
                break;
            }
        case OP_MOD16_REG_REG:
            {
                byte dest = operand1 & 0x07;
                byte src = operand2 & 0x07;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_MOD, vm->registers.ern[dest], vm->registers.ern[src], &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_MOD16_REG_IMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte dest = operand1 & 0x07;
                word imm = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_MOD, vm->registers.ern[dest], imm, &result);
                vm->registers.ern[dest] = result;
                vm->pc += 3;
                vm->psw = flags;
                break;
            }
        case OP_AND8_REG_REG:
            {
                byte dest = operand1;
                byte src = operand2;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_AND, vm->registers.rn[dest], vm->registers.rn[src], &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_AND16_REG_REG:
            {
                byte dest = operand1 & 0x07;
                byte src = operand2 & 0x07;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_AND, vm->registers.ern[dest], vm->registers.ern[src], &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_AND16_REG_IMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte dest = operand1 & 0x07;
                word imm = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_AND, vm->registers.ern[dest], imm, &result);
                vm->registers.ern[dest] = result;
                vm->pc += 3;
                vm->psw = flags;
                break;
            }
        case OP_OR8_REG_REG:
            {
                byte dest = operand1;
                byte src = operand2;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_OR, vm->registers.rn[dest], vm->registers.rn[src], &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_OR16_REG_REG:
            {
                byte dest = operand1 & 0x07;
                byte src = operand2 & 0x07;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_OR, vm->registers.ern[dest], vm->registers.ern[src], &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_OR16_REG_IMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte dest = operand1 & 0x07;
                word imm = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_OR, vm->registers.ern[dest], imm, &result);
                vm->registers.ern[dest] = result;
                vm->pc += 3;
                vm->psw = flags;
                break;
            }
        case OP_XOR8_REG_REG:
            {
                byte dest = operand1;
                byte src = operand2;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_XOR, vm->registers.rn[dest], vm->registers.rn[src], &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_XOR16_REG_REG:
            {
                byte dest = operand1 & 0x07;
                byte src = operand2 & 0x07;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_XOR, vm->registers.ern[dest], vm->registers.ern[src], &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_XOR16_REG_IMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte dest = operand1 & 0x07;
                word imm = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_XOR, vm->registers.ern[dest], imm, &result);
                vm->registers.ern[dest] = result;
                vm->pc += 3;
                vm->psw = flags;
                break;
            }
        case OP_NOT8_REG:
            {
                byte dest = operand1;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_NOT, vm->registers.rn[dest], 0, &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_NOT16_REG:
            {
                byte dest = operand1 & 0x07;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_NOT, vm->registers.ern[dest], 0, &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }

        case OP_CMP8_REG_REG:
            {
                byte reg1 = operand1;
                byte reg2 = operand2;
                psw_flags_t flags = cmp_bytes(vm->registers.rn[reg1], vm->registers.rn[reg2]);
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        
        case OP_CMP16_REG_REG:
            {
                byte reg1 = operand1 & 0x07;
                byte reg2 = operand2 & 0x07;
                psw_flags_t flags = cmp_words(vm->registers.ern[reg1], vm->registers.ern[reg2]);
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_CMP16_REG_IMM:
            {
                if (vm->pc + 2 >= code_end) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                byte reg1 = operand1 & 0x07;
                word imm = (word)(vm->code[vm->pc + 1] | (vm->code[vm->pc + 2] << 8));
                psw_flags_t flags = cmp_words(vm->registers.ern[reg1], imm);
                vm->pc += 3;
                vm->psw = flags;
                break;
            }
        case OP_SLL8_REG_REG:
            {
                byte dest = operand1;
                byte src = operand2;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_SLL, vm->registers.rn[dest], vm->registers.rn[src], &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_SLL8_REG_IMM:
            {
                byte dest = operand1;
                byte imm = operand2;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_SLL, vm->registers.rn[dest], imm, &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_SLL16_REG_REG:
            {
                byte dest = operand1 & 0x07;
                byte src = operand2 & 0x07;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_SLL, vm->registers.ern[dest], vm->registers.ern[src], &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_SLL16_REG_IMM:
            {
                byte dest = operand1 & 0x07;
                byte imm = operand2;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_SLL, vm->registers.ern[dest], (word)imm, &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_SRL8_REG_REG:
            {
                byte dest = operand1;
                byte src = operand2;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_SRL, vm->registers.rn[dest], vm->registers.rn[src], &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_SRL8_REG_IMM:
            {
                byte dest = operand1;
                byte imm = operand2;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_SRL, vm->registers.rn[dest], imm, &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_SRL16_REG_REG:
            {
                byte dest = operand1 & 0x07;
                byte src = operand2 & 0x07;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_SRL, vm->registers.ern[dest], vm->registers.ern[src], &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_SRL16_REG_IMM:
            {
                byte dest = operand1 & 0x07;
                byte imm = operand2;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_SRL, vm->registers.ern[dest], (word)imm, &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_SRA8_REG_REG:
            {
                byte dest = operand1;
                byte src = operand2;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_SRA, vm->registers.rn[dest], vm->registers.rn[src], &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_SRA8_REG_IMM:
            {
                byte dest = operand1;
                byte imm = operand2;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_SRA, vm->registers.rn[dest], imm, &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_SRA16_REG_REG:
            {
                byte dest = operand1 & 0x07;
                byte src = operand2 & 0x07;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_SRA, vm->registers.ern[dest], vm->registers.ern[src], &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_SRA16_REG_IMM:
            {
                byte dest = operand1 & 0x07;
                byte imm = operand2;
                word result = 0;
                psw_flags_t flags = alu_operation_word(ALU_OP_SRA, vm->registers.ern[dest], (word)imm, &result);
                vm->registers.ern[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_B_REG:
            {
                byte addr_reg = operand1 & 0x07;
                word address = vm->registers.ern[addr_reg];
                if (address < 4 || address + 1 >= (word)(vm->vm_properties.code_size + 4)) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                vm->pc = address;
                break;
            }
        case OP_B_IMM:
            {
                vm->pc += 1;
                if (vm->pc + 1 >= (word)(vm->vm_properties.code_size + 4)) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                word address = (word)(vm->code[vm->pc] | (vm->code[vm->pc + 1] << 8));
                if (address < 4 || address + 1 >= (word)(vm->vm_properties.code_size + 4)) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                vm->pc = address;
                break;
            }
        case OP_BEQ_IMM:
            {
                sbyte off8 = (sbyte)operand_whole;
                sword off16 = (sword)off8 * 2;
                sword base = (sword)vm->pc - 1;
                word address = (word)(base + off16);
                if (UNSIGNED_EQ(vm->psw)) {
                    vm->pc = address;
                } else {
                    vm->pc += 1;
                }
                break;
            }
        case OP_BNE_IMM:
            {
                sbyte off8 = (sbyte)operand_whole;
                sword off16 = (sword)off8 * 2;
                sword base = (sword)vm->pc - 1;
                word address = (word)(base + off16);
                if (UNSIGNED_NE(vm->psw)) {
                    vm->pc = address;
                } else {
                    vm->pc += 1;
                }
                break;
            }
        case OP_BLT_IMM:
            {
                sbyte off8 = (sbyte)operand_whole;
                sword off16 = (sword)off8 * 2;
                sword base = (sword)vm->pc - 1;
                word address = (word)(base + off16);
                if (UNSIGNED_LT(vm->psw)) {
                    vm->pc = address;
                } else {
                    vm->pc += 1;
                }
                break;
            }
        case OP_BLE_IMM:
            {
                sbyte off8 = (sbyte)operand_whole;
                sword off16 = (sword)off8 * 2;
                sword base = (sword)vm->pc - 1;
                word address = (word)(base + off16);
                if (UNSIGNED_LE(vm->psw)) {
                    vm->pc = address;
                } else {
                    vm->pc += 1;
                }
                break;
            }
        case OP_BGT_IMM:
            {
                sbyte off8 = (sbyte)operand_whole;
                sword off16 = (sword)off8 * 2;
                sword base = (sword)vm->pc - 1;
                word address = (word)(base + off16);
                if (UNSIGNED_GT(vm->psw)) {
                    vm->pc = address;
                } else {
                    vm->pc += 1;
                }
                break;
            }
        case OP_BGE_IMM:
            {
                sbyte off8 = (sbyte)operand_whole;
                sword off16 = (sword)off8 * 2;
                sword base = (sword)vm->pc - 1;
                word address = (word)(base + off16);
                if (UNSIGNED_GE(vm->psw)) {
                    vm->pc = address;
                } else {
                    vm->pc += 1;
                }
                break;
            }
        case OP_BL_REG:
            {
                byte addr_reg = operand1 & 0x07;
                word address = vm->registers.ern[addr_reg];
                if (address < 4 || address + 1 >= (word)(vm->vm_properties.code_size + 4)) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                vm->lr = vm->pc;
                vm->pc = address;
                break;
            }
        case OP_BL_IMM:
            {
                vm->pc += 1;
                if (vm->pc + 1 >= (word)(vm->vm_properties.code_size + 4)) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                word address = (word)(vm->code[vm->pc] | (vm->code[vm->pc + 1] << 8));
                if (address < 4 || address + 1 >= (word)(vm->vm_properties.code_size + 4)) { trigger_bsod(ERROR_VM_INVALID_INSTRUCTION); return; }
                vm->lr = vm->pc + 2;
                vm->pc = address;
                break;
            }

        // OP_MOV8_REG_IMM 0x60-0x6F
        case OP_MOV8_REG_IMM + 0: case OP_MOV8_REG_IMM + 1: case OP_MOV8_REG_IMM + 2: case OP_MOV8_REG_IMM + 3:
        case OP_MOV8_REG_IMM + 4: case OP_MOV8_REG_IMM + 5: case OP_MOV8_REG_IMM + 6: case OP_MOV8_REG_IMM + 7:
        case OP_MOV8_REG_IMM + 8: case OP_MOV8_REG_IMM + 9: case OP_MOV8_REG_IMM + 10: case OP_MOV8_REG_IMM + 11:
        case OP_MOV8_REG_IMM + 12: case OP_MOV8_REG_IMM + 13: case OP_MOV8_REG_IMM + 14: case OP_MOV8_REG_IMM + 15:
            {
                byte dest = opcode & 0x0F;
                byte imm = operand_whole;
                vm->registers.rn[dest] = imm;
                vm->pc += 1;
                vm->psw.raw = 0;
                vm->psw.field.zero = (imm == 0);
                break;
            }

        // OP_ADD8_REG_IMM 0x70-0x7F
        case OP_ADD8_REG_IMM + 0: case OP_ADD8_REG_IMM + 1: case OP_ADD8_REG_IMM + 2: case OP_ADD8_REG_IMM + 3:
        case OP_ADD8_REG_IMM + 4: case OP_ADD8_REG_IMM + 5: case OP_ADD8_REG_IMM + 6: case OP_ADD8_REG_IMM + 7:
        case OP_ADD8_REG_IMM + 8: case OP_ADD8_REG_IMM + 9: case OP_ADD8_REG_IMM + 10: case OP_ADD8_REG_IMM + 11:
        case OP_ADD8_REG_IMM + 12: case OP_ADD8_REG_IMM + 13: case OP_ADD8_REG_IMM + 14: case OP_ADD8_REG_IMM + 15:
            {
                byte dest = opcode & 0x0F;
                byte imm = operand_whole;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_ADD, vm->registers.rn[dest], imm, &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }

        // OP_SUB8_REG_IMM 0x80-0x8F
        case OP_SUB8_REG_IMM + 0: case OP_SUB8_REG_IMM + 1: case OP_SUB8_REG_IMM + 2: case OP_SUB8_REG_IMM + 3:
        case OP_SUB8_REG_IMM + 4: case OP_SUB8_REG_IMM + 5: case OP_SUB8_REG_IMM + 6: case OP_SUB8_REG_IMM + 7:
        case OP_SUB8_REG_IMM + 8: case OP_SUB8_REG_IMM + 9: case OP_SUB8_REG_IMM + 10: case OP_SUB8_REG_IMM + 11:
        case OP_SUB8_REG_IMM + 12: case OP_SUB8_REG_IMM + 13: case OP_SUB8_REG_IMM + 14: case OP_SUB8_REG_IMM + 15:
            {
                byte dest = opcode & 0x0F;
                byte imm = operand_whole;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_SUB, vm->registers.rn[dest], imm, &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }

        // OP_MUL8_REG_IMM 0x90-0x9F
        case OP_MUL8_REG_IMM + 0: case OP_MUL8_REG_IMM + 1: case OP_MUL8_REG_IMM + 2: case OP_MUL8_REG_IMM + 3:
        case OP_MUL8_REG_IMM + 4: case OP_MUL8_REG_IMM + 5: case OP_MUL8_REG_IMM + 6: case OP_MUL8_REG_IMM + 7:
        case OP_MUL8_REG_IMM + 8: case OP_MUL8_REG_IMM + 9: case OP_MUL8_REG_IMM + 10: case OP_MUL8_REG_IMM + 11:
        case OP_MUL8_REG_IMM + 12: case OP_MUL8_REG_IMM + 13: case OP_MUL8_REG_IMM + 14: case OP_MUL8_REG_IMM + 15:
            {
                byte dest = opcode & 0x0F;
                byte imm = operand_whole;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_MUL, vm->registers.rn[dest], imm, &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }

        // OP_DIV8_REG_IMM 0xA0-0xAF
        case OP_DIV8_REG_IMM + 0: case OP_DIV8_REG_IMM + 1: case OP_DIV8_REG_IMM + 2: case OP_DIV8_REG_IMM + 3:
        case OP_DIV8_REG_IMM + 4: case OP_DIV8_REG_IMM + 5: case OP_DIV8_REG_IMM + 6: case OP_DIV8_REG_IMM + 7:
        case OP_DIV8_REG_IMM + 8: case OP_DIV8_REG_IMM + 9: case OP_DIV8_REG_IMM + 10: case OP_DIV8_REG_IMM + 11:
        case OP_DIV8_REG_IMM + 12: case OP_DIV8_REG_IMM + 13: case OP_DIV8_REG_IMM + 14: case OP_DIV8_REG_IMM + 15:
            {
                byte dest = opcode & 0x0F;
                byte imm = operand_whole;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_DIV, vm->registers.rn[dest], imm, &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }

        // OP_AND8_REG_IMM 0xB0-0xBF
        case OP_AND8_REG_IMM + 0: case OP_AND8_REG_IMM + 1: case OP_AND8_REG_IMM + 2: case OP_AND8_REG_IMM + 3:
        case OP_AND8_REG_IMM + 4: case OP_AND8_REG_IMM + 5: case OP_AND8_REG_IMM + 6: case OP_AND8_REG_IMM + 7:
        case OP_AND8_REG_IMM + 8: case OP_AND8_REG_IMM + 9: case OP_AND8_REG_IMM + 10: case OP_AND8_REG_IMM + 11:
        case OP_AND8_REG_IMM + 12: case OP_AND8_REG_IMM + 13: case OP_AND8_REG_IMM + 14: case OP_AND8_REG_IMM + 15:
            {
                byte dest = opcode & 0x0F;
                byte imm = operand_whole;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_AND, vm->registers.rn[dest], imm, &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }

        // OP_OR8_REG_IMM 0xC0-0xCF
        case OP_OR8_REG_IMM + 0: case OP_OR8_REG_IMM + 1: case OP_OR8_REG_IMM + 2: case OP_OR8_REG_IMM + 3:
        case OP_OR8_REG_IMM + 4: case OP_OR8_REG_IMM + 5: case OP_OR8_REG_IMM + 6: case OP_OR8_REG_IMM + 7:
        case OP_OR8_REG_IMM + 8: case OP_OR8_REG_IMM + 9: case OP_OR8_REG_IMM + 10: case OP_OR8_REG_IMM + 11:
        case OP_OR8_REG_IMM + 12: case OP_OR8_REG_IMM + 13: case OP_OR8_REG_IMM + 14: case OP_OR8_REG_IMM + 15:
            {
                byte dest = opcode & 0x0F;
                byte imm = operand_whole;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_OR, vm->registers.rn[dest], imm, &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }

        // OP_XOR8_REG_IMM 0xD0-0xDF
        case OP_XOR8_REG_IMM + 0: case OP_XOR8_REG_IMM + 1: case OP_XOR8_REG_IMM + 2: case OP_XOR8_REG_IMM + 3:
        case OP_XOR8_REG_IMM + 4: case OP_XOR8_REG_IMM + 5: case OP_XOR8_REG_IMM + 6: case OP_XOR8_REG_IMM + 7:
        case OP_XOR8_REG_IMM + 8: case OP_XOR8_REG_IMM + 9: case OP_XOR8_REG_IMM + 10: case OP_XOR8_REG_IMM + 11:
        case OP_XOR8_REG_IMM + 12: case OP_XOR8_REG_IMM + 13: case OP_XOR8_REG_IMM + 14: case OP_XOR8_REG_IMM + 15:
            {
                byte dest = opcode & 0x0F;
                byte imm = operand_whole;
                byte result = 0;
                psw_flags_t flags = alu_operation_byte(ALU_OP_XOR, vm->registers.rn[dest], imm, &result);
                vm->registers.rn[dest] = result;
                vm->pc += 1;
                vm->psw = flags;
                break;
            }    

        // OP_CMP8_REG_IMM 0xE0-0xEF
        case OP_CMP8_REG_IMM + 0: case OP_CMP8_REG_IMM + 1: case OP_CMP8_REG_IMM + 2: case OP_CMP8_REG_IMM + 3:
        case OP_CMP8_REG_IMM + 4: case OP_CMP8_REG_IMM + 5: case OP_CMP8_REG_IMM + 6: case OP_CMP8_REG_IMM + 7:
        case OP_CMP8_REG_IMM + 8: case OP_CMP8_REG_IMM + 9: case OP_CMP8_REG_IMM + 10: case OP_CMP8_REG_IMM + 11:
        case OP_CMP8_REG_IMM + 12: case OP_CMP8_REG_IMM + 13: case OP_CMP8_REG_IMM + 14: case OP_CMP8_REG_IMM + 15:
            {
                byte dest = opcode & 0x0F;
                byte imm = operand_whole;
                psw_flags_t flags = cmp_bytes(vm->registers.rn[dest], imm);
                vm->pc += 1;
                vm->psw = flags;
                break;
            }
        case OP_SYSCALL:
            {
                vm_syscall(vm, operand_whole);
                vm->pc += 1;
                break;
            }
        case OP_END:
			{
				vm->vm_properties.running = 0;
				break;
			}

        default:
            trigger_bsod(ERROR_VM_INVALID_INSTRUCTION);
            break;
    }
}

// Execute single instruction for all active VMs in pool
void vm_step_all(void) {
    for (word i = 0; i < vm_capacity; i++) {
        if (vm_pool[i] != 0) {
            vm_step(vm_pool[i]);
        }
    }
}

// Destroy all VMs and free the pool array
void vm_shutdown(void) {
    // Destroy all VMs in pool
    for (word i = 0; i < vm_capacity; i++) {
        if (vm_pool[i] != 0) {
            hfree(vm_pool[i]);
            vm_pool[i] = 0;
        }
    }
    // Free pool array
    if (vm_pool) {
        hfree(vm_pool);
        vm_pool = 0;
    }
    vm_capacity = 0;
}

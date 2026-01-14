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
// 0x00 - 0x0F: Data movement
// 0x10 - 0x1F: Stack operations
// 0x20 - 0x2F: ALU operations
// 0x30 - 0x3F: Logical operations
// 0x40 - 0x4F: Misc Alu/Logical/Comparison
// 0x50 - 0x5F: Control flow
// 0x60 - 0x6F: 
typedef enum {
    OP_MOV8_REG_REG, // Data movement
    OP_MOV8_REG_IMM,
    OP_MOV16_REG_REG,
    OP_MOV16_REG_IMM,
    OP_LOAD8_REG_MREG,
    OP_LOAD8_REG_MIMM,
    OP_LOAD16_REG_MREG,
    OP_LOAD16_REG_MIMM,
    OP_STORE8_MREG_REG,
    OP_STORE8_MIMM_REG,
    OP_STORE16_MREG_REG,
    OP_STORE16_MIMM_REG,
    // (4 more opcodes here if needed)

    OP_PUSH8_REG = 0x10, // Stack operations
    OP_PUSH8_IMM,
    OP_PUSH16_REG,
    OP_PUSH16_IMM,
    OP_POP8_REG,
    OP_POP16_REG,
    OP_PUSH_LR,
    OP_POP_PC,
    OP_PUSH_SP,
    OP_POP_SP,
    // (6 more opcodes here if needed)

    OP_ADD8_REG_REG = 0x20, // ALU operations
    OP_ADD8_REG_IMM,
    OP_ADD16_REG_REG,
    OP_ADD16_REG_IMM,
    OP_SUB8_REG_REG,
    OP_SUB8_REG_IMM,
    OP_SUB16_REG_REG,
    OP_SUB16_REG_IMM,
    OP_MUL8_REG_REG,
    OP_MUL8_REG_IMM,
    OP_MUL16_REG_REG,
    OP_MUL16_REG_IMM,
    OP_DIV8_REG_REG,
    OP_DIV8_REG_IMM,
    OP_DIV16_REG_REG,
    OP_DIV16_REG_IMM,
    
    OP_AND8_REG_REG, // Logical operations
    OP_AND8_REG_IMM,
    OP_AND16_REG_REG,
    OP_AND16_REG_IMM,
    OP_OR8_REG_REG,
    OP_OR8_REG_IMM,
    OP_OR16_REG_REG,
    OP_OR16_REG_IMM,
    OP_XOR8_REG_REG,
    OP_XOR8_REG_IMM,
    OP_XOR16_REG_REG,
    OP_XOR16_REG_IMM,
    OP_NOT8_REG,
    OP_NOT16_REG,
    // (2 more opcodes here if needed)
    
    OP_CMP8_REG_REG, // Comparison operations
    OP_CMP8_REG_IMM,
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

    OP_B_REG = 0x50, // Control flow
    OP_B_IMM,
    OP_BEQ_IMM,
    OP_BNE_IMM,
    OP_BLT_IMM,
    OP_BLE_IMM,
    OP_BGT_IMM,
    OP_BGE_IMM,
    OP_BL_REG, // Branch link, pc is saved to lr, then branch
    OP_BL_IMM,
    // (6 more opcodes here if needed)


} opcode_t;

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
            unsigned int temp = (unsigned int)a + (unsigned int)b;
            res = (byte)(temp & 0xFF);
            flags.field.carry = (temp > 0xFF);
            flags.field.overflow = (((a ^ res) & (b ^ res) & 0x80) != 0);
            break;
        }
        case ALU_OP_SUB: {
            unsigned int temp = (unsigned int)a - (unsigned int)b;
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
        case ALU_OP_SLL:
            res = a << 1;
            flags.field.carry = (a & 0x80) != 0;
            break;
        case ALU_OP_SRL:
            res = a >> 1;
            flags.field.carry = (a & 0x01) != 0;
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
            unsigned int temp = (unsigned int)a + (unsigned int)b;
            res = (word)(temp & 0xFFFF);
            flags.field.carry = (temp > 0xFFFF);
            flags.field.overflow = (((a ^ res) & (b ^ res) & 0x8000) != 0);
            break;
        }
        case ALU_OP_SUB: {
            unsigned int temp = (unsigned int)a - (unsigned int)b;
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
        case ALU_OP_SLL:
            res = a << 1;
            flags.field.carry = (a & 0x8000) != 0;
            break;
        case ALU_OP_SRL:
            res = a >> 1;
            flags.field.carry = (a & 0x0001) != 0;
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
        case GET_ELEMENT_FIELD:
            break;
        case SET_ELEMENT_FIELD:
            break;
        case RENDER_ELEMENT:
            break;
        case DRAW_LINE:
            break;
        case SET_PIXEL:
            break;
        case GET_PIXEL:
            break;
        case DRAW_IMAGE:
            break;
        case DRAW_RECT:
            break;
        case DRAW_TEXT:
            break;
        case SLEEP:
            break;
        case STOP:
            break;
        case END:
            break;
        default:
            trigger_bsod(ERROR_VM_INVALID_SYSCALL);
            break;
    }
}

// Initialize VM system with heap-allocated pool of NULL pointers
void vm_init_system() {
    vm_capacity = MAX_VMS;
    vm_pool = (TangentMachine**)hcalloc(vm_capacity, sizeof(TangentMachine*));
}

// Initialize individual VM structure with external code pointer and RAM pointers
void vm_init(TangentMachine* vm, byte* code) {
    if (!vm || !code) return;
    
    // RAM is allocated immediately after the VM structure
    vm->ram = (byte*)vm + sizeof(TangentMachine);
    
    // Code is external, just store the pointer
    vm->code = code;
    vm->vm_properties.code_size = *((word*)code);
    vm->vm_properties.ram_size = *((word*)(code + 2));
    vm->vm_properties.uses_ram = 1; /* ensure memory ops are enabled */
    vm->vm_properties.running = 1;
    vm->sp = vm->vm_properties.ram_size; // Stack pointer starts at top of RAM
    vm->pc = 4; /* code starts after the 2-word header */
}

// Create new VM from bytecode, add to pool, expand pool if needed
TangentMachine* vm_spawn(byte* code) {
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

    // Get RAM size from code header
    word ram_size = *((word*)(code + 2));
    word total_size = sizeof(TangentMachine) + ram_size;

    // Allocate VM structure + RAM from heap (no code)
    TangentMachine* vm = (TangentMachine*)hcalloc(1, total_size);
    if (!vm) return 0;

    // Initialize VM with external code pointer
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
                vm->pc += 2;
                vm->psw.raw  = 0;
                vm->psw.field.zero = (vm->registers.rn[operand1] == 0);
                break;
            }
        case 0x67: // Test
            {
                vm->ram[0]++;
                vm->pc += 1;
                break;
            }
        case 0x61:
            {
                vm->pc = 2;
                break;
            }
        default:
            trigger_bsod(ERROR_VM_INVALID_INSTRUCTION);
            break;
    }
}

// Execute single instruction for all active VMs in pool
void vm_step_all() {
    for (word i = 0; i < vm_capacity; i++) {
        if (vm_pool[i] != 0) {
            vm_step(vm_pool[i]);
        }
    }
}

// Destroy all VMs and free the pool array
void vm_shutdown() {
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
/*
 * vm.c
 * Implementation of virtual machine
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#include "vm.h"
#include "Tui/glib.h"
#include "heap.h"
#include "debug.h"

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
    DUMMY,
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
        case DUMMY:
            // Dummy syscall does nothing
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
    vm->vm_properties.running = 1;
    vm->sp = vm->vm_properties.ram_size; // Stack pointer starts at top of RAM
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

    switch(opcode) {
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
# Tangent VM (TangentMachine) reference

## Purpose
This document describes the Tangent virtual machine implementation found in `src/vm/vm.c`. It explains the VM design, data layout, registers, instruction encoding, ALU semantics, memory, stack behavior, lifecycle and syscalls.
---

## Table of contents
- [Overview](#overview)
- [Memory layout and headers](#memory-layout-and-headers)
- [Registers and PSW](#registers-and-psw)
- [Instruction encoding](#instruction-encoding)
- [Opcode reference](#opcode-reference)
- [Opcode categories and behavior](#opcode-categories-and-behavior)
- [ALU operations and flags](#alu-operations-and-flags)
- [Memory access and stack operations](#memory-access-and-stack-operations)
- [Comparisons](#comparisons)
- [Shifts](#shifts)
- [Control flow](#control-flow)
- [Syscalls](#syscalls)
- [VM lifecycle API](#vm-lifecycle-api)
- [Error handling and invariants](#error-handling-and-invariants)
- [Extension and debugging notes](#extension-and-debugging-notes)
- [Examples and bytecode snippets](#examples-and-bytecode-snippets)

---

## Overview
The Tangent VM (`TangentMachine`) executes a compact bytecode format. Each VM instance holds a pointer to immutable code and (optionally) a contiguous RAM area allocated immediately after the `TangentMachine` structure. The VM executes one instruction at a time via `vm_step()`; all active VMs may be stepped with `vm_step_all()`.

Key properties:
- Little-endian word representation.
- Code is immutable and provided externally (pointer stored in `vm->code`).
- Stack grows downward inside the VM RAM; `sp` is a word index into RAM.
- `pc` points into the code region and begins at offset 4 (the code area starts after a 4-byte header).

Source reference: `src/vm/vm.c`.

---

## Memory layout and headers
Bytecode layout (external array pointed to by `vm->code`):
- Offset 0..1: `code_size` (word, little-endian) - number of bytes in code region
- Offset 2..3: `ram_size` (word, little-endian) - bytes of RAM to allocate for the VM
- Offset 4..(4 + code_size - 1): code bytes executed by the VM

The VM checks code bounds using:
- `code_start = 4`
- `code_end = code_start + vm->vm_properties.code_size` (exclusive)

Words in memory and headers are always stored little-endian when read/written by `read_memory_word` / `write_memory_word`.

---

## Registers and PSW
Registers (nomenclature used in code):
- `rn[16]` - sixteen 8-bit general registers (byte-sized)
- `ern[8]` - eight 16-bit general registers (word-sized) used for addresses and wide arithmetic
- `sp` - stack pointer (word), initialized to `ram_size` (top of RAM)
- `pc` - program counter (word), initialized to 4 (first instruction)
- `lr` - link register (word), used by branch-and-link operations
- `psw` - processor status word (flags) represented as `psw_flags_t` with fields:
  - `zero` - set when result == 0
  - `neg` - sign bit set (MSB) of result
  - `carry` - carry for unsigned ops / borrow for subtraction
  - `overflow` - signed overflow indicator

Helper macros in the source provide signed/unsigned comparisons using `psw` (for example `SIGNED_LT(psw)`).

---

## Instruction encoding
Each instruction is a 1-byte opcode followed by an operand byte (packed nibbles) and optionally more immediate bytes depending on the instruction.

- Layout for common instructions:
  - [opcode:1][operand:1][...optional immediates...]
  - The operand byte packs two 4-bit fields:
    - `operand1 = (operand >> 4) & 0x0F`
    - `operand2 = operand & 0x0F`

- Many instructions interpret the two operands differently (e.g., dest/src registers, or dest/reg + immediate).

- Some opcodes use special ranges where the low nibble encodes a destination register and the instruction has a single-byte immediate following the opcode (16-entry blocks):
  - `OP_MOV8_REG_IMM = 0x60` (0x60–0x6F)
  - `OP_ADD8_REG_IMM = 0x70` (0x70–0x7F)
  - `OP_SUB8_REG_IMM = 0x80` (0x80–0x8F)
  - `OP_MUL8_REG_IMM = 0x90` (0x90–0x9F)
  - `OP_DIV8_REG_IMM = 0xA0` (0xA0–0xAF)
  - `OP_AND8_REG_IMM = 0xB0` (0xB0–0xBF)
  - `OP_OR8_REG_IMM  = 0xC0` (0xC0–0xCF)
  - `OP_XOR8_REG_IMM = 0xD0` (0xD0–0xDF)
  - `OP_CMP8_REG_IMM = 0xE0` (0xE0–0xEF)

- For multi-byte immediates (16-bit), the instruction reads the next two code bytes as a little-endian word.

Note: `vm_step()` always validates there are enough bytes remaining for the expected immediates and will trigger a BSOD on underflow.

---

## Opcode categories and behavior
This project defines many opcodes. Below are the broad categories and examples (refer to `vm.c` for the full switch table):

- Data movement
  - `OP_MOV8_REG_REG` - move 8-bit register to register
  - `OP_MOV16_REG_REG`, `OP_MOV16_REG_IMM` - 16-bit moves
  - Memory loads/stores: `OP_LOAD8_REG_MREG`, `OP_STORE8_MIMM_REG`, `OP_LOAD16_REG_MIMM`, etc.

- Stack instructions
  - `OP_PUSH8_REG`, `OP_PUSH16_IMM`, `OP_POP8_REG`, `OP_POP16_REG`
  - `OP_PUSH_LR`, `OP_POP_PC` - support call/return semantics
  - `OP_PUSH_SP`, `OP_POP_SP`

- Arithmetic and logic (ALU)
  - Byte/word variants: add, sub, mul, div, mod, and/or/xor, not, inc, dec
  - Shift operations: SLL, SRL, SRA (byte/word variants)
  - CMP instructions set `psw` without changing operands

- Branch and control flow
  - `OP_B_REG`, `OP_B_IMM` - unconditional
  - Conditional branches using `psw`: `OP_BEQ_IMM`, `OP_BNE_IMM`, `OP_BLT_IMM`, `OP_BLE_IMM`, `OP_BGT_IMM`, `OP_BGE_IMM`
  - Branch-and-link (`OP_BL_REG`, `OP_BL_IMM`) saves `pc` to `lr` before jumping

- Immediate-modifying registers
  - The 16-entry blocks (0x60–0xEF ranges) provide compact immediate forms for common byte ops.

Semantics: after an instruction executes, `vm->pc` is advanced to the next instruction; many instructions update `vm->psw` based on the result.

---
## Opcode reference
This section documents every VM instruction implemented in `vm_step()` with these fields:
- Encoding: bytes consumed and operand layout.
- Behavior: what the instruction does.
- PSW effects: which `psw` flags are modified and how.
- Errors: fatal conditions that trigger BSOD.

All instructions use the operand byte (high nibble = `operand1`, low nibble = `operand2`) unless stated otherwise.

### Opcode index
A quick index of all opcodes (click a name to jump to its detailed entry):

- **Data movement**
  - [OP_MOV8_REG_REG](#op_mov8_reg_reg)
  - [OP_MOV16_REG_REG](#op_mov16_reg_reg)
  - [OP_MOV16_REG_IMM](#op_mov16_reg_imm)
  - [OP_LOAD8_REG_MREG](#op_load8_reg_mreg)
  - [OP_LOAD8_REG_MIMM](#op_load8_reg_mimm)
  - [OP_LOAD16_REG_MREG](#op_load16_reg_mreg)
  - [OP_LOAD16_REG_MIMM](#op_load16_reg_mimm)
  - [OP_STORE8_MREG_REG](#op_store8_mreg_reg)
  - [OP_STORE8_MREG_IMM](#op_store16_mreg_imm)
  - [OP_STORE8_MIMM_REG](#op_store8_mimm_reg)
  - [OP_STORE8_MIMM_IMM](#op_store8_mimm_imm)
  - [OP_STORE16_MREG_REG](#op_store16_mreg_reg)
  - [OP_STORE16_MREG_IMM](#op_store16_mreg_imm)
  - [OP_STORE16_MIMM_REG](#op_store16_mimm_reg)
  - [OP_STORE16_MIMM_IMM](#op_store16_mimm_imm)

- **Stack**
  - [OP_PUSH8_REG](#op_push8_reg)
  - [OP_PUSH8_IMM](#op_push8_imm)
  - [OP_PUSH16_REG](#op_push16_reg)
  - [OP_PUSH16_IMM](#op_push16_imm)
  - [OP_POP8_REG](#op_pop8_reg)
  - [OP_POP16_REG](#op_pop16_reg)
  - [OP_PUSH_LR](#op_push_lr)
  - [OP_POP_PC](#op_pop_pc)
  - [OP_PUSH_SP](#op_push_sp)
  - [OP_POP_SP](#op_pop_sp)

- **Arithmetic**
  - [OP_ADD8_REG_REG](#op_add8_reg_reg)
  - [OP_ADD16_REG_REG](#op_add16_reg_reg)
  - [OP_ADD16_REG_IMM](#op_add16_reg_imm)
  - [OP_SUB8_REG_REG](#op_sub8_reg_reg)
  - [OP_SUB16_REG_REG](#op_sub16_reg_reg)
  - [OP_SUB16_REG_IMM](#op_sub16_reg_imm)
  - [OP_INC8_REG](#op_inc8_reg)
  - [OP_INC16_REG](#op_inc16_reg)
  - [OP_DEC8_REG](#op_dec8_reg)
  - [OP_DEC16_REG](#op_dec16_reg)
  - [OP_MUL8_REG_REG](#op_mul8_reg_reg)
  - [OP_MUL16_REG_REG](#op_mul16_reg_reg)
  - [OP_MUL16_REG_IMM](#op_mul16_reg_imm)
  - [OP_DIV8_REG_REG](#op_div8_reg_reg)
  - [OP_DIV16_REG_REG](#op_div16_reg_reg)
  - [OP_DIV16_REG_IMM](#op_div16_reg_imm)
  - [OP_MOD8_REG_REG](#op_mod8_reg_reg)
  - [OP_MOD8_REG_IMM](#op_mod8_reg_imm)
  - [OP_MOD16_REG_REG](#op_mod16_reg_reg)
  - [OP_MOD16_REG_IMM](#op_mod16_reg_imm)

- **Logic**
  - [OP_AND8_REG_REG](#op_and8_reg_reg)
  - [OP_AND16_REG_REG](#op_and16_reg_reg)
  - [OP_AND16_REG_IMM](#op_and16_reg_imm)
  - [OP_OR8_REG_REG](#op_or8_reg_reg)
  - [OP_OR16_REG_REG](#op_or16_reg_reg)
  - [OP_OR16_REG_IMM](#op_or16_reg_imm)
  - [OP_XOR8_REG_REG](#op_xor8_reg_reg)
  - [OP_XOR16_REG_REG](#op_xor16_reg_reg)
  - [OP_XOR16_REG_IMM](#op_xor16_reg_imm)
  - [OP_NOT8_REG](#op_not8_reg)
  - [OP_NOT16_REG](#op_not16_reg)

- **Comparisons**
  - [OP_CMP8_REG_REG](#op_cmp8_reg_reg)
  - [OP_CMP16_REG_REG](#op_cmp16_reg_reg)
  - [OP_CMP16_REG_IMM](#op_cmp16_reg_imm)
  - [OP_CMP8_REG_IMM](#op_cmp8_reg_imm)

- **Shifts**
  - [OP_SLL8_REG_REG](#op_sll8_reg_reg)
  - [OP_SLL8_REG_IMM](#op_sll8_reg_imm)
  - [OP_SLL16_REG_REG](#op_sll16_reg_reg)
  - [OP_SLL16_REG_IMM](#op_sll16_reg_imm)
  - [OP_SRL8_REG_REG](#op_srl8_reg_reg)
  - [OP_SRL8_REG_IMM](#op_srl8_reg_imm)
  - [OP_SRL16_REG_REG](#op_srl16_reg_reg)
  - [OP_SRL16_REG_IMM](#op_srl16_reg_imm)
  - [OP_SRA8_REG_REG](#op_sra8_reg_reg)
  - [OP_SRA8_REG_IMM](#op_sra8_reg_imm)
  - [OP_SRA16_REG_REG](#op_sra16_reg_reg)
  - [OP_SRA16_REG_IMM](#op_sra16_reg_imm)

- **Control flow**
  - [OP_B_REG](#op_b_reg)
  - [OP_B_IMM](#op_b_imm)
  - [OP_BEQ_IMM](#short-conditional-branches)
  - [OP_BNE_IMM](#short-conditional-branches)
  - [OP_BLT_IMM](#short-conditional-branches)
  - [OP_BLE_IMM](#short-conditional-branches)
  - [OP_BGT_IMM](#short-conditional-branches)
  - [OP_BGE_IMM](#short-conditional-branches)
  - [OP_BL_REG](#op_bl_reg)
  - [OP_BL_IMM](#op_bl_imm)

- **Compact immediate forms (16 byte ranges)**
  - [OP_MOV8_REG_IMM](#op_mov8_reg_imm)
  - [OP_ADD8_REG_IMM](#op_add8_reg_imm)
  - [OP_SUB8_REG_IMM](#op_sub8_reg_imm)
  - [OP_MUL8_REG_IMM](#op_mul8_reg_imm)
  - [OP_DIV8_REG_IMM](#op_div8_reg_imm)
  - [OP_AND8_REG_IMM](#op_and8_reg_imm)
  - [OP_OR8_REG_IMM](#op_or8_reg_imm)
  - [OP_XOR8_REG_IMM](#op_xor8_reg_imm)
  - [OP_CMP8_REG_IMM](#op_cmp8_reg_imm)

### Data movement
This section documents the data movement instructions in detail. Each entry includes pseudocode, the exact PSW behavior, an example, and notes about bounds/errors.

#### OP_MOV8_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1 (index into `rn[]`), src = operand2 (index into `rn[]`).
  - Behavior (pseudocode):

```c
/* MOV8_REG_REG */
byte tmp = rn[operand2];
rn[operand1] = tmp;
```

  - Notes: 8-bit move, no sign/overflow semantics; used to transfer byte values.
  - PSW effects: `psw.raw = 0` then `psw.field.zero = (tmp == 0)`; `neg`, `carry`, `overflow` are cleared.
  - PC behavior: advance +1
  - Errors: none
  - Example: to move `rn1` -> `rn2`, operand = (1<<4) | 2.
  - Notes: 8-bit move, no sign/overflow semantics; used to transfer byte values.

#### OP_MOV16_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1 & 0x07, src = operand2 & 0x07 (index into `ern[]`).
- Behavior:

```c
/* MOV16_REG_REG */
word tmp = ern[operand2 & 0x07];
ern[operand1 & 0x07] = tmp;
```

  - PC: +1
  - Example: copy 16-bit pointer or immediate value between `ern` registers.
  - PSW effects: `psw.raw = 0` then `psw.field.zero = (tmp == 0)`; other flags cleared.
  - PC: +1
  - Example: copy 16-bit pointer or immediate value between `ern` registers.

#### OP_MOV16_REG_IMM
- Encoding: [opcode][operand][imm_lo][imm_hi]
- Operands: dest = operand1 & 0x07, imm = little-endian word
- Behavior:

```c
/* MOV16_REG_IMM */
word imm = code[pc+1] | (code[pc+2] << 8);
ern[operand1 & 0x07] = imm;
```

  - PSW effects: `psw.raw = 0` then `psw.field.zero = (imm == 0)`
  - PC: +3
  - Errors: `ERROR_VM_INVALID_INSTRUCTION` if imm bytes aren't available
  - PSW effects: `psw.raw = 0` then `psw.field.zero = (imm == 0)`
  - PC: +3
  - Errors: `ERROR_VM_INVALID_INSTRUCTION` if imm bytes aren't available
  - Example: set `ern0` to 0x1234 -> `[OP_MOV16_REG_IMM][0x00][0x34][0x12]`

#### OP_LOAD8_REG_MREG
- Encoding: [opcode][operand]
- Operands: dest = operand1 (rn index), addr_reg = operand2 & 0x07 (ern index)
- Behavior (pseudocode):

```c
/* LOAD8_REG_MREG */
word address = ern[operand2 & 0x07];
byte value = read_memory_byte(vm, address);
rn[operand1] = value;
```

  - PSW: `psw.raw = 0`; `psw.field.zero = (value == 0)`; other flags cleared.
  - PSW: `psw.raw = 0`; `psw.field.zero = (value == 0)`; other flags cleared.
  - PC: +1
  - Errors: `ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS` for invalid address
  - Notes: reads a single byte; address taken from 16-bit register.

#### OP_LOAD8_REG_MIMM
- Encoding: [opcode][operand][addr_lo][addr_hi]
- Operands: dest = operand1, addr = imm word
- Behavior:

```c
/* LOAD8_REG_MIMM */
word addr = code[pc+1] | (code[pc+2] << 8);
byte value = read_memory_byte(vm, addr);
rn[operand1] = value;
```

  - PSW: `zero` updated; other flags cleared
  - PC: +3
  - Errors: invalid code length or memory bounds
  - PSW: `zero` updated; other flags cleared
  - PC: +3
  - Errors: invalid code length or memory bounds
  - Example: load byte at 0x200 -> operand=dest, bytes [0x00,0x02]

#### OP_LOAD16_REG_MREG
- Encoding: [opcode][operand]
- Operands: dest = operand1 & 0x07, addr_reg = operand2 & 0x07
- Behavior:

```c
/* LOAD16_REG_MREG */
word address = ern[operand2 & 0x07];
word value = read_memory_word(vm, address);
ern[operand1 & 0x07] = value;
```

  - PSW: `psw.raw = 0`; `zero` = (value == 0)
  - PC: +1
  - PSW: `psw.raw = 0`; `zero` = (value == 0)
  - PC: +1
  - Errors: memory bounds
  - Notes: word reads require two bytes in RAM at `address` and `address+1`.

#### OP_LOAD16_REG_MIMM
- Encoding: [opcode][operand][addr_lo][addr_hi]
- Operands: dest = operand1 & 0x07, addr = imm word
- Behavior:

```c
/* LOAD16_REG_MIMM */
word addr = code[pc+1] | (code[pc+2] << 8);
word value = read_memory_word(vm, addr);
ern[operand1 & 0x07] = value;
```

  - PSW: `zero` updated; other flags cleared
  - PC: +3
  - Errors: invalid code length / memory bounds
  - PSW: `zero` updated; other flags cleared
  - PC: +3
  - Errors: invalid code length / memory bounds

#### OP_STORE8_MREG_REG
- Encoding: [opcode][operand]
- Operands: addr_reg = operand1 & 0x07 (ern index), src = operand2 (rn index)
- Behavior:

```c
/* STORE8_MREG_REG */
word address = ern[operand1 & 0x07];
byte val = rn[operand2];
write_memory_byte(vm, address, val);
```

  - PSW: unchanged
  - PC: +1
  - PSW: unchanged
  - PC: +1
  - Errors: `ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS`
  - Notes: useful for writing single-byte values like colours or flags.

#### OP_STORE8_MREG_IMM
- Encoding: [opcode][operand][imm]
- Operands: src = operand2 & 0x07 (rn index used to select address register), imm = next code byte
- Behavior:

```c
/* STORE8_MREG_IMM */
byte src = operand2 & 0x07;
word address = ern[src];
word value = code[pc+1];
write_memory_byte(vm, address, value);
```

  - PSW: unchanged
  - PC: +3
  - Errors: `ERROR_VM_INVALID_INSTRUCTION` if immediate byte isn't available, `ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS` for invalid address
  - Notes: follows the implementation in `src/vm/vm.c` exactly (reads one immediate byte and uses the `ern[]` register selected by `operand2` as the destination address).

#### OP_STORE8_MIMM_REG
- Encoding: [opcode][operand][addr_lo][addr_hi]
- Operands: src = operand2, addr = immediate word
- Behavior:

```c
/* STORE8_MIMM_REG */
word addr = code[pc+1] | (code[pc+2] << 8);
byte val = rn[operand2];
write_memory_byte(vm, addr, val);
```

  - PSW: unchanged
  - PC: +3
  - Errors: invalid code length / memory bounds
  - PSW: unchanged
  - PC: +3
  - Errors: invalid code length / memory bounds

#### OP_STORE8_MIMM_IMM
- Encoding: [opcode][operand][addr_lo][addr_hi]
- Operands: addr = immediate word from the next two code bytes, value = full operand byte (`operand_whole`)
- Behavior:

```c
/* STORE8_MIMM_IMM */
word address = code[pc+1] | (code[pc+2] << 8);
byte value = operand_whole;
write_memory_byte(vm, address, value);
```

  - PSW: unchanged
  - PC: +3
  - Errors: `ERROR_VM_INVALID_INSTRUCTION` if immediate bytes aren't available, `ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS` for invalid address
  - Notes: per `src/vm/vm.c` the opcode uses the operand byte as the value and reads a two-byte address following the operand; the implementation advances `pc` by 3.

#### OP_STORE16_MREG_REG
- Encoding: [opcode][operand]
- Operands: addr_reg = operand1 & 0x07, src = operand2 & 0x07
- Behavior:

```c
/* STORE16_MREG_REG */
word address = ern[operand1 & 0x07];
word val = ern[operand2 & 0x07];
write_memory_word(vm, address, val);
```

  - PSW: unchanged
  - PC: +1
  - PSW: unchanged
  - PC: +1
  - Errors: `ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS`

#### OP_STORE16_MREG_IMM
- Encoding: [opcode][operand][imm_lo][imm_hi]
- Operands: src = operand2 & 0x07 (index into `ern[]` to select address register), imm = little-endian word immediate
- Behavior:

```c
/* STORE16_MREG_IMM */
byte src = operand2 & 0x07;
word address = vm->registers.ern[src];
word value = (word)(code[pc+1] | (code[pc+2] << 8));
write_memory_word(vm, address, value);
```

  - PSW: unchanged
  - PC: +3
  - Errors: `ERROR_VM_INVALID_INSTRUCTION` if immediate bytes aren't available, `ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS` for invalid address
  - Notes: per `src/vm/vm.c` the opcode reads a 16-bit immediate value and writes it to the address held in `ern[operand2 & 0x07]`.

#### OP_STORE16_MIMM_REG
- Encoding: [opcode][operand][addr_lo][addr_hi]
- Operands: src = operand2 & 0x07, addr = immediate word
- Behavior:

```c
/* STORE16_MIMM_REG */
word addr = code[pc+1] | (code[pc+2] << 8);
word val = ern[operand2 & 0x07];
write_memory_word(vm, addr, val);
```

  - PSW: unchanged
  - PC: +3
  - Errors: invalid code length / out-of-bounds memory
  - PSW: unchanged
  - PC: +3
  - Errors: invalid code length / out-of-bounds memory
  - Notes: use for storing pointers or 16-bit values to RAM.

#### OP_STORE16_MIMM_IMM
- Encoding: [opcode][operand][addr_lo][addr_hi][val_lo][val_hi]
- Operands: addr = little-endian word from the next two code bytes, value = little-endian word from the following two code bytes
- Behavior:

```c
/* STORE16_MIMM_IMM */
word address = code[pc+1] | (code[pc+2] << 8);
word value = (word)(code[pc+3] | (code[pc+4] << 8));
write_memory_word(vm, address, value);
```

  - PSW: unchanged
  - PC: +5
  - Errors: `ERROR_VM_INVALID_INSTRUCTION` if immediate bytes aren't available, `ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS` for invalid address
  - Notes: implementation in `src/vm/vm.c` advances `pc` by 5 after writing the 16-bit immediate value to the specified address.

### Stack operations
All stack operations operate on the VM's RAM using `sp` as a byte index. The stack grows downward: pushes decrement `sp`, pops increment `sp`. All stack pushes check for overflow (insufficient space at lower addresses), and pops check for underflow (reading beyond top-of-RAM).

#### OP_PUSH8_REG
- Encoding: [opcode][operand]
- Operands: src = operand1 (rn index)
- Behavior (pseudocode):

```c
/* PUSH8_REG */
if (vm->sp == 0) trigger_bsod(ERROR_VM_STACK_OVERFLOW);
vm->sp--;
write_memory_byte(vm, vm->sp, rn[operand1]);
```

  - PSW: unchanged
  - PC: +1
  - Example: push `rn2` onto stack
  - PSW: unchanged
  - PC: +1
  - Example: push `rn2` onto stack

#### OP_PUSH8_IMM
- Encoding: [opcode][operand]
- Operands: imm = operand_whole (full operand byte)
- Behavior (pseudocode):

```c
/* PUSH8_IMM */
byte imm = operand_whole;
if (vm->sp == 0) trigger_bsod(ERROR_VM_STACK_OVERFLOW);
vm->sp--;
write_memory_byte(vm, vm->sp, imm);
```

  - PSW: unchanged
  - PC: +1
  - Example: push value 0xFF -> operand = 0xFF
  - PSW: unchanged
  - PC: +1
  - Example: push value 0xFF -> operand = 0xFF

#### OP_PUSH16_REG
- Encoding: [opcode][operand]
- Operands: src = operand1 & 0x07 (ern index)
- Behavior (pseudocode):

```c
/* PUSH16_REG */
if (vm->sp < 2) trigger_bsod(ERROR_VM_STACK_OVERFLOW);
vm->sp -= 2;
write_memory_word(vm, vm->sp, ern[operand1 & 0x07]);
```

  - PSW: unchanged
  - PC: +1
  - Example: push `ern0` (16-bit value) onto stack
  - PSW: unchanged
  - PC: +1
  - Example: push `ern0` (16-bit value) onto stack

#### OP_PUSH16_IMM
- Encoding: [opcode][operand][imm_lo][imm_hi]
- Behavior (pseudocode):

```c
/* PUSH16_IMM */
word imm = code[pc+1] | (code[pc+2] << 8);
if (vm->sp < 2) trigger_bsod(ERROR_VM_STACK_OVERFLOW);
vm->sp -= 2;
write_memory_word(vm, vm->sp, imm);
```

  - PSW: unchanged
  - PC: +3
  - Errors: invalid code length or stack overflow
  - PSW: unchanged
  - PC: +3
  - Errors: invalid code length or stack overflow

#### OP_POP8_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1 (rn index)
- Behavior (pseudocode):

```c
/* POP8_REG */
if (vm->sp >= vm->vm_properties.ram_size) trigger_bsod(ERROR_VM_STACK_UNDERFLOW);
byte value = read_memory_byte(vm, vm->sp);
vm->sp++;
rn[operand1] = value;
```

  - PSW: `psw.raw = 0`; `psw.field.zero = (value == 0)`; other flags cleared
  - PC: +1
  - Example: pop value from stack into `rn3`
  - PSW: `psw.raw = 0`; `psw.field.zero = (value == 0)`; other flags cleared
  - PC: +1
  - Example: pop value from stack into `rn3`

#### OP_POP16_REG
- Encoding: [opcode][operand]
- Behavior (pseudocode):

```c
/* POP16_REG */
if (vm->sp + 1 >= vm->vm_properties.ram_size) trigger_bsod(ERROR_VM_STACK_UNDERFLOW);
word value = read_memory_word(vm, vm->sp);
vm->sp += 2;
ern[operand1 & 0x07] = value;
```

  - PSW: `psw.raw = 0`; `psw.field.zero = (value == 0)`
  - PC: +1
  - Errors: `ERROR_VM_STACK_UNDERFLOW`
  - PSW: `psw.raw = 0`; `psw.field.zero = (value == 0)`
  - PC: +1
  - Errors: `ERROR_VM_STACK_UNDERFLOW`

#### OP_PUSH_LR
- Encoding: [opcode][operand]
- Behavior (pseudocode):

```c
/* PUSH_LR */
if (vm->sp < 2) trigger_bsod(ERROR_VM_STACK_OVERFLOW);
vm->sp -= 2;
write_memory_word(vm, vm->sp, vm->lr);
```

  - PSW: unchanged
  - PC: +1
  - Errors: `ERROR_VM_STACK_OVERFLOW`
  - PSW: unchanged
  - PC: +1
  - Errors: `ERROR_VM_STACK_OVERFLOW`

#### OP_POP_PC
- Encoding: [opcode][operand]
- Behavior (pseudocode):

```c
/* POP_PC */
word value = pop_stack_word(vm);
if (value < code_start || value + 1 >= code_end) trigger_bsod(ERROR_VM_INVALID_INSTRUCTION);
vm->pc = value;
```

  - PSW: unchanged
  - PC: overwritten by popped value
  - Notes: used to return from calls saved with `BL_*` or `PUSH_LR`.
  - PSW: unchanged
  - PC: overwritten by popped value
  - Notes: used to return from calls saved with `BL_*` or `PUSH_LR`.

#### OP_PUSH_SP
- Encoding: [opcode][operand]
- Behavior: push current `sp` value onto stack (word)
- PSW: unchanged
  - PC: +1
  - Errors: `ERROR_VM_STACK_OVERFLOW`

#### OP_POP_SP
- Encoding: [opcode][operand]
- Behavior: pop a word from stack into `sp`
- PSW: unchanged
  - PC: +1
  - Errors: `ERROR_VM_STACK_UNDERFLOW`
  - Notes: manipulating SP directly can be used for low-level context switches; ensure value is valid for RAM layout.

### Arithmetic and logic (ALU)
This expanded ALU section describes each ALU instruction with clear pseudocode, exact PSW effects, examples, and edge-case notes.

Overview of helper semantics:
- alu_operation_byte/word always compute result truncated to width (8 or 16 bits), then set `flags.field.zero = (res == 0)` and `flags.field.neg = MSB(res)`.
- For add/mul: `carry` is set when the full precision operation exceeds the destination width.
- For sub: `carry` is used as a borrow indicator (`carry = (a < b)`).
- `overflow` is computed using two's complement formulas implemented in the helpers.

#### OP_ADD8_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1, src = operand2
- Pseudocode:

```c
/* OP_ADD8_REG_REG */
byte a = rn[operand1];
byte b = rn[operand2];
dword temp = (dword)a + (dword)b;
byte res = (byte)(temp & 0xFF);
rn[operand1] = res;
```

  - PSW exacts:
  - PSW exacts:
    - `carry` = (temp > 0xFF)
    - `overflow` = (((a ^ res) & (b ^ res) & 0x80) != 0)
    - `neg` = (res & 0x80) != 0
    - `zero` = (res == 0)
  - PC: +1
  - Example: 0x7F + 0x01 => res=0x80, carry=0, overflow=1, neg=1

#### OP_ADD16_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1 & 0x07 (ern index), src = operand2 & 0x07 (ern index)
- Pseudocode:

```c
/* OP_ADD16_REG_REG */
word a = ern[operand1 & 0x07];
word b = ern[operand2 & 0x07];
dword temp = (dword)a + (dword)b;
word res = (word)(temp & 0xFFFF);
ern[operand1 & 0x07] = res;
```

  - PSW exacts:
  - PSW exacts:
    - `carry` = (temp > 0xFFFF)
    - `overflow` = (((a ^ res) & (b ^ res) & 0x8000) != 0)
    - `neg` = (res & 0x8000) != 0
    - `zero` = (res == 0)
  - PC: +1
  - Errors: none
  - Notes: recommended for 16-bit arithmetic and pointer adjustments.

#### OP_ADD16_REG_IMM
- Encoding: [opcode][operand][imm_lo][imm_hi]
- Operands: dest = operand1 & 0x07, imm = next two code bytes (little-endian)
- Pseudocode:

```c
/* OP_ADD16_REG_IMM */
word imm = code[pc+1] | (code[pc+2] << 8);
word a = ern[operand1 & 0x07];
dword temp = (dword)a + (dword)imm;
word res = (word)(temp & 0xFFFF);
ern[operand1 & 0x07] = res;
```

  - PSW exacts: same as `OP_ADD16_REG_REG` (carry/overflow/neg/zero computed with 0xFFFF/0x8000 masks)
  - PC: +3
  - Errors: triggers `ERROR_VM_INVALID_INSTRUCTION` if immediate bytes are not available

#### OP_SUB8_REG_REG
- Pseudocode:

```c
/* OP_SUB8_REG_REG */
byte a = rn[operand1];
byte b = rn[operand2];
dword temp = (dword)a - (dword)b;
byte res = (byte)(temp & 0xFF);
rn[operand1] = res;
```

  - PSW:
  - PSW:
    - `carry` = (a < b)  // borrow
    - `overflow` = (((a ^ b) & (a ^ res) & 0x80) != 0)
    - `neg`/`zero` from res
  - PC: +1
  - Notes: use carry as borrow flag in unsigned comparisons.

#### OP_SUB16_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1 & 0x07, src = operand2 & 0x07
- Pseudocode:

```c
/* OP_SUB16_REG_REG */
word a = ern[operand1 & 0x07];
word b = ern[operand2 & 0x07];
dword temp = (dword)a - (dword)b;
word res = (word)(temp & 0xFFFF);
ern[operand1 & 0x07] = res;
```

  - PSW exacts:
  - PSW exacts:
    - `carry` = (a < b)  // borrow for unsigned semantics
    - `overflow` = (((a ^ b) & (a ^ res) & 0x8000) != 0)
    - `neg` = (res & 0x8000) != 0
    - `zero` = (res == 0)
  - PC: +1
  - Errors: none

#### OP_SUB16_REG_IMM
- Encoding: [opcode][operand][imm_lo][imm_hi]
- Operands: dest = operand1 & 0x07, imm = immediate word
- Pseudocode:

```c
/* OP_SUB16_REG_IMM */
word imm = code[pc+1] | (code[pc+2] << 8);
word a = ern[operand1 & 0x07];
dword temp = (dword)a - (dword)imm;
word res = (word)(temp & 0xFFFF);
ern[operand1 & 0x07] = res;
```

  - PSW: as above (carry/overflow/neg/zero)
  - PC: +3
  - Errors: `ERROR_VM_INVALID_INSTRUCTION` if immediate bytes are truncated

#### OP_INC8_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1 (rn index)
- Pseudocode:

```c
/* OP_INC8_REG */
byte a = rn[operand1];
byte res = (byte)(a + 1);
rn[operand1] = res;
```

  - PSW exacts:
    - `overflow` = (a == 0x7F) // signed overflow from 0x7F to 0x80
    - `neg` = (res & 0x80) != 0
    - `zero` = (res == 0)
    - `carry` = 0 (not set by INC helper)
  - PC: +1
  - Example: increment 0x7F -> res=0x80 (overflow, neg)

#### OP_INC16_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1 & 0x07 (ern index)
- Pseudocode:

```c
/* OP_INC16_REG */
word a = ern[operand1 & 0x07];
word res = (word)(a + 1);
ern[operand1 & 0x07] = res;
```

  - PSW exacts:
    - `overflow` = (a == 0x7FFF)
    - `neg` = (res & 0x8000) != 0
    - `zero` = (res == 0)
    - `carry` = 0
  - PC: +1

#### OP_DEC8_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1
- Pseudocode:

```c
/* OP_DEC8_REG */
byte a = rn[operand1];
byte res = (byte)(a - 1);
rn[operand1] = res;
```

  - PSW exacts:
    - `overflow` = (a == 0x80) // wrapped sign change
    - `neg` = (res & 0x80) != 0
    - `zero` = (res == 0)
    - `carry` = 0
  - PC: +1

#### OP_DEC16_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1 & 0x07
- Pseudocode:

```c
/* OP_DEC16_REG */
word a = ern[operand1 & 0x07];
word res = (word)(a - 1);
ern[operand1 & 0x07] = res;
```

  - PSW exacts:
    - `overflow` = (a == 0x8000)
    - `neg` = (res & 0x8000) != 0
    - `zero` = (res == 0)
    - `carry` = 0
  - PC: +1

#### OP_MUL8_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1, src = operand2
- Pseudocode:

```c
/* OP_MUL8_REG_REG */
byte a = rn[operand1];
byte b = rn[operand2];
dword temp = (dword)a * (dword)b;
byte res = (byte)(temp & 0xFF);
rn[operand1] = res;
```

  - PSW exacts:
    - `carry` = (temp > 0xFF)
    - `zero` = (res == 0)
    - `neg` = (res & 0x80) != 0
    - `overflow` = 0 (not set by MUL helper)
  - PC: +1
  - Notes: low 8 bits of the product are stored; `carry` indicates truncation.

#### OP_MUL16_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1 & 0x07, src = operand2 & 0x07
- Pseudocode:

```c
/* OP_MUL16_REG_REG */
word a = ern[operand1 & 0x07];
word b = ern[operand2 & 0x07];
dword temp = (dword)a * (dword)b;
word res = (word)(temp & 0xFFFF);
ern[operand1 & 0x07] = res;
```

  - PSW exacts:
  - PSW exacts:
    - `carry` = (temp > 0xFFFF)
    - `zero` = (res == 0)
    - `neg` = (res & 0x8000) != 0
    - `overflow` = 0
  - PC: +1
  - Notes: full 32-bit product is truncated to 16 bits; `carry` flags overflow.

#### OP_MUL16_REG_IMM
- Encoding: [opcode][operand][imm_lo][imm_hi]
- Operands: dest = operand1 & 0x07, imm = next two code bytes (little-endian)
- Pseudocode:

```c
/* OP_MUL16_REG_IMM */
word imm = code[pc+1] | (code[pc+2] << 8);
word a = ern[operand1 & 0x07];
dword temp = (dword)a * (dword)imm;
word res = (word)(temp & 0xFFFF);
ern[operand1 & 0x07] = res;
```

  - PSW: same as `OP_MUL16_REG_REG` (carry/zero/neg)
  - PC: +3
  - Errors: `ERROR_VM_INVALID_INSTRUCTION` if immediate truncated

#### OP_DIV8_REG_REG
- Encoding: [opcode][operand]
- Pseudocode:

```c
/* OP_DIV8_REG_REG */
byte a = rn[operand1];
byte b = rn[operand2];
byte res = (b == 0) ? 0 : (byte)(a / b);
rn[operand1] = res;
```

  - PSW:
  - PSW:
    - `zero` = (res == 0)
    - `neg` = (res & 0x80) != 0
    - `carry`/`overflow` cleared (helpers do not set for DIV)
  - PC: +1
  - Note: division-by-zero is handled silently by returning 0.

#### OP_DIV16_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1 & 0x07, src = operand2 & 0x07
- Pseudocode:

```c
/* OP_DIV16_REG_REG */
word a = ern[operand1 & 0x07];
word b = ern[operand2 & 0x07];
word res = (b == 0) ? 0 : (word)(a / b);
ern[operand1 & 0x07] = res;
```

  - PSW:
  - PSW:
    - `zero` and `neg` set from res
    - `carry`/`overflow` cleared
  - PC: +1

#### OP_DIV16_REG_IMM
- Encoding: [opcode][operand][imm_lo][imm_hi]
- Behavior: divide `ern[dest]` by immediate word (imm), if imm == 0 -> res = 0
- Pseudocode:

```c
/* OP_DIV16_REG_IMM */
word imm = code[pc+1] | (code[pc+2] << 8);
word a = ern[operand1 & 0x07];
word res = (imm == 0) ? 0 : (word)(a / imm);
ern[operand1 & 0x07] = res;
```

  - PSW: `zero`/`neg` accordingly
  - PC: +3
  - Errors: `ERROR_VM_INVALID_INSTRUCTION` if immediate missing

#### OP_MOD8_REG_REG
- Encoding: [opcode][operand]
- Pseudocode:

```c
/* OP_MOD8_REG_REG */
byte a = rn[operand1];
byte b = rn[operand2];
byte res = (b == 0) ? 0 : (byte)(a % b);
rn[operand1] = res;
```

  - PSW:
  - PSW: `zero`/`neg` from result
  - PC: +1

#### OP_MOD8_REG_IMM
- Encoding: [opcode][operand][imm]
- Pseudocode:

```c
/* OP_MOD8_REG_IMM */
byte a = rn[operand1];
byte b = code[pc+1];
byte res = (b == 0) ? 0 : (byte)(a % b);
rn[operand1] = res;
```

  - PSW: `zero`/`neg` from result
  - PC: +3
  - PSW: `zero`/`neg` from result
  - PC: +3

#### OP_MOD16_REG_REG
- Encoding: [opcode][operand]
- Pseudocode:

```c
/* OP_MOD16_REG_REG */
word a = ern[operand1 & 0x07];
word b = ern[operand2 & 0x07];
word res = (b == 0) ? 0 : (word)(a % b);
ern[operand1 & 0x07] = res;
```

  - PSW: `zero`/`neg` from result
  - PC: +1

#### OP_MOD16_REG_IMM
- Encoding: [opcode][operand][imm_lo][imm_hi]
- Pseudocode:

```c
/* OP_MOD16_REG_IMM */
word imm = code[pc+1] | (code[pc+2] << 8);
word a = ern[operand1 & 0x07];
word res = (imm == 0) ? 0 : (word)(a % imm);
ern[operand1 & 0x07] = res;
```

  - PSW: `zero`/`neg`
  - PC: +3

#### OP_AND8_REG_REG
- Encoding: [opcode][operand]
- Pseudocode:

```c
/* OP_AND8_REG_REG */
byte res = rn[operand1] & rn[operand2];
rn[operand1] = res;
```

  - PSW:
    - `zero` = (res == 0)
    - `neg` = (res & 0x80) != 0
    - `carry` = 0
    - `overflow` = 0
  - PC: +1
  - PSW:
    - `zero` = (res == 0)
    - `neg` = (res & 0x80) != 0
    - `carry` = 0
    - `overflow` = 0
  - PC: +1

#### OP_AND16_REG_REG
- Encoding: [opcode][operand]
- Pseudocode:

```c
/* OP_AND16_REG_REG */
word res = ern[operand1 & 0x07] & ern[operand2 & 0x07];
ern[operand1 & 0x07] = res;
```

  - PSW:
    - `zero` = (res == 0)
    - `neg` = (res & 0x8000) != 0
    - `carry` = 0
    - `overflow` = 0
  - PC: +1
  - PSW:
    - `zero` = (res == 0)
    - `neg` = (res & 0x8000) != 0
    - `carry` = 0
    - `overflow` = 0
  - PC: +1

#### OP_AND16_REG_IMM
- Encoding: [opcode][operand][imm_lo][imm_hi]
- Pseudocode:

```c
/* OP_AND16_REG_IMM */
word imm = code[pc+1] | (code[pc+2] << 8);
word res = ern[operand1 & 0x07] & imm;
ern[operand1 & 0x07] = res;
```

  - PSW: `zero`/`neg` from result; `carry`/`overflow` cleared
  - PC: +3

#### OP_OR8_REG_REG
- Encoding: [opcode][operand]
- Pseudocode:

```c
/* OP_OR8_REG_REG */
byte res = rn[operand1] | rn[operand2];
rn[operand1] = res;
```

  - PSW: `zero` = (res == 0); `neg` = (res & 0x80) != 0
  - PC: +1
  - PSW: `zero` = (res == 0); `neg` = (res & 0x80) != 0
  - PC: +1

#### OP_OR16_REG_REG
- Encoding: [opcode][operand]
- Pseudocode:

```c
/* OP_OR16_REG_REG */
word res = ern[operand1 & 0x07] | ern[operand2 & 0x07];
ern[operand1 & 0x07] = res;
```

  - PSW: `zero` = (res == 0); `neg` = (res & 0x8000) != 0
  - PC: +1
  - PSW: `zero` = (res == 0); `neg` = (res & 0x8000) != 0
  - PC: +1

#### OP_OR16_REG_IMM
- Encoding: [opcode][operand][imm_lo][imm_hi]
- Behavior: ern[dest] |= imm (16-bit immediate)
- PSW: `zero`/`neg` from result
- PC: +3

#### OP_XOR8_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1, src = operand2
- Pseudocode:

```c
/* OP_XOR8_REG_REG */
byte res = rn[operand1] ^ rn[operand2];
rn[operand1] = res;
```

  - PSW:
    - `zero` = (res == 0)
    - `neg` = (res & 0x80) != 0
    - `carry` = 0
    - `overflow` = 0
  - PC: +1
  - PSW:
    - `zero` = (res == 0)
    - `neg` = (res & 0x80) != 0
    - `carry` = 0
    - `overflow` = 0
  - PC: +1

#### OP_XOR16_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1 & 0x07, src = operand2 & 0x07
- Pseudocode:

```c
/* OP_XOR16_REG_REG */
word res = ern[operand1 & 0x07] ^ ern[operand2 & 0x07];
ern[operand1 & 0x07] = res;
```

  - PSW:
    - `zero` = (res == 0)
    - `neg` = (res & 0x8000) != 0
    - `carry` = 0
    - `overflow` = 0
  - PC: +1
  - PSW:
    - `zero` = (res == 0)
    - `neg` = (res & 0x8000) != 0
    - `carry` = 0
    - `overflow` = 0
  - PC: +1

#### OP_XOR16_REG_IMM
- Encoding: [opcode][operand][imm_lo][imm_hi]
- Operands: dest = operand1 & 0x07, imm = immediate word
- Behavior: res = ern[dest] ^ imm; ern[dest] = res
- PSW: `zero`/`neg` from result; `carry`/`overflow` cleared
- PC: +3

#### OP_NOT8_REG
- Behavior: bitwise NOT (res = ~a) (8-bit)

#### OP_NOT16_REG
- Behavior: bitwise NOT (res = ~a) (16-bit)
- PSW: `zero` = (res == 0); `neg` from MSB; `carry` and `overflow` cleared
- PC: +1
- Notes: `NOT` is useful for bit masks and clearing flags in-place.

### Comparisons
Comparison instructions compute flags using subtraction semantics without storing the result. They are the canonical way to prepare `psw` for conditional branching.

#### OP_CMP8_REG_REG
- Encoding: [opcode][operand]
- Operands: reg1 = operand1, reg2 = operand2
- Behavior (pseudocode):
    - a = rn[reg1]
    - b = rn[reg2]
    - res = (byte)(a - b)
    - flags = cmp_bytes(a, b) // zero, neg, carry (borrow), overflow
    - vm->psw = flags
  - PSW details:
    - `zero` = (res == 0)
    - `neg` = MSB(res)
    - `carry` = (a < b) // borrow
    - `overflow` = ((a ^ b) & (a ^ res) & 0x80) != 0
  - PC: +1
  - Notes: after CMP, branches like `BEQ` or `BLT` use these flags to make decisions.

#### OP_CMP16_REG_REG
- Encoding: [opcode][operand]
- Operands: reg1 = operand1 & 0x07, reg2 = operand2 & 0x07
- Pseudocode:

```c
/* OP_CMP16_REG_REG */
word a = ern[operand1 & 0x07];
word b = ern[operand2 & 0x07];
word res = (word)(a - b);
flags.field.zero = (res == 0);
flags.field.neg = (res & 0x8000) != 0;
flags.field.carry = (a < b);
flags.field.overflow = (((a ^ b) & (a ^ res) & 0x8000) != 0);
vm->psw = flags;
```


  - PC: +1
  - Notes: use `OP_CMP16_REG_REG` prior to signed or unsigned conditional branches that inspect `psw`.

#### OP_CMP16_REG_IMM
- Encoding: [opcode][operand][imm_lo][imm_hi]
- Operands: reg = operand1 & 0x07, imm = 16-bit little-endian immediate
- Pseudocode:

```c
/* OP_CMP16_REG_IMM */
word imm = code[pc+1] | (code[pc+2] << 8);
word a = ern[operand1 & 0x07];
word b = imm;
word res = (word)(a - b);
flags.field.zero = (res == 0);
flags.field.neg = (res & 0x8000) != 0;
flags.field.carry = (a < b);
flags.field.overflow = (((a ^ b) & (a ^ res) & 0x8000) != 0);
vm->psw = flags;
```

  - PC: +3
  - Errors: `ERROR_VM_INVALID_INSTRUCTION` if immediate bytes missing

#### OP_CMP8_REG_IMM
- (0xD0..0xDF) - Compact immediate block
- Encoding: [opcode = base | dest][operand]
- Behavior: compare `rn[dest]` with imm (operand byte)
- PSW: set by `cmp_bytes`
- PC: +1
- Example: opcode 0xD2 compares `rn2` with immediate operand.

### Shifts
Shifts are implemented via `ALU_OP_SLL`, `ALU_OP_SRL`, and `ALU_OP_SRA` helpers. The shift count is taken from the second operand or immediate and masked as follows:
- byte shifts: `n = (b & 7)` (0..7)
- word shifts: `n = (b & 15)` (0..15)

For all shift instructions, the helpers set `zero` and `neg` from the resulting truncated value and set `carry` to the last bit shifted out (or 0 when `n == 0`). `overflow` is not set by shifts in the helper implementations.

#### OP_SLL8_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1, src = operand2
- Pseudocode:

```c
/* OP_SLL8_REG_REG */
byte a = rn[operand1];
dword n = rn[operand2] & 7;
byte res;
byte carry;
if (n == 0) { res = a; carry = 0; }
else { res = (byte)(a << n); carry = (a >> (8 - n)) & 1; }
rn[operand1] = res;
```

  - PSW:
    - `carry` = last bit shifted out (see above)
    - `zero` = (res == 0)
    - `neg` = (res & 0x80) != 0
    - `overflow` = 0
  - PC: +1

#### OP_SLL8_REG_IMM
- Encoding: [opcode][operand]
- Operands: dest = operand1, imm = operand2 (4-bit immediate)
- Pseudocode:

```c
/* OP_SLL8_REG_IMM */
byte a = rn[operand1];
dword n = operand2 & 7;
byte res;
byte carry;
if (n == 0) { res = a; carry = 0; }
else { res = (byte)(a << n); carry = (a >> (8 - n)) & 1; }
rn[operand1] = res;
```

  - PSW: same as OP_SLL8_REG_REG
  - PC: +1

#### OP_SLL16_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1 & 0x07, src = operand2 & 0x07
- Pseudocode:

```c
/* OP_SLL16_REG_REG */
word a = ern[operand1 & 0x07];
dword n = ern[operand2 & 0x07] & 15;
word res;
word carry;
if (n == 0) { res = a; carry = 0; }
else { res = (word)(a << n); carry = (a >> (16 - n)) & 1; }
ern[operand1 & 0x07] = res;
```

  - PSW:
    - `carry` = last bit shifted out
    - `zero` = (res == 0)
    - `neg` = (res & 0x8000) != 0
  - PC: +1

#### OP_SLL16_REG_IMM
- Encoding: [opcode][operand]
- Operands: dest = operand1 & 0x07, imm = operand2 (4-bit immediate)
- Behavior: same as OP_SLL16_REG_REG but `n = imm & 15`
- PSW: same as OP_SLL16_REG_REG
- PC: +1

#### OP_SRL8_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1, src = operand2
- Pseudocode:

```c
/* OP_SRL8_REG_REG */
byte a = rn[operand1];
dword n = rn[operand2] & 7;
byte res;
byte carry;
if (n == 0) { res = a; carry = 0; }
else { res = (byte)(a >> n); carry = (a >> (n - 1)) & 1; }
rn[operand1] = res;
```

  - PSW: `carry` from last shifted bit, `zero` = (res == 0), `neg` = (res & 0x80) != 0
  - PC: +1

#### OP_SRL8_REG_IMM
- Encoding: [opcode][operand]
- Operands: dest = operand1, imm = operand2
- Behavior: same as OP_SRL8_REG_REG but `n = imm & 7`
- PSW: same as OP_SRL8_REG_REG
- PC: +1

#### OP_SRL16_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1 & 0x07, src = operand2 & 0x07
- Pseudocode:

```c
/* OP_SRL16_REG_REG */
word a = ern[operand1 & 0x07];
dword n = ern[operand2 & 0x07] & 15;
word res;
word carry;
if (n == 0) { res = a; carry = 0; }
else { res = (word)(a >> n); carry = (a >> (n - 1)) & 1; }
ern[operand1 & 0x07] = res;
```

  - PSW: `carry`/`zero`/`neg` as above
  - PC: +1

#### OP_SRL16_REG_IMM
- Encoding: [opcode][operand][imm]
- Behavior: same as OP_SRL16_REG_REG but `n = imm & 15`
- PSW: same as OP_SRL16_REG_REG
- PC: +1

#### OP_SRA8_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1, src = operand2
- Pseudocode:

```c
/* OP_SRA8_REG_REG */
sbyte a = (sbyte)rn[operand1];
dword n = rn[operand2] & 7;
byte res;
byte carry;
if (n == 0) { res = (byte)a; carry = 0; }
else { res = (byte)((a >> n) & 0xFF); carry = (rn[operand1] >> (n - 1)) & 1; }
rn[operand1] = res;
```

  - PSW: `neg` from sign, `zero` if res==0, `carry` last bit shifted out
  - PC: +1
    - else: res = (byte)((a >> n) & 0xFF); carry = (rn[dest] >> (n - 1)) & 1
    - rn[dest] = res
  - PSW: `neg` from sign, `zero` if res==0, `carry` last bit shifted out
  - PC: +1

#### OP_SRA8_REG_IMM
- Encoding: [opcode][operand]
- Behavior: arithmetic right shift with `n = imm & 7`
- PSW: same as OP_SRA8_REG_REG
- PC: +1

#### OP_SRA16_REG_REG
- Encoding: [opcode][operand]
- Operands: dest = operand1 & 0x07, src = operand2 & 0x07
- Pseudocode:

```c
/* OP_SRA16_REG_REG */
sword a = (sword)ern[operand1 & 0x07];
dword n = ern[operand2 & 0x07] & 15;
word res;
word carry;
if (n == 0) { res = (word)a; carry = 0; }
else { res = (word)((a >> n) & 0xFFFF); carry = (ern[operand1 & 0x07] >> (n - 1)) & 1; }
ern[operand1 & 0x07] = res;
```

  - PSW: `neg`/`zero`/`carry` set from result and last bit shifted
  - PC: +1

#### OP_SRA16_REG_IMM
- Encoding: [opcode][operand]
- Behavior: arithmetic right shift with `n = imm & 15`
- PSW: same as OP_SRA16_REG_REG
- PC: +1

- Notes: all shifts clear or set `overflow` only if helpers were to do so (current implementation does not set `overflow` for shifts). Edge cases with `n==0` leave `carry` cleared and result unchanged.

### Control flow
Control-flow instructions perform absolute and relative jumps with validation and call/return support.

#### OP_B_REG
- Encoding: [opcode][operand]
- Operands: addr_reg = operand1 & 0x07
- Behavior:
    - address = ern[addr_reg]
    - if address < code_start or address + 1 >= code_end -> trigger_bsod(ERROR_VM_INVALID_INSTRUCTION)
    - vm->pc = address
  - PSW: unchanged
  - Notes: absolute branch using register-held address (16-bit). Validate before jumping to avoid executing outside code region.

#### OP_B_IMM
- Encoding: [opcode][operand][addr_lo][addr_hi]
- Behavior:
    - address = read_word_from_code(vm->pc, vm->pc+1)
    - validate address within code bounds
    - vm->pc = address
  - PC: pc is set to address; instruction consumes the two immediate bytes during fetch
  - Errors: insufficient code bytes or invalid target -> `ERROR_VM_INVALID_INSTRUCTION`

#### Short conditional branches
**(OP_BEQ_IMM, OP_BNE_IMM, OP_BLT_IMM, OP_BLE_IMM, OP_BGT_IMM, OP_BGE_IMM)**
  - Encoding: [opcode][rel8]
  - Behavior:
    - rel8 = (sbyte)operand_whole
    - off16 = rel8 * 2 (scale by 2)
    - base = vm->pc - 1
    - target = base + off16
    - Evaluate condition using `vm->psw` macros (UNSIGNED_* helpers). If true: vm->pc = target else vm->pc += 1
  - Conditions and mapping to PSW macros:
    - BEQ: UNSIGNED_EQ (zero)
    - BNE: UNSIGNED_NE
    - BLT: UNSIGNED_LT (carry)
    - BLE: UNSIGNED_LE
    - BGT: UNSIGNED_GT
    - BGE: UNSIGNED_GE
  - PSW: not modified by branch instruction
  - Notes: offset is signed and scaled by 2 for word alignment; negative offsets branch backwards.

#### OP_BL_REG
- Encoding: [opcode][operand]
- Behavior:
    - address = ern[ addr_reg ]
    - validate address
    - vm->lr = vm->pc
    - vm->pc = address
  - PSW: unchanged
  - Notes: `lr` contains the return address (the PC after the operand byte), suitable for `POP_PC` to return.

#### OP_BL_IMM
- Encoding: [opcode][operand][addr_lo][addr_hi]
- Behavior:
    - address = immediate word at vm->pc, vm->pc+1
    - vm->lr = vm->pc + 2  // next instruction after the two immediate bytes
    - vm->pc = address
  - PC: overwrites pc with target; `lr` set so a return can be performed
  - Errors: invalid immediate or invalid target

### Compact immediate forms (0x60–0xEF)
The 16-entry opcode blocks encode the destination register in the opcode low nibble and place the 8-bit immediate in the operand byte. General pattern:
- opcode = BASE | dest  (BASE = 0x60, 0x70, ..., 0xE0)
- operand = imm (full 8-bit immediate)
- instruction consumes one operand byte and advances PC by +1

Concrete behaviors and PSW effects (compact immediate blocks):

General encoding pattern for these blocks:
- opcode = BASE | dest (BASE is 0x60, 0x70, ..., 0xE0)
- operand byte contains the 8-bit immediate `imm`
- dest = opcode & 0x0F (index into `rn[]`)
- instructions consume the operand byte and advance `pc` by +1

#### OP_MOV8_REG_IMM
- Range: 0x60..0x6F
- Encoding: [opcode = 0x60 + dest][operand = imm]
- Pseudocode:

```c
/* OP_MOV8_REG_IMM (compact) */
dword dest = opcode & 0x0F;
byte imm = code[pc+1];
rn[dest] = imm;
```

  - PSW: `psw.raw = 0`; `psw.field.zero = (imm == 0)`; `neg`/`carry`/`overflow` cleared
  - PC: +1
  - Notes: efficient immediate load into 8-bit register.

#### OP_ADD8_REG_IMM
- Range: 0x70..0x7F
- Encoding: [0x70 + dest][imm]
- Pseudocode:

```c
/* OP_ADD8_REG_IMM (compact) */
dword dest = opcode & 0x0F;
byte a = rn[dest];
byte b = code[pc+1];
dword temp = (dword)a + (dword)b;
byte res = (byte)(temp & 0xFF);
rn[dest] = res;
```

  - PSW exacts: same as `ALU_OP_ADD` at byte width:
    - `carry` = (temp > 0xFF)
    - `overflow` = (((a ^ res) & (b ^ res) & 0x80) != 0)
    - `neg` = (res & 0x80) != 0
    - `zero` = (res == 0)
  - PC: +1

#### OP_SUB8_REG_IMM
- Range: 0x80..0x8F
- Encoding: [0x80 + dest][imm]
- Pseudocode: dest = (byte)(rn[dest] - imm)
- PSW exacts per `ALU_OP_SUB` (borrow encoded in `carry`, overflow via two's-complement formula, `neg` and `zero` from result)
- PC: +1

#### OP_MUL8_REG_IMM
- Range: 0x90..0x9F
- Encoding: [0x90 + dest][imm]
- Behavior: res = low 8 bits of (rn[dest] * imm)
  - PSW exacts:
    - `carry` = product > 0xFF
    - `zero`/`neg` from truncated result
  - PC: +1

#### OP_DIV8_REG_IMM
- Range: 0xA0..0xAF
- Encoding: [0xA0 + dest][imm]
- Behavior: if imm == 0 -> res = 0 else res = rn[dest] / imm
  - PSW: `zero`/`neg` from result
  - PC: +1

#### OP_AND8_REG_IMM
- Range: 0xB0..0xBF
- Encoding: [0xB0 + dest][imm]
- Behavior: res = rn[dest] & imm
  - PSW: `zero` = (res == 0); `neg` from MSB; `carry`/`overflow` cleared
  - PC: +1

#### OP_OR8_REG_IMM
- Range: 0xC0..0xCF
- Encoding: [0xC0 + dest][imm]
- Behavior: res = rn[dest] | imm
  - PSW: `zero`/`neg` from result
  - PC: +1

#### OP_XOR8_REG_IMM
- Range: 0xD0..0xDF
- Encoding: [0xD0 + dest][imm]
- Behavior: res = rn[dest] ^ imm
  - PSW: `zero`/`neg` from result
  - PC: +1

#### OP_CMP8_REG_IMM
- Range: 0xE0..0xEF
- Encoding: [0xE0 + dest][imm]
- Behavior: compute flags for `rn[dest] - imm` using `cmp_bytes` (no register written)
  - PSW exacts:
    - `zero` = ((rn[dest] - imm) == 0)
    - `neg` = MSB(result)
    - `carry` = (rn[dest] < imm) // borrow
    - `overflow` = ((rn[dest] ^ imm) & (rn[dest] ^ result) & 0x80) != 0
  - PC: +1

Notes: these compact encodings represent 16 distinct opcodes per base (one per destination register). They are optimized for immediate arithmetic and comparisons.

---

## Notes on PSW behavior
- Most arithmetic and logical operations overwrite `vm->psw` with the flags returned from `alu_operation_*` or `cmp_*`.
- Simple moves and loads set `psw.raw = 0` and only set `zero` when the result equals zero; they clear other flags.
- Shifts set `carry` to the last bit shifted out, and set `zero`/`neg` from the result; `overflow` remains cleared unless explicitly set by the ALU helper for certain ops.

---

## ALU implementation caveats
- Division and modulo by zero return zero (no exception).
- Overflow detection for add/sub/inc/dec is implemented using two's-complement rules.

---

## ALU and instruction examples
- Add immediate to `rn0`: bytes: `[0x60 | 0x0][imm]` (i.e., opcode 0x60, operand = imm)
- Branch-if-equal short: `[OP_BEQ_IMM][rel8]` where `rel8` is signed and scaled by 2.

---
## ALU operations and flags
ALU operations are implemented in two helpers:
- `alu_operation_byte(alu_op_t op, byte a, byte b, byte* result)`
- `alu_operation_word(alu_op_t op, word a, word b, word* result)`

ALU details:
- Arithmetic sets `carry` for unsigned overflow (or borrow for subtraction), `overflow` for two's complement signed overflow, `zero` when result == 0, and `neg` when the high bit is set (MSB of result).
- Multiply sets `carry` if the full product does not fit in the result width.
- Divide/mod by zero returns zero (no exception) - the operation in `alu_operation_*` sets result to 0 when divisor is zero.
- Shifts:
  - Logical left (SLL) and right (SRL) shift by `n & 7` (byte) or `n & 15` (word).
  - Arithmetic right (SRA) preserves sign bit; carry is set from the last bit shifted out.

Important: `trigger_bsod(ERROR_VM_INVALID_ALU_OPERATION)` is invoked on unexpected `alu_op_t` values.

---

## Memory access and stack operations
Memory helper functions:
- `write_memory_byte(vm, addr, value)` and `write_memory_word(vm, addr, value)` verify `uses_ram` and bounds, otherwise trigger `ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS`.
- `read_memory_byte(vm, addr)` and `read_memory_word(vm, addr)` do similar checks.

Stack helpers:
- `push_stack_byte`, `push_stack_word` decrement `sp` and write to memory; they check for stack overflow (underflow of `sp` below 0 / cannot allocate).
- `pop_stack_byte`, `pop_stack_word` read memory at `sp` and increment `sp`; they check for underflow (read past top of RAM) and trigger `ERROR_VM_STACK_UNDERFLOW`.

Stack invariants:
- Stack pointer `sp` is a word index into RAM and must be in range `[0, ram_size]`.

---

## Comparisons, branches and control flow
- Comparisons:
  - `cmp_bytes` and `cmp_words` compute `a - b` and populate `psw` similarly to ALU subtraction semantics (zero/neg/carry/overflow).

- Branch conditions rely on `psw` and helper macros (e.g. `SIGNED_LT(psw)`), which map `zero`, `neg`, `overflow`, and `carry` into signed and unsigned comparisons.

- Branch validation: target addresses are validated to be within `[code_start, code_end)`; branching to an invalid address triggers `ERROR_VM_INVALID_INSTRUCTION`.

Call/return: `OP_BL_*` stores return address to `lr`. `OP_PUSH_LR` / `OP_POP_PC` allow saving and restoring return addresses on the stack.

---

## Syscalls
Syscall numbers are defined in `syscall_t` and include:
- `GET_ELEMENT_FIELD`, `SET_ELEMENT_FIELD`, `RENDER_ELEMENT` - interface to TML/TUI element rendering and manipulation
- Drawing primitives: `DRAW_LINE`, `SET_PIXEL`, `GET_PIXEL`, `DRAW_IMAGE`, `DRAW_RECT`, `DRAW_TEXT`
- `SLEEP`, `STOP`, `END`

Syscalls read registers for arguments (e.g., `vm->registers.rn[]` and `vm->registers.ern[]`) and delegate to TUI helper functions (e.g., `tml_render_element`, `tui_draw_line`, etc.). Some syscalls validate element addresses and raise `ERROR_TUI_INVALID_ELEMENT` on invalid inputs.

Note: `SLEEP` is a placeholder in the VM and currently does not pause only the VM (code contains a `TODO` comment to improve it later).

---

## VM lifecycle API
- `vm_init_system()` - allocate an initial pool for VM pointers using `hcalloc`. Sets `vm_capacity = MAX_VMS` and `vm_pool` zeros.
- `vm_spawn(code)` - allocates `TangentMachine` + RAM (`ram_size` from header), initializes fields, sets `vm->code = code`, calls `vm_init(vm, code)`, stores the VM in the pool. Expands the pool by `MAX_VMS` when needed using `hrealloc` and initializes new slots to `0`.
- `vm_run_file(fs_node_t* parent, const char *filename)` - loads a vm wil the code from a given file.
- `vm_init(vm, code)` - sets `vm->ram`, `vm->code`, `vm->vm_properties.running = 1`, `vm->sp = vm->vm_properties.ram_size`, `vm->pc = 4` and ensures `uses_ram` matches `ram_size`.
- `vm_destroy(vm)` - finds VM in `vm_pool`, frees it with `hfree` and sets the pool slot to `0`.
- `vm_step(vm)` - execute a single instruction for `vm` (if running), validating code bounds and instruction lengths, updating `pc` and `psw` appropriately.
- `vm_step_all()` - iterates `vm_pool` and calls `vm_step` for each non-NULL entry.
- `vm_shutdown()` - frees each VM in the pool and the pool itself.

Notes on allocation: the code uses project helpers `hcalloc`, `hrealloc`, `hfree` for heap management.

---

## Error handling and invariants
The VM uses `trigger_bsod(ERROR_XXX)` to handle fatal errors. Common error codes used in `vm.c` include:
- `ERROR_VM_INVALID_ALU_OPERATION`
- `ERROR_VM_OUT_OF_BOUNDS_MEMORY_ACCESS`
- `ERROR_VM_STACK_OVERFLOW`
- `ERROR_VM_STACK_UNDERFLOW`
- `ERROR_TUI_INVALID_ELEMENT`
- `ERROR_VM_INVALID_SYSCALL`
- `ERROR_VM_INVALID_INSTRUCTION`

Invariant checks performed by the VM:
- Ensure `vm->pc` lies inside code bounds before fetching an opcode
- Validate enough bytes remain for immediates and memory reads
- Validate stack pointer bounds on push/pop
- Validate memory access ranges for reads and writes

> Note: BSOD handling is fatal and intentionally halts or crashes the system in response to invalid VM state.

---

## Extension and debugging notes
- Adding opcodes: append to `opcode_t`, implement handling in `vm_step()` and ensure PC advance, operand decoding and flag updates are correct.
- Adding syscalls: update `syscall_t` and implement the handler in `vm_syscall()`; ensure argument passing conventions remain consistent.
- Unit tests: construct minimal bytecode blobs (header + code region) and assert final register state or memory after a sequence of `vm_step()` calls.

---

## Examples and bytecode snippets
Header + single instruction examples (little-endian):

- Create a VM with no RAM and an empty program:
```
code[0..3] = {0x00, 0x00, 0x00, 0x00}  // code_size = 0, ram_size = 0
```

- MOV16_REG_IMM sample (pseudo-assembly):
  - opcode: `OP_MOV16_REG_IMM` (value from `opcode_t`)
  - operand: dest register in high nibble (e.g., dest=1 -> operand = 0x10)
  - immediate: two bytes (little-endian)

Bytecode layout:
```
[code_size_lo, code_size_hi, ram_size_lo, ram_size_hi, opcode, operand, imm_lo, imm_hi, ...]
```

Testing tip: always keep the code area length (`code_size`) correct; `vm_step()` validates PC and immediate availability and will call `trigger_bsod(ERROR_VM_INVALID_INSTRUCTION)` if an instruction overruns the code region.

---

## References
- Implementation: `src/vm/vm.c`
- Related modules: `src/tui/` (drawing & element management), `src/debug/` (BSOD & error codes), heap allocators used: `hcalloc`, `hrealloc`, `hfree`.
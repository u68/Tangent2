#ifndef LCONFIG_H
#define LCONFIG_H

#include "lbase.h"

#define NUM_REGS (const byte) 16
#define RAM_SIZE (const word) 256
#define CODE_SIZE (const word) 1024

#define PROGRAM_SIZE (const word) 10 + NUM_REGS + RAM_SIZE + CODE_SIZE // PID + PC + SP + LR + FLAGS + regs + ram + code

#define STORAGE_START (const word) 0xE000

#define vram1 (const word) 0x9002
#define vram2 (const word) 0x9602

#define PROGRAM_COUNT (const word) 0x9000
#endif

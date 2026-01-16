/*
 * main.c
 * Implementation of you know what
 * Virtual Machine speed test
 *  Created on: Jan 4, 2026
 *  	Author: harma
 *
*/

#include "base.h"
#include "heap/heap.h"
#include "tui/graphics.h"
#include "vm/vm.h"
#include "debug/debug.h"
#include "debug/bsod.h"

// Custom breakpoint handler (called when BRK executed in asm)
void custom_break() {
	while (1) {
		deref(0x9000) += 1;
	}
}

#define num_vms 10

// Main entry point
int main() {
	/*
	byte* snap = 0;
	get_regs_snapshot(snap);
	bsod_show(ERROR_VM_INVALID_INSTRUCTION, snap);
	custom_break();*/
    Write2RealScreen = 0;
    tui_clear_screen();

    hinit();

    vm_init_system();
    // Example VM program: compute Fibonacci(n) into ram[0]
    static const byte example_program[] = {
        30, 0,  // code size
        2, 0,   // ram size
        0x02, 0x00, 0x00, 0x00, // MOV16_REG_IMM er0, 0
        0x02, 0x10, 0x01, 0x00, // MOV16_REG_IMM er1, 1
        0x56, 0x17,             // MOV_RN_IMM r6, 23
		0x1D, 0x60, 		   	// DEC8_REG r6 (zero index stuff)
        // loop:
        0x01, 0x20, // MOV16_REG_REG er2, er0
        0x16, 0x21, // ADD16_REG_REG er2, er1
        0x01, 0x01, // MOV16_REG_REG er0, er1
        0x01, 0x12, // MOV16_REG_REG er1, er2
        0x1D, 0x60, // DEC8_REG r6
        0xD6, 0x00, // CMP8_REG_IMM r6, 0
        0x46, -6,   // BNE_IMM back to loop
        0x0A, 0x01, 0x00, 0x00 // STORE16_MIMM_REG ern1, 0x0000
    };

    TangentMachine* vms[num_vms];
    for (int i = 0; i < num_vms; i++) {
        vms[i] = vm_spawn((byte*)example_program);
    }

    while(1) {
    	if (deref(0xE010) == 0x67) {
while (vms[0]->pc < (vms[0]->vm_properties.code_size + 4)) {
    			vm_step_all();
    		}

    		deref(0xE010) = 0x00;
    	}
    	if (deref(0xE010) == 0x68) {
			break;
    	}
    }
    /*for (int step = 0; step < 5; step++) {
            vm_step_all();
        }*/

    // Store each VM's ram into E000...
    for (int i = 0; i < num_vms; i++) {
        deref(0xE000 + (i<<1)) = vms[i] ? vms[i]->ram[0] : 0;
        deref(0xE000 + (i<<1)+1) = vms[i] ? vms[i]->ram[1] : 0;
    }
	return 0;
}


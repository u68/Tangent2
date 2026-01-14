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
	byte* snap = 0;
	get_regs_snapshot(snap);
	bsod_show(ERROR_VM_INVALID_INSTRUCTION, snap);
	custom_break();
    Write2RealScreen = 0;
    tui_clear_screen();

    hinit();

    vm_init_system();
    // Set code size to 6, ram to 2, with five 0x67 test instructions
    static const byte example_program[] = { 6, 0, 2, 0, 0x67, 0x67, 0x67, 0x67, 0x67, 0x61 };

    TangentMachine* vms[num_vms];
    for (int i = 0; i < num_vms; i++) {
        vms[i] = vm_spawn((byte*)example_program);
    }

    while(1) {
    	if (deref(0xE010) == 0x67) {
    		break;
    	}
    }

    // Step all VMs until E010 is FF
    while(1) {
    	if (deref(0xE010) == 0xFF) {
			break;
		}
		vm_step_all();
    }
    deref(0xE011) = 0x67;
    /*for (int step = 0; step < 5; step++) {
            vm_step_all();
        }*/

    // Store each VM's ram[0] into 0x9100..0x9163 to inspect results
    for (int i = 0; i < num_vms; i++) {
        deref(0x9100 + i) = vms[i] ? vms[i]->ram[0] : 0;
    }
	return 0;
}


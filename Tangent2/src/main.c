/*
 * main.c
 * Implementation of you know what
 * Virtual Machine speed test
 *  Created on: Jan 4, 2026
 *      Author: harma
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

#define num_vms 2

// Main entry point
int main() {
	/*
	byte* snap = 0;
	get_regs_snapshot(snap);
	bsod_show(ERROR_VM_INVALID_INSTRUCTION, snap);
	custom_break();*/
	for (word i = 0x9000; i < 0xE000; i++) {
				hw_deref(i) = 0;
	}
    Write2RealScreen = 0;
    tui_clear_screen();

    hinit();

    vm_init_system();
	static const byte example_program[] = {
		//head
		0xFF,00,
		0xFF,00,
		//mov r0, #0
		OP_MOV8_REG_IMM, 0,
		//fill_loop:
			//cmp r0, #20
			OP_CMP8_REG_IMM, 20,
			//bge fill_end
			OP_BGE_IMM, 0x0A,
			//mov er3, er0
			OP_MOV16_REG_REG, 0x30,
			//mov r7, #0
			OP_MOV8_REG_IMM+7, 0,
			//add er3, #17
			OP_ADD16_REG_IMM, 0x30, 17, 0,
			//st [er3], #2
			OP_STORE8_MREG_IMM, 0x03, 2, 0,
			//inc r0
			OP_INC8_REG, 0x00,
			//b fill_loop
			OP_B_IMM, 0x00, 0x06, 0x00,
		//fill_end:
		//st #17, '3'
		OP_STORE8_MIMM_IMM, '3', 0, 0,
		//st #18, '.'
		OP_STORE8_MIMM_IMM, '.', 1, 0,
		//mov r0, #2
		OP_MOV8_REG_IMM, 2,
		//pi_loop:
			//cmp r0, #17
			OP_CMP8_REG_IMM, 17,
			//bge pi_end
			OP_BGE_IMM, 35,
			//mov r3, #0
			OP_MOV8_REG_IMM+3, 0,
			//mov r1, #0
			OP_MOV8_REG_IMM+1, 0,
			//proc_loop:
				//cmp r1, #20
				OP_CMP8_REG_IMM+1, 20,
				//bge proc_end
				OP_BGE_IMM, 20,
				//mov er3, er0
				OP_MOV16_REG_REG, 0x30,
				//srl er3, #8
				OP_SRL16_REG_IMM, 0x38,
				//add er3, #17
				OP_ADD16_REG_IMM, 0x30, 17, 0,
				//l r4, [er3]
				OP_LOAD8_REG_MREG, 0x43,
				//mul r4, #10
				OP_MUL8_REG_IMM+4, 10,
				//mov r2, r4
				OP_MOV8_REG_REG, 0x24,
				//add r2, r3
				OP_ADD8_REG_REG, 0x23,
				//mov r3, r2
				OP_MOV8_REG_REG, 0x32,
				//srl r3, #4
				OP_SRL8_REG_IMM, 0x34,
				//and r2, #15
				OP_AND8_REG_IMM+2, 15,
				//mov er3, er0
				OP_MOV16_REG_REG, 0x30,
				//srl er3, #8
				OP_SRL16_REG_IMM, 0x38,
				//add er3, #17
				OP_ADD16_REG_IMM, 0x30, 17, 0,
				//st [er3], r2
				OP_STORE8_MREG_REG, 0x32,
				//inc r1
				OP_INC8_REG, 0x10,
				//b proc_loop
				OP_B_IMM, 0, 0x30, 0,
			//proc_end:
			//l r5, #17
			OP_LOAD8_REG_MIMM, 0x50, 17, 0,
			//add r5, '0'
			OP_ADD8_REG_IMM+5, '0',
			//mov er3, er0
			OP_MOV16_REG_REG, 0x30,
			//mov r7, #0
			OP_MOV8_REG_IMM+7, 0,
			//st [er3], r5
			OP_STORE8_MREG_REG, 0x35,
			//inc r0
			OP_INC8_REG, 0,
			//OP_END, 0x00,
			//b pi_loop
			OP_B_IMM, 0, 0x26, 0,
		//pi_end:
		//end
		OP_STORE8_MIMM_REG, 0x00, 0x06, 0,
		OP_STORE8_MIMM_IMM, 0xFF, 0x07, 0,
		OP_STORE8_MIMM_REG, 0x00, 0x06, 0,
		OP_END, 0,

	};

    TangentMachine* vms[num_vms];
    for (byte i = 0; i < num_vms; i++) {
        vms[i] = vm_spawn(example_program);
    }

    for (word step = 0; step < 1000; step++) {
            vm_step_all();
            deref(0x9A00) = step;
        }

    // Store each VM's ram into E000...
    for (byte i = 0; i < 18; i++) {
        deref(0x9B60 + i) = vms[0]->ram[i];
    }
	return 0;
}


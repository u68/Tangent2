/*
 * main.c
 * Implementation of you know what
 * Virtual Machine test
 *  Created on: Jan 4, 2026
 *  	Author: harma
 *
*/

#include "tui/glib.h"
#include "heap/heap.h"
#include "desktop/desktop.h"
#include "vm/vm.h"

// Custom breakpoint handler (called when BRK executed in asm)
void custom_break() {

}

// Main entry point
int main() {
    Write2RealScreen = 0;
    tui_clear_screen();

    hinit();

    vm_init_system();
    // Set code size to 10, and ram to 10, and 5 67's (test instruction that increments ram[0])
    static const byte example_program[] = { 10, 0, 10, 0, 0x67, 0x67, 0x67, 0x67, 0x67 };

    TangentMachine* vm1 = vm_spawn((byte*)example_program);
    TangentMachine* vm2 = vm_spawn((byte*)example_program);
    TangentMachine* vm3 = vm_spawn((byte*)example_program);
    TangentMachine* vm4 = vm_spawn((byte*)example_program);
    TangentMachine* vm5 = vm_spawn((byte*)example_program);

    // Step all VMs 5 times
    for (int i = 0; i < 5; i++) {
        vm_step_all();
    }

    // Store each VM's ram[0] into 0x9100..0x9104 to inspect results
    deref(0x9100) = vm1 ? vm1->ram[0] : 0;
    deref(0x9101) = vm2 ? vm2->ram[0] : 0;
    deref(0x9102) = vm3 ? vm3->ram[0] : 0;
    deref(0x9103) = vm4 ? vm4->ram[0] : 0;
    deref(0x9104) = vm5 ? vm5->ram[0] : 0;
	return 0;
}

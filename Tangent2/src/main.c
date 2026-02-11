/*
 * main.c
 * keybart test
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#include "libcw.h"
#include "input/input.h"

// Custom breakpoint handler (called when BRK executed in asm)
void custom_break(void) {
	while (1) {
		deref(0x9000) += 1;
	}
}

int main(void) {
	char text[33] = {0}; // 32 chars max + NUL

	Write2RealScreen = 0;

	tui_clear_screen();
	tui_render_buffer();

	while (1) {
		if (input_text(text, sizeof(text))) {
			//tui_clear_screen();
			tui_draw_text(0, 0, text, TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
			tui_render_buffer();
		}
	}
	return 0;
}



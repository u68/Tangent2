/*
 * main.c
 * Implementation of you know what
 * File System test
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#include "libcw.h"
#include "tui/glib.h"

static char to_hex_nibble(byte value) {
	value &= 0x0F;
	return (value < 10) ? ('0' + value) : ('A' + (value - 10));
}

// Custom breakpoint handler (called when BRK executed in asm)
void custom_break(void) {
	while (1) {
		deref(0x9000) += 1;
	}
}

// Main entry point
int main(void) {
	Write2RealScreen = 0;
	tui_clear_screen();

	char msg[] = "BTN: 0x00";
	int last_drawn = -1;

	while (1) {
		byte a = CheckButtons();
		if (a != 0xFF) {
			hw_deref(0xA010) = a;
			msg[6] = to_hex_nibble(a >> 4);
			msg[7] = to_hex_nibble(a);
			tui_clear_screen();
			tui_draw_text(4, 4, msg, TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
			tui_render_buffer();
		}
	}

	return 0;
}


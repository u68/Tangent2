/*
 * main.c
 * Implementation of you know what
 *  Created on: Jan 4, 2026
 *  	Author: harma
 *
*/

#include "Tui/glib.h"
#include "heap.h"
#include "desktop.h"

void custom_break() {

}

int main() {
	Write2RealScreen = 0;
	tui_clear_screen();

	hinit();

	TmlElement* desktop = desktop_init();

	word wx = 40, wy = 15;
	sbyte dx = 1, dy = 1;

	while (1) {
		tui_clear_screen();

		wx += dx;
		wy += dy;

		if (wx > (192 - 114) || wx < 1) dx = -dx;
		if (wy > (63 - 36) || wy < 2) dy = -dy;

		TmlElement* window = desktop_get_window(desktop);
		if (window) {
			tml_set_position(window, (byte)wx, (byte)wy);
		}

		desktop_render(desktop);
		tui_render_buffer();
	}
	return 0;
}

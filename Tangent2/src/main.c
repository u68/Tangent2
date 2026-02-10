/*
 * main.c
 * Implementation of you know what
 * Keyboard Test
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#include "libcw.h"
#include "tui/glib.h"

// Custom breakpoint handler (called when BRK executed in asm)
void custom_break(void) {
	while (1) {
		deref(0x9000) += 1;
	}
}
const char horizontal[2][28] = {
    "0        1         2      ",
    "12345678901234567890123456"
};

const char vertical[4][2] = {
    "1",
    "2",
    "3",
    "4"
};

const char ascii[4][27] = {
    "abcdefghijklmnopqrstuvwxyz",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    "  ~!@#$%^&*()_+{}|:\"<>?   ",
    "  1234567890-=[]\\;',./    "
};

typedef enum {
	MODE_PICK_ROW = 0,
	MODE_PICK_TENS,
	MODE_PICK_ONES,
} SelectMode;

static byte key_to_digit(byte key, byte *out_digit) {
	switch (key) {
	case B_0: *out_digit = 0; return 1;
	case B_1: *out_digit = 1; return 1;
	case B_2: *out_digit = 2; return 1;
	case B_3: *out_digit = 3; return 1;
	case B_4: *out_digit = 4; return 1;
	case B_5: *out_digit = 5; return 1;
	case B_6: *out_digit = 6; return 1;
	case B_7: *out_digit = 7; return 1;
	case B_8: *out_digit = 8; return 1;
	case B_9: *out_digit = 9; return 1;
	default:
		return 0;
	}
	return 0; // stupid warning
}

int main(void) {
    Write2RealScreen = 0;

	const byte table_x = 22;
	const byte table_cols = 26;
	const byte char_w = 6;
	const byte row_h = 7;
	const byte row_y0 = 16;

	SelectMode mode = MODE_PICK_ROW;
	byte selected_row = 0;
	byte selected_col = 0;
	sbyte tens = -1;
	sbyte ones = -1;

	byte last_key = 0xff;

	byte hi_row = 0;
	byte hi_col = 0;
	byte hi_col_end = 0;
	byte hi_kind = 0;

	tui_clear_screen();
	tui_draw_text(table_x, row_y0-10, horizontal[0], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x, row_y0-3, horizontal[1], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x-9, row_y0+7, vertical[0], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x-9, row_y0+14, vertical[1], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x-9, row_y0+21, vertical[2], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x-9, row_y0+28, vertical[3], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);

	tui_draw_text(table_x, row_y0+7, ascii[0], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x, row_y0+14, ascii[1], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x, row_y0+21, ascii[2], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x, row_y0+28, ascii[3], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);

	tui_render_buffer();

	while (1) {
		byte key = CheckButtons();
		if (key == 0xff) {
			last_key = 0xff;
			continue;
		}
		if (key == last_key) {
			continue;
		}
		last_key = key;

		if (key == SP_UP || key == SP_DOWN || key == SP_LEFT || key == SP_RIGHT) {
			if (!selected_row) {
				selected_row = 1;
			}
			if (!selected_col) {
				selected_col = 1;
			}

			mode = MODE_PICK_ROW;
			tens = -1;
			ones = -1;

			switch (key) {
			case SP_LEFT:
				if (selected_col > 1) {
					selected_col--;
				}
				break;
			case SP_RIGHT:
				if (selected_col < table_cols) {
					selected_col++;
				}
				break;
			case SP_UP:
				if (selected_row > 1) {
					selected_row--;
				}
				break;
			case SP_DOWN:
				if (selected_row < 4) {
					selected_row++;
				}
				break;
			default:
				break;
			}
		}
		if (key == SP_BACK) {
			if (mode == MODE_PICK_ONES) {
				ones = -1;
				mode = MODE_PICK_TENS;
			} else if (mode == MODE_PICK_TENS) {
				tens = -1;
				selected_row = 0;
				selected_col = 0;
				mode = MODE_PICK_ROW;
			} else {
			}
		}

		switch (mode) {
		case MODE_PICK_ROW:
			if (key == B_1 || key == B_2 || key == B_3 || key == B_4) {
				selected_row = (key == B_1) ? 1 : (key == B_2) ? 2 : (key == B_3) ? 3 : 4;
				selected_col = 0;
				tens = -1;
				ones = -1;
				mode = MODE_PICK_TENS;
			}
			break;

		case MODE_PICK_TENS: {
			byte d;
			if (key_to_digit(key, &d) && d <= 2) {
				tens = (sbyte)d;
				ones = -1;
				mode = MODE_PICK_ONES;
			}
			break;
		}

		case MODE_PICK_ONES: {
			byte d;
			if (key_to_digit(key, &d)) {
				ones = (sbyte)d;
				int col = (int)tens * 10 + (int)ones;
				if (col >= 1 && col <= 26) {
					selected_col = (byte)col;
					mode = MODE_PICK_ROW;
					tens = -1;
					ones = -1;
				} else {
					selected_col = 0;
					ones = -1;
					mode = MODE_PICK_TENS;
				}
			}
			break;
		}
		}

		byte want_row = 0;
		byte want_col = 0;
		byte want_col_end = 0;
		byte want_kind = 0;
		if (selected_row && mode == MODE_PICK_TENS && !selected_col) {
			want_row = selected_row;
			want_kind = 1;
		} else if (selected_row && mode == MODE_PICK_ONES && !selected_col) {
			byte start = 0;
			byte end = 0;
			if (tens == 0) {
				start = 1;
				end = 9;
			} else if (tens == 1) {
				start = 10;
				end = 19;
			} else if (tens == 2) {
				start = 20;
				end = 26;
			}
			if (start) {
				want_row = selected_row;
				want_col = start;
				want_col_end = end;
				want_kind = 2;
			}
		} else if (selected_row && selected_col) {
			want_row = selected_row;
			want_col = selected_col;
			want_kind = 0;
		}

		if (hi_row) {
			byte y = (byte)(row_y0 + row_h * hi_row);
			if (hi_kind == 1) {
				tui_invert_area(table_x, y, (byte)(table_cols * char_w), row_h, 0, 0, 0);
			} else if (hi_kind == 2 && hi_col && hi_col_end) {
				byte x = (byte)(table_x + (byte)((hi_col - 1) * char_w));
				byte w = (byte)((hi_col_end - hi_col + 1) * char_w);
				tui_invert_area(x, y, w, row_h, 0, 0, 0);
			} else if (hi_col) {
				byte x = (byte)(table_x + (byte)((hi_col - 1) * char_w));
				tui_invert_area(x, y, char_w, row_h, 0, 0, 0);
			}
		}

		if (want_row) {
			byte y = (byte)(row_y0 + row_h * want_row);
			if (want_kind == 1) {
				tui_invert_area(table_x, y, (byte)(table_cols * char_w), row_h, 0, 0, 0);
			} else if (want_kind == 2 && want_col && want_col_end) {
				byte x = (byte)(table_x + (byte)((want_col - 1) * char_w));
				byte w = (byte)((want_col_end - want_col + 1) * char_w);
				tui_invert_area(x, y, w, row_h, 0, 0, 0);
			} else if (want_col) {
				byte x = (byte)(table_x + (byte)((want_col - 1) * char_w));
				tui_invert_area(x, y, char_w, row_h, 0, 0, 0);
			}
		}

		hi_row = want_row;
		hi_col = want_col;
		hi_col_end = want_col_end;
		hi_kind = want_kind;
		tui_render_buffer();
	}
    return 0;
}



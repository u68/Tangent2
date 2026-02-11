/*
 * input.c
 * Implementation of input utilities + on-screen ASCII keyboard
 *  Created on: Feb 11, 2026
 *      Author: harma
 */

#include "input.h"

static byte input_strlen_n(const char *s, byte max_len) {
	byte i = 0;
	while (i < max_len) {
		if (!s[i]) {
			break;
		}
		i++;
	}
	return i;
}

static byte input_append_char(char *buffer, byte max_len, char c) {
	byte len;
	if (!max_len) {
		return 0;
	}
	len = input_strlen_n(buffer, (byte)(max_len - 1));
	if (len >= (byte)(max_len - 1)) {
		return 0;
	}
	buffer[len] = c;
	buffer[len + 1] = 0;
	return 1;
}

static byte input_backspace(char *buffer, byte max_len) {
	byte len;
	if (!max_len) {
		return 0;
	}
	len = input_strlen_n(buffer, (byte)(max_len - 1));
	if (!len) {
		return 0;
	}
	buffer[len - 1] = 0;
	return 1;
}

static byte input_key_to_digit(byte key, byte *out_digit) {
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
}

// Normal map
static byte input_key_to_char_normal(byte key, char *out_char) {
	switch (key) {
	case B_0: *out_char = '0'; return 1;
	case B_1: *out_char = '1'; return 1;
	case B_2: *out_char = '2'; return 1;
	case B_3: *out_char = '3'; return 1;
	case B_4: *out_char = '4'; return 1;
	case B_5: *out_char = '5'; return 1;
	case B_6: *out_char = '6'; return 1;
	case B_7: *out_char = '7'; return 1;
	case B_8: *out_char = '8'; return 1;
	case B_9: *out_char = '9'; return 1;

	case SP_PLUS: *out_char = '+'; return 1;
	case SP_MINUS: *out_char = '-'; return 1;
	case SP_MUL: *out_char = '*'; return 1;
	case SP_DIV: *out_char = '/'; return 1;
	case SP_DOT: *out_char = '.'; return 1;
	case SP_LEFT_PAREN: *out_char = '('; return 1;
	case SP_RIGHT_PAREN: *out_char = ')'; return 1;
    case SP_X : *out_char = 'X'; return 1;

	default:
		return 0;
	}
}

// Shift map
static byte input_key_to_char_shift(byte key, char *out_char) {
	switch (key) {
	case SC_A: *out_char = 'A'; return 1;
	case SC_B: *out_char = 'B'; return 1;
	case SC_C: *out_char = 'C'; return 1;
	case SC_D: *out_char = 'D'; return 1;
	case SC_E: *out_char = 'E'; return 1;
	case SC_F: *out_char = 'F'; return 1;
	case SC_X: *out_char = 'X'; return 1;
	case SC_Y: *out_char = 'Y'; return 1;
	case SC_Z: *out_char = 'Z'; return 1;
	case SC_EQUALS: *out_char = '='; return 1;
	case SC_COMMA: *out_char = ','; return 1;
	default:
		return 0;
	}
}

// keyboat

#define INPUT_KB_MODE_PICK_ROW  0
#define INPUT_KB_MODE_PICK_TENS 1
#define INPUT_KB_MODE_PICK_ONES 2

static const char input_kb_horizontal[2][28] = {
	"0        1         2      ",
	"12345678901234567890123456"
};

static const char input_kb_vertical[4][2] = {
	"1",
	"2",
	"3",
	"4"
};

static const char input_kb_ascii[4][27] = {
	"abcdefghijklmnopqrstuvwxyz",
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
	"  ~!@#$%^&*()_+{}|:\"<>?   ",
	"  1234567890-=[]\\;',./    "
};

static byte input_keyboard_get_char(char *out_char) {
	Write2RealScreen = 1;

	// 67
	const byte table_x = 22;
	const byte table_cols = 26;
	const byte char_w = 6;
	const byte row_h = 7;
	const byte row_y0 = 16;

	byte mode = INPUT_KB_MODE_PICK_ROW;
	byte selected_row = 0;
	byte selected_col = 0;
	sbyte tens = -1;
	sbyte ones = -1;

	byte last_key = 0xff;

	byte hi_row = 0;
	byte hi_col = 0;
	byte hi_col_end = 0;
	byte hi_kind = 0;

	// Draw once
	tui_clear_screen();
	tui_draw_text(table_x, row_y0 - 10, input_kb_horizontal[0], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x, row_y0 - 3, input_kb_horizontal[1], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x - 9, row_y0 + 7, input_kb_vertical[0], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x - 9, row_y0 + 14, input_kb_vertical[1], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x - 9, row_y0 + 21, input_kb_vertical[2], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x - 9, row_y0 + 28, input_kb_vertical[3], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);

	tui_draw_text(table_x, row_y0 + 7, input_kb_ascii[0], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x, row_y0 + 14, input_kb_ascii[1], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x, row_y0 + 21, input_kb_ascii[2], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(table_x, row_y0 + 28, input_kb_ascii[3], TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);

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

		if (key == SP_BACK) {
			Write2RealScreen = 0;
			tui_render_buffer();
			return 0;
		}

		// Accept selection
		if ((key == SP_OKAY || key == SP_EXE) && selected_row && selected_col) {
			*out_char = input_kb_ascii[selected_row - 1][selected_col - 1];
			Write2RealScreen = 0;
			tui_render_buffer();
			return 1;
		}

		// Cursor navigation
		if (key == SP_UP || key == SP_DOWN || key == SP_LEFT || key == SP_RIGHT) {
			if (!selected_row) {
				selected_row = 1;
			}
			if (!selected_col) {
				selected_col = 1;
			}
			mode = INPUT_KB_MODE_PICK_ROW;
			tens = -1;
			ones = -1;

			switch (key) {
			case SP_LEFT:
				if (selected_col > 1) selected_col--;
				break;
			case SP_RIGHT:
				if (selected_col < table_cols) selected_col++;
				break;
			case SP_UP:
				if (selected_row > 1) selected_row--;
				break;
			case SP_DOWN:
				if (selected_row < 4) selected_row++;
				break;
			default:
				break;
			}
		}

		// Numeric selection
		switch (mode) {
		case INPUT_KB_MODE_PICK_ROW:
			if (key == B_1 || key == B_2 || key == B_3 || key == B_4) {
				selected_row = (key == B_1) ? 1 : (key == B_2) ? 2 : (key == B_3) ? 3 : 4;
				selected_col = 0;
				tens = -1;
				ones = -1;
				mode = INPUT_KB_MODE_PICK_TENS;
			}
			break;

		case INPUT_KB_MODE_PICK_TENS: {
			byte d;
			if (input_key_to_digit(key, &d) && d <= 2) {
				tens = (sbyte)d;
				ones = -1;
				mode = INPUT_KB_MODE_PICK_ONES;
			}
			break;
		}

		case INPUT_KB_MODE_PICK_ONES: {
			byte d;
			if (input_key_to_digit(key, &d)) {
				ones = (sbyte)d;
				sword col = (sword)((sword)tens * 10 + (sword)ones);
				if (col >= 1 && col <= 26) {
					selected_col = (byte)col;
					mode = INPUT_KB_MODE_PICK_ROW;
					tens = -1;
					ones = -1;
				} else {
					selected_col = 0;
					ones = -1;
					mode = INPUT_KB_MODE_PICK_TENS;
				}
			}
			break;
		}
		}

		// Gaslight
		byte want_row = 0;
		byte want_col = 0;
		byte want_col_end = 0;
		byte want_kind = 0;

		if (selected_row && mode == INPUT_KB_MODE_PICK_TENS && !selected_col) {
			want_row = selected_row;
			want_kind = 1;
		} else if (selected_row && mode == INPUT_KB_MODE_PICK_ONES && !selected_col) {
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

		// Introvert previous highlight
		if (hi_row) {
			byte y = row_y0 + row_h * hi_row;
			if (hi_kind == 1) {
				tui_invert_area(table_x, y, (byte)(table_cols * char_w), row_h, 0, 0, 0);
			} else if (hi_kind == 2 && hi_col && hi_col_end) {
				byte x = table_x + (hi_col - 1) * char_w;
				byte w = (hi_col_end - hi_col + 1) * char_w;
				tui_invert_area(x, y, w, row_h, 0, 0, 0);
			} else if (hi_col) {
				byte x = table_x + (hi_col - 1) * char_w;
				tui_invert_area(x, y, char_w, row_h, 0, 0, 0);
			}
		}

		// Invert new highlight
		if (want_row) {
			byte y = row_y0 + row_h * want_row;
			if (want_kind == 1) {
				tui_invert_area(table_x, y, (byte)(table_cols * char_w), row_h, 0, 0, 0);
			} else if (want_kind == 2 && want_col && want_col_end) {
				byte x = table_x + (want_col - 1) * char_w;
				byte w = (want_col_end - want_col + 1) * char_w;
				tui_invert_area(x, y, w, row_h, 0, 0, 0);
			} else if (want_col) {
				byte x = table_x + (want_col - 1) * char_w;
				tui_invert_area(x, y, char_w, row_h, 0, 0, 0);
			}
		}

		hi_row = want_row;
		hi_col = want_col;
		hi_col_end = want_col_end;
		hi_kind = want_kind;
	}
}

byte input_text(char *buffer, byte max_len) {
	static byte last_key = 0xff;
	static byte shift_armed = 0;

	byte key = CheckButtons();
	if (key == 0xff) {
		last_key = 0xff;
		return 0;
	}
	if (key == last_key) {
		return 0;
	}
	last_key = key;

	// Shift is smth
	if (key == SP_SHIFT) {
		shift_armed = 1;
		return 0;
	}

	// Open keyboard
	if (key == SP_CATALOG) {
		char c;
		if (input_keyboard_get_char(&c)) {
			return input_append_char(buffer, max_len, c);
		}
		return 0;
	}

	// Editing
	if (key == SP_DEL) {
		shift_armed = 0;
		return input_backspace(buffer, max_len);
	}

	// Character typing
	{
		char out;
		byte ok = 0;

		if (shift_armed) {
			ok = input_key_to_char_shift(key, &out);
			shift_armed = 0;
		} else {
			ok = input_key_to_char_normal(key, &out);
		}

		if (ok) {
			return input_append_char(buffer, max_len, out);
		}
	}
	shift_armed = 0;
	return 0;
}

/*
 * desktop.c
 * Implementation of desktop environment
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#include "desktop.h"
#include "Tui/glib.h"
#include "heap.h"

#define ID_TASKBAR 1
#define ID_WINDOW 2
#define ID_WINDOW_TITLE 3
#define ID_WINDOW_CLOSE_BTN 4
#define ID_WINDOW_CONTENT 5
#define ID_ICON1 6
#define ID_ICON1_TEXT 7
#define ID_ICON2 8
#define ID_ICON2_TEXT 9
#define ID_TIME 10

#define WORD(w) ((w) & 0xFF), (((w) >> 8) & 0xFF)

static const byte desktop_ui_data[] = {
	'<', TML_TYPE_DIV,
		FIELD_ID, WORD(ID_TASKBAR),
		FIELD_X, 0,
		FIELD_Y, 55,
		FIELD_WIDTH, 192,
		FIELD_HEIGHT, 8,
		FIELD_COLOUR, TUI_COLOUR_WHITE,
		FIELD_THICKNESS, 0,
	'>',

	'<', TML_TYPE_DIV,
		FIELD_ID, WORD(ID_WINDOW),
		FIELD_X, 40,
		FIELD_Y, 15,
		FIELD_WIDTH, 112,
		FIELD_HEIGHT, 35,
		FIELD_COLOUR, TUI_COLOUR_BLACK,
		FIELD_THICKNESS, 1,
		FIELD_STYLE, TUI_LINE_STYLE_SOLID,
		
		'<', TML_TYPE_LINE,
			FIELD_ID, WORD(ID_WINDOW_TITLE),
			FIELD_X, 1,
			FIELD_Y, 9,
			FIELD_END_X, 111,
			FIELD_END_Y, 9,
			FIELD_COLOUR, TUI_COLOUR_DARK_GREY,
			FIELD_THICKNESS, 1,
		'>',
		
		'<', TML_TYPE_TEXT,
			FIELD_ID, WORD(ID_WINDOW_CLOSE_BTN),
			FIELD_X, 106,
			FIELD_Y, 2,
			FIELD_COLOUR, TUI_COLOUR_BLACK,
			FIELD_FONT, TUI_FONT_SIZE_6x7,
			FIELD_TEXT, 'X', 0,
		'>',
		
		'<', TML_TYPE_DIV,
			FIELD_ID, WORD(ID_WINDOW_CONTENT),
			FIELD_X, 1,
			FIELD_Y, 10,
			FIELD_WIDTH, 110,
			FIELD_HEIGHT, 24,
			FIELD_COLOUR, TUI_COLOUR_LIGHT_GREY,
			FIELD_THICKNESS, 1,
		'>',
	'>',

	'<', TML_TYPE_DIV,
		FIELD_ID, WORD(ID_ICON1),
		FIELD_X, 10,
		FIELD_Y, 10,
		FIELD_WIDTH, 16,
		FIELD_HEIGHT, 16,
		FIELD_COLOUR, TUI_COLOUR_DARK_GREY,
		FIELD_THICKNESS, 1,
	'>',

	'<', TML_TYPE_TEXT,
		FIELD_ID, WORD(ID_ICON1_TEXT),
		FIELD_X, 10,
		FIELD_Y, 28,
		FIELD_COLOUR, TUI_COLOUR_BLACK,
		FIELD_FONT, TUI_FONT_SIZE_6x8,
		FIELD_TEXT, 'F', 'i', 'l', 'e', 's', 0,
	'>',

	'<', TML_TYPE_DIV,
		FIELD_ID, WORD(ID_ICON2),
		FIELD_X, 10,
		FIELD_Y, 35,
		FIELD_WIDTH, 16,
		FIELD_HEIGHT, 16,
		FIELD_COLOUR, TUI_COLOUR_DARK_GREY,
		FIELD_THICKNESS, 1,
	'>',

	'<', TML_TYPE_TEXT,
		FIELD_ID, WORD(ID_ICON2_TEXT),
		FIELD_X, 10,
		FIELD_Y, 53,
		FIELD_COLOUR, TUI_COLOUR_BLACK,
		FIELD_FONT, TUI_FONT_SIZE_6x8,
		FIELD_TEXT, 'T', 'e', 'r', 'm', 0,
	'>',

	'<', TML_TYPE_TEXT,
		FIELD_ID, WORD(ID_TIME),
		FIELD_X, 170,
		FIELD_Y, 56,
		FIELD_COLOUR, TUI_COLOUR_BLACK,
		FIELD_FONT, TUI_FONT_SIZE_6x7,
		FIELD_TEXT, '1', '2', ':', '0', '0', 0,
	'>',

	0
};

TmlElement* desktop_init(void) {
	const byte max_elems = 10;
	TmlElement* elements = (TmlElement*)hcalloc(max_elems, sizeof(TmlElement));
	return tml_parse(desktop_ui_data, elements, max_elems);
}

void desktop_render(TmlElement* root) {
	tml_render(root);
}

TmlElement* desktop_get_window(TmlElement* root) {
	return tml_find_by_id(root, ID_WINDOW);
}

TmlElement* desktop_get_time_text(TmlElement* root) {
	return tml_find_by_id(root, ID_TIME);
}

/*
 * main.c
 * Implementation of Tangent2 main operation
 * (line transform demo using heap)
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#include "Tui/glib.h"
#include "heap.h"

#define ID_LINE1 1
#define ID_LINE2 2
#define ID_LINE3 3
#define ID_LINE4 4

#define WORD(w) ((w) & 0xFF), (((w) >> 8) & 0xFF)

static const byte ui_data[] = {
	'<', TML_TYPE_LINE,
		FIELD_ID, WORD(ID_LINE1),
		FIELD_X, 20,
		FIELD_Y, 20,
		FIELD_END_X, 15,
		FIELD_END_Y, 0,
		FIELD_COLOUR, TUI_COLOUR_BLACK,
		FIELD_THICKNESS, 1,
		
		'<', TML_TYPE_LINE,
			FIELD_ID, WORD(ID_LINE2),
			FIELD_X, 15,
			FIELD_Y, 0,
			FIELD_END_X, 15,
			FIELD_END_Y, 0,
			FIELD_COLOUR, TUI_COLOUR_BLACK,
			FIELD_THICKNESS, 1,
			
			'<', TML_TYPE_LINE,
				FIELD_ID, WORD(ID_LINE3),
				FIELD_X, 15,
				FIELD_Y, 0,
				FIELD_END_X, 15,
				FIELD_END_Y, 0,
				FIELD_COLOUR, TUI_COLOUR_BLACK,
				FIELD_THICKNESS, 1,
				
				'<', TML_TYPE_LINE,
					FIELD_ID, WORD(ID_LINE4),
					FIELD_X, 15,
					FIELD_Y, 0,
					FIELD_END_X, 15,
					FIELD_END_Y, 0,
					FIELD_COLOUR, TUI_COLOUR_BLACK,
					FIELD_THICKNESS, 1,
				'>',
			'>',
		'>',
	'>',
	
	0
};

void custom_break() {

}

int main(void) {
	Write2RealScreen = 1; // Speeddd
	tui_clear_screen();

	// Initialize heap
	hinit();

	// Allocate elements array on heap to avoid stack usage
	const byte max_elems = 4;
	TmlElement* elements = (TmlElement*)hcalloc(max_elems, sizeof(TmlElement));

	// Parse/compile UI
	TmlElement* root = tml_parse(ui_data, elements, max_elems);

	// Main loop: increment rotation for every element then rende
	word rot = 0;
	while (1) {
		tui_clear_screen();
		rot = (rot + 1) % 360;
		TmlElement* e;
		e = tml_find_by_id(root, ID_LINE1); if (e) tml_set_rotation(e, rot);
		e = tml_find_by_id(root, ID_LINE2); if (e) tml_set_rotation(e, rot);
		e = tml_find_by_id(root, ID_LINE3); if (e) tml_set_rotation(e, rot);
		e = tml_find_by_id(root, ID_LINE4); if (e) tml_set_rotation(e, rot);

		tml_render(root);
		//tui_render_buffer(); (if write2realscreen is 0)
	}
	return 0;
}

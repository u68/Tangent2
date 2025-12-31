/*
 * glib.h
 *
 *  Created on: Dec 2, 2025
 *      Author: harma
 */

#ifndef GLIB_H_
#define GLIB_H_

#include "base.h"
#include "graphics.h"

enum element {
	TML_TEXT,
	TML_BUTTON,
	TML_DIV,
	TML_INPUT,
	TML_CHECKBOX,
	TML_RADIO,
	TML_LINE,
};

enum align {
	ALIGN_LEFT,
	ALIGN_TOP_LEFT = ALIGN_LEFT,
	ALIGN_CENTER_LEFT,
	ALIGN_BOTTOM_LEFT,
	ALIGN_RIGHT,
	ALIGN_TOP_RIGHT = ALIGN_RIGHT,
	ALIGN_CENTER_RIGHT,
	ALIGN_BOTTOM_RIGHT,
	ALIGN_CENTER,
	ALIGN_TOP_CENTER = ALIGN_CENTER,
	ALIGN_BOTTOM_CENTER,
	ALIGN_MIDDLE_CENTER,
};

enum element_field {
	E_FIELD_ID,
	E_FIELD_X,
	E_FIELD_X0 = E_FIELD_X,
	E_FIELD_Y,
	E_FIELD_Y0 = E_FIELD_Y,
	E_FIELD_WIDTH,
	E_FIELD_X1 = E_FIELD_WIDTH,
	E_FIELD_HEIGHT,
	E_FIELD_Y1 = E_FIELD_HEIGHT,
	E_FIELD_ANCHOR_X,
	E_FIELD_ANCHOR_Y,
	E_FIELD_COLOUR,
	E_FIELD_BORDER_COLOUR = E_FIELD_COLOUR,
	E_FIELD_STYLE,
	E_FIELD_BORDER_STYLE = E_FIELD_STYLE,
	E_FIELD_ALIGN,
	E_FIELD_TEXT_ALIGN,
	E_FIELD_BORDER_THICKNESS,
	E_FIELD_TEXT_COLOUR,
	E_FIELD_FONT_SIZE,
	E_FIELD_VALUE,
	E_FIELD_CHECKED = E_FIELD_VALUE,
	E_FIELD_ROTATION,
	E_FIELD_TEXT,
	MAX_E_FIELD
};

const byte __get_element_field(const byte* idxadr, byte field);
//const byte* __get_element_image(const byte* idxadr, byte *not_found); Future me problem
const char* __get_element_text(const byte* idxadr, byte *not_found);
void parse_tml(const byte* data, word length);
void splash(const byte* data, word duration);
const byte* get_element(word id);
byte get_element_field(word id, byte field);
void set_element(word id, byte field, byte* data);

#endif /* GLIB_H_ */

/*
 * glib.h
 * Interface for TML parsing and rendering
 *  Created on: Dec 2, 2025
 *      Author: harma
 */

#ifndef GLIB_H_
#define GLIB_H_

#include "../base.h"
#include "graphics.h"

#define TML_ROOT_ELEMENT_ADDR *((volatile __near word *)0x9C02)

enum element {
	TML_ROOT,
	TML_TEXT,
	TML_BUTTON,
	TML_DIV,
	TML_INPUT,
	TML_CHECKBOX,
	TML_RADIO,
	TML_LINE,
};

enum tml_align {
	TML_ALIGN_LEFT,
	TML_ALIGN_TOP_LEFT = TML_ALIGN_LEFT,
	TML_ALIGN_CENTER_LEFT,
	TML_ALIGN_BOTTOM_LEFT,
	TML_ALIGN_RIGHT,
	TML_ALIGN_TOP_RIGHT = TML_ALIGN_RIGHT,
	TML_ALIGN_CENTER_RIGHT,
	TML_ALIGN_BOTTOM_RIGHT,
	TML_ALIGN_CENTER,
	TML_ALIGN_TOP_CENTER = TML_ALIGN_CENTER,
	TML_ALIGN_BOTTOM_CENTER,
	TML_ALIGN_MIDDLE_CENTER,
};

enum tml_element_field {
	TML_E_FIELD_ID,
	TML_E_FIELD_PARENT_ID,
	TML_E_FIELD_FIELD_SIZE,
	TML_E_FIELD_X,
	TML_E_FIELD_X0 = TML_E_FIELD_X,
	TML_E_FIELD_Y,
	TML_E_FIELD_Y0 = TML_E_FIELD_Y,
	TML_E_FIELD_WIDTH,
	TML_E_FIELD_X1 = TML_E_FIELD_WIDTH,
	TML_E_FIELD_HEIGHT,
	TML_E_FIELD_Y1 = TML_E_FIELD_HEIGHT,
	TML_E_FIELD_ANCHOR_X,
	TML_E_FIELD_ANCHOR_Y,
	TML_E_FIELD_COLOUR,
	TML_E_FIELD_BORDER_COLOUR = TML_E_FIELD_COLOUR,
	TML_E_FIELD_STYLE,
	TML_E_FIELD_BORDER_STYLE = TML_E_FIELD_STYLE,
	TML_E_FIELD_ALIGN,
	TML_E_FIELD_TEXT_ALIGN,
	TML_E_FIELD_BORDER_THICKNESS,
	TML_E_FIELD_TEXT_COLOUR,
	TML_E_FIELD_FONT_SIZE,
	TML_E_FIELD_VALUE,
	TML_E_FIELD_CHECKED = TML_E_FIELD_VALUE,
	TML_E_FIELD_ROTATION,
	TML_E_FIELD_TEXT,
	TML_MAX_E_FIELD
};

const byte __tml_get_element_field(const byte* idxadr, byte field);
const byte* __tml_get_element_field_addr(const byte* idxadr, byte field);
//const byte* __tml_get_element_image(const byte* idxadr, byte *not_found); Future me problem
const char* __tml_get_element_text(const byte* idxadr, byte *not_found);
const byte* __tml_get_root_element();
void __tml_get_offsets(word id, byte* x, byte* y, word* rotation);
void tml_parse(const byte* data);
void tml_render_element(const byte* data);
void tml_splash(const byte* data, word duration);
const byte* tml_get_element(const byte* tml_data, word id);
byte tml_get_element_field(const byte* tml_data, word id, byte field);
const char* tml_get_element_text(const byte* tml_data, word id, byte *not_found);
void tml_set_element_field(const byte* tml_data, word id, byte field, byte data);
void tml_set_element_text(const byte* tml_data, word id, const char* text);

#endif /* GLIB_H_ */

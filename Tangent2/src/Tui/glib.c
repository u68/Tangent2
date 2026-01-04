/*
 * glib.c
 * Implementation of TML parsing and rendering
 *  Created on: Dec 2, 2025
 *      Author: harma
 */

#include "glib.h"
// Search for specific field in TML element
const byte __tml_get_element_field(const byte* idxadr, byte field) {
	byte i;
	// Start from 6 to skip ID and PARENT fields  I
	for (i = 6; i < (TML_MAX_E_FIELD << 1); i+= 2){
		if (idxadr[i] == field) {
			//derefw(0xA050) = (word)&idxadr[i + 1];
			return idxadr[i + 1];
		} else if (idxadr[i] == TML_E_FIELD_TEXT || idxadr[i] == 0x3C) {
			break;
		}
	}
	return 0;
}

// Search for specific field in TML element but return address
const byte* __tml_get_element_field_addr(const byte* idxadr, byte field) {
	byte i;
	// Start from 6 to skip ID and PARENT fields
	for (i = 6; i < (TML_MAX_E_FIELD << 1); i+= 2){
		if (idxadr[i] == field) {
			return &idxadr[i + 1];
		} else if (idxadr[i] == TML_E_FIELD_TEXT || idxadr[i] == 0x3C) {
			break;
		}
	}
	return 0;
}

// Search for text field in TML element
const char* __tml_get_element_text(const byte* idxadr, byte *not_found) {
	byte i;
	// Start from 6 to skip ID and PARENT fields
	for (i = 6; i < (TML_MAX_E_FIELD << 1); i+= 2){
		if (idxadr[i] == TML_E_FIELD_TEXT) {
			return (const char*)&idxadr[i+1];
		}
	}
	*not_found = 1;
	return 0;
}

// Return root element
const byte* __tml_get_root_element() {
	//word parent_id = (idxadr[4] << 8) | idxadr[5];
	return (const byte*)TML_ROOT_ELEMENT_ADDR;// + 2; // Skip '<' and element type;
}

// Get offsets for element (Fowards search from root)
void __tml_get_offsets(word id, byte* x, byte* y, word* rotation) {
	const byte* root = (const byte*)TML_ROOT_ELEMENT_ADDR;
	word i = 0;
	word pos_history[10] = {0,0,0,0,0,0,0,0,0,0};
	word rotation_history[10] = {0,0,0,0,0,0,0,0,0,0};
	pos_history[0] = 0x0000;
	// Store current offset position
	word toprotation = 0;
	byte topx = 0, topy = 0;
	byte history_index = 0;
	byte rtopx = 0, rtopy = 0;
	for (i = 0; ;) {
		if (root[i] == '<') {
			// Adjust offset position
			if (root[i+3] == (byte)(id >> 8)) {
				if (root[i+4] == (byte)(id & 0xff)) {
					*x = topx;
					*y = topy;
					*rotation = toprotation;
					return;
				}
			}
			i+=8;
			toprotation += __tml_get_element_field(&root[i-6], TML_E_FIELD_ROTATION);
			rtopx += __tml_get_element_field(&root[i-6], TML_E_FIELD_X);
			rtopy += __tml_get_element_field(&root[i-6], TML_E_FIELD_Y);
			//tui_rotate_point(topx, topy, );
			pos_history[history_index] = ((word)topx << 8) | topy;
			rotation_history[history_index] = toprotation;
			history_index++;
			i+=__tml_get_element_field(&root[i-6], TML_E_FIELD_FIELD_SIZE);
			// If element is found, return offsets
		} else if (root[i] == '>') {
			history_index--;
			if (history_index == 0) {
				return;
			}
			topx = (pos_history[history_index] >> 8) & 0xff;
			topy = pos_history[history_index] & 0xff;
			toprotation = rotation_history[history_index];
		}
	}
}
// Function to parse TML data and render elements, (root)
// It can also be used to render any element
void tml_parse(const byte* data) {
	TML_ROOT_ELEMENT_ADDR = (word)&data;
	tml_render_element(data);
}

void tml_render_element(const byte* data) {
	// History stacks
	word pos_history[10] = {0,0,0,0,0,0,0,0,0,0};
	word rotation_history[10] = {0,0,0,0,0,0,0,0,0,0};
	pos_history[0] = 0x0000;
	// Store current offset position
	word rotation = 0;
	byte topx = 0, topy = 0;
	if (data[1] != TML_ROOT) {
		__tml_get_offsets(((word)data[3] << 8) | data[4], &topx, &topy, &rotation);
	}
	byte history_index = 0;
	word i = 0;
	for (i = 0; ;) {
		// If element is found
		if (data[i] == '<') {
			pos_history[history_index] = ((word)topx << 8) | topy;
			rotation_history[history_index] = rotation;
			history_index++;
			// < type id x y parent x y
			// 0 1    2  3 4 5      6 7 8
			byte element_type = data[i+1];
			i+=8;
			topx += __tml_get_element_field(&data[i-6], TML_E_FIELD_X);
			topy += __tml_get_element_field(&data[i-6], TML_E_FIELD_Y);
			rotation += __tml_get_element_field(&data[i-6], TML_E_FIELD_ROTATION);

			byte ax, ay, font_size, rotation, colour, width, height, border_thickness, border_style, not_found, text_align, align;
			const char* text;
			const byte* image;
			const byte* tml_element = &data[i-6];
			// Get common fields
			ax = __tml_get_element_field(tml_element, TML_E_FIELD_ANCHOR_X);
			ay = __tml_get_element_field(tml_element, TML_E_FIELD_ANCHOR_Y);
			//rotation = __tml_get_element_field(tml_element, TML_E_FIELD_ROTATION);
			colour = __tml_get_element_field(tml_element, TML_E_FIELD_COLOUR);
			// Render element based on type
			switch(element_type) {
			case TML_TEXT:
				// Draws text element
				font_size = __tml_get_element_field(tml_element, TML_E_FIELD_FONT_SIZE);
				not_found = 0;
				text = __tml_get_element_text(tml_element, &not_found);
				if (not_found) break;
				tui_draw_text(topx, topy, text, font_size, ax, ay, rotation, colour);
				break;
			case TML_BUTTON:
				// Draws button element with text alignment
				width = __tml_get_element_field(tml_element, TML_E_FIELD_WIDTH);
				height = __tml_get_element_field(tml_element, TML_E_FIELD_HEIGHT);
				font_size = __tml_get_element_field(tml_element, TML_E_FIELD_FONT_SIZE);
				border_thickness = __tml_get_element_field(tml_element, TML_E_FIELD_BORDER_THICKNESS);
				border_style = __tml_get_element_field(tml_element, TML_E_FIELD_BORDER_STYLE);
				not_found = 0;
				text = __tml_get_element_text(tml_element, &not_found);
				text_align = __tml_get_element_field(tml_element, TML_E_FIELD_TEXT_ALIGN);
				byte text_width, text_height;
				byte otopx = topx;
				byte otopy = topy;
				byte topxr;
				byte topyr;
				tui_get_text_size(font_size, text, &text_width, &text_height);
				// Do text alignment magic
				switch (text_align) {
				case TML_ALIGN_CENTER_LEFT:
					topy += (height >> 1) - (text_height >> 1);
					break;
				case TML_ALIGN_BOTTOM_LEFT:
					topy += height - text_height;
					break;
				case TML_ALIGN_RIGHT:
					topx += width - text_width;
					topy += (height >> 1) - (text_height >> 1);
					break;
				case TML_ALIGN_CENTER_RIGHT:
					topx += width - text_width;
					topy += (height >> 1) - (text_height >> 1);
					break;
				case TML_ALIGN_BOTTOM_RIGHT:
					topx += width - text_width;
					topy += height - text_height;
					break;
				case TML_ALIGN_CENTER:
					topx += (width >> 1) - (text_width >> 1);
					break;
				case TML_ALIGN_BOTTOM_CENTER:
					topx += (width >> 1) - (text_width >> 1);
					topy += height - text_height;
					break;
				case TML_ALIGN_MIDDLE_CENTER:
					topx += (width >> 1) - (text_width >> 1);
					topy += (height >> 1) - (text_height >> 1);
					break;
				}
				tui_rotate_point(otopx, otopy, topx - ax + 2, topy - ay + 2, rotation, &topxr, &topyr);
				if (!not_found) tui_draw_text(topxr, topyr, text, font_size, ax, ay, rotation, colour);
				topx = otopx;
				topy = otopy;
				tui_draw_rectangle(topx, topy, width, height, ax, ay, rotation, colour, border_thickness, border_style);
				break;
			case TML_DIV:
				// Draws a rectangle, with element alignment
				width = __tml_get_element_field(tml_element, TML_E_FIELD_WIDTH);
				height = __tml_get_element_field(tml_element, TML_E_FIELD_HEIGHT);
				border_thickness = __tml_get_element_field(tml_element, TML_E_FIELD_BORDER_THICKNESS);
				border_style = __tml_get_element_field(tml_element, TML_E_FIELD_BORDER_STYLE);
				align = __tml_get_element_field(tml_element, TML_E_FIELD_ALIGN);
				tui_draw_rectangle(topx, topy, width, height, ax, ay, rotation, colour, border_thickness, border_style);
				// Do alignment magic
				switch (text_align) {
				case TML_ALIGN_CENTER_LEFT:
					topy += (height >> 1);
					break;
				case TML_ALIGN_BOTTOM_LEFT:
					topy += height;
					break;
				case TML_ALIGN_RIGHT:
					topx += width;
					topy += (height >> 1);
					break;
				case TML_ALIGN_CENTER_RIGHT:
					topx += width;
					topy += (height >> 1);
					break;
				case TML_ALIGN_BOTTOM_RIGHT:
					topx += width;
					topy += height;
					break;
				case TML_ALIGN_CENTER:
					topx += (width >> 1);
					break;
				case TML_ALIGN_BOTTOM_CENTER:
					topx += (width >> 1);
					topy += height;
					break;
				case TML_ALIGN_MIDDLE_CENTER:
					topx += (width >> 1);
					topy += (height >> 1);
					break;
				}
				break;
			case TML_RADIO:
				// Draws a circle for now
				width = __tml_get_element_field(tml_element, TML_E_FIELD_WIDTH);
				border_thickness = __tml_get_element_field(tml_element, TML_E_FIELD_BORDER_THICKNESS);
				tui_draw_circle(topx, topy, width >> 1, ax, ay, border_thickness, colour);
				break;
			case TML_CHECKBOX:
				// Draws a square for now
				width = __tml_get_element_field(tml_element, TML_E_FIELD_WIDTH);
				border_thickness = __tml_get_element_field(tml_element, TML_E_FIELD_BORDER_THICKNESS);
				border_style = __tml_get_element_field(tml_element, TML_E_FIELD_BORDER_STYLE);
				tui_draw_rectangle(topx, topy, width, width, ax, ay, rotation, colour, border_thickness, border_style);
				break;
			case TML_LINE:
				// Draws a line element
				width = __tml_get_element_field(tml_element, TML_E_FIELD_X1);
				height = __tml_get_element_field(tml_element, TML_E_FIELD_Y1);
				border_thickness = __tml_get_element_field(tml_element, TML_E_FIELD_BORDER_THICKNESS);
				border_style = __tml_get_element_field(tml_element, TML_E_FIELD_BORDER_STYLE);
				tui_draw_line(topx - ax, topy - ay, topx + width - ax, topy + height - ay, colour, border_thickness, border_style);
				break;
			}
			i+= __tml_get_element_field(tml_element, TML_E_FIELD_FIELD_SIZE);
		} else if (data[i] == '>') {

			// Restore previous offset position when end of element is found
			history_index--;
			if (history_index == 0) {
				return;
			}
			topx = (pos_history[history_index] >> 8) & 0xff;
			topy = pos_history[history_index] & 0xff;
			rotation = rotation_history[history_index];
		}
	}
}

void delay2(ushort after_ticks)
{
    if ((FCON & 2) != 0)
        FCON &= 0xfd;
    __DI();
    Timer0Interval = after_ticks;
    Timer0Counter = 0;
    Timer0Control = 0x0101;
    InterruptPending_W0 = 0;
    StopAcceptor = 0x50;
    StopAcceptor = 0xa0;
    StopControl = 2;
    __asm("nop");
    __asm("nop");
    __EI();
}

// Display a splash screen for a set duration
void tml_splash(const byte* data, word duration) {
	// NOTE: Make a dedicated splash screen function, draw image slow asf
	byte old = Write2RealScreen;
	Write2RealScreen = 1;
	tui_draw_image(0, 1, 192, 63, data, 0, 0, 0, TUI_COLOUR_IMAGE);
	delay2(duration << 3);
	tui_draw_image(0, 1, 192, 63, data, 0, 0, 0, TUI_COLOUR_LIGHT_GREY);
	delay2(4000);
	Write2RealScreen = old;
}

// Similar to __tml_get_element_field but uses tml_get_element to get element data by ID
byte tml_get_element_field(const byte* tml_data, word id, byte field) {
	return __tml_get_element_field(tml_get_element(tml_data, id), field);
	return 0;
}

// Returns pointer to element text
const char* tml_get_element_text(const byte* tml_data, word id, byte *not_found) {
	return __tml_get_element_text(tml_get_element(tml_data, id), not_found);
	return 0;
}

// Query element by ID
const byte* tml_get_element(const byte* tml_data, word id) {
	word i;
	word e_counter = 0;
	for (i = 0; ; i++) {
		if (tml_data[i] == '<') {
			e_counter++;
			//derefw(0xA050) = (word)&tml_data[i+3];
			if (tml_data[i + 3] == (byte)(id >> 8)) {
				if (tml_data[i + 4] == (byte)(id & 0xff)) {
					return &tml_data[i];
				}
			}
		} else if (tml_data[i] == '>') {
			e_counter--;
			if (e_counter == 0) {
				break;
			}
		}
	}
	return 0;
}

// Set element field data by ID
void tml_set_element_field(const byte* tml_data, word id, byte field, byte data) {
	//derefw(0xA000) = (word)__tml_get_element_field_addr(tml_get_element(tml_data, id), field);
	deref((word)__tml_get_element_field_addr(tml_get_element(tml_data, id), field)) = data;
}

// Set text field of element by ID
void tml_set_element_text(const byte* tml_data, word id, const char* text) {
	byte not_found = 0;
	char* ntext = (char*)__tml_get_element_text(tml_get_element(tml_data, id), &not_found);
	if (not_found) {
		return;
	}
	byte i = 0;
	while (text[i] != 0 && ntext[i] != 0) {
		ntext[i] = text[i];
		i++;
		i+=1;
	}
}

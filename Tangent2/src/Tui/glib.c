/*
 * glib.c
 *
 *  Created on: Dec 2, 2025
 *      Author: harma
 */

#include "glib.h"

// Search for specific field in TML element
const byte __get_element_field(const byte* idxadr, byte field) {
	byte i;
	for (i = 0; i < (MAX_E_FIELD << 1); i+= 2){
		if (idxadr[i] == E_FIELD_ID) {
			i++;
			continue;
		} else if (idxadr[i] == field) {
			//derefw(0xA050) = (word)&idxadr[i + 1];
			return idxadr[i + 1];
		} else if (idxadr[i] == E_FIELD_TEXT) {
			break;
		}
	}
	return 0;
}

// Search for text field in TML element
const char* __get_element_text(const byte* idxadr, byte *not_found) {
	byte i;
	for (i = 0; i < (MAX_E_FIELD << 1); i+= 2){
		if (idxadr[i] == E_FIELD_ID) {
			i++;
			continue;
		} else if (idxadr[i] == E_FIELD_TEXT) {
			return (const char*)&idxadr[i+1];
		}
	}
	*not_found = 1;
	return 0;
}

// Function to parse TML data and render elements
void parse_tml(const byte* data, word length) {
	// Position history for nested elements
	word pos_history[10] = {0,0,0,0,0,0,0,0,0,0};
	pos_history[0] = 0x0000;
	// Store current offset position
	byte topx = 0, topy = 0;
	byte history_index = 0;
	word i = 0;
	for (i = 0; i < length; i++) {
		// If element is found
		if (data[i] == '<') {
			history_index++;
			// Adjust offset position
			topx += __get_element_field(&data[i], E_FIELD_X);
			topy += __get_element_field(&data[i], E_FIELD_Y);
			pos_history[history_index] = (topx << 8) | topy;

			i++;
			byte ax, ay, font_size, rotation, colour, width, height, border_thickness, border_style, not_found, text_align, align;
			const char* text;
			const byte* image;
			const byte* tml_element = &data[i + 1];
			// Get common fields
			ax = __get_element_field(tml_element, E_FIELD_ANCHOR_X);
			ay = __get_element_field(tml_element, E_FIELD_ANCHOR_Y);
			rotation = __get_element_field(tml_element, E_FIELD_ROTATION);
			colour = __get_element_field(tml_element, E_FIELD_COLOUR);
			// Render element based on type
			switch(data[i]) {
			case TML_TEXT:
				// Draws text element
				font_size = __get_element_field(tml_element, E_FIELD_FONT_SIZE);
				not_found = 0;
				text = __get_element_text(tml_element, &not_found);
				if (not_found) break;
				draw_text(topx, topy, text, font_size, ax, ay, rotation, colour);
				break;
			case TML_BUTTON:
				// Draws button element with text alignment
				width = __get_element_field(tml_element, E_FIELD_WIDTH);
				height = __get_element_field(tml_element, E_FIELD_HEIGHT);
				font_size = __get_element_field(tml_element, E_FIELD_FONT_SIZE);
				border_thickness = __get_element_field(tml_element, E_FIELD_BORDER_THICKNESS);
				border_style = __get_element_field(tml_element, E_FIELD_BORDER_STYLE);
				not_found = 0;
				text = __get_element_text(tml_element, &not_found);
				text_align = __get_element_field(tml_element, E_FIELD_TEXT_ALIGN);
				byte text_width, text_height;
				byte otopx = topx;
				byte otopy = topy;
				byte topxr;
				byte topyr;
				get_text_size(font_size, text, &text_width, &text_height);
				// Do text alignment magic
				switch (text_align) {
				case ALIGN_CENTER_LEFT:
					topy += (height >> 1) - (text_height >> 1);
					break;
				case ALIGN_BOTTOM_LEFT:
					topy += height - text_height;
					break;
				case ALIGN_RIGHT:
					topx += width - text_width;
					topy += (height >> 1) - (text_height >> 1);
					break;
				case ALIGN_CENTER_RIGHT:
					topx += width - text_width;
					topy += (height >> 1) - (text_height >> 1);
					break;
				case ALIGN_BOTTOM_RIGHT:
					topx += width - text_width;
					topy += height - text_height;
					break;
				case ALIGN_CENTER:
					topx += (width >> 1) - (text_width >> 1);
					break;
				case ALIGN_BOTTOM_CENTER:
					topx += (width >> 1) - (text_width >> 1);
					topy += height - text_height;
					break;
				case ALIGN_MIDDLE_CENTER:
					topx += (width >> 1) - (text_width >> 1);
					topy += (height >> 1) - (text_height >> 1);
					break;
				}
				rotate_point(otopx, otopy, topx - ax + 2, topy - ay + 2, rotation, &topxr, &topyr);
				if (!not_found) draw_text(topxr, topyr, text, font_size, ax, ay, rotation, colour);
				topx = otopx;
				topy = otopy;
				draw_rectangle(topx, topy, width, height, ax, ay, rotation, colour, border_thickness, border_style);
				break;
			case TML_DIV:
				// Draws a rectangle, with element alignment
				width = __get_element_field(tml_element, E_FIELD_WIDTH);
				height = __get_element_field(tml_element, E_FIELD_HEIGHT);
				border_thickness = __get_element_field(tml_element, E_FIELD_BORDER_THICKNESS);
				border_style = __get_element_field(tml_element, E_FIELD_BORDER_STYLE);
				align = __get_element_field(tml_element, E_FIELD_ALIGN);
				draw_rectangle(topx, topy, width, height, ax, ay, rotation, colour, border_thickness, border_style);
				// Do alignment magic
				switch (text_align) {
				case ALIGN_CENTER_LEFT:
					topy += (height >> 1);
					break;
				case ALIGN_BOTTOM_LEFT:
					topy += height;
					break;
				case ALIGN_RIGHT:
					topx += width;
					topy += (height >> 1);
					break;
				case ALIGN_CENTER_RIGHT:
					topx += width;
					topy += (height >> 1);
					break;
				case ALIGN_BOTTOM_RIGHT:
					topx += width;
					topy += height;
					break;
				case ALIGN_CENTER:
					topx += (width >> 1);
					break;
				case ALIGN_BOTTOM_CENTER:
					topx += (width >> 1);
					topy += height;
					break;
				case ALIGN_MIDDLE_CENTER:
					topx += (width >> 1);
					topy += (height >> 1);
					break;
				}
				break;
			case TML_RADIO:
				// Draws a circle for now
				width = __get_element_field(tml_element, E_FIELD_WIDTH);
				border_thickness = __get_element_field(tml_element, E_FIELD_BORDER_THICKNESS);
				draw_circle(topx, topy, width >> 1, ax, ay, border_thickness, colour);
				break;
			case TML_CHECKBOX:
				// Draws a square for now
				width = __get_element_field(tml_element, E_FIELD_WIDTH);
				border_thickness = __get_element_field(tml_element, E_FIELD_BORDER_THICKNESS);
				border_style = __get_element_field(tml_element, E_FIELD_BORDER_STYLE);
				draw_rectangle(topx, topy, width, width, ax, ay, rotation, colour, border_thickness, border_style);
				break;
			}
			i++;
		} else if (data[i] == '>') {
			// Restore previous offset position when end of element is found
			history_index--;
			topx = (pos_history[history_index] >> 8) & 0xff;
			topy = pos_history[history_index] & 0xff;
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
void splash(const byte* data, word duration) {
	// NOTE: Make a dedicated splash screen function, draw image slow asf
	byte old = Write2RealScreen;
	Write2RealScreen = 1;
	draw_image(0, 1, 192, 63, data, 0, 0, 0, COLOUR_IMAGE);
	delay2(duration << 3);
	draw_image(0, 1, 192, 63, data, 0, 0, 0, COLOUR_LIGHT_GREY);
	delay2(4000);
	Write2RealScreen = old;
}

// Similar to __get_element_field but uses get_element to get element data by ID
byte get_element_field(word id, byte field) {
	return 0;
}

// Query element by ID
const byte* get_element(word id) {
	return 0;
}

// Set element field data by ID
void set_element(word id, byte field, byte* data) {
}

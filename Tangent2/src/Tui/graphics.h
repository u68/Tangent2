/*
 * graphics.h
 * Interface for TUI graphics functions
 *  Created on: Nov 11, 2025
 *      Author: harma
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "../base.h"

#define VRAM 0x9000
#define Write2RealScreen *((volatile __near byte *)0x9C00)
#define BufSelSFR *((volatile __near byte *)0xF037)

enum tui_colour {
	TUI_COLOUR_WHITE,
	TUI_COLOUR_LIGHT_GREY,
	TUI_COLOUR_DARK_GREY,
	TUI_COLOUR_BLACK,
	TUI_COLOUR_IMAGE
};

enum tui_style {
	TUI_STYLE_NONE,
	TUI_STYLE_SOLID,
	TUI_STYLE_DOTTED,
	TUI_STYLE_DASHED,
	TUI_STYLE_DOUBLE
};

enum tui_line_style {
	TUI_LINE_STYLE_NONE,
	TUI_LINE_STYLE_SOLID = 0xFF,
	TUI_LINE_STYLE_DOTTED = 0xAA,
	TUI_LINE_STYLE_DASHED = 0xF8,
};

enum tui_font_size {
	TUI_FONT_SIZE_6x7,
	TUI_FONT_SIZE_6x8,
	TUI_FONT_SIZE_6x10,
	TUI_FONT_SIZE_7x10,
	TUI_FONT_SIZE_8x8,
	TUI_FONT_SIZE_8x12,
	TUI_FONT_SIZE_12x16,
};

enum tui_fill_style {
	TUI_FILL_STYLE_NONE,
	TUI_FILL_STYLE_SOLID,
	TUI_FILL_STYLE_CHECKERED,
	TUI_FILL_STYLE_GRADIENT,
	TUI_FILL_STYLE_DITHER,
};

//void __tui_clear_screen_real_buf_1();
//void __tui_clear_screen_real_buf_2();
void tui_rotate_point(byte ax, byte ay, byte px, byte py, word angle, byte *out_x, byte *out_y);
void tui_simple_line(byte x0, byte y0, byte x1, byte y1, byte colour);
void tui_advanced_draw_line(byte* data, byte bit_length, byte x0, byte y0, byte x1, byte y1, byte colour, byte thickness);
void tui_pattern_draw_line(byte pattern, byte x0, byte y0, byte x1, byte y1, byte colour, byte thickness);
void tui_render_buffer();
void tui_clear_screen();
byte tui_get_pixel_b(byte x, byte y, byte buf);
byte tui_get_pixel(byte x, byte y);
//void __tui_set_pixel_real(byte x, byte y, byte colour);
//void __tui_set_pixel(byte x, byte y, byte colour);
void tui_set_pixel(byte x, byte y, byte colour, byte size);
void tui_circle(byte centerX, byte centerY, byte radius, byte c);
void tui_draw_line(byte x0, byte y0, byte x1, byte y1, byte colour, byte thickness, byte style);
void tui_draw_rectangle(byte x, byte y, byte width, byte height, sbyte ax, sbyte ay, word rotation, byte colour, byte thickness, byte style);
void tui_draw_points(byte cx, byte cy, byte x, byte y, byte thickness, byte colour);
void tui_draw_circle(byte cx, byte cy, byte radius, sbyte ax, sbyte ay, byte thickness, byte colour);
void tui_get_font_size(byte font_size, byte* width, byte* height);
void tui_get_text_size(byte font_size, const char* text, byte* width, byte* height);
void tui_draw_text(byte x, byte y, const char* text, byte font_size, sbyte ax, sbyte ay, word rotation, byte colour);
void tui_draw_byte(byte x, byte y, byte data, byte data2, byte mask);
void tui_draw_image(byte x, byte y, byte width, byte height, const byte* bitmap, sbyte ax, sbyte ay, word rotation, byte colour);
void tui_draw_char(byte x, byte y, char c, byte font_size, sbyte ax, sbyte ay, word rotation, byte colour);
void tui_draw_full_image(const word* bitmap, byte colour);

#endif /* GRAPHICS_H_ */

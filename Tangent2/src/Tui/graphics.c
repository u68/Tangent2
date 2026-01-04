/*
 * graphics.c
 * Implementation of TUI graphics functions
 *  Created on: Nov 11, 2025
 *      Author: harma
 */


#include "graphics.h"
#include "lut.h"
#include "ysglfonts.h"

// Rotates point (ax, ay) around anchor (px, py) by angle (in degrees)
void tui_rotate_point(byte ax, byte ay, byte px, byte py, word angle, byte *out_x, byte *out_y) {
	if (!angle) {
		*out_x = px;
		*out_y = py;
		return;
	}
	// Nerdy trig stuff
	sword x = (sword)px - (sword)ax;
	sword y = (sword)py - (sword)ay;
	sbyte cs = tui_cos_table[angle];
	sbyte sn = tui_sin_table[angle];
	sword xr = ((sword)x * cs - (sword)y * sn ) >> 7;
	sword yr = ((sword)x * sn + (sword)y * cs ) >> 7;
	*out_x = (byte)(ax + xr);
	*out_y = (byte)(ay + yr);
}

int abs(int x);

// Simple Bresenham line drawing
void tui_simple_line(byte x0, byte y0, byte x1, byte y1, byte colour) {
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;
	while (1) {
		__tui_set_pixel(x0, y0, colour);
		if (x0 == x1 && y0 == y1) break;
		e2 = err << 1;
		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}
}

// Draw a line with a pattern stretched across its length
void tui_advanced_draw_line(byte* data, byte bit_length, byte x0, byte y0, byte x1, byte y1, byte colour, byte thickness)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;
	int line_len = dx > -dy ? dx : -dy;
	int pix_index = 0;
	while (1) {
		int bit_index = ((long)pix_index * bit_length) / line_len;
		if (data[bit_index >> 3] & (0x80 >> (bit_index & 7))) {
			tui_set_pixel(x0, y0, colour, thickness);
		}
		if (x0 == x1 && y0 == y1) break;
		pix_index++;
		e2 = err << 1;
		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}
}

// Draw a line with a repeating pattern
void tui_pattern_draw_line(byte pattern, byte x0, byte y0, byte x1, byte y1, byte colour, byte thickness) {
	byte lcount = 0;
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;
	while (1) {
		// Draw pixel based on pattern
		if (pattern == 0xFF) {
			tui_set_pixel(x0, y0, colour, thickness);
		} else if (pattern & (0x80 >> (lcount & 7))) {
			tui_set_pixel(x0, y0, colour, thickness);
		} else {
			tui_set_pixel(x0, y0, (byte)TUI_COLOUR_WHITE, thickness);
		}
		lcount++;
		if (x0 == x1 && y0 == y1) break;
		e2 = err << 1;
		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}
}

// Draw the contents of the VRAM buffer to the real VRAM
void tui_render_buffer() {
	word i = 0;
	word j = 0;
	// Lower bitplane
	deref(0xf037) = 0;
	for(i = 0; i < 0x0600; i+=2)
	{
		derefw(0xf800 + j) = derefw(VRAM + i);
		j+=2;
		if((j & 0x001F) == 0x18)
		{
			j+=8;
		}
	}
	// Upper bitplane
	deref(0xf037) = 4;
	j = 0;
	for(i = 0; i < 0x0600; i+=2)
	{
		derefw(0xf800 + j) = derefw(VRAM + i + 0x600);
		j+=2;
		if((j & 0x001F) == 0x18)
		{
			j+=8;
		}
	}
}

// An attempt to work around compiler optimization issues
void __tui_clear_screen_real_buf_2() {
	word i;
	hw_deref(0xF037) = 4; // hw_deref using volatile to prevent optimization issues
	for (i = 0; i < 0x800; i+=2) {
		if ((i & 0x1F) == 0x18) i += 8;
		hw_derefw(0xF800 + i) = 0x0000; // hw_deref using volatile to prevent optimization issues
	}
}

void __tui_clear_screen_real_buf_1() {
	word i;
	hw_deref(0xF037) = 0; // hw_deref using volatile to prevent optimization issues
	for (i = 0; i < 0x800; i+=2) {
		if ((i & 0x1F) == 0x18) i += 8;
		hw_derefw(0xF800 + i) = 0x00; // hw_deref using volatile to prevent optimization issues
	}
}

// I wonder what this does
void tui_clear_screen() {
	// Determine whether to clear real screen or buffer
	if (Write2RealScreen) {
		__tui_clear_screen_real_buf_1();
		__tui_clear_screen_real_buf_2();
		return;
	} else {
		word i;
		for (i = 0; i < 0xC00; i+=2) {
			derefw(VRAM + i) = 0x0000;
		}
	}
}

// Some testing stuff that may be useful later
byte tui_get_pixel_b(byte x, byte y, byte buf) {
	if (x > 191 || y > 63) {
		return 0;
	}
	if (buf == 2) {
		return tui_get_pixel(x, y);
	}
	byte a, b;
	byte ty = 0x80>>(x & 7);
	if (Write2RealScreen) {
		if (buf) {
			BufSelSFR = 0;
		} else {
			BufSelSFR = 4;
		}
		return deref((y<<5) + (x >> 3) + 0xF800) & ty ? 1 : 0;
	} else {
		word addr = (y << 4) + (y << 3) + (x >> 3) + VRAM;
		if (buf) {
			return deref(addr) & ty ? 1 : 0;
		} else {
			return deref(addr+0x600) & ty ? 2 : 0;
		}
	}
}

// Get pixel colour at x,y
byte tui_get_pixel(byte x, byte y) {
	if (x > 191 || y > 63) {
		return 0;
	}
	byte a, b;
	byte ty = 0x80>>(x & 7);
	// Determine whether to read from real screen or buffer
	if (Write2RealScreen) {
		BufSelSFR = 0;
		a = deref((y<<5) + (x >> 3) + 0xF800) & ty ? 1 : 0;
		BufSelSFR = 4;
		b = deref((y<<5) + (x >> 3) + 0xF800) & ty ? 2 : 0;
		return a + b;
	} else {
		word addr = (y << 4) + (y << 3) + (x >> 3) + VRAM;
		a = deref(addr) & ty ? 1 : 0;
		b = deref(addr+0x600) & ty ? 2 : 0;
		return a + b;
	}
}

// Internal function to set a pixel in real VRAM
void __tui_set_pixel_real(byte x, byte y, byte colour) {
	if (x > 191 || y > 63) {
		return;
	}
	word addr = (y<<5) + (x >> 3) + 0xF800;
	byte ty = 0x80>>(x & 7);
	deref(0xF037) = 0;
	// Use switch for efficiency
	switch(colour) {
	case 0:
		deref(addr) &= ~ty; // Set both bitplanes bits to 0
		deref(0xF037) = 4;
		deref(addr) &= ~ty;
		break;
	case 1:
		deref(addr) |= ty; // Set lower bitplane bit to 1, upper to 0
		deref(0xF037) = 4;
		deref(addr) &= ~ty;
		break;
	case 2:
		deref(addr) &= ~ty; // Set lower bitplane bit to 0, upper to 1
		deref(0xF037) = 4;
		deref(addr) |= ty;
		break;
	case 3:
		deref(addr) |= ty; // Set both bitplanes bits to 1
		deref(0xF037) = 4;
		deref(addr) |= ty;
		break;
	}
}

// Internal function to set a pixel in VRAM buffer
void __tui_set_pixel(byte x, byte y, byte colour) {
	if (x > 191 || y > 63) {
		return;
	} //else
	// STUPID COMPILER OPTIMIZATIONS
	void (*volatile fp)(byte, byte, byte) = __tui_set_pixel_real;

	if (Write2RealScreen) {
		fp(x, y, colour);
	    return;
	}
	// Set pixel in VRAM buffer
	word addr = (y << 4) + (y << 3) + (x >> 3) + VRAM;
	byte ty = 0x80>>(x & 7);
	switch(colour)	{
	case 0:
		deref(addr) &= ~ty;     // Set both bitplanes bits to 0
		deref(addr+0x600) &= ~ty;
		break;
	case 1:
		deref(addr) |= ty;		// Set lower bitplane bit to 1, upper to 0
		deref(addr+0x600) &= ~ty;
		break;
	case 2:
		deref(addr) &= ~ty;		// Set lower bitplane bit to 0, upper to 1
		deref(addr+0x600) |= ty;
		break;
	case 3:
		deref(addr) |= ty;		// Set both bitplanes bits to 1
		deref(addr+0x600) |= ty;
		break;
	}
}

// Basically drawing a circle, but has some optimizations for small sizes
void tui_set_pixel(byte x, byte y, byte colour, byte size) {
	switch(size) {
	case 0:
		return;
	case 1:
		__tui_set_pixel(x, y, colour);
		return;
	case 2:
		__tui_set_pixel(x, y, colour);
		__tui_set_pixel(x+1, y, colour);
		__tui_set_pixel(x, y+1, colour);
		__tui_set_pixel(x-1, y, colour);
		__tui_set_pixel(x, y-1, colour);
		return;
	default:
		tui_circle(x, y, size>>1, colour);
		return;
	}
}

// Square but filled in
void tui_circle(byte cx, byte cy, byte r, byte col)
{
	int x = r;
	int y = 0;
	int err = 1 - r;
	while (x >= y)
	{
		for (int yy = cy - x; yy <= cy + x; yy++)
		{
			__tui_set_pixel(cx + y, yy, col);
			__tui_set_pixel(cx - y, yy, col);
		}
		for (int yy = cy - y; yy <= cy + y; yy++)
		{
			__tui_set_pixel(cx + x, yy, col);
			__tui_set_pixel(cx - x, yy, col);
		}
		y++;
		if (err < 0)
			err += (y << 1) + 1;
		else {
			x--;
			err += ((y - x) << 1) + 1;
		}
	}
}

// Draw a line with specified style and thickness
void tui_draw_line(byte x0, byte y0, byte x1, byte y1, byte colour, byte thickness, byte style) {
	byte pattern = style;
	switch(style) {
	case TUI_STYLE_NONE:
	case TUI_STYLE_SOLID:
		pattern = 0xFF;
		break;
	case TUI_STYLE_DOTTED:
		pattern = 0xAA;
		break;
	case TUI_STYLE_DASHED:
		pattern = 0xF8;
		break;
	}
	tui_pattern_draw_line(pattern, x0, y0, x1, y1, colour, thickness);
}

// Draw a rectangle with rotation around anchor point
void tui_draw_rectangle(byte x, byte y, byte width, byte height, sbyte ax, sbyte ay, word rotation, byte colour, byte thickness, byte style) {
	// Assume rotation is 0
	byte nx0 = x - ax;
	byte ny0 = y - ay;
	byte nx1 = x + width - ax;
	byte ny1 = y - ay;
	byte nx2 = x - ax;
	byte ny2 = y - ay + height;
	byte nx3 = x + width - ax;
	byte ny3 = y - ay + height;
	if (rotation >= 360) rotation -= 360;
	if (rotation < 0) rotation += 360;
	if (rotation) {
		// Rotate all points around x, y
		if (ax || ay) {
			tui_rotate_point(x, y, x - ax, y - ay, rotation, &nx0, &ny0);
		}
		tui_rotate_point(x, y, x + width - ax, y - ay, rotation, &nx1, &ny1);
		tui_rotate_point(x, y, x - ax, y - ay + height, rotation, &nx2, &ny2);
		tui_rotate_point(x, y, x + width - ax, y - ay + height, rotation, &nx3, &ny3);
	}
	// Draw lines between points
	tui_draw_line(nx0, ny0, nx1, ny1, colour, thickness, style);
	tui_draw_line(nx0, ny0, nx2, ny2, colour, thickness, style);
	tui_draw_line(nx2, ny2, nx3, ny3, colour, thickness, style);
	tui_draw_line(nx1, ny1, nx3, ny3, colour, thickness, style);
}

// Helper function to draw circles
void tui_draw_points(byte cx, byte cy, byte x, byte y, byte thickness, byte colour) {
	// Blah blah blah
	tui_set_pixel(cx + x, cy + y, colour, thickness);
	tui_set_pixel(cx - x, cy + y, colour, thickness);
	tui_set_pixel(cx + x, cy - y, colour, thickness);
	tui_set_pixel(cx - x, cy - y, colour, thickness);
	tui_set_pixel(cx + y, cy + x, colour, thickness);
	tui_set_pixel(cx - y, cy + x, colour, thickness);
	tui_set_pixel(cx + y, cy - x, colour, thickness);
	tui_set_pixel(cx - y, cy - x, colour, thickness);
}

// Draw a circle with rotation around cx, cy with anchor point
void tui_draw_circle(byte cx, byte cy, byte radius, sbyte ax, sbyte ay, byte thickness, byte colour) {
	byte x = 0;
	byte y = radius;
	sword d = 3 - (radius << 1);

	tui_draw_points(cx - ax, cy - ay, x, y, thickness, colour);

	while (y >= x) {
		if (d > 0) {
			y--;
			d = d + ((x - y) << 2) + 10;
		} else {
			d = d + (x << 2) + 6;
		}
		x++;
		tui_draw_points(cx - ax, cy - ay, x, y, thickness, colour);
	}
}

// Returns the width and height of a given font
void tui_get_font_size(byte font_size, byte* width, byte* height) {
	switch(font_size) {
	case TUI_FONT_SIZE_6x7:
		*width = 6;
		*height = 7;
		return;
	case TUI_FONT_SIZE_6x8:
		*width = 6;
		*height = 8;
		return;
	case TUI_FONT_SIZE_6x10:
		*width = 6;
		*height = 10;
		return;
	case TUI_FONT_SIZE_7x10:
		*width = 7;
		*height = 10;
		return;
	case TUI_FONT_SIZE_8x8:
		*width = 8;
		*height = 8;
		return;
	case TUI_FONT_SIZE_8x12:
		*width = 8;
		*height = 12;
		return;
	case TUI_FONT_SIZE_12x16:
		*width = 12;
		*height = 16;
		return;
	}
}

// Returns the width and height of the given text
void tui_get_text_size(byte font_size, const char* text, byte* width, byte* height) {
	byte cwidth = 0, cheight = 0, i = 0;
	tui_get_font_size(font_size, &cwidth, &cheight);
	// Count characters and multiply by width
	while (text[i]) i++;
	*width = cwidth * i;
	*height = cheight;
}

// Draw text with specified font size, rotation, and style
void tui_draw_text(byte x, byte y, const char* text, byte font_size, sbyte ax, sbyte ay, word rotation, byte colour) {
	byte cheight;
	byte cwidth;
	byte i = 0;
	byte tx, ty;
	byte ox = x;
	// Get character dimensions
	tui_get_font_size(font_size, &cwidth, &cheight);
	while (text[i]) {
		if (rotation) tui_rotate_point(x, y, ox - ax, y - ay, rotation, &tx, &ty);
		else {
			tx = ox - ax;
			ty = y - ay;
		}
		// Draw character, anchor x, y isn't really used but whatever
		tui_draw_char(tx, ty, text[i], font_size, 0, 0, rotation, colour);
		ox += cwidth;
		i++;
	}
}

// Draw a byte at x,y with mask
void tui_draw_byte(byte x, byte y, byte data, byte data2, byte mask) {
	// I'm sorry this is messy, but that's not my problem
	if (x > 191 || y > 63) {
		return;
	}
	byte bitpos = x & 7;
	byte lbyte = data >> bitpos;
	byte hbyte = data << (8 - bitpos);
	byte lbyte2 = data2 >> bitpos;
	byte hbyte2 = data2 << (8 - bitpos);
	byte temp;
	word addr;
	// Determine starting address
	if (Write2RealScreen) {
		addr = (y << 5) + (x >> 3) + 0xF800;
	} else {
		addr = (y << 4) + (y << 3) + (x >> 3) + VRAM;
	}
	// Write high bytes first if it exists
	if (hbyte | hbyte2) {
		if (Write2RealScreen) {
			BufSelSFR = 0;
		}
		temp = deref(addr + 1);
		temp &= (0xFF >> bitpos);
		temp |= (hbyte & (mask << (8 - bitpos)));
		deref(addr + 1) = temp;
		if (Write2RealScreen) {
			BufSelSFR = 4;
		} else {
			addr += 0x600;
		}
		temp = deref(addr + 1);
		temp &= (0xFF >> bitpos);
		temp |= (hbyte2 & (mask << (8 - bitpos)));
		deref(addr + 1) = temp;
		if (!Write2RealScreen) {
			addr -= 0x600;
		}
	}
	if (Write2RealScreen) {
		BufSelSFR = 0;
	}
	// Draw low bytes
	temp = deref(addr);
	temp &= (0xFF << (8 - bitpos));
	temp |= (lbyte & (mask >> bitpos));
	deref(addr) = temp;
	if (Write2RealScreen) {
		BufSelSFR = 4;
	} else {
		addr += 0x600;
	}
	temp = deref(addr);
	temp &= (0xFF << (8 - bitpos));
	temp |= (lbyte2 & (mask >> bitpos));
	deref(addr) = temp;
}

// Helper functions to get min and max of 4 bytes
byte tui_min4(byte a, byte b, byte c, byte d) {
	byte m = a;
	if (b < m) m = b;
	if (c < m) m = c;
	if (d < m) m = d;
	return m;
}

byte tui_max4(byte a, byte b, byte c, byte d) {
	byte m = a;
	if (b > m) m = b;
	if (c > m) m = c;
	if (d > m) m = d;
	return m;
}


// Draw an image bitmap with rotation around anchor point
void tui_draw_image(byte x, byte y, byte width, byte height, const byte* bitmap, sbyte ax, sbyte ay, word rotation, byte colour) {
	word bwidth = (width + 7) >> 3;
	word psize = bwidth * height;
	byte iy, ix;
	word mapindex;

	if (!rotation) {
		byte osf = (width & 7) ? 1 : 0;
		// No rotation, simple blit, but it's massive (https://x.com/Ninja/status/1862933454767239616) becuase doing the switch when it comes to draw the byte increases the amount of cycles
		switch (colour) {
		case TUI_COLOUR_WHITE:
			return;
		case TUI_COLOUR_LIGHT_GREY:
			for (iy = 0; iy < height; iy++) {
				for (ix = 0; ix < bwidth; ix++) {
					mapindex = iy * bwidth + ix;
					byte mask = (ix == bwidth - osf && osf) ? (0xFF << (8 - (width & 7))) : 0xFF;
					tui_draw_byte(x + (ix << 3) - ax, y - ay + iy, bitmap[mapindex], 0, mask);
				}
			}
			return;
		case TUI_COLOUR_DARK_GREY:
			for (iy = 0; iy < height; iy++) {
				for (ix = 0; ix < bwidth; ix++) {
					mapindex = iy * bwidth + ix;
					byte mask = (ix == bwidth - osf && osf) ? (0xFF << (8 - (width & 7))) : 0xFF;
					tui_draw_byte(x + (ix << 3) - ax, y - ay + iy, 0, bitmap[mapindex], mask);
				}
			}
			return;
		case TUI_COLOUR_BLACK:
			for (iy = 0; iy < height; iy++) {
				for (ix = 0; ix < bwidth; ix++) {
					mapindex = iy * bwidth + ix;
					byte mask = (ix == bwidth - osf && osf) ? (0xFF << (8 - (width & 7))) : 0xFF;
					tui_draw_byte(x + (ix << 3) - ax, y - ay + iy, bitmap[mapindex], bitmap[mapindex], mask);
				}
			}
			return;
		default:
			for (iy = 0; iy < height; iy++) {
				for (ix = 0; ix < bwidth; ix++) {
					mapindex = iy * bwidth + ix;
					byte mask = (ix == bwidth - osf && osf) ? (0xFF << (8 - (width & 7))) : 0xFF;
					tui_draw_byte(x + (ix << 3) - ax, y - ay + iy, bitmap[mapindex], bitmap[mapindex + psize], mask);
				}
			}
			return;
		}
	} else {
		// If there is rotation, we have to do it pixel by pixel :(
		byte nx, ny, cx1, cy1, cx2, cy2, cx3, cy3, bx, by, rx, ry, hx, hy;
		byte cx0 = x, cy0 = y;
		// Rotate corners
		if (ax || ay) {
			tui_rotate_point(x, y, x - ax, y - ay, rotation, &cx0, &cy0);
		}
		tui_rotate_point(x, y, x + width - ax, y - ay, rotation, &cx1, &cy1);
		tui_rotate_point(x, y, x - ax, y - ay + height, rotation, &cx2, &cy2);
		tui_rotate_point(x, y, x + width - ax, y - ay + height, rotation, &cx3, &cy3);
		// Get bounding box from the rotated corners
		bx = tui_min4(cx0, cx1, cx2, cx3);
		by = tui_min4(cy0, cy1, cy2, cy3);
		rx = tui_max4(cx0, cx1, cx2, cx3) + 1;
		ry = tui_max4(cy0, cy1, cy2, cy3) + 1;
		if (rx > 192) rx = 192;
		if (ry > 64) ry = 64;
		// For each pixel in bounding box, rotate backwards and sample from bitmap, that way there are no gaps
		for (byte i = bx; i < rx; i++) {
			for (byte j = by; j < ry; j++) {
				tui_rotate_point(x, y, i, j, 360 - rotation, &hx, &hy);
				hx -= (x - ax);
				hy -= (y - ay);
				if (hx < 0 || hy < 0 || hx >= width || hy >= height) {
					continue;
				}
				byte col = 0;
				word addr = hy * bwidth + (hx >> 3);
				byte bdata = bitmap[addr];
				byte bdata2 = (colour == TUI_COLOUR_IMAGE) ? bitmap[addr + psize] : 0;
				byte bmask = 0x80 >> (hx & 7);
				if (colour == TUI_COLOUR_IMAGE) {
					if (bdata2 & bmask) {
						col |= 2;
					}
				}
				if (bdata & bmask) {
					if (colour != TUI_COLOUR_IMAGE) {
						col = colour;
					} else {
						col |= 1;
					}
				}
				tui_set_pixel(i, j, col, 1);
			}
		}
	}
}

// Draw_image wrapper to draw a character from a font
void tui_draw_char(byte x, byte y, char c, byte font_size, sbyte ax, sbyte ay, word rotation, byte colour) {
	const byte* font_data;
	byte font_width;
	byte font_height;
	// This can probably use tui_get_font_size but tui_get_font_size doesnt get dont_data so like idc
	switch(font_size) {
	case TUI_FONT_SIZE_6x7:
		font_data = YsFont6x7;
		font_width = 6;
		font_height = 7;
		break;
	case TUI_FONT_SIZE_6x8:
		font_data = YsFont6x8;
		font_width = 6;
		font_height = 8;
		break;
	case TUI_FONT_SIZE_6x10:
		font_data = YsFont6x10;
		font_width = 6;
		font_height = 10;
		break;
	case TUI_FONT_SIZE_7x10:
		font_data = YsFont7x10;
		font_width = 7;
		font_height = 10;
		break;
	case TUI_FONT_SIZE_8x8:
		font_data = YsFont8x8;
		font_width = 8;
		font_height = 8;
		break;
	case TUI_FONT_SIZE_8x12:
		font_data = YsFont8x12;
		font_width = 8;
		font_height = 12;
		break;
	case TUI_FONT_SIZE_12x16:
		font_data = YsFont12x16;
		font_width = 12;
		font_height = 16;
		break;
	default:
		return;
	}
	tui_draw_image(x, y, font_width, font_height, &font_data[(byte)c * (font_height * ((font_width + 7) >> 3))], ax, ay, rotation, colour);
}


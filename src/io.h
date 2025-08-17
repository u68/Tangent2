#ifndef IO_H
#define IO_H

#include "base.h"
//legacy stuff from cosine
enum BUTTON
{
	B_0 = 0xb,
	B_1 = 0x3f,
	B_2 = 0x37,
	B_3 = 0x2f,
	B_4 = 0x3e,
	B_5 = 0x36,
	B_6 = 0x2e,
	B_7 = 0x3d,
	B_8 = 0x35,
	B_9 = 0x2d,

	B_A = 0x3c,
	B_B = 0x34,
	B_C = 0x2c,
	B_D = 0x24,
	B_E = 0x1c,
	B_F = 0x14,

	B_G = 0x3d,
	B_H = 0x35,
	B_I = 0x2d,
	B_J = 0x25,
	B_K = 0x1d,

	B_L = 0x3e,
	B_M = 0x36,
	B_N = 0x2e,
	B_O = 0x26,
	B_P = 0x1e,

	B_Q = 0x3f,
	B_R = 0x37,
	B_S = 0x2f,
	B_T = 0x27,
	B_U = 0x1f,

	B_V = 0xb,
	B_W = 0xc,
	B_X = 0xd,
	B_Y = 0xe,
	B_Z = 0xf,

    BUTTON_COUNT = 0x40
};

enum SPECIAL_CHARS
{
	SP_EXE = '\n',
	SP_TAB = '\t',
	SP_SPACE = ' ',
	SP_BACKSPACE = '\b',

	SP_HOME = 0x2A,
	SP_END = 0x1A,
	SP_YES = 0x3A,
	SP_NO = 0x12,
	SP_OK = 0x21,
	SP_UP = 0x20,
	SP_DOWN = 0x22,
	SP_LEFT = 0x29,
	SP_RIGHT = 0x19,
	SP_PLUS = 0x10,
	SP_MINUS = 0x11,
	SP_ALT = 0x31,
	SP_ABC = 0x30,
	SP_ESC = 0x39,
	SP_SELECTLEFT,
	SP_SELECTRIGHT,
	SP_PASTE,
	SP_COPY,

	SPECIAL_MAX
};

//ccu8 complains abt ts so this fixed it

extern const byte button_to_char[64];

extern const byte button_to_char_abc[64];

extern const byte button_to_char_alt[64];

//yummers

void draw_byte1(byte x, byte y, byte sprite);
void charprint1(byte x, byte y, word smth);

void draw_byte2(byte x, byte y, byte sprite);
void charprint2(byte x, byte y, word smth);

word print(const byte* str, byte x, byte y,byte color);

//ts sucks but is fast
void drawbitmap(const byte* ptr, word offset, byte width, byte height,byte color);

void render(word addr);

void set_pixel(byte x, byte y, byte color);

int abs(int number);
//plot line is the ONLY thing that uses abs 🥀
void plot_line(int x0, int y0, int x1, int y1, byte color); //I did NOT make ts 🥀✌️😭 Bresenham made the thingy

byte CheckButtons();

#endif

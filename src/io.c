//io is a moon or io is input output idk twin
#include "base.h"
#include "io.h"
#include "thefont.h"

//legacy cosine shi

const byte button_to_char[64] = {
    0,        0,       0,    0,    0,    0,   0,    0,
    0,        0,       0,  '0',  ' ',  ',', '.', '\n',
SP_PLUS, SP_MINUS,   SP_NO,  ']',  ')',    0,   0,    0,
    0, SP_RIGHT,  SP_END,  '[',  '(', '\b', '#',  '%',
SP_UP,    SP_OK, SP_DOWN, '\'',  '/',  '!', '@',  '$',
    0,  SP_LEFT, SP_HOME,  ';', '\\',  '9', '6',  '3',
SP_ABC,   SP_ALT,     '*',  '-',  '=',  '8', '5',  '2',
    0,   SP_ESC,  SP_YES, '\t',  '`',  '7', '4',  '1',
};

const byte button_to_char_abc[64] = {
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0, 'V', 'W', 'X', 'Y', 'Z',
  0,   0,   0,   0, 'F',   0,   0,   0,
  0,   0,   0,   0, 'E', 'K', 'P', 'U',
  0,   0,   0,   0, 'D', 'J', 'O', 'T',
  0,   0,   0,   0, 'C', 'I', 'N', 'S',
  0,   0,   0,   0, 'B', 'H', 'M', 'R',
  0,   0,   0,   0, 'A', 'G', 'L', 'Q',
};

const byte button_to_char_alt[64] = {
  0,              0,        0,   0,   0,   0,   0,   0,
  0,              0,        0,   0,   0, '<', '>',   0,
  0,              0, SP_PASTE, '}', '^',   0,   0,   0,
  0, SP_SELECTRIGHT,        0, '{', '*',   0,   0,   0,
  0,              0,        0, '"', '?',   0,   0,   0,
  0,  SP_SELECTLEFT,        0, ':', '|',   0,   0,   0,
  0,              0,      '&', '_', '+',   0,   0,   0,
  0,              0,  SP_COPY,   0, '~',   0,   0,   0,
};

//actual io shi but it is mostly copied from tangent 🥀

void draw_byte1(byte x, byte y, byte sprite) {
    byte bit_index = x & 7;
    word index = ((y << 4) + (y << 3)) + x;
    deref(vram1 + index) |= (sprite >> bit_index);
    deref(vram1 + index + 1) |= (sprite << (8 - bit_index));
}

void draw_byte2(byte x, byte y, byte sprite) {
    word bit_index = x & 7;
    word index = y * 24 + x;
    deref(vram2 + index) |= sprite >> bit_index;
    if (bit_index != 0 && x + 1 < 24) {
    	deref(vram2 + index + 1) |= sprite << (8 - bit_index);
    }
}

void charprint1(byte x, byte y, word smth)
{
	word i = 0;
	for(i = 0;i < 8;i++)
	{
		draw_byte1(x,y+i,image_raw[i+(smth<<4)]);
	}
}

void charprint2(byte x, byte y, word smth)
{
	word i = 0;
	for(i = 0;i < 8;i++)
	{
		draw_byte2(x,y+i,image_raw[i+(smth<<4)]);
	}
}

//unreadable bs
//TODO: make it write to buffer buffer not directly to screen and debug it
void drawbitmap(const byte* ptr, word offset, byte width, byte height,byte color)
{
	word x;
	word y;
	word i = 0;
	word j = 0;
	deref(0xF037) = 0; //magic buffer switch to make gr -> e <- yscale
	if(color == 1)
	{
		deref(0xF037) = 4;
	}
	for(y = 0; y < offset; y++)
	{
		++j;
		if((j&0x1f) == 0x18)
		{
			j += 0x8;
		}
	}

	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
			if(color == 2)
			{
				deref(0xF037) = 0;
				deref(0xf800+j) = ptr[i]; //the screen is f800
				deref(0xF037) = 4;
			}
			deref(0xf800+j) = ptr[i];
			++j;
			if((j&0x1f) == 0x18)
			{
				j += 0x8;
			}
			++i;
		}
		j += 32-width;
		if((j&0x1f) == 0x18)
		{
			j += 0x8;
		}
	}
}

//optimise ts twin 🥀

void render(word addr)
{
    word i = 0;
    word j = 0;
    deref(0xf037) = 0;
    for(i = 0; i < 0x0800; i++)
    {
        j++;
        if((j & 0x001F) == 0x18)
        {
            j+=8;
        }
        *((word *)(0xf800 + j)) = *((word *)(addr + i));
    }
    j = 0;
    deref(0xf037) = 4;
    for(i = 0; i < 0x0800; i++)
    {
        j++;
        if((j & 0x001F) == 0x18)
        {
            j+=8;
        }
        *((word *)(0xf800 + j)) = *((word *)(addr + i + 0x600));
    }
}

word print//job//application//🥀
(const byte* str, byte x, byte y,byte color)
{
	const byte* what = str;
	word curoffset = (word)x+((word)y<<8)+((word)y<<7);
	//derefw(0xB510) = (word)(what);a //leftover debug (ts codebase is a museum)
	word i = 0;
	while(*what)
	{
		//deref(0xB500+i) = *what;

		drawbitmap(image_raw+((word)(*what)<<4),curoffset,1,16,color);
		++curoffset;
		++what;
		++i;
		//++i;
	}
	return i;
}

void set_pixel(byte x, byte y, byte color)
{
	word byteIndex = (y*24) + (x >> 3);
	byte bitIndex = x & 7;
	if (x > 191)
	{
		return;
	}
	if (y > 63)
	{
		return;
	}
	if (byteIndex + vram2 > 0x9c02)
	{
		return;
	}
	deref(byteIndex + vram1) &= ~(0x80>>bitIndex);
	deref(byteIndex + vram2) &= ~(0x80>>bitIndex);
    if(color == 1)
    {
    	deref(byteIndex + vram1) |= (0x80>>bitIndex);
    }
    if(color > 1)
    {
    	deref(byteIndex + vram2) |= (0x80>>bitIndex);
    }
    if(color == 3)
    {
        deref(byteIndex + vram1) |= (0x80>>bitIndex);
    }
}

void plot_line (int x0, int y0, int x1, int y1, byte color)
{
  int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;

  for (;;){
    set_pixel (x0,y0,color);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

//wav3 made this and idk how it works tbh I am scared to touch it

byte lastbutton = 0xff;
byte CheckButtons()
{
	byte x;
	byte y;
	byte i = 0;
	for(x = 0x80; x != 0; x = x >> 1)
	{
		deref(0xf046) = x;
		for(y = 0x80; y != 0; y = y >> 1)
		{
			if((deref(0xf040) & y) == 0)
			{
				if(i != lastbutton)
				{
					lastbutton = i;
					return i;
				}
				return 0xff;
			}
			++i;
		}
	}
	lastbutton = 0x50;
	return 0xff;
}
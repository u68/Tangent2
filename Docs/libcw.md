# libcw v1.45 Reference

## Overview
`libcw.h` is a single header that exposes common types, graphics helpers, heap allocation, a small in memory filesystem, RTC helpers, delays, and keyboard scan codes for Tangent2 targets.

This document covers the public surface declared in `Tangent2/src/libcw.h` and the time format enum implemented in `Tangent2/src/libcw.c`.

## IMPORTANT!

### It is advised to not use standard headers/libc with libcw, as libcw aims to replace it and uses hardcoded addresses etc (and basically takes up all ram) and using the stack is risky as it isn't very big (after not much you will start interfereing with the file system).

### I have never tested it with standard headers/libraries/whatever/witchcraft and I never will.

## Table of contents
- [Integration And Configuration](#integration-and-configuration)
- [Core Types](#core-types)
- [Platform Macros And Runtime Controls](#platform-macros-and-runtime-controls)
- [TUI Graphics](#tui-graphics)
- [Heap Management](#heap-management)
- [Filesystem](#filesystem)
- [Time And RTC](#time-and-rtc)
- [Keyboard Input](#keyboard-input)

## Integration And Configuration

### Section contents
- [Including libcw](#including-libcw)
- [Selecting The Target](#selecting-the-target)
- [Initialization Order](#initialization-order)
- [Shared Example Preamble](#shared-example-preamble)
- [General Notes](#general-notes)

### Including libcw
Add `libcw.h` once in each translation unit that uses the library.

```c
#include "libcw.h"
```

### Selecting The Target
The header changes several memory layout constants depending on whether `IS_CWX` is defined.

- Define `IS_CWX` before including `libcw.h` when building for the CWX target.
- Leave `IS_CWX` undefined when building for the non CWX target used by Tangent2's current default layout.

CWX example:

```c
#define IS_CWX
#include "libcw.h"
```

Non CWX example:

```c
#include "libcw.h"
```

### Initialization Order
`libcw` does not automatically initialize every subsystem.

Recommended startup order:

1. Set the drawing target by writing `Write2RealScreen`.
2. Call `hinit()` before using heap allocation.
3. Call `fs_init()` before using filesystem functions.
4. Call `rtc_enable()` before relying on RTC reads.

### Shared Example Preamble
Unless a snippet states otherwise, the following setup is assumed.

```c
#include "libcw.h"

static void libcw_setup(void)
{
    Write2RealScreen = 0;
    hinit();
    fs_init();
    rtc_enable();
}
```

### General Notes
- Examples in this document do not use standard library headers.
- Examples omit repeated `#include "libcw.h"` lines after the shared preamble.
- TUI coordinates are byte sized and intended for a 192 by 64 display.
- `tui_rotate_point()` expects an angle in the range `0` through `359`.
- RTC helper functions operate on hardware register values. Time register fields are treated as BCD values by the formatting code.
- `fs_write_file()` and `fs_write()` align the requested size to `HEAP_BLOCK_ALIGN`. In practice, use buffers whose logical size is already even.

## Core Types

### Section contents
- [Scalar Integer Types](#scalar-integer-types)
- [Time Format Type](#time-format-type)
- [Core Type Example](#core-type-example)

### Scalar Integer Types
`libcw` defines these scalar aliases:

| Type | Meaning | Width intent |
| --- | --- | --- |
| `byte` | unsigned byte | 8 bit |
| `sbyte` | signed byte | 8 bit |
| `word` | unsigned word | 16 bit |
| `sword` | signed word | 16 bit |
| `dword` | unsigned double word | 32 bit |
| `sdword` | signed double word | 32 bit |

### Time Format Type
`format_t` is forward declared in `libcw.h` and implemented in `libcw.c` as:

| Value | Meaning |
| --- | --- |
| `TIME_FORMAT_24H` | `HH:MM` |
| `TIME_FORMAT_12H` | `HH:MM AM` or `HH:MM PM` |
| `TIME_FORMAT_24H_WITH_SECONDS` | `HH:MM:SS` |
| `TIME_FORMAT_12H_WITH_SECONDS` | `HH:MM:SS AM` or `HH:MM:SS PM` |

### Core Type Example
```c
static void type_example(void)
{
    byte alpha = 255;
    sbyte beta = -12;
    word gamma = 4096;
    sword delta = -300;
    dword pixels = 12288UL;
    sdword offset = -1024L;
    format_t clock_mode = TIME_FORMAT_24H_WITH_SECONDS;

    if (alpha && beta && gamma && delta && pixels && offset)
    {
        get_time_string(clock_mode, (char *)0x9100);
    }
}
```

## Platform Macros And Runtime Controls

### Section contents
- [Version Macro](#version-macro)
- [Display And Memory Layout Macros](#display-and-memory-layout-macros)
- [Filesystem Permission Presets](#filesystem-permission-presets)
- [RTC Register Macros](#rtc-register-macros)
- [Time Constants](#time-constants)
- [Interrupt And Buffer Controls](#interrupt-and-buffer-controls)

### Version Macro

| Macro | Value | Meaning |
| --- | --- | --- |
| `LIBCW_VERSION` | `"1.3"` | header version string |

Example:

```c
static byte using_expected_version(void)
{
    return LIBCW_VERSION[0] == '1';
}
```

### Display And Memory Layout Macros
These macros define the address layout for drawing, heap, and filesystem storage.

CWX values:

| Macro | Value |
| --- | --- |
| `VRAM` | `0xD000` |
| `HEAP_START_ADDR` | `VRAM + 0x604` |
| `HEAP_MAX_SIZE` | `0x0A00` |
| `FS_MAX_SIZE` | `0x1000` |
| `FS_MAX_NODES` | `64` |

Non CWX values:

| Macro | Value |
| --- | --- |
| `VRAM` | `0x9000` |
| `HEAP_START_ADDR` | `VRAM + 0xC04` |
| `HEAP_MAX_SIZE` | `0x1400` |
| `FS_MAX_SIZE` | `0x3800` |
| `FS_MAX_NODES` | `255` |

Shared layout macros:

| Macro | Meaning |
| --- | --- |
| `Write2RealScreen` | lvalue flag that selects real VRAM or the offscreen buffer |
| `HEAP_BLOCK_ALIGN` | heap and file data alignment, currently `2` |
| `FS_START_ADDR` | start address of filesystem region |
| `FS_NAME_MAX_LEN` | maximum node name buffer length, currently `12` |
| `FS_INVALID_IDX` | invalid node index sentinel, `0xFF` |
| `FS_NULL_OFFSET` | invalid file data offset sentinel, `0xFFFF` |
| `FS_ROOT` | pointer to root node, `&FS_NODES[0]` |

Example:

```c
static void layout_example(void)
{
    word heap_start = HEAP_START_ADDR;
    word fs_start = FS_START_ADDR;
    byte max_name = FS_NAME_MAX_LEN;

    Write2RealScreen = 0;
    if (heap_start < fs_start && max_name >= 8)
    {
        tui_clear_screen();
    }
}
```

### Filesystem Permission Presets
The permission preset macros build `fs_perms_t` values.

| Macro | Read | Write | Execute |
| --- | --- | --- | --- |
| `PERMS_RWX` | 1 | 1 | 1 |
| `PERMS_RW` | 1 | 1 | 0 |
| `PERMS_R` | 1 | 0 | 0 |
| `PERMS_WX` | 0 | 1 | 1 |
| `PERMS_W` | 0 | 1 | 0 |
| `PERMS_X` | 0 | 0 | 1 |
| `PERMS_RX` | 1 | 0 | 1 |
| `PERMS_NONE` | 0 | 0 | 0 |

Example:

```c
static void perms_example(void)
{
    fs_node_t *bin_dir = fs_create_directory(FS_ROOT, "bin", PERMS_RWX);
    fs_node_t *note = fs_create_file(FS_ROOT, "note", PERMS_RW);
    fs_node_t *script = fs_create_file(FS_ROOT, "boot", PERMS_RX);

    if (bin_dir && note && script)
    {
        fs_chmod_node(note, PERMS_R);
    }
}
```

### RTC Register Macros
These macros expose direct RTC register access.

| Macro | Meaning |
| --- | --- |
| `RTC_ENABLE` | RTC enable register |
| `RTC_SECONDS` | seconds register |
| `RTC_MINUTES` | minutes register |
| `RTC_HOURS` | hours register |
| `RTC_DAY` | day register |
| `RTC_WEEK` | week register |
| `RTC_MONTH` | month register |
| `RTC_YEAR` | year register with 2026 based offset in helper functions |

Example:

```c
static void rtc_register_example(void)
{
    RTC_ENABLE = 1;
    RTC_HOURS = 0x14;
    RTC_MINUTES = 0x37;
    RTC_SECONDS = 0x05;
}
```

### Time Constants

| Macro | Value |
| --- | --- |
| `SECONDS_PER_MINUTE` | `60` |
| `MINUTES_PER_HOUR` | `60` |
| `HOURS_PER_DAY` | `24` |
| `DAYS_PER_WEEK` | `7` |
| `MONTHS_PER_YEAR` | `12` |
| `TICKS_PER_MS` | `8` |
| `TICKS_PER_SECOND` | `8000` |

Example:

```c
static word two_minutes_in_ticks(void)
{
    return 2 * MINUTES_PER_HOUR * TICKS_PER_SECOND / 60;
}
```

### Interrupt And Buffer Controls

| Symbol | Availability | Meaning |
| --- | --- | --- |
| `__DI()` | all targets | disable interrupts |
| `__EI()` | all targets | enable interrupts |
| `BufSelSFR` | non CWX only | screen buffer selection register used by some drawing code |

Example:

```c
static void protected_draw(void)
{
    __DI();
#ifndef IS_CWX
    BufSelSFR = 0;
#endif
    Write2RealScreen = 1;
    tui_set_pixel(10, 10, TUI_COLOUR_BLACK, 1);
    __EI();
}
```

## TUI Graphics

### Section contents
- [Graphics Enums](#graphics-enums)
- [tui_rotate_point](#tui_rotate_point)
- [tui_simple_line](#tui_simple_line)
- [tui_advanced_draw_line](#tui_advanced_draw_line)
- [tui_pattern_draw_line](#tui_pattern_draw_line)
- [tui_render_buffer](#tui_render_buffer)
- [tui_render_adr](#tui_render_adr)
- [tui_clear_screen](#tui_clear_screen)
- [tui_get_pixel_b](#tui_get_pixel_b)
- [tui_get_pixel](#tui_get_pixel)
- [tui_set_pixel](#tui_set_pixel)
- [tui_circle](#tui_circle)
- [tui_draw_line](#tui_draw_line)
- [tui_draw_rectangle](#tui_draw_rectangle)
- [tui_draw_points](#tui_draw_points)
- [tui_draw_circle](#tui_draw_circle)
- [tui_get_font_size](#tui_get_font_size)
- [tui_get_text_size](#tui_get_text_size)
- [tui_draw_text](#tui_draw_text)
- [tui_draw_byte](#tui_draw_byte)
- [tui_draw_image](#tui_draw_image)
- [tui_draw_char](#tui_draw_char)
- [tui_draw_full_image](#tui_draw_full_image)
- [tui_invert_area](#tui_invert_area)

### Graphics Enums

#### tui_colour_t

| Value | Meaning |
| --- | --- |
| `TUI_COLOUR_WHITE` | clear pixel |
| `TUI_COLOUR_LIGHT_GREY` | non CWX only |
| `TUI_COLOUR_DARK_GREY` | non CWX only |
| `TUI_COLOUR_BLACK` | set pixel |
| `TUI_COLOUR_IMAGE` | image mode color, equals black on CWX |

#### tui_style_t
This enum exists in the header but is currently marked as not really in use.

| Value |
| --- |
| `TUI_STYLE_NONE` |
| `TUI_STYLE_SOLID` |
| `TUI_STYLE_DOTTED` |
| `TUI_STYLE_DASHED` |
| `TUI_STYLE_DOUBLE` |

#### tui_line_style_t

| Value | Pattern byte |
| --- | --- |
| `TUI_LINE_STYLE_NONE` | `0x00` |
| `TUI_LINE_STYLE_SOLID` | `0xFF` |
| `TUI_LINE_STYLE_DOTTED` | `0xAA` |
| `TUI_LINE_STYLE_DASHED` | `0xF8` |

#### tui_font_t

| Value |
| --- |
| `TUI_FONT_SIZE_6x7` |
| `TUI_FONT_SIZE_6x8` |
| `TUI_FONT_SIZE_6x10` |
| `TUI_FONT_SIZE_7x10` |
| `TUI_FONT_SIZE_8x8` |
| `TUI_FONT_SIZE_8x12` |
| `TUI_FONT_SIZE_12x16` |

#### tui_fill_style_t
This enum is public but is not consumed by the current drawing API in `libcw.c`.

| Value |
| --- |
| `TUI_FILL_STYLE_NONE` |
| `TUI_FILL_STYLE_SOLID` |
| `TUI_FILL_STYLE_CHECKERED` |
| `TUI_FILL_STYLE_GRADIENT` |
| `TUI_FILL_STYLE_DITHER` |

Enum example:

```c
static void graphics_enum_example(void)
{
    tui_colour_t colour = TUI_COLOUR_BLACK;
    tui_line_style_t line = TUI_LINE_STYLE_DASHED;
    tui_font_t font = TUI_FONT_SIZE_6x8;
    tui_fill_style_t fill = TUI_FILL_STYLE_DITHER;
    tui_style_t style = TUI_STYLE_SOLID;

    if (colour == TUI_COLOUR_BLACK && line == TUI_LINE_STYLE_DASHED)
    {
        tui_draw_text(4, 4, "UI", font, 0, 0, 0, colour);
    }

    if (fill == TUI_FILL_STYLE_DITHER && style == TUI_STYLE_SOLID)
    {
        tui_draw_line(4, 14, 40, 14, colour, 1, TUI_LINE_STYLE_SOLID);
    }
}
```

### tui_rotate_point
Signature:

```c
void tui_rotate_point(byte ax, byte ay, byte px, byte py, word angle, byte *out_x, byte *out_y);
```

Rotates point `(px, py)` around anchor `(ax, ay)`.

Example:

```c
static void rotate_point_example(void)
{
    byte out_x;
    byte out_y;

    tui_rotate_point(32, 32, 48, 32, 90, &out_x, &out_y);
}
```

### tui_simple_line
Signature:

```c
void tui_simple_line(byte x0, byte y0, byte x1, byte y1, tui_colour_t colour);
```

Draws a one pixel Bresenham line.

Example:

```c
static void simple_line_example(void)
{
    tui_simple_line(0, 0, 191, 63, TUI_COLOUR_BLACK);
}
```

### tui_advanced_draw_line
Signature:

```c
void tui_advanced_draw_line(byte *data, byte bit_length, byte x0, byte y0, byte x1, byte y1, tui_colour_t colour, byte thickness);
```

Draws a line using an arbitrary bit pattern stretched across the line length.

Example:

```c
static byte advanced_bits[1] = { 0xCC };

static void advanced_line_example(void)
{
    tui_advanced_draw_line(advanced_bits, 8, 4, 20, 100, 20, TUI_COLOUR_BLACK, 2);
}
```

### tui_pattern_draw_line
Signature:

```c
void tui_pattern_draw_line(byte pattern, byte x0, byte y0, byte x1, byte y1, tui_colour_t colour, byte thickness);
```

Draws a line using an 8 bit repeating pattern.

Example:

```c
static void pattern_line_example(void)
{
    tui_pattern_draw_line(0xAA, 4, 24, 100, 24, TUI_COLOUR_BLACK, 1);
}
```

### tui_render_buffer
Signature:

```c
void tui_render_buffer(void);
```

Copies the current offscreen VRAM buffer to the real screen.

Example:

```c
static void render_buffer_example(void)
{
    Write2RealScreen = 0;
    tui_draw_text(0, 0, "buffer", TUI_FONT_SIZE_6x8, 0, 0, 0, TUI_COLOUR_BLACK);
    tui_render_buffer();
}
```

### tui_render_adr
Signature:

```c
void tui_render_adr(word adr);
```

Copies a buffer at `adr` to the real screen using the display's VRAM layout.

Example:

```c
static void render_address_example(void)
{
    tui_render_adr(VRAM);
}
```

### tui_clear_screen
Signature:

```c
void tui_clear_screen(void);
```

Clears either real VRAM or the offscreen buffer depending on `Write2RealScreen`.

Example:

```c
static void clear_screen_example(void)
{
    Write2RealScreen = 1;
    tui_clear_screen();
    Write2RealScreen = 0;
    tui_clear_screen();
}
```

### tui_get_pixel_b
Signature:

```c
byte tui_get_pixel_b(byte x, byte y, byte buf);
```

Reads a single bitplane from either the current target or a selected internal buffer.

Example:

```c
static byte read_bitplane_example(void)
{
    return tui_get_pixel_b(10, 10, 0);
}
```

### tui_get_pixel
Signature:

```c
byte tui_get_pixel(byte x, byte y);
```

Reads the full color value at a pixel position.

Example:

```c
static byte read_pixel_example(void)
{
    tui_set_pixel(8, 8, TUI_COLOUR_BLACK, 1);
    return tui_get_pixel(8, 8);
}
```

### tui_set_pixel
Signature:

```c
void tui_set_pixel(byte x, byte y, tui_colour_t colour, byte size);
```

Draws a pixel or a small filled shape depending on `size`.

Example:

```c
static void set_pixel_example(void)
{
    tui_set_pixel(20, 8, TUI_COLOUR_BLACK, 1);
    tui_set_pixel(24, 8, TUI_COLOUR_BLACK, 2);
    tui_set_pixel(30, 8, TUI_COLOUR_BLACK, 4);
}
```

### tui_circle
Signature:

```c
void tui_circle(byte centerX, byte centerY, byte radius, byte c);
```

Draws a filled circular area using the raw color byte `c`.

Example:

```c
static void circle_fill_example(void)
{
    tui_circle(40, 20, 6, TUI_COLOUR_BLACK);
}
```

### tui_draw_line
Signature:

```c
void tui_draw_line(byte x0, byte y0, byte x1, byte y1, tui_colour_t colour, byte thickness, tui_line_style_t line_style);
```

High level line drawing entry point.

Example:

```c
static void draw_line_example(void)
{
    tui_draw_line(0, 30, 120, 30, TUI_COLOUR_BLACK, 2, TUI_LINE_STYLE_DOTTED);
}
```

### tui_draw_rectangle
Signature:

```c
void tui_draw_rectangle(byte x, byte y, byte width, byte height, sbyte ax, sbyte ay, word rotation, tui_colour_t colour, byte thickness, tui_line_style_t line_style);
```

Draws an outlined rectangle, optionally rotated around the anchor offset.

Example:

```c
static void rectangle_example(void)
{
    tui_draw_rectangle(60, 24, 30, 12, 15, 6, 20, TUI_COLOUR_BLACK, 1, TUI_LINE_STYLE_SOLID);
}
```

### tui_draw_points
Signature:

```c
void tui_draw_points(byte cx, byte cy, byte x, byte y, byte thickness, tui_colour_t colour);
```

Draws the eight symmetric circle points used by the circle algorithm.

Example:

```c
static void draw_points_example(void)
{
    tui_draw_points(70, 20, 5, 3, 1, TUI_COLOUR_BLACK);
}
```

### tui_draw_circle
Signature:

```c
void tui_draw_circle(byte cx, byte cy, byte radius, sbyte ax, sbyte ay, byte thickness, tui_colour_t colour);
```

Draws a circle outline relative to an anchor offset.

Example:

```c
static void draw_circle_example(void)
{
    tui_draw_circle(90, 20, 10, 0, 0, 1, TUI_COLOUR_BLACK);
}
```

### tui_get_font_size
Signature:

```c
void tui_get_font_size(tui_font_t font, byte *width, byte *height);
```

Returns the dimensions of one character cell for a font.

Example:

```c
static void font_size_example(void)
{
    byte width;
    byte height;

    tui_get_font_size(TUI_FONT_SIZE_8x12, &width, &height);
}
```

### tui_get_text_size
Signature:

```c
void tui_get_text_size(tui_font_t font, const char *text, byte *width, byte *height);
```

Returns the width and height of a string for a specific font.

Example:

```c
static void text_size_example(void)
{
    byte width;
    byte height;

    tui_get_text_size(TUI_FONT_SIZE_6x8, "Menu", &width, &height);
}
```

### tui_draw_text
Signature:

```c
void tui_draw_text(byte x, byte y, const char *text, tui_font_t font, sbyte ax, sbyte ay, word rotation, tui_colour_t colour);
```

Draws a string using one of the built in bitmap fonts.

Example:

```c
static void draw_text_example(void)
{
    tui_draw_text(4, 40, "Status", TUI_FONT_SIZE_6x8, 0, 0, 0, TUI_COLOUR_BLACK);
}
```

### tui_draw_byte
Signature:

```c
void tui_draw_byte(byte x, byte y, byte data, byte data2, byte mask);
```

Low level byte blit that writes one 8 pixel span with masking.

Example:

```c
static void draw_byte_example(void)
{
    tui_draw_byte(16, 48, 0xF0, 0x0F, 0xFF);
}
```

### tui_draw_image
Signature:

```c
void tui_draw_image(byte x, byte y, byte width, byte height, const byte *bitmap, sbyte ax, sbyte ay, word rotation, tui_colour_t colour);
```

Draws a bitmap image with optional rotation.

Example:

```c
static const byte image8x8[8] = {
    0x3C,
    0x42,
    0xA5,
    0x81,
    0xA5,
    0x99,
    0x42,
    0x3C
};

static void draw_image_example(void)
{
    tui_draw_image(110, 8, 8, 8, image8x8, 0, 0, 0, TUI_COLOUR_BLACK);
}
```

### tui_draw_char
Signature:

```c
void tui_draw_char(byte x, byte y, char c, tui_font_t font, sbyte ax, sbyte ay, word rotation, tui_colour_t colour);
```

Draws one character from the built in font tables.

Example:

```c
static void draw_char_example(void)
{
    tui_draw_char(124, 8, 'A', TUI_FONT_SIZE_8x12, 0, 0, 0, TUI_COLOUR_BLACK);
}
```

### tui_draw_full_image
Signature:

```c
void tui_draw_full_image(const word *bitmap, tui_colour_t colour);
```

Draws a full screen image using the display's packed word layout.

Example:

```c
static word full_image[2048];

static void draw_full_image_example(void)
{
    word i;

    for (i = 0; i < 2048; i++)
    {
        full_image[i] = 0;
    }

    tui_draw_full_image(full_image, TUI_COLOUR_IMAGE);
}
```

### tui_invert_area
Signature:

```c
void tui_invert_area(byte x, byte y, byte width, byte height, sbyte ax, sbyte ay, word rotation);
```

Inverts the pixels in a rectangle, with optional rotation.

Example:

```c
static void invert_area_example(void)
{
    tui_draw_text(10, 54, "SELECT", TUI_FONT_SIZE_6x8, 0, 0, 0, TUI_COLOUR_BLACK);
    tui_invert_area(8, 52, 48, 10, 0, 0, 0);
}
```

## Heap Management

### Section contents
- [block_t](#block_t)
- [hinit](#hinit)
- [hmerge](#hmerge)
- [halloc](#halloc)
- [hcalloc](#hcalloc)
- [hrealloc](#hrealloc)
- [hfree](#hfree)

### block_t
`block_t` is the public heap block header structure.

```c
typedef struct block {
    word size;
    struct block *next;
    byte free;
} block_t;
```

Example:

```c
static void block_type_example(void)
{
    block_t *head = (block_t *)HEAP_START_ADDR;
    word first_block_size = head->size;

    if (first_block_size)
    {
        head->free = head->free;
    }
}
```

### hinit
Signature:

```c
void hinit(void);
```

Initializes the heap region with one free block.

Example:

```c
static void heap_init_example(void)
{
    hinit();
}
```

### hmerge
Signature:

```c
void hmerge(void);
```

Merges adjacent free blocks.

Example:

```c
static void heap_merge_example(void)
{
    void *a;
    void *b;

    hinit();
    a = halloc(16);
    b = halloc(16);
    hfree(a);
    hfree(b);
    hmerge();
}
```

### halloc
Signature:

```c
void *halloc(word size);
```

Allocates `size` bytes from the heap and returns a user pointer or `0` on failure.

Example:

```c
static void heap_alloc_example(void)
{
    byte *buffer = (byte *)halloc(32);

    if (buffer)
    {
        buffer[0] = 1;
        buffer[1] = 2;
    }
}
```

### hcalloc
Signature:

```c
void *hcalloc(word num, word size);
```

Allocates and zeroes `num * size` bytes.

Example:

```c
static void heap_calloc_example(void)
{
    word *values = (word *)hcalloc(8, sizeof(word));

    if (values)
    {
        values[3] = 99;
    }
}
```

### hrealloc
Signature:

```c
void *hrealloc(void *ptr, word size);
```

Allocates a larger block when needed, copies the old contents, and returns the new pointer.

Example:

```c
static void heap_realloc_example(void)
{
    byte *buffer = (byte *)halloc(8);

    if (buffer)
    {
        buffer[0] = 10;
        buffer = (byte *)hrealloc(buffer, 20);
        if (buffer)
        {
            buffer[8] = 20;
        }
    }
}
```

### hfree
Signature:

```c
void hfree(void *ptr);
```

Marks a previously allocated block as free.

Example:

```c
static void heap_free_example(void)
{
    void *buffer = halloc(12);

    if (buffer)
    {
        hfree(buffer);
    }
}
```

## Filesystem

### Section contents
- [fs_perms_t](#fs_perms_t)
- [fs_node_t](#fs_node_t)
- [fs_extent_t](#fs_extent_t)
- [Filesystem Layout Macros](#filesystem-layout-macros)
- [fs_init](#fs_init)
- [fs_create_file](#fs_create_file)
- [fs_create_directory](#fs_create_directory)
- [fs_get_node_from_path](#fs_get_node_from_path)
- [fs_delete_node](#fs_delete_node)
- [fs_move_node](#fs_move_node)
- [fs_rename_node](#fs_rename_node)
- [fs_chmod_node](#fs_chmod_node)
- [fs_write_file](#fs_write_file)
- [fs_read_file](#fs_read_file)
- [fs_mkdir](#fs_mkdir)
- [fs_touch](#fs_touch)
- [fs_lookup](#fs_lookup)
- [fs_dir_lookup](#fs_dir_lookup)
- [fs_read](#fs_read)
- [fs_write](#fs_write)
- [fs_list_dir](#fs_list_dir)

### fs_perms_t
`fs_perms_t` stores file permissions and directory state in one byte.

```c
typedef union fs_perms {
    byte raw;
    struct {
        byte read:1;
        byte write:1;
        byte execute:1;
        byte is_directory:1;
        byte reserved:4;
    } field;
} fs_perms_t;
```

Example:

```c
static void fs_perms_type_example(void)
{
    fs_perms_t perms = PERMS_RW;
    perms.field.execute = 1;
}
```

### fs_node_t
`fs_node_t` represents a file or directory entry.

```c
typedef struct fs_node {
    fs_perms_t perms;
    char name[FS_NAME_MAX_LEN];
    byte parent;
    byte first_child;
    byte next_sibling;
    word size;
    word data_offset;
} fs_node_t;
```

Example:

```c
static void fs_node_type_example(void)
{
    fs_node_t *root = FS_ROOT;
    byte has_children = root->first_child != FS_INVALID_IDX;

    if (has_children)
    {
        root->size = root->size;
    }
}
```

### fs_extent_t
`fs_extent_t` is the public data pool extent header used by the filesystem allocator.

```c
typedef struct fs_extent {
    word size;
    word next;
} fs_extent_t;
```

Example:

```c
static void fs_extent_type_example(void)
{
    fs_extent_t *first_extent = (fs_extent_t *)FS_DATA_POOL;
    word available = first_extent->size;

    if (available)
    {
        first_extent->next = first_extent->next;
    }
}
```

### Filesystem Layout Macros

| Macro | Meaning |
| --- | --- |
| `FS_FREE_LIST` | word at the filesystem start that stores the free list head offset |
| `FS_NODES` | node table base pointer |
| `FS_NODE_TABLE_SIZE` | total byte size of the node table |
| `FS_DATA_POOL` | file data pool base pointer |
| `FS_DATA_POOL_SIZE` | total byte size of the file data pool |

Example:

```c
static void fs_layout_macro_example(void)
{
    word first_free = FS_FREE_LIST;
    fs_node_t *nodes = FS_NODES;
    byte *pool = FS_DATA_POOL;

    if (first_free != FS_NULL_OFFSET && nodes && pool)
    {
        first_free = first_free;
    }
}
```

### fs_init
Signature:

```c
void fs_init(void);
```

Initializes the node table, root node, and free list.

Example:

```c
static void fs_init_example(void)
{
    fs_init();
}
```

### fs_create_file
Signature:

```c
fs_node_t *fs_create_file(fs_node_t *parent, const char *name, fs_perms_t perms);
```

Creates a file node under `parent`.

Example:

```c
static void fs_create_file_example(void)
{
    fs_node_t *file = fs_create_file(FS_ROOT, "notes", PERMS_RW);

    if (file)
    {
        file->name[0] = 'n';
    }
}
```

### fs_create_directory
Signature:

```c
fs_node_t *fs_create_directory(fs_node_t *parent, const char *name, fs_perms_t perms);
```

Creates a directory node under `parent`.

Example:

```c
static void fs_create_directory_example(void)
{
    fs_create_directory(FS_ROOT, "apps", PERMS_RWX);
}
```

### fs_get_node_from_path
Signature:

```c
fs_node_t *fs_get_node_from_path(const char *path, fs_node_t *start);
```

Resolves an absolute or relative path to a node.

Example:

```c
static fs_node_t *fs_get_node_example(void)
{
    fs_mkdir(FS_ROOT, "/cfg", PERMS_RWX);
    return fs_get_node_from_path("/cfg", FS_ROOT);
}
```

### fs_delete_node
Signature:

```c
byte fs_delete_node(fs_node_t *node);
```

Deletes a file or directory subtree, except the root node.

Example:

```c
static byte fs_delete_example(void)
{
    fs_node_t *temp = fs_create_file(FS_ROOT, "temp", PERMS_RW);

    if (!temp)
    {
        return 0;
    }

    return fs_delete_node(temp);
}
```

### fs_move_node
Signature:

```c
byte fs_move_node(fs_node_t *node, fs_node_t *new_parent);
```

Moves a node to a different directory.

Example:

```c
static byte fs_move_example(void)
{
    fs_node_t *src_dir = fs_create_directory(FS_ROOT, "src", PERMS_RWX);
    fs_node_t *dst_dir = fs_create_directory(FS_ROOT, "dst", PERMS_RWX);
    fs_node_t *file;

    if (!src_dir || !dst_dir)
    {
        return 0;
    }

    file = fs_create_file(src_dir, "item", PERMS_RW);
    if (!file)
    {
        return 0;
    }

    return fs_move_node(file, dst_dir);
}
```

### fs_rename_node
Signature:

```c
byte fs_rename_node(fs_node_t *node, const char *new_name);
```

Renames a node within its current parent directory.

Example:

```c
static byte fs_rename_example(void)
{
    fs_node_t *file = fs_create_file(FS_ROOT, "old", PERMS_RW);

    if (!file)
    {
        return 0;
    }

    return fs_rename_node(file, "new");
}
```

### fs_chmod_node
Signature:

```c
byte fs_chmod_node(fs_node_t *node, fs_perms_t new_perms);
```

Replaces a node's permission byte.

Example:

```c
static byte fs_chmod_example(void)
{
    fs_node_t *file = fs_create_file(FS_ROOT, "ro", PERMS_RW);

    if (!file)
    {
        return 0;
    }

    return fs_chmod_node(file, PERMS_R);
}
```

### fs_write_file
Signature:

```c
byte fs_write_file(fs_node_t *file, const void *data, word size);
```

Writes a buffer directly to a file node.

Example:

```c
static const byte save_data[4] = { 1, 2, 3, 4 };

static byte fs_write_file_example(void)
{
    fs_node_t *file = fs_create_file(FS_ROOT, "save", PERMS_RW);

    if (!file)
    {
        return 0;
    }

    return fs_write_file(file, save_data, 4);
}
```

### fs_read_file
Signature:

```c
word fs_read_file(fs_node_t *file, void *buffer, word buffer_size);
```

Reads file contents from a node into a caller supplied buffer.

Example:

```c
static word fs_read_file_example(void)
{
    byte buffer[4];
    fs_node_t *file = fs_create_file(FS_ROOT, "read", PERMS_RW);

    if (!file)
    {
        return 0;
    }

    fs_write_file(file, save_data, 4);
    return fs_read_file(file, buffer, 4);
}
```

### fs_mkdir
Signature:

```c
fs_node_t *fs_mkdir(fs_node_t *parent, const char *path, fs_perms_t perms);
```

Creates a directory path, creating missing intermediate directories.

Example:

```c
static void fs_mkdir_example(void)
{
    fs_mkdir(FS_ROOT, "/usr/data", PERMS_RWX);
}
```

### fs_touch
Signature:

```c
fs_node_t *fs_touch(fs_node_t *parent, const char *path, fs_perms_t perms);
```

Creates a file path, creating missing intermediate directories as needed.

Example:

```c
static void fs_touch_example(void)
{
    fs_touch(FS_ROOT, "/usr/data/log", PERMS_RW);
}
```

### fs_lookup
Signature:

```c
fs_node_t *fs_lookup(fs_node_t *parent, const char *path);
```

Resolves a path and returns it only if it is a file.

Example:

```c
static fs_node_t *fs_lookup_example(void)
{
    fs_touch(FS_ROOT, "/tmp/out", PERMS_RW);
    return fs_lookup(FS_ROOT, "/tmp/out");
}
```

### fs_dir_lookup
Signature:

```c
fs_node_t *fs_dir_lookup(fs_node_t *parent, const char *path);
```

Resolves a path and returns it only if it is a directory.

Example:

```c
static fs_node_t *fs_dir_lookup_example(void)
{
    fs_mkdir(FS_ROOT, "/tmp", PERMS_RWX);
    return fs_dir_lookup(FS_ROOT, "/tmp");
}
```

### fs_read
Signature:

```c
word fs_read(fs_node_t *parent, const char *path, void *buffer, word buffer_size);
```

Reads a file by path relative to `parent`.

Example:

```c
static word fs_read_example(void)
{
    byte buffer[4];

    fs_touch(FS_ROOT, "/save/a", PERMS_RW);
    fs_write(FS_ROOT, "/save/a", save_data, 4);
    return fs_read(FS_ROOT, "/save/a", buffer, 4);
}
```

### fs_write
Signature:

```c
byte fs_write(fs_node_t *parent, const char *path, const void *data, word size);
```

Writes a file by path relative to `parent`.

Example:

```c
static byte fs_write_example(void)
{
    fs_touch(FS_ROOT, "/cfg/key", PERMS_RW);
    return fs_write(FS_ROOT, "/cfg/key", save_data, 4);
}
```

### fs_list_dir
Signature:

```c
byte fs_list_dir(fs_node_t *dir, fs_node_t **out_list, byte max_count);
```

Lists a directory's immediate children.

Example:

```c
static byte fs_list_dir_example(void)
{
    fs_node_t *items[4];

    fs_create_file(FS_ROOT, "a", PERMS_RW);
    fs_create_file(FS_ROOT, "b", PERMS_RW);
    return fs_list_dir(FS_ROOT, items, 4);
}
```

## Time And RTC

### Section contents
- [format_t Values](#format_t-values)
- [get_time_string](#get_time_string)
- [rtc_reset](#rtc_reset)
- [rtc_enable](#rtc_enable)
- [rtc_disable](#rtc_disable)
- [rtc_set_time](#rtc_set_time)
- [rtc_set_seconds](#rtc_set_seconds)
- [rtc_set_minutes](#rtc_set_minutes)
- [rtc_set_hours](#rtc_set_hours)
- [rtc_get_time](#rtc_get_time)
- [rtc_get_seconds](#rtc_get_seconds)
- [rtc_get_minutes](#rtc_get_minutes)
- [rtc_get_hours](#rtc_get_hours)
- [rtc_set_date](#rtc_set_date)
- [rtc_set_day](#rtc_set_day)
- [rtc_set_month](#rtc_set_month)
- [rtc_set_year](#rtc_set_year)
- [rtc_get_date](#rtc_get_date)
- [rtc_get_day](#rtc_get_day)
- [rtc_get_month](#rtc_get_month)
- [rtc_get_year](#rtc_get_year)
- [delay_ms](#delay_ms)
- [delay_s](#delay_s)
- [ms_to_ticks](#ms_to_ticks)
- [s_to_ticks](#s_to_ticks)
- [ticks_to_ms](#ticks_to_ms)
- [ticks_to_s](#ticks_to_s)

### format_t Values

| Value | Output |
| --- | --- |
| `TIME_FORMAT_24H` | `HH:MM` |
| `TIME_FORMAT_12H` | `HH:MM AM` or `HH:MM PM` |
| `TIME_FORMAT_24H_WITH_SECONDS` | `HH:MM:SS` |
| `TIME_FORMAT_12H_WITH_SECONDS` | `HH:MM:SS AM` or `HH:MM:SS PM` |

Example:

```c
static void format_value_example(void)
{
    char out[12];
    get_time_string(TIME_FORMAT_12H_WITH_SECONDS, out);
}
```

### get_time_string
Signature:

```c
void get_time_string(format_t format, char *out);
```

Formats the current RTC time into `out`.

Example:

```c
static void get_time_string_example(void)
{
    char out[12];
    get_time_string(TIME_FORMAT_24H, out);
}
```

### rtc_reset
Signature:

```c
void rtc_reset(void);
```

Resets the RTC registers to their default library values.

Example:

```c
static void rtc_reset_example(void)
{
    rtc_reset();
}
```

### rtc_enable
Signature:

```c
void rtc_enable(void);
```

Enables the RTC.

Example:

```c
static void rtc_enable_example(void)
{
    rtc_enable();
}
```

### rtc_disable
Signature:

```c
void rtc_disable(void);
```

Disables the RTC.

Example:

```c
static void rtc_disable_example(void)
{
    rtc_disable();
}
```

### rtc_set_time
Signature:

```c
void rtc_set_time(byte hours, byte minutes, byte seconds);
```

Writes all time fields.

Example:

```c
static void rtc_set_time_example(void)
{
    rtc_set_time(0x14, 0x37, 0x05);
}
```

### rtc_set_seconds
Signature:

```c
void rtc_set_seconds(byte seconds);
```

Writes the seconds register.

Example:

```c
static void rtc_set_seconds_example(void)
{
    rtc_set_seconds(0x30);
}
```

### rtc_set_minutes
Signature:

```c
void rtc_set_minutes(byte minutes);
```

Writes the minutes register.

Example:

```c
static void rtc_set_minutes_example(void)
{
    rtc_set_minutes(0x45);
}
```

### rtc_set_hours
Signature:

```c
void rtc_set_hours(byte hours);
```

Writes the hours register.

Example:

```c
static void rtc_set_hours_example(void)
{
    rtc_set_hours(0x09);
}
```

### rtc_get_time
Signature:

```c
void rtc_get_time(byte *hours, byte *minutes, byte *seconds);
```

Reads all time fields.

Example:

```c
static void rtc_get_time_example(void)
{
    byte hours;
    byte minutes;
    byte seconds;

    rtc_get_time(&hours, &minutes, &seconds);
}
```

### rtc_get_seconds
Signature:

```c
byte rtc_get_seconds(void);
```

Returns the seconds register.

Example:

```c
static byte rtc_get_seconds_example(void)
{
    return rtc_get_seconds();
}
```

### rtc_get_minutes
Signature:

```c
byte rtc_get_minutes(void);
```

Returns the minutes register.

Example:

```c
static byte rtc_get_minutes_example(void)
{
    return rtc_get_minutes();
}
```

### rtc_get_hours
Signature:

```c
byte rtc_get_hours(void);
```

Returns the hours register.

Example:

```c
static byte rtc_get_hours_example(void)
{
    return rtc_get_hours();
}
```

### rtc_set_date
Signature:

```c
void rtc_set_date(word year, byte month, byte day, byte week);
```

Writes date fields. The helper stores `year - 2026` in the RTC year register.

Example:

```c
static void rtc_set_date_example(void)
{
    rtc_set_date(2028, 0x03, 0x10, 0x02);
}
```

### rtc_set_day
Signature:

```c
void rtc_set_day(byte day);
```

Writes the day register.

Example:

```c
static void rtc_set_day_example(void)
{
    rtc_set_day(0x10);
}
```

### rtc_set_month
Signature:

```c
void rtc_set_month(byte month);
```

Writes the month register.

Example:

```c
static void rtc_set_month_example(void)
{
    rtc_set_month(0x03);
}
```

### rtc_set_year
Signature:

```c
void rtc_set_year(word year);
```

Writes the year using the same 2026 based offset rule.

Example:

```c
static void rtc_set_year_example(void)
{
    rtc_set_year(2030);
}
```

### rtc_get_date
Signature:

```c
void rtc_get_date(word *year, byte *month, byte *day, byte *week);
```

Reads all date fields.

Example:

```c
static void rtc_get_date_example(void)
{
    word year;
    byte month;
    byte day;
    byte week;

    rtc_get_date(&year, &month, &day, &week);
}
```

### rtc_get_day
Signature:

```c
byte rtc_get_day(void);
```

Returns the day register.

Example:

```c
static byte rtc_get_day_example(void)
{
    return rtc_get_day();
}
```

### rtc_get_month
Signature:

```c
byte rtc_get_month(void);
```

Returns the month register.

Example:

```c
static byte rtc_get_month_example(void)
{
    return rtc_get_month();
}
```

### rtc_get_year
Signature:

```c
word rtc_get_year(void);
```

Returns the year with the 2026 base restored.

Example:

```c
static word rtc_get_year_example(void)
{
    return rtc_get_year();
}
```

### delay_ms
Signature:

```c
void delay_ms(word ms);
```

Blocks for approximately `ms` milliseconds.

Example:

```c
static void delay_ms_example(void)
{
    delay_ms(250);
}
```

### delay_s
Signature:

```c
void delay_s(word s);
```

Blocks for approximately `s` seconds.

Example:

```c
static void delay_s_example(void)
{
    delay_s(1);
}
```

### ms_to_ticks
Signature:

```c
word ms_to_ticks(word ms);
```

Converts milliseconds to timer ticks.

Example:

```c
static word ms_to_ticks_example(void)
{
    return ms_to_ticks(25);
}
```

### s_to_ticks
Signature:

```c
word s_to_ticks(word s);
```

Converts seconds to timer ticks.

Example:

```c
static word s_to_ticks_example(void)
{
    return s_to_ticks(2);
}
```

### ticks_to_ms
Signature:

```c
word ticks_to_ms(word ticks);
```

Converts timer ticks to milliseconds.

Example:

```c
static word ticks_to_ms_example(void)
{
    return ticks_to_ms(80);
}
```

### ticks_to_s
Signature:

```c
word ticks_to_s(word ticks);
```

Converts timer ticks to seconds.

Example:

```c
static word ticks_to_s_example(void)
{
    return ticks_to_s(16000);
}
```

## Keyboard Input

### Section contents
- [CheckButtons](#checkbuttons)
- [button_t](#button_t)
- [special_char_t](#special_char_t)
- [shift_special_t](#shift_special_t)

### CheckButtons
Signature:

```c
byte CheckButtons(void);
```

Scans the keyboard matrix and returns a key code or `0xFF` when no key is active.

Example:

```c
static void check_buttons_example(void)
{
    byte key = CheckButtons();

    if (key != 0xFF)
    {
        tui_draw_char(150, 8, 'K', TUI_FONT_SIZE_6x8, 0, 0, 0, TUI_COLOUR_BLACK);
    }
}
```

### button_t
`button_t` maps key positions to matrix scan codes.

| Name | Value | Name | Value |
| --- | --- | --- | --- |
| `B_0` | `0x0B` | `B_1` | `0x3F` |
| `B_2` | `0x37` | `B_3` | `0x2F` |
| `B_4` | `0x3E` | `B_5` | `0x36` |
| `B_6` | `0x2E` | `B_7` | `0x3D` |
| `B_8` | `0x35` | `B_9` | `0x2D` |
| `B_A` | `0x3C` | `B_B` | `0x34` |
| `B_C` | `0x2C` | `B_D` | `0x24` |
| `B_E` | `0x1C` | `B_F` | `0x14` |
| `B_G` | `0x3D` | `B_H` | `0x35` |
| `B_I` | `0x2D` | `B_J` | `0x25` |
| `B_K` | `0x1D` | `B_L` | `0x3E` |
| `B_M` | `0x36` | `B_N` | `0x2E` |
| `B_O` | `0x26` | `B_P` | `0x1E` |
| `B_Q` | `0x3F` | `B_R` | `0x37` |
| `B_S` | `0x2F` | `B_T` | `0x27` |
| `B_U` | `0x1F` | `B_V` | `0x0B` |
| `B_W` | `0x0C` | `B_X` | `0x0D` |
| `B_Y` | `0x0E` | `B_Z` | `0x0F` |
| `BUTTON_COUNT` | `0x40` |  |  |

Some names intentionally share the same physical scan code.

Example:

```c
static byte is_digit_or_letter(void)
{
    byte key = CheckButtons();
    return key == B_1 || key == B_A || key == B_Z;
}
```

### special_char_t
`special_char_t` defines scan codes for non alphanumeric keys.

| Name | Value | Name | Value |
| --- | --- | --- | --- |
| `SP_HOME` | `0x30` | `SP_UP` | `0x20` |
| `SP_PGUP` | `0x10` | `SP_SETTINGS` | `0x39` |
| `SP_BACK` | `0x31` | `SP_LEFT` | `0x29` |
| `SP_OKAY` | `0x21` | `SP_RIGHT` | `0x19` |
| `SP_PGDOWN` | `0x11` | `SP_SHIFT` | `0x3A` |
| `SP_VAR` | `0x32` | `SP_FUNC` | `0x2A` |
| `SP_DOWN` | `0x22` | `SP_CATALOG` | `0x1A` |
| `SP_TOOlS` | `0x12` | `SP_X` | `0x3B` |
| `SP_FRAC` | `0x33` | `SP_SQRT` | `0x2B` |
| `SP_POWER` | `0x23` | `SP_SQUARED` | `0x1B` |
| `SP_LOGAB` | `0x13` | `SP_ANS` | `0x3C` |
| `SP_SIN` | `0x34` | `SP_COS` | `0x2C` |
| `SP_TAN` | `0x24` | `SP_LEFT_PAREN` | `0x1C` |
| `SP_RIGHT_PAREN` | `0x14` | `SP_DEL` | `0x25` |
| `SP_AC` | `0x1D` | `SP_MUL` | `0x26` |
| `SP_DIV` | `0x1E` | `SP_PLUS` | `0x27` |
| `SP_MINUS` | `0x1F` | `SP_DOT` | `0x0C` |
| `SP_SCI` | `0x0D` | `SP_FORMAT` | `0x0E` |
| `SP_EXE` | `0x0F` |  |  |

Example:

```c
static byte is_navigation_key(void)
{
    byte key = CheckButtons();
    return key == SP_UP || key == SP_DOWN || key == SP_LEFT || key == SP_RIGHT;
}
```

### shift_special_t
`shift_special_t` defines shifted aliases for existing key codes.

| Name | Value source | Name | Value source |
| --- | --- | --- | --- |
| `SC_QR` | `SP_X` | `SC_MIXFRAC` | `SP_FRAC` |
| `SC_NROOT` | `SP_SQRT` | `SC_INVERSE` | `SP_POWER` |
| `SC_LOG` | `SP_SQUARED` | `SC_LN` | `SP_LOGAB` |
| `SC_PREANS` | `SP_ANS` | `SC_ASIN` | `SP_SIN` |
| `SC_ACOS` | `SP_COS` | `SC_ATAN` | `SP_TAN` |
| `SC_EQUALS` | `SP_LEFT_PAREN` | `SC_COMMA` | `SP_RIGHT_PAREN` |
| `SC_PI` | `B_7` | `SC_EULER` | `B_8` |
| `SC_IMAGINE` | `B_9` | `SC_INS` | `SP_DEL` |
| `SC_OFF` | `SP_AC` | `SC_A` | `B_4` |
| `SC_B` | `B_5` | `SC_C` | `B_6` |
| `SC_D` | `B_1` | `SC_E` | `B_2` |
| `SC_F` | `B_3` | `SC_DMS` | `SP_PLUS` |
| `SC_NEG` | `SP_MINUS` | `SC_X` | `B_0` |
| `SC_Y` | `SP_DOT` | `SC_Z` | `SP_SCI` |
| `SC_ESTIMATE` | `SP_EXE` |  |  |

Example:

```c
static byte is_shift_alias(byte key)
{
    return key == SC_PI || key == SC_EQUALS || key == SC_ESTIMATE;
}
```

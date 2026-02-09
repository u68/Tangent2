/*
 * libcw.h version 1.3
 * A compilation of things you might want for CW dev.
 * Contians: 
 *  - Type definitions 
 *  - Hardware register definitions
 *  - Useful macros
 *  - Stack/heap implementation
 *  - Graphics library functions
 *  - File system implementation
 *  - RTC functions
 *  - Key detection/conversion (only cwii for conversion right now)
 * 
 * Created on: Feb 4 2026
 * Copyright (C) 2026 u68
 */

#ifndef LIBCW_H_
#define LIBCW_H_

#define LIBCW_VERSION "1.3"

 // Telecomadm1145 did most of this sfr stuff
 typedef unsigned char byte;
 typedef signed char sbyte;
 typedef unsigned short word;
 typedef signed short sword;
 typedef unsigned long dword;
 typedef signed long sdword;

// Everything

typedef struct fs_node fs_node_t;
typedef union fs_perms fs_perms_t;
typedef enum time_format format_t;

void tui_rotate_point(byte ax, byte ay, byte px, byte py, word angle, byte *out_x, byte *out_y);
void tui_simple_line(byte x0, byte y0, byte x1, byte y1, byte colour);
void tui_advanced_draw_line(byte* data, byte bit_length, byte x0, byte y0, byte x1, byte y1, byte colour, byte thickness);
void tui_pattern_draw_line(byte pattern, byte x0, byte y0, byte x1, byte y1, byte colour, byte thickness);
void tui_render_buffer(void);
void tui_clear_screen(void);
byte tui_get_pixel_b(byte x, byte y, byte buf);
byte tui_get_pixel(byte x, byte y);
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
int abs(int x);
void hinit(void);
void hmerge(void);
void *halloc(word size);
void *hcalloc(word num, word size);
void *hrealloc(void *ptr, word size);
void hfree(void *ptr);
void fs_init(void);
fs_node_t *fs_create_file(fs_node_t *parent, const char *name, fs_perms_t perms);
fs_node_t *fs_create_directory(fs_node_t *parent, const char *name, fs_perms_t perms);
fs_node_t *fs_get_node_from_path(const char *path, fs_node_t *start);
byte fs_delete_node(fs_node_t *node);
byte fs_move_node(fs_node_t *node, fs_node_t *new_parent);
byte fs_rename_node(fs_node_t *node, const char *new_name);
byte fs_chmod_node(fs_node_t *node, fs_perms_t new_perms);
byte fs_write_file(fs_node_t *file, const void *data, word size);
word fs_read_file(fs_node_t *file, void *buffer, word buffer_size);
fs_node_t *fs_mkdir(fs_node_t *parent, const char *path, fs_perms_t perms);
fs_node_t *fs_touch(fs_node_t *parent, const char *path, fs_perms_t perms);
fs_node_t *fs_lookup(fs_node_t *parent, const char *path);
fs_node_t *fs_dir_lookup(fs_node_t *parent, const char *path);
word fs_read(fs_node_t *parent, const char *path, void *buffer, word buffer_size);
byte fs_write(fs_node_t *parent, const char *path, const void *data, word size);
void get_time_string(format_t format, char* out);
void rtc_reset(void);
void rtc_enable(void);
void rtc_disable(void);
void rtc_set_time(byte hours, byte minutes, byte seconds);
void rtc_set_seconds(byte seconds);
void rtc_set_minutes(byte minutes);
void rtc_set_hours(byte hours);
void rtc_get_time(byte* hours, byte* minutes, byte* seconds);
byte rtc_get_seconds();
byte rtc_get_minutes();
byte rtc_get_hours();
void rtc_set_date(word year, byte month, byte day, byte week);
void rtc_set_day(byte day);
void rtc_set_month(byte month);
void rtc_set_year(word year);
void rtc_get_date(word* year, byte* month, byte* day, byte* week);
byte rtc_get_day();
byte rtc_get_month();
word rtc_get_year();
void delay_ms(word ms);
void delay_s(word s);
word ms_to_ticks(word ms);
word s_to_ticks(word s);
word ticks_to_ms(word ticks);
word ticks_to_s(word ticks);

// Settings
#ifdef IS_CWX
#   define VRAM 0xD000
#   define Write2RealScreen *((volatile __near byte *)(VRAM + 0x602))
#   define HEAP_START_ADDR (VRAM + 0x604)
#   define HEAP_MAX_SIZE 0xA00
#   define FS_MAX_SIZE 0x1000
#   define FS_MAX_NODES 64
#else
#   define VRAM 0x9000
#   define Write2RealScreen *((volatile __near byte *)(VRAM + 0xC02))
#   define HEAP_START_ADDR (VRAM + 0xC04)
#   define HEAP_MAX_SIZE 0x1400
#   define FS_MAX_SIZE 0x3800
#   define FS_MAX_NODES 255
#endif

#define HEAP_BLOCK_ALIGN 2

// File system
#define FS_START_ADDR (HEAP_START_ADDR + HEAP_MAX_SIZE)
#define FS_NAME_MAX_LEN 12
#define FS_INVALID_IDX 0xFF
#define FS_NULL_OFFSET 0xFFFF

// Perm presets
#define PERMS_RWX  ((fs_perms_t){ .field.read=1, .field.write=1, .field.execute=1 })
#define PERMS_RW   ((fs_perms_t){ .field.read=1, .field.write=1, .field.execute=0 })
#define PERMS_R    ((fs_perms_t){ .field.read=1, .field.write=0, .field.execute=0 })
#define PERMS_WX   ((fs_perms_t){ .field.read=0, .field.write=1, .field.execute=1 })
#define PERMS_W    ((fs_perms_t){ .field.read=0, .field.write=1, .field.execute=0 })
#define PERMS_X    ((fs_perms_t){ .field.read=0, .field.write=0, .field.execute=1 })
#define PERMS_RX   ((fs_perms_t){ .field.read=1, .field.write=0, .field.execute=1 })
#define PERMS_NONE ((fs_perms_t){ .field.read=0, .field.write=0, .field.execute=0 })

// Rewt
#define FS_ROOT (&FS_NODES[0])

// Time
#define RTC_ENABLE *((volatile __near byte *)0xF0C7)
#define RTC_SECONDS *((volatile __near byte *)0xF0C0)
#define RTC_MINUTES *((volatile __near byte *)0xF0C1)
#define RTC_HOURS *((volatile __near byte *)0xF0C2)
#define RTC_DAY *((volatile __near byte *)0xF0C3) // Note: ML620909 RTC clock does not use crystal oscillator.
#define RTC_WEEK *((volatile __near byte *)0xF0C4) // Therefore, it may drift significantly over time (~2 minutes per hour)
#define RTC_MONTH *((volatile __near byte *)0xF0C5) // It also depends on certain core configurations and battery voltage, 
#define RTC_YEAR *((volatile __near byte *)0xF0C6) // so it is advised to not use the RTC for precise timekeeping.

// Time constants
#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define HOURS_PER_DAY 24
#define DAYS_PER_WEEK 7
#define MONTHS_PER_YEAR 12

#define TICKS_PER_MS 8
#define TICKS_PER_SECOND 8000

// Continuing from Telecomadm1145's sfr stuff
 #define deref(x) (*((__near byte*)(x)))
 #define derefw(x) (*((__near word*)(x)))

 #ifndef __near
 #define __near
 #endif

 typedef unsigned short ushort;

 #define val(x) *((volatile __near byte *)x)
 #define valw(x) *((volatile __near word *)x)

 #define hw_deref(x)  (*((volatile __near byte*)(x)))
 #define hw_derefw(x) (*((volatile __near word*)(x)))
 // STPACP
 // STPACP is a write-only special function register (SFR) that is used for setting a STOP mode.
 // When STPACP is read, “0x00” is read.
 // When data is written to STPACP in the order of “0x5n”(n: an arbitrary value) and “0xAn”(n: an arbitrary value), the
 // stop code acceptor is enabled. When the STP bit of the standby control register (SBYCON) is set to “1” in this state,
 // the mode is changed to the STOP mode. When the STOP mode is set, the STOP code acceptor is disabled.
 // When another instruction is executed between the instruction that writes “0x5n” to STPACP and the instruction that
 // writes “0xAn”, the stop code acceptor is enabled after “0xAn” is written. However, if data other than “0xAn” is
 // written to STPACP after “0x5n” is written, the “0x5n” write processing becomes invalid so that data must be written
 // again starting from “0x5n”.
 // During a system reset, the stop code acceptor is disabled.
 #define StopAcceptor *((volatile __near byte *)0xF008)
 // SBYCON
 // 0b00 Program run mode (initial value)
 // 0b01 HALT mode
 // 0b10 STOP mode
 // 0b11 Prohibited
 #define StopControl *((volatile __near byte *)0xF009)
 // FCON
 // 0 OSCLK_1 OSCLK_2 OSCLK_3
 // 0 0       LSCLK_1 LSCLK_2
 // OSCLK is the clock divider.
 // Only when LSCLK = 1, low speed clock is enabled.
 #define FCON *((volatile __near byte *)0xF00A)
 #define InterruptMask0 *((volatile __near byte *)0xF010)
 #define InterruptMask1 *((volatile __near byte *)0xF011)
 #define InterruptMask_W0 *((volatile __near ushort *)0xF010)
 #define InterruptMask2 *((volatile __near byte *)0xF012)
 #define InterruptMask3 *((volatile __near byte *)0xF013)
 #define InterruptMask_W1 *((volatile __near ushort *)0xF012)
 #define InterruptPending0 *((volatile __near byte *)0xF014)
 #define InterruptPending1 *((volatile __near byte *)0xF015)
 #define InterruptPending_W0 *((volatile __near ushort *)0xF014)
 #define InterruptPending2 *((volatile __near byte *)0xF016)
 #define InterruptPending3 *((volatile __near byte *)0xF017)
 #define InterruptPending_W1 *((volatile __near ushort *)0xF016)
 // EXICON
 // Each external input uses 2 bits.
 // External input 0 is keyboard.
 // 0b00 means trigger when H level turns to L level
 // 0b01 means trigger when L level turns to H level
 // 0b10 means trigger when H level
 // 0b11 means trigger when L level
 #define ExternalInterruptControl *((volatile __near byte *)0xF018)
 // TM0D
 #define Timer0Interval *((volatile __near ushort *)0xF020)
 // TM0C
 #define Timer0Counter *((volatile __near ushort *)0xF022)
 // TM0CON
 // 0   0   0   0
 // CS3 CS2 CS1 CS0
 // 0   0   0   0
 // 0   0   0   E
 // CS0-2 mean clock divider
 // CS3 enables HSCLK
 // E: Enables the timer.
 #define Timer0Control *((volatile __near ushort *)0xF024)
 // TM0CON0
 // 0   0   0   0
 // CS3 CS2 CS1 CS0
 // CS0-2 mean clock divider
 // CS3 enables HSCLK
 #define Timer0Control0 *((volatile __near byte *)0xF024)
 // TM0CON1
 // 0 0 0 0
 // 0 0 0 E
 // E: Enables the timer.
 #define Timer0Control1 *((volatile __near byte *)0xF025)
 // BLKCON
 #define BlockControl *((volatile __near byte *)0xF028)
 // Screen display range
 #define ScreenRange *((volatile __near byte *)0xF030)
 // Screen mode
 #define ScreenMode *((volatile __near byte *)0xF031)
 #define ScreenContrast *((volatile __near byte *)0xF032)
 #define ScreenBrightness *((volatile __near byte *)0xF033)
 #define ScreenInterval *((volatile __near byte *)0xF034)
 #define ScreenUnk1 *((volatile __near byte *)0xF035)
 #define ScreenUnk2 *((volatile __near byte *)0xF036)
 #define ScreenSelect *((volatile __near byte *)0xF037)
 #define ScreenOffset *((volatile __near byte *)0xF039)
 #define ScreenPower *((volatile __near byte *)0xF03D)
 #define KeyboardIn *((volatile __near byte *)0xF040)
 #define KeyboardInPullUp *((volatile __near byte *)0xF041)
 #define KeyboardInMask *((volatile __near byte *)0xF042)
 // Although KO has 2 bytes.but only 7 bits are used. so just ignore that.
 #define KeyboardOutMask *((volatile __near byte *)0xF044)
 #define KeyboardOut *((volatile __near byte *)0xF046)

// Back to my stuff

#ifndef __DI
extern void __DI(void);
#endif
#ifndef __EI
extern void __EI(void);
#endif

#ifndef IS_CWX
#   define BufSelSFR *((volatile __near byte *)0xF037)
#endif

enum tui_colour {
	TUI_COLOUR_WHITE,
#ifndef IS_CWX
	TUI_COLOUR_LIGHT_GREY,
	TUI_COLOUR_DARK_GREY,
#endif
	TUI_COLOUR_BLACK,
#ifdef IS_CWX
	TUI_COLOUR_IMAGE = TUI_COLOUR_BLACK, // CWX is only black and white so colour image will just be the same as drawing a black image
#else
    TUI_COLOUR_IMAGE,
#endif
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
//void __tui_set_pixel_real(byte x, byte y, byte colour);
//void __tui_set_pixel(byte x, byte y, byte colour);

/*
static const byte YsFont6x7[1792];
static const byte YsFont6x8[2048];
static const byte YsFont6x10[2560];
static const byte YsFont7x10[2560];
static const byte YsFont8x8[2048];
static const byte YsFont8x12[3072];
static const byte YsFont12x16[8192];

static const sbyte tui_sin_table[360];
static const sbyte tui_cos_table[360];

static void __tui_clear_screen_real_buf_2(void);
static void __tui_clear_screen_real_buf_1(void);
static void __tui_set_pixel_real(byte x, byte y, byte colour);
static void __tui_set_pixel(byte x, byte y, byte colour);*/

typedef struct block {
    word size;
    struct block *next;
    byte free;
} block_t;

// File permissions structure (type is within fs_perms for that one extra byte of less space)
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

// Filesystem node structure
typedef struct fs_node {
    fs_perms_t perms;
    char name[FS_NAME_MAX_LEN];
    byte parent;
    byte first_child;
    byte next_sibling;
    word size;
    word data_offset;
} fs_node_t;

// Filesystem extent structure for file data chaining
typedef struct fs_extent {
    word size;
    word next;
} fs_extent_t;

// More filesystem definitions
#define FS_FREE_LIST (*(word*)FS_START_ADDR)
#define FS_NODES ((fs_node_t*)((byte*)FS_START_ADDR + sizeof(word)))
#define FS_NODE_TABLE_SIZE (sizeof(fs_node_t) * FS_MAX_NODES)
#define FS_DATA_POOL ((byte*)FS_START_ADDR + sizeof(word) + FS_NODE_TABLE_SIZE)
#define FS_DATA_POOL_SIZE (FS_MAX_SIZE - sizeof(word) - FS_NODE_TABLE_SIZE)

// Get key
byte CheckButtons();

// Button enums (Cosine)
enum BUTTON {
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

enum SPECIAL_CHARS {
	SP_HOME = 0x30,
    SP_UP = 0x20,
    SP_PGUP = 0x10,
    SP_SETTINGS = 0x39,
    SP_BACK = 0x31,
    SP_LEFT = 0x29,
    SP_OKAY = 0x21,
    SP_RIGHT = 0x19,
    SP_PGDOWN = 0x11,
    SP_SHIFT = 0x3A,
    SP_VAR = 0x32,
    SP_FUNC = 0x2A,
    SP_DOWN = 0x22,
    SP_CATALOG = 0x1A,
    SP_TOOlS = 0x12,
    SP_X = 0x3B,
    SP_FRAC = 0x33,
    SP_SQRT = 0x2B,
    SP_POWER = 0x23,
    SP_SQUARED = 0x1B,
    SP_LOGAB = 0x13,
    SP_ANS = 0x3C,
    SP_SIN = 0x34,
    SP_COS = 0x2C,
    SP_TAN = 0x24,
    SP_LEFT_PAREN = 0x1C,
    SP_RIGHT_PAREN = 0x14,

    //789

    SP_DEL = 0x25,
    SP_AC = 0x1D,

    //456

    SP_MUL = 0x26,
    SP_DIV = 0x1E,

    //123

    SP_PLUS = 0x27,
    SP_MINUS = 0x1F,

    //0

    SP_DOT = 0x0C,
    SP_SCI = 0x0D,
    SP_FORMAT = 0x0E,
    SP_EXE = 0x0F,
};

enum SHIFT_SPECIAL {
    SC_QR = SP_X,
    SC_MIXFRAC = SP_FRAC,
    SC_NROOT = SP_SQRT,
    SC_INVERSE = SP_POWER,
    SC_LOG = SP_SQUARED,
    SC_LN = SP_LOGAB,
    SC_PREANS = SP_ANS,
    SC_ASIN = SP_SIN,
    SC_ACOS = SP_COS,
    SC_ATAN = SP_TAN,
    SC_EQUALS = SP_LEFT_PAREN,
    SC_COMMA = SP_RIGHT_PAREN,
    SC_PI = B_7,
    SC_EULER = B_8,
    SC_IMAGINE = B_9,
    SC_INS = SP_DEL,
    SC_OFF = SP_AC,
    SC_A = B_4,
    SC_B = B_5,
    SC_C = B_6,
    SC_D = B_1,
    SC_E = B_2,
    SC_F = B_3,
    SC_DMS = SP_PLUS,
    SC_NEG = SP_MINUS,
    SC_X = B_0,
    SC_Y = SP_DOT,
    SC_Z = SP_SCI,
    SC_ESTIMATE = SP_EXE,
};



#endif /* LIBCW_H_ */

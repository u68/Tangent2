/*
 * bsod.c
 * Implementation of Black Screen of Death
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#include "debug.h"
#include "bsod.h"
#include "../tui/glib.h"

// Helper: convert nibble to hex char
static char nibble_to_hex(byte n) {
    if (n < 10) return '0' + n;
    return 'A' + n - 10;
}

// Helper: convert byte to hex string (2 chars)
static void byte_to_hex(byte b, char* out) {
    out[0] = nibble_to_hex((b >> 4) & 0x0F);
    out[1] = nibble_to_hex(b & 0x0F);
}

// Helper: format register line (e.g., "R00: 01 02 03 04")
static void format_reg_line(byte idx, byte* regs, char* out) {
    byte pos = 0;
    
    // Register number
    out[pos++] = 'R';
    out[pos++] = nibble_to_hex(((idx >> 4) & 0x0F)<<2);
    out[pos++] = nibble_to_hex((idx & 0x0F)<<2);
    out[pos++] = ':';
    out[pos++] = ' ';
    
    // 4 bytes in hex
    for (byte j = 0; j < 4 && (idx * 4 + j) < 24; j++) {
        byte_to_hex(regs[idx * 4 + j], out + pos);
        pos += 2;
        if (j < 3) out[pos++] = ' ';
    }
    
    out[pos] = 0;
}

// Show BSOD with error code and registers snapshot
void bsod_show(error_t error_code, byte* regs_snapshot) {
	Write2RealScreen = 1;
    char buf[32];
    
    // Fill screen with black
    for (word i = 0; i < 2048; i++) {
        BufSelSFR = 0;
        deref(0xF800 + i) = 0xFF;
        BufSelSFR = 4;
        deref(0xF800 + i) = 0xFF;
    }
    
    // Title
    tui_draw_text(60, 1, "FATAL ERROR", TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
    
    // Error message
    const char* error_str = error_to_string(error_code);
    tui_draw_text(1, 8, error_str, TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
    
    // Register header
    tui_draw_text(1, 16, "REGISTER SNAPSHOT:", TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
    
    // Display registers in rows of 4 bytes
    for (byte i = 0; i < 4; i++) {
        format_reg_line(i, regs_snapshot, buf);
        tui_draw_text(1, 24 + i * 8, buf, TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_BLACK);
    }


}

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
    out[pos++] = nibble_to_hex((idx >> 4) & 0x0F);
    out[pos++] = nibble_to_hex(idx & 0x0F);
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
    char buf[32];
    
    // Fill screen with black
    for (word i = 0; i < 2048; i++) {
        BufSelSFR = 0;
        deref(0xF800 + i) = 0x01;
        BufSelSFR = 4;
        deref(0xF800 + i) = 0x01;
    }
    
    // Draw border box
    tui_draw_rectangle(2, 1, 188, 61, 0, 0, 0, TUI_COLOUR_WHITE, 2, TUI_LINE_STYLE_SOLID);
    
    // Title line
    for (byte i = 4; i < 188; i++) {
        tui_set_pixel(i, 4, TUI_COLOUR_WHITE, 1);
    }
    
    // Title
    tui_draw_text((192 - 12 * 6) >> 1, 6, "FATAL ERROR", TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_WHITE);
    
    // Error message
    const char* error_str = error_to_string(error_code);
    tui_draw_text(6, 14, error_str, TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_WHITE);
    
    // Divider
    for (byte i = 4; i < 188; i++) {
        tui_set_pixel(i, 20, TUI_COLOUR_WHITE, 1);
    }
    
    // Register header
    tui_draw_text(6, 23, "REGISTER SNAPSHOT:", TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_WHITE);
    
    // Display registers in rows of 4 bytes
    for (byte i = 0; i < 6; i++) {
        format_reg_line(i, regs_snapshot, buf);
        tui_draw_text(6, 30 + i * 8, buf, TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_WHITE);
    }
    
    // Bottom divider
    for (byte i = 4; i < 188; i++) {
        tui_set_pixel(i, 52, TUI_COLOUR_WHITE, 1);
    }
    
    // Footer
    tui_draw_text((192 - 20 * 6) >> 1, 55, "SYSTEM HALTED", TUI_FONT_SIZE_6x7, 0, 0, 0, TUI_COLOUR_WHITE);
    
    // Infinite halt loop
    while (1) {
    }
}
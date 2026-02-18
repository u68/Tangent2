/*
 * main.c
 * keybart test
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#include "libcw.h"
#include "media/media.h"
#include "media/generated_media_asset.h"

// Custom breakpoint handler (called when BRK executed in asm)
void custom_break(void) {
	while (1) {
		deref(0x9000) += 1;
	}
}

// Help
static void itoa(word n, char* s) {
	int i = 0;
	if (n == 0) s[i++] = '0';
	else {
		while (n > 0) {
			s[i++] = (n % 10) + '0';
			n /= 10;
		}
	}
	s[i] = '\0';
	for (int j = 0; j < i / 2; j++) {
		char t = s[j];
		s[j] = s[i - j - 1];
		s[i - j - 1] = t;
	}
}

int main(void) {
	word out_size, ratio;
	byte t_bcd;
	char s_str[24], r_str[8], t_str[8];

	hinit();
	fs_init();

	// Compress
	Write2RealScreen = 0;
	tui_clear_screen();
	tui_draw_text(15, 15, "Compressing...", TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_render_buffer();
	Write2RealScreen = 1;

	RTC_SECONDS = 0;
	RTC_ENABLE = 1;
	byte *comp_data = compress_media(generated_media_raw, 0xC00, MEDIA_COMPRESS_LZMA, &out_size);
	RTC_ENABLE = 0;
	t_bcd = RTC_SECONDS;

	// Store
	fs_node_t *cfile = fs_touch(FS_ROOT, "t.tz", PERMS_RW);
	fs_write(FS_ROOT, "t.tz", comp_data, out_size);
	hfree(comp_data);

	// Show
	tui_draw_text(15, 15, "Showing...    ", TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
	show_media(FS_ROOT, "t.tz", MEDIA_COMPRESS_LZMA);

	// Stats

	// Size formatting
	itoa(out_size, s_str);
	int l = 0; while (s_str[l]) l++;
	s_str[l++] = ' '; s_str[l++] = 'b'; s_str[l++] = 'y'; s_str[l++] = 't'; s_str[l++] = 'e'; s_str[l++] = 's'; s_str[l] = 0;

	// Ratio formatting
	ratio = ((dword)out_size * 100) / 0xC00;
	itoa(ratio, r_str);
	l = 0; while (r_str[l]) l++;
	r_str[l++] = '%'; r_str[l] = 0;

	// Time formatting (BCD)
	t_str[0] = ((t_bcd >> 4) & 0xF) + '0';
	t_str[1] = (t_bcd & 0xF) + '0';
	t_str[2] = 's';
	t_str[3] = 0;

	tui_draw_text(10, 10, "Compressed:", TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(100, 10, s_str, TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(10, 21, "Ratio:", TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(100, 21, r_str, TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(10, 32, "Time:", TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
	tui_draw_text(100, 32, t_str, TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);

	return 0;
}




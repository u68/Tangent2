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
	char s_str[24], r_str[8], t_str[8], m_str[8];
	byte i;
	const media_compression_t methods[3] = {
		MEDIA_COMPRESS_RAW,
		MEDIA_COMPRESS_RLE,
		MEDIA_COMPRESS_LZMA
	};
	const char *method_names[3] = {
		"RAW",
		"RLE",
		"LZMA"
	};
	const char *file_names[3] = {
		"t.tw",
		"t.tl",
		"t.tz"
	};

	hinit();
	fs_init();

	for (i = 0; i < 3; i++) {
		byte *comp_data;
		int l;

		Write2RealScreen = 0;
		tui_clear_screen();
		tui_draw_text(15, 15, "Compressing...", TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
		tui_render_buffer();
		Write2RealScreen = 1;

		rtc_set_seconds(0);
		rtc_enable();
		comp_data = (byte*)compress_media(generated_media_raw, 0xC00, methods[i], &out_size);
		rtc_disable();
		t_bcd = rtc_get_seconds();

		if (comp_data == 0) {
			return 1;
		}

		fs_touch(FS_ROOT, file_names[i], PERMS_RW);
		if (fs_write(FS_ROOT, file_names[i], comp_data, out_size)) {
			hfree(comp_data);
			return 1;
		}
		hfree(comp_data);

		tui_draw_text(15, 15, "Showing...    ", TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
		if (show_media(FS_ROOT, file_names[i], methods[i])) {
			return 1;
		}

		itoa(out_size, s_str);
		l = 0; while (s_str[l]) l++;
		s_str[l++] = ' '; s_str[l++] = 'b'; s_str[l++] = 'y'; s_str[l++] = 't'; s_str[l++] = 'e'; s_str[l++] = 's'; s_str[l] = 0;

		ratio = ((dword)out_size * 100) / 0xC00;
		itoa(ratio, r_str);
		l = 0; while (r_str[l]) l++;
		r_str[l++] = '%'; r_str[l] = 0;

		t_str[0] = ((t_bcd >> 4) & 0xF) + '0';
		t_str[1] = (t_bcd & 0xF) + '0';
		t_str[2] = 's';
		t_str[3] = 0;

		l = 0;
		while (l < 7 && method_names[i][l]) {
			m_str[l] = method_names[i][l];
			l++;
		}
		m_str[l] = 0;

		tui_draw_text(10, 10, "Method:", TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
		tui_draw_text(100, 10, m_str, TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
		tui_draw_text(10, 21, "Compressed:", TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
		tui_draw_text(100, 21, s_str, TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
		tui_draw_text(10, 32, "Ratio:", TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
		tui_draw_text(100, 32, r_str, TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
		tui_draw_text(10, 43, "Time:", TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);
		tui_draw_text(100, 43, t_str, TUI_FONT_SIZE_7x10, 0, 0, 0, TUI_COLOUR_BLACK);

		if (i < 2) {
			delay_s(3);
		}
	}

	return 0;
}




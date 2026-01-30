/*
 * main.c
 * Implementation of you know what
 * File System test
 *  Created on: Jan 4, 2026
 *      Author: harma
 */


#include "base.h"
#include "fs/fs.h"
#include "tui/glib.h"

// Custom breakpoint handler (called when BRK executed in asm)
void custom_break(void) {
	while (1) {
		deref(0x9000) += 1;
	}
}

// Main entry point
int main(void) {
	fs_init();

    fs_node_t *cd = fs_mkdir(FS_ROOT, "/home/documents", PERMS_RW);
    fs_node_t *file = fs_create_file(cd, "test.txt", PERMS_RW);
    const char *data = "Hello, Tangent2 File System!";
    fs_write_file(file, data, sizeof(data));
	char buf[32];
    fs_read_file(file, buf, sizeof(buf));

	Write2RealScreen = 0;
	tui_clear_screen();
	tui_draw_text(15,15,buf,TUI_FONT_SIZE_6x7,0,0,0,TUI_COLOUR_BLACK);
	tui_render_buffer();

	return 0;
}


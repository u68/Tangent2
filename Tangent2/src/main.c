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

	// Permissions: read/write
	fs_perms_t dir_perms = { .raw = 0 };
	dir_perms.field.read = 1;
	dir_perms.field.write = 1;

	// Create /home
	fs_node_t *root = &FS_NODES[0];
	fs_node_t *home = fs_get_node_from_path("/home", root);
	if (!home) home = fs_create_directory(root, "home", dir_perms);

	// Create /home/documents
	fs_node_t *documents = fs_get_node_from_path("/home/documents", root);
	if (!documents) documents = fs_create_directory(home, "documents", dir_perms);

	// Create test.txt in /home/documents
	fs_perms_t file_perms = { .raw = 0 };
	file_perms.field.read = 1;
	file_perms.field.write = 1;
	fs_node_t *test_txt = fs_get_node_from_path("/home/documents/test.txt", root);
	if (!test_txt) test_txt = fs_create_file(documents, "test.txt", file_perms);

	// Write to test.txt
	const char *msg = "This is NOT a test document";
	fs_write_file(test_txt, msg, 24);

	// Read test.txt into buffer
	char buf[32] = {0};
	word read = fs_read_file(test_txt, buf, sizeof(buf));

	Write2RealScreen = 0;
	tui_clear_screen();
	tui_draw_text(15,15,buf,TUI_FONT_SIZE_6x7,0,0,0,TUI_COLOUR_BLACK);
	tui_render_buffer();

	return 0;
}


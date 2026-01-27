/*
 * fs.c
 * Implementation of filesystem operations
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#include "fs.h"

//Internals
static fs_node_t *fs_find_free_node(void) {
	return 0;
}

static word fs_allocate_data(word size) {
	return 0;
}

static void fs_free_data(word offset) {
	return;
}


static fs_node_t *fs_find_child_by_name(fs_node_t *parent, const char *name) {
	(void)parent;
	(void)name;
	return 0;
}

/* Public filesystem interface */
void fs_init(void) {
	return;
}

/* Node operations */
fs_node_t *fs_create_file(fs_node_t *parent, const char *name, fs_perms_t perms) {
	(void)parent;
	(void)name;
	(void)perms;
	return 0;
}

fs_node_t *fs_create_directory(fs_node_t *parent, const char *name, fs_perms_t perms) {
	(void)parent;
	(void)name;
	(void)perms;
	return 0;
}

fs_node_t *fs_get_node_from_path(const char *path, fs_node_t *start) {
	(void)path;
	(void)start;
	return 0;
}

/* Node manipulation */
byte fs_delete_node(fs_node_t *node) {
	(void)node;
	return 0;
}

byte fs_move_node(fs_node_t *node, fs_node_t *new_parent) {
	(void)node;
	(void)new_parent;
	return 0;
}

byte fs_rename_node(fs_node_t *node, const char *new_name) {
	(void)node;
	(void)new_name;
	return 0;
}

byte fs_chmod_node(fs_node_t *node, fs_perms_t new_perms) {
	(void)node;
	(void)new_perms;
	return 0;
}

/* File operations */
byte fs_write_file(fs_node_t *file, const void *data, word size) {
	(void)file;
	(void)data;
	(void)size;
	return 0;
}

word fs_read_file(fs_node_t *file, void *buffer, word buffer_size) {
	(void)file;
	(void)buffer;
	(void)buffer_size;
	return 0;
}


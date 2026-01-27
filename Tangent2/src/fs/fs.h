/*
 * fs.h
 * Interface for filesystem operations
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#ifndef FS_H_
#define FS_H_

#include "../base.h"
#include "../heap/heap.h"

// Filesystem layout definitions
#define FS_START_ADDR (HEAP_START_ADDR + HEAP_MAX_SIZE)
#define FS_MAX_SIZE 0x3800
#define FS_NAME_MAX_LEN 12
#define FS_INVALID_IDX 0xFF
#define FS_NULL_OFFSET 0xFFFF

// File permissions structure (type is within fs_perms for that one extra byte of less space)
typedef union fs_perms {
    byte raw;
    struct {
        byte read:1;
        byte write:1;
        byte execute:1;
        byte is_directory:1;
        byte reserved:4;
    };
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
#define FS_MAX_NODES 255
#define FS_DATA_POOL ((byte*)FS_START_ADDR + sizeof(word) + FS_NODE_TABLE_SIZE)
#define FS_DATA_POOL_SIZE (FS_MAX_SIZE - sizeof(word) - FS_NODE_TABLE_SIZE)

// Public filesystem interface
void fs_init(void);

// Node operations
fs_node_t *fs_create_file(fs_node_t *parent, const char *name, fs_perms_t perms);
fs_node_t *fs_create_directory(fs_node_t *parent, const char *name, fs_perms_t perms);
fs_node_t *fs_get_node_from_path(const char *path, fs_node_t *start);

// Node manipulation
byte fs_delete_node(fs_node_t *node);
byte fs_move_node(fs_node_t *node, fs_node_t *new_parent);
byte fs_rename_node(fs_node_t *node, const char *new_name);
byte fs_chmod_node(fs_node_t *node, fs_perms_t new_perms);

// File operations
byte fs_write_file(fs_node_t *file, const void *data, word size);
word fs_read_file(fs_node_t *file, void *buffer, word buffer_size);

#endif /* FS_H_ */
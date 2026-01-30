/*
 * fs.c
 * Implementation of filesystem operations
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#include "fs.h"

//Internals
static byte fs_node_index(fs_node_t *node) {
    return (byte)(node - FS_NODES);
}

static fs_node_t *fs_find_free_node(void) {
    for (byte i = 0; i < FS_MAX_NODES; i++) {
        if (FS_NODES[i].name[0] == 0 && FS_NODES[i].parent == FS_INVALID_IDX) {
            return &FS_NODES[i];
        }
    }
    return 0;
}

static word fs_allocate_data(word size) {
    if (size == 0 || size > FS_DATA_POOL_SIZE - sizeof(fs_extent_t)) return FS_NULL_OFFSET;

    word prev = FS_NULL_OFFSET;
    word current = FS_FREE_LIST;

    while (current != FS_NULL_OFFSET) {
        fs_extent_t *extent = (fs_extent_t *)(FS_DATA_POOL + current);

        if (extent->size >= size) {
            word data_offset = current + sizeof(fs_extent_t);

            // Ensure offset is within pool
            if (data_offset >= FS_DATA_POOL_SIZE || data_offset + size > FS_DATA_POOL_SIZE)
                return FS_NULL_OFFSET;

            // Split extent if leftover is usable
            if (extent->size >= size + sizeof(fs_extent_t) + 1) {
                word new_extent_offset = data_offset + size;
                fs_extent_t *new_extent = (fs_extent_t *)(FS_DATA_POOL + new_extent_offset);
                new_extent->size = extent->size - size - sizeof(fs_extent_t);
                new_extent->next = extent->next;

                if (prev == FS_NULL_OFFSET)
                    FS_FREE_LIST = new_extent_offset;
                else
                    ((fs_extent_t *)(FS_DATA_POOL + prev))->next = new_extent_offset;

                extent->size = size;
            } else {
                if (prev == FS_NULL_OFFSET)
                    FS_FREE_LIST = extent->next;
                else
                    ((fs_extent_t *)(FS_DATA_POOL + prev))->next = extent->next;
            }
            return data_offset;
        }
        prev = current;
        current = extent->next;
    }
    return FS_NULL_OFFSET;
}

static void fs_free_data(word data_offset) {
    if (data_offset == FS_NULL_OFFSET) return;

    word freed_offset = data_offset - sizeof(fs_extent_t);
    if (freed_offset >= FS_DATA_POOL_SIZE) return;

    fs_extent_t *freed = (fs_extent_t *)(FS_DATA_POOL + freed_offset);
    freed->next = FS_NULL_OFFSET;

    // Insert into sorted free list
    if (FS_FREE_LIST == FS_NULL_OFFSET || freed_offset < FS_FREE_LIST) {
        freed->next = FS_FREE_LIST;
        FS_FREE_LIST = freed_offset;
    } else {
        word prev = FS_FREE_LIST;
        fs_extent_t *prev_extent = (fs_extent_t *)(FS_DATA_POOL + prev);
        while (prev_extent->next != FS_NULL_OFFSET && prev_extent->next < freed_offset) {
            prev = prev_extent->next;
            prev_extent = (fs_extent_t *)(FS_DATA_POOL + prev);
        }
        freed->next = prev_extent->next;
        prev_extent->next = freed_offset;
    }

    // Merge adjacent blocks
    word current = FS_FREE_LIST;
    while (current != FS_NULL_OFFSET) {
        fs_extent_t *cur_extent = (fs_extent_t *)(FS_DATA_POOL + current);
        word next_offset = cur_extent->next;
        if (next_offset != FS_NULL_OFFSET) {
            if (next_offset > FS_DATA_POOL_SIZE) break;
            fs_extent_t *next_extent = (fs_extent_t *)(FS_DATA_POOL + next_offset);
            if ((current + sizeof(fs_extent_t) + cur_extent->size) == next_offset) {
                cur_extent->size += sizeof(fs_extent_t) + next_extent->size;
                cur_extent->next = next_extent->next;
                continue; // Check again with next
            }
        }
        current = cur_extent->next;
    }
}

static fs_node_t *fs_find_child_by_name(fs_node_t *parent, const char *name) {
    if (!parent || !parent->perms.field.is_directory || !parent->perms.field.read) return 0;
    byte idx = parent->first_child;

    while (idx != FS_INVALID_IDX) {
        fs_node_t *child = &FS_NODES[idx];

        byte match = 1;
        for (byte i = 0; i < FS_NAME_MAX_LEN; i++) {
            if (child->name[i] != name[i]) {
                match = 0;
                break;
            }
            if (child->name[i] == 0 && name[i] == 0) break;
        }

        if (match) return child;
        idx = child->next_sibling;
    }
    return 0;
}


// I hate recursion but whatever
static void fs_delete_subtree(fs_node_t *node) {
    if (!node) return;

    // Recursively delete all children
    byte child_idx = node->first_child;
    while (child_idx != FS_INVALID_IDX) {
        fs_node_t *child = &FS_NODES[child_idx];
        byte next_sibling = child->next_sibling;
        fs_delete_subtree(child);
        child_idx = next_sibling;
    }

    // Free file data
    fs_free_data(node->data_offset);

    // Clear node
    node->name[0] = 0;
    node->parent = FS_INVALID_IDX;
    node->first_child = FS_INVALID_IDX;
    node->next_sibling = FS_INVALID_IDX;
    node->size = 0;
    node->data_offset = FS_NULL_OFFSET;
    node->perms.raw = 0;
}


// Public interface

void fs_init(void) {
    // Clear all nodes
    for (byte i = 0; i < FS_MAX_NODES; i++) {
        FS_NODES[i].name[0] = 0;
        FS_NODES[i].parent = FS_INVALID_IDX;
        FS_NODES[i].first_child = FS_INVALID_IDX;
        FS_NODES[i].next_sibling = FS_INVALID_IDX;
        FS_NODES[i].size = 0;
        FS_NODES[i].data_offset = FS_NULL_OFFSET;
        FS_NODES[i].perms.raw = 0;
    }
    // Initialize root node
    fs_node_t *root = FS_ROOT;
    root->perms.field.read = 1;
    root->perms.field.write = 1;
    root->perms.field.execute = 1;
    root->perms.field.is_directory = 1;
    root->name[0] = '/';
    root->name[1] = 0;
    root->parent = FS_INVALID_IDX;
    root->first_child = FS_INVALID_IDX;
    root->next_sibling = FS_INVALID_IDX;
    // Initialize the first free extent for the data pool
    if (FS_DATA_POOL_SIZE > sizeof(fs_extent_t)) {
        fs_extent_t *e = (fs_extent_t *)FS_DATA_POOL;
        e->size = FS_DATA_POOL_SIZE - sizeof(fs_extent_t);
        e->next = FS_NULL_OFFSET;
        FS_FREE_LIST = 0;  // offset 0 relative to FS_DATA_POOL
    } else {
        FS_FREE_LIST = FS_NULL_OFFSET; // no space available
    }
}


// Insert a node as first child of parent and return pointer to it
fs_node_t *fs_create_file(fs_node_t *parent, const char *name, fs_perms_t perms) {
    // Validation of parent, name, uniqueness, and type of the parent
    if (!parent || !name[0] || !parent->perms.field.is_directory || !parent->perms.field.write || fs_find_child_by_name(parent, name)) return 0;
    
    // Find free node and index
    fs_node_t *node = fs_find_free_node();
    if (!node) return 0;
    byte idx = fs_node_index(node);

    // Initialize node
    node->perms = perms;
    node->perms.field.is_directory = 0;
    node->parent = fs_node_index(parent);
    node->first_child = FS_INVALID_IDX;
    node->next_sibling = parent->first_child;
    node->size = 0;
    node->data_offset = FS_NULL_OFFSET;

    // Copy name
    for (byte i = 0; i < FS_NAME_MAX_LEN - 1; i++) {
        node->name[i] = name[i];
        if (name[i] == 0) break;
    }
    node->name[FS_NAME_MAX_LEN - 1] = 0;

    // Insert into parent's child list
    parent->first_child = idx;
    return node;
}

// Insert a directory node as first child of parent and return pointer to it
fs_node_t *fs_create_directory(fs_node_t *parent, const char *name, fs_perms_t perms) {
    // Create a file node first and then set is_directory flag
    fs_node_t *dir = fs_create_file(parent, name, perms);
    if (!dir) return 0;
    dir->perms.field.is_directory = 1;
    return dir;
}

// Get node from path
fs_node_t *fs_get_node_from_path(const char *path, fs_node_t *start) {
    // Validate inputs
    if (!path || !start || !start->perms.field.is_directory || !start->perms.field.read) return 0;

    fs_node_t *current = (*path == '/') ? FS_ROOT : start;
    if (*path == '/') path++;

    char name[FS_NAME_MAX_LEN];

    while (*path) {
        byte i = 0;
        // Extract the next component of the path
        while (*path && *path != '/' && i < FS_NAME_MAX_LEN - 1) {
            name[i++] = *path++;
        }
        name[i] = 0;
        // Handle parent directory
        if (name[0] == '.' && name[1] == '.' && name[2] == 0) {
            if (current->parent != FS_INVALID_IDX) {
                current = &FS_NODES[current->parent];
            } // else stay at root
        } else if (name[0] != '\0' && !(name[0] == '.' && name[1] == 0)) {
            // Normal child lookup (skip "." components)
            current = fs_find_child_by_name(current, name);
            if (!current) return 0;
        }

        if (*path == '/') path++;
    }
    return current;
}

byte fs_delete_node(fs_node_t *node) {
    // Validate node
    if (!node || node == FS_ROOT || !node->perms.field.write) return 0;

    if (node->parent != FS_INVALID_IDX) {
        fs_node_t *parent = &FS_NODES[node->parent];
        byte *link = &parent->first_child;

        while (*link != FS_INVALID_IDX) {
            if (*link == fs_node_index(node)) {
                *link = node->next_sibling;
                break;
            }
            link = &FS_NODES[*link].next_sibling;
        }
    }

    fs_delete_subtree(node);
    return 1;
}

static byte fs_is_ancestor(fs_node_t *node, fs_node_t *possible_child) {
    while (possible_child != FS_ROOT) {
        if (possible_child == node) return 1;
        possible_child = &FS_NODES[possible_child->parent];
    }
    return 0;
}

byte fs_move_node(fs_node_t *node, fs_node_t *new_parent) {
    // Validate nodes
    if (!node || !new_parent || !node->perms.field.write || !new_parent->perms.field.is_directory || !new_parent->perms.field.write) return 0;
    if (node == new_parent || fs_is_ancestor(node, new_parent)) return 0;

    fs_node_t *old_parent = &FS_NODES[node->parent];
    byte *link = &old_parent->first_child;
    // Unlink node from old parent's child list
    while (*link != FS_INVALID_IDX) {
        if (*link == fs_node_index(node)) {
            *link = node->next_sibling;
            break;
        }
        link = &FS_NODES[*link].next_sibling;
    }
    // Insert node into new parent's child list
    node->parent = fs_node_index(new_parent);
    node->next_sibling = new_parent->first_child;
    new_parent->first_child = fs_node_index(node);

    return 1;
}

byte fs_rename_node(fs_node_t *node, const char *new_name) {
    // Validate node and new name
    if (!node || !new_name[0] || !node->perms.field.write) return 0;

    fs_node_t *parent = &FS_NODES[node->parent];
    // Ensure new name is unique among siblings
    if (fs_find_child_by_name(parent, new_name)) return 0;

    // Copy new name
    for (byte i = 0; i < FS_NAME_MAX_LEN - 1; i++) {
        node->name[i] = new_name[i];
        if (new_name[i] == 0) break;
    }
    node->name[FS_NAME_MAX_LEN - 1] = 0;

    return 1;
}

byte fs_chmod_node(fs_node_t *node, fs_perms_t new_perms) {
    // Validate node
    if (!node) return 0;

    node->perms = new_perms;
    return 1;
}

byte fs_write_file(fs_node_t *file, const void *data, word size) {
    if (!file || file->perms.field.is_directory || !file->perms.field.write) return 0;
    if (size > FS_DATA_POOL_SIZE - sizeof(fs_extent_t)) return 0;

    // Free existing data
    fs_free_data(file->data_offset);

    // Allocate new data
    word offset = fs_allocate_data(size);
    if (offset == FS_NULL_OFFSET) return 0;

    // Copy data
    byte *dest = FS_DATA_POOL + offset;
    const byte *src = (const byte *)data;
    for (word i = 0; i < size; i++) dest[i] = src[i];

    file->data_offset = offset;
    file->size = size;
    return 1;
}

word fs_read_file(fs_node_t *file, void *buffer, word buffer_size) {
    // Validate inputs
    if (!file || file->perms.field.is_directory || !file->perms.field.read) return 0;
    if (file->data_offset == FS_NULL_OFFSET || file->size == 0) return 0;

    word to_read = (file->size < buffer_size) ? file->size : buffer_size;
    byte *src = FS_DATA_POOL + file->data_offset;
    byte *dest = (byte *)buffer;

    for (word i = 0; i < to_read; i++) dest[i] = src[i];
    return to_read;
}

// Extra

// Create full directory path from parent, creating any missing directories
fs_node_t *fs_mkdir(fs_node_t *parent, const char *path, fs_perms_t perms) {
    if (!path || !*path || !parent || !parent->perms.field.is_directory || !parent->perms.field.write) return 0;

    // If path is 0
    if (path[0] == 0) return 0;

    // Start from root if path is absolute
    fs_node_t *current = (*path == '/') ? FS_ROOT : parent;
    if (*path == '/') path++;

    char name[FS_NAME_MAX_LEN];

    while (*path) {
        byte i = 0;
        while (*path && *path != '/' && i < FS_NAME_MAX_LEN - 1) {
            name[i++] = *path++;
        }
        name[i] = 0;

        if (name[0] != '\0' && !(name[0] == '.' && name[1] == 0)) {
            fs_node_t *next = fs_find_child_by_name(current, name);
            if (!next) {
                next = fs_create_directory(current, name, perms);
                if (!next) return 0;
            }
            current = next;
        }

        if (*path == '/') path++;
    }
    return current;
}

// Create full file path from parent, creating any missing directories
fs_node_t *fs_touch(fs_node_t *parent, const char *path, fs_perms_t perms) {
    if (!path || !*path || !parent || !parent->perms.field.is_directory || !parent->perms.field.write) return 0;

    // If path is 0
    if (path[0] == 0) return 0;
    // Check if path ends with '/', which is invalid for touch
    const char *p = path;
    while (*p) p++;
    p--;
    if (*p == '/') return 0;

    fs_node_t *current = (*path == '/') ? FS_ROOT : parent;
    if (*path == '/') path++;

    char name[FS_NAME_MAX_LEN];

    while (*path) {
        byte i = 0;
        while (*path && *path != '/' && i < FS_NAME_MAX_LEN - 1) {
            name[i++] = *path++;
        }
        name[i] = 0;

        if (name[0] != '\0' &&
            !(name[0] == '.' && name[1] == 0)) {

            fs_node_t *next = fs_find_child_by_name(current, name);

            byte is_last = (*path == 0);

            if (!next) {
                if (is_last) {
                    next = fs_create_file(current, name, perms);
                } else {
                    next = fs_create_directory(current, name, perms);
                }
                if (!next) return 0;
            }

            current = next;
        }

        if (*path == '/') path++;
    }

    return current;
}

// Read a file from path relative to parent
word fs_read(fs_node_t *parent, const char *path, void *buffer, word buffer_size) {
    fs_node_t *file = fs_get_node_from_path(path, parent);
    if (!file) return 0;
    return fs_read_file(file, buffer, buffer_size);
}

// Write to a file from path relative to parent
byte fs_write(fs_node_t *parent, const char *path, const void *data, word size) {
    fs_node_t *file = fs_get_node_from_path(path, parent);
    if (!file) return 0;
    return fs_write_file(file, data, size);
}
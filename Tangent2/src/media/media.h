/*
 * media.h
 * Interface for media handling
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#ifndef MEDIA_H_
#define MEDIA_H_

#include "../libcw.h"

byte show_media(fs_node_t *parent, const char *path);
const byte* compress_media(const byte* data, word size, word* out_size);

#endif

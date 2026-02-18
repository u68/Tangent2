/*
 * media.h
 * Interface for media handling
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#ifndef MEDIA_H_
#define MEDIA_H_

#include "../libcw.h"

typedef enum {
	MEDIA_COMPRESS_LZMA = 0,
	MEDIA_COMPRESS_RAW,
	MEDIA_COMPRESS_RLE
} media_compression_t;

byte show_media(fs_node_t *parent, const char *path, media_compression_t method);
const byte* compress_media(const byte* data, word size, media_compression_t method, word* out_size);

#endif

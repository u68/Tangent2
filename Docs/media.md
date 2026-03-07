# media Reference

## Overview
`media.h` exposes Tangent2's media display and media compression interface.

The public API is small, but the implementation in `Tangent2/src/media/media.c` has several fixed assumptions that matter to callers:

- media data represents a 192 by 63 render target
- the decoded or raw image buffer size is `0xC00` bytes
- `show_media()` reads file data directly from the filesystem data pool
- `compress_media()` allocates its output on the libcw heap

This document covers the public declarations in `Tangent2/src/media/media.h` and the current behavior implemented in `Tangent2/src/media/media.c`.

## Table of contents
- [Integration And Configuration](#integration-and-configuration)
- [Compression Types](#compression-types)
- [Public Functions](#public-functions)
- [Operational Notes](#operational-notes)

## Integration And Configuration

### Section contents
- [Including media](#including-media)
- [Dependencies](#dependencies)
- [Shared Example Preamble](#shared-example-preamble)
- [General Notes](#general-notes)

### Including media
Include `media.h` in the translation unit that uses the media API.

```c
#include "media/media.h"
```

### Dependencies
`media.h` includes `libcw.h` and depends on libcw services.

Current implementation requirements:

1. Call `hinit()` before using `compress_media()`.
2. Call `fs_init()` and populate the filesystem before using `show_media()`.
3. Use `hfree()` on buffers returned by `compress_media()` when they are no longer needed.
4. Supply image buffers that match the renderer's expected size of `0xC00` bytes.

### Shared Example Preamble
Unless a snippet states otherwise, the following setup is assumed.

```c
#include "media/media.h"

static void media_setup(void)
{
    Write2RealScreen = 0;
    hinit();
    fs_init();
}
```

### General Notes
- Examples in this document do not use standard library headers.
- Examples omit repeated includes after the shared preamble.
- The current implementation treats valid media payloads as render buffers of exactly `0xC00` bytes.
- `show_media()` renders raw data directly, but decompresses RLE and LZMA data into an internal scratch buffer first.
- `compress_media()` returns `0` on failure and writes `0` to `*out_size` when it can determine that the request failed.

## Compression Types

### Section contents
- [media_compression_t](#media_compression_t)
- [Compression Type Example](#compression-type-example)

### media_compression_t
`media_compression_t` selects how media data is stored or decoded.

| Value | Meaning |
| --- | --- |
| `MEDIA_COMPRESS_LZMA` | LZMA based encoded media payload |
| `MEDIA_COMPRESS_RAW` | uncompressed `0xC00` byte render buffer |
| `MEDIA_COMPRESS_RLE` | run length encoded media payload |

Implementation details:

- `MEDIA_COMPRESS_RAW` expects the input buffer to already be in the device render layout.
- `MEDIA_COMPRESS_RLE` currently encodes the full `0xC00` byte render buffer bit by bit.
- `MEDIA_COMPRESS_LZMA` uses a custom raw LZMA stream with a small header written by `compress_lzma_raw()`.

### Compression Type Example
```c
static media_compression_t choose_format(byte use_smallest)
{
    if (use_smallest)
    {
        return MEDIA_COMPRESS_LZMA;
    }

    return MEDIA_COMPRESS_RAW;
}
```

## Public Functions

### Section contents
- [show_media](#show_media)
- [compress_media](#compress_media)

### show_media
Signature:

```c
byte show_media(fs_node_t *parent, const char *path, media_compression_t method);
```

Reads a file from the libcw filesystem and renders it to the display.

Parameters:

| Parameter | Meaning |
| --- | --- |
| `parent` | starting directory for `fs_lookup()` |
| `path` | path to the media file |
| `method` | compression format used by the file |

Current behavior by format:

- `MEDIA_COMPRESS_RAW`: renders the file data directly with `tui_render_adr()`.
- `MEDIA_COMPRESS_RLE`: decodes into an internal `0xC00` byte render buffer, then renders it.
- `MEDIA_COMPRESS_LZMA`: decodes into the same internal render buffer, then renders it.

Current return codes from `media.c`:

| Code | Meaning |
| --- | --- |
| `0` | success |
| `1` | file not found |
| `2` | invalid direct data pointer check failed |
| `3` | LZMA decode failed |
| `4` | unsupported compression method |
| other non zero | RLE decode failure code forwarded by `rle_decode()` |

Example:

```c
static byte show_media_example(void)
{
    static const byte raw_image[0x0C00] = { 0 };

    fs_node_t *file = fs_create_file(FS_ROOT, "splash.bin", PERMS_RW);
    if (!file)
    {
        return 1;
    }

    if (!fs_write_file(file, raw_image, 0x0C00))
    {
        return 2;
    }

    return show_media(FS_ROOT, "splash.bin", MEDIA_COMPRESS_RAW);
}
```

RLE example:

```c
static byte show_rle_media_example(void)
{
    static const byte raw_image[0x0C00] = { 0 };
    word packed_size;
    const byte *packed = compress_media(raw_image, 0x0C00, MEDIA_COMPRESS_RLE, &packed_size);
    fs_node_t *file;
    byte status;

    if (!packed)
    {
        return 1;
    }

    file = fs_create_file(FS_ROOT, "splash.rle", PERMS_RW);
    if (!file)
    {
        hfree((void *)packed);
        return 2;
    }

    if (!fs_write_file(file, packed, packed_size))
    {
        hfree((void *)packed);
        return 3;
    }

    hfree((void *)packed);
    status = show_media(FS_ROOT, "splash.rle", MEDIA_COMPRESS_RLE);
    return status;
}
```

### compress_media
Signature:

```c
const byte *compress_media(const byte *data, word size, media_compression_t method, word *out_size);
```

Compresses or copies a render buffer and returns a heap allocated buffer.

Parameters:

| Parameter | Meaning |
| --- | --- |
| `data` | source render buffer |
| `size` | source size, must currently be `0x0C00` |
| `method` | requested compression format |
| `out_size` | receives encoded output size |

Current behavior by format:

- `MEDIA_COMPRESS_RAW`: allocates a new `0xC00` byte buffer and copies the input.
- `MEDIA_COMPRESS_RLE`: allocates an RLE stream sized by the encoder.
- `MEDIA_COMPRESS_LZMA`: allocates an LZMA stream sized by the encoder.

Ownership:

- the returned pointer must be freed with `hfree()`
- the return type is `const byte *`, but the implementation actually allocates writable heap memory

Failure conditions:

- `data == 0`
- `out_size == 0`
- `size != 0x0C00`
- heap allocation failure
- encoder failure
- unsupported `method`

Example:

```c
static byte compress_media_example(void)
{
    static byte raw_image[0x0C00];
    word packed_size;
    const byte *packed;
    word i;

    for (i = 0; i < 0x0C00; i++)
    {
        raw_image[i] = 0;
    }

    packed = compress_media(raw_image, 0x0C00, MEDIA_COMPRESS_LZMA, &packed_size);
    if (!packed)
    {
        return 1;
    }

    hfree((void *)packed);
    return 0;
}
```

Raw copy example:

```c
static byte compress_raw_example(void)
{
    static byte raw_image[0x0C00];
    word packed_size;
    const byte *copied = compress_media(raw_image, 0x0C00, MEDIA_COMPRESS_RAW, &packed_size);

    if (!copied)
    {
        return 1;
    }

    hfree((void *)copied);
    return 0;
}
```

## Operational Notes

### Section contents
- [Expected Buffer Layout](#expected-buffer-layout)
- [Filesystem Interaction](#filesystem-interaction)
- [Heap Interaction](#heap-interaction)
- [Recommended Workflow](#recommended-workflow)

### Expected Buffer Layout
The current implementation assumes the media buffer is the full display render buffer.

Important values from `media.c`:

| Name | Value | Meaning |
| --- | --- | --- |
| `IMAGE_WIDTH` | `192` | display width |
| `IMAGE_HEIGHT` | `63` | rendered image height |
| `BITPLANE_SIZE` | `0x600` | one bitplane size |
| `RENDER_BUFFER_SIZE` | `0xC00` | full render buffer size |

The public API does not expose these names, but the functions behave according to these fixed values.

### Filesystem Interaction
`show_media()` does not copy the file into heap memory before use. It calculates a direct pointer into the libcw filesystem data pool:

```c
compressed_data = (byte *)(FS_DATA_POOL + file->data_offset);
```

That means:

- the file must already exist in the libcw filesystem
- the file data must remain valid during rendering
- raw mode can render directly from filesystem storage

### Heap Interaction
`compress_media()` always returns heap allocated memory on success. The caller is responsible for releasing it.

Example:

```c
static void heap_ownership_example(void)
{
    static byte raw_image[0x0C00];
    word out_size;
    const byte *packed = compress_media(raw_image, 0x0C00, MEDIA_COMPRESS_RLE, &out_size);

    if (packed)
    {
        hfree((void *)packed);
    }
}
```

### Recommended Workflow
Typical usage flow:

1. Prepare a `0xC00` byte render buffer.
2. Call `compress_media()` with the chosen compression mode.
3. Store the returned buffer in a libcw file with `fs_write_file()`.
4. Free the returned compression buffer with `hfree()`.
5. Later, call `show_media()` with the same compression mode to display it.

Example:

```c
static byte full_media_workflow_example(void)
{
    static byte raw_image[0x0C00];
    word out_size;
    const byte *packed;
    fs_node_t *file;

    packed = compress_media(raw_image, 0x0C00, MEDIA_COMPRESS_LZMA, &out_size);
    if (!packed)
    {
        return 1;
    }

    file = fs_create_file(FS_ROOT, "boot.tz", PERMS_RW);
    if (!file)
    {
        hfree((void *)packed);
        return 2;
    }

    if (!fs_write_file(file, packed, out_size))
    {
        hfree((void *)packed);
        return 3;
    }

    hfree((void *)packed);
    return show_media(FS_ROOT, "boot.tz", MEDIA_COMPRESS_LZMA);
}
```
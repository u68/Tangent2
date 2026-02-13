# Tangent UI (TUI) Reference

## Overview
This document describes Tangent's UI subsystem and its intended usage patterns. The subsystem is composed of two layers: the TML engine, which defines a tree of `TmlElement` objects representing layout and properties, and the TUI graphics layer, which provides primitives for pixels, lines, shapes, text, and image blits.

The renderer traverses a TML element tree and issues low-level draw calls; transforms (position, anchor, rotation) are applied from parent to child during traversal. The implementation targets constrained, embedded-like environments: callers provide fixed storage for element pools, nesting depth is limited (`TML_MAX_DEPTH`), and the code avoids dynamic memory allocation. Limits and types (coordinate range, fonts, color enums) are defined in `src/tui/graphics.h` and `src/tui/glib.h`.

## Concepts
- Coordinate system: X = 0..191, Y = 0..63 (pixels). Units are bytes.
- Rotation: 0..359 degrees (word). Rotation is applied around an element's anchor point.
- Anchors: `anchor_x`, `anchor_y` are pixel offsets used when rendering and rotating elements.
- Colours: use the `tui_colour` enum (`TUI_COLOUR_WHITE`, `TUI_COLOUR_LIGHT_GREY`, `TUI_COLOUR_DARK_GREY`, `TUI_COLOUR_BLACK`, `TUI_COLOUR_IMAGE`).
- Fonts: see `tui_font_size` enum. Character width/height returned by `tui_get_font_size`.
- VRAM/Output: `Write2RealScreen` controls whether drawing writes to real VRAM or the offscreen buffer.
- Limits: tree depth capped by `TML_MAX_DEPTH` (16); element IDs are 16-bit (`word`).

---

## TML (tml_*) API Reference

### Data types
- `TmlElement` - main element struct. Key fields: `type`, `id`, `x`, `y`, `anchor_x`, `anchor_y`, `rotation`, `colour`, `parent`, `first_child`, `next_sibling`, and a union `data` for element-specific data (text, button, div, line, checkbox, radio, input).
- `TmlElementType`, `TmlAlignment`, `TmlField` - enums used by parser and initializers.

### Functions
- `void tml_render(TmlElement* root)`
  - Renders an element tree starting at `root`. Traverses depth-first, applies parent transforms to children, and calls element-specific renderers. Safe to call from main loop; does not allocate memory.

- `void tml_render_element(TmlElement* elem, TmlTransform* transform)`
  - Low-level single-element renderer (internal; provided for completeness).

- Initializers (set defaults, not allocate storage):
  - `void tml_init_root(TmlElement* elem)`
  - `void tml_init_text(TmlElement* elem, word id, byte x, byte y, const char* text, byte font_size, byte colour)`
  - `void tml_init_button(TmlElement* elem, word id, byte x, byte y, byte w, byte h, const char* text, byte font_size, byte colour)`
  - `void tml_init_div(TmlElement* elem, word id, byte x, byte y, byte w, byte h, byte colour, byte thickness, byte style)`
  - `void tml_init_line(TmlElement* elem, word id, byte x, byte y, byte x1, byte y1, byte colour, byte thickness, byte style)`
  - `void tml_init_checkbox(TmlElement* elem, word id, byte x, byte y, byte size, byte colour, byte checked)`
  - `void tml_init_radio(TmlElement* elem, word id, byte x, byte y, byte size, byte colour, byte selected)`

- Tree manipulation:
  - `void tml_add_child(TmlElement* parent, TmlElement* child)` - appends `child` as the last child of `parent`.
  - `void tml_add_sibling(TmlElement* elem, TmlElement* sibling)` - insert `sibling` after `elem`.
  - `TmlElement* tml_find_by_id(TmlElement* root, word id)` - iterative search; returns `NULL` if not found.

- Property setters:
  - `void tml_set_position(TmlElement* elem, byte x, byte y)`
  - `void tml_set_rotation(TmlElement* elem, word rotation)` - value is normalized to 0..359.
  - `void tml_set_anchor(TmlElement* elem, byte ax, byte ay)`
  - `void tml_set_colour(TmlElement* elem, byte colour)`

- Utilities:
  - `void tml_splash(const byte* image_data, word duration)` - blocking splash utility; temporarily writes to real screen.
  - `TmlElement* tml_parse(const byte* data, TmlElement* elements, byte max_elems)` - parse a compact TML byte stream into `elements` array and return root pointer; uses `TML_START`/`TML_END` and `FIELD_*` tags.

Notes:
- Parser returns a pointer to the first (root) element within the `elements` array; user-supplied storage is required.
- Field encodings: IDs are little-endian (`read_word` helper), strings are null-terminated in the data stream.

---

## TUI Graphics API Reference

### Enums
- `enum tui_colour` - see **Colours** above.
- `enum tui_style` - general style (solid/dotted/dashed/double).
- `enum tui_line_style` - byte constants for line patterns.
- `enum tui_font_size` - supported font sizes.
- `enum tui_fill_style` - fill modes (not extensively used in current APIs).

### Functions (brief)
- `void tui_rotate_point(byte ax, byte ay, byte px, byte py, word angle, byte *out_x, byte *out_y)`
  - Rotate point `(px,py)` around `(ax,ay)` by `angle` degrees. Outputs clamped byte coordinates.

- Line drawing
  - `void tui_simple_line(byte x0, byte y0, byte x1, byte y1, byte colour)` - Bresenham, single pixel thickness.
  - `void tui_advanced_draw_line(byte* data, byte bit_length, byte x0, byte y0, byte x1, byte y1, byte colour, byte thickness)` - patterned stretch based on bit-sequence.
  - `void tui_pattern_draw_line(byte pattern, byte x0, byte y0, byte x1, byte y1, byte colour, byte thickness)` - repeating pattern.
  - `void tui_draw_line(byte x0, byte y0, byte x1, byte y1, byte colour, byte thickness, byte style)` - high-level wrapper supporting styles & thickness.

- Pixel & buffer
  - `void tui_render_buffer()` - blit VRAM buffer to real VRAM.
  - `void tui_clear_screen()` - clear real or buffer depending on `Write2RealScreen`.
  - `byte tui_get_pixel(byte x, byte y)` - read pixel colour (returns 0..3).
  - `void tui_set_pixel(byte x, byte y, byte colour, byte size)` - draw pixel or small filled circle for `size` > 1.
  - `void tui_draw_byte(byte x, byte y, byte data, byte data2, byte mask)` - low-level byte blit with mask.

- Primitives
  - `void tui_circle(byte cx, byte cy, byte r, byte col)` - filled circle helper.
  - `void tui_draw_rectangle(byte x, byte y, byte width, byte height, sbyte ax, sbyte ay, word rotation, byte colour, byte thickness, byte style)`
  - `void tui_draw_points(byte cx, byte cy, byte x, byte y, byte thickness, byte colour)` - helper used by circle algorithm.
  - `void tui_draw_circle(byte cx, byte cy, byte radius, sbyte ax, sbyte ay, byte thickness, byte colour)` - draw (outline) circle with anchor/rotation.

- Text & images
  - `void tui_get_font_size(byte font_size, byte* width, byte* height)`
  - `void tui_get_text_size(byte font_size, const char* text, byte* width, byte* height)`
  - `void tui_draw_text(byte x, byte y, const char* text, byte font_size, sbyte ax, sbyte ay, word rotation, byte colour)`
  - `void tui_draw_char(byte x, byte y, char c, byte font_size, sbyte ax, sbyte ay, word rotation, byte colour)`
  - `void tui_draw_image(byte x, byte y, byte width, byte height, const byte* bitmap, sbyte ax, sbyte ay, word rotation, byte colour)`
  - `void tui_invert_area(byte x, byte y, byte width, byte height, sbyte ax, sbyte ay, word rotation)`
  - `void tui_draw_full_image(const word* bitmap, byte colour)`

Notes:
- Many functions validate coordinates and trigger BSOD on invalid params; check limits (X>191 or Y>63) to avoid errors.
- Drawing functions accept `colour` enums as defined in `graphics.h`.

---

## Examples

### 1) Basic element tree and render
```c
TmlElement elems[8];
tml_init_root(&elems[0]);
TmlElement* root = &elems[0];
TmlElement text; tml_init_text(&text, 1, 10, 10, "Hello", TUI_FONT_SIZE_6x8, TUI_COLOUR_WHITE);
tml_add_child(root, &text);
// Render
tml_render(root);
```

### 2) Button with centered text
```c
TmlElement btn;
tml_init_button(&btn, 2, 20, 20, 40, 14, "OK", TUI_FONT_SIZE_6x8, TUI_COLOUR_WHITE);
btn.anchor_x = 0; btn.anchor_y = 0; // anchor at top-left
// Add to root and render
tml_add_child(root, &btn);
tml_render(root);
```

### 3) Parser example: nested elements
```c
// Byte-encoded TML example demonstrating a DIV with two children: TEXT and BUTTON.
// Layout (visual): DIV -> { TEXT(id=2, "Hello"), BUTTON(id=3, "OK") }
const byte sample[] = {
    TML_START, TML_DIV,
        FIELD_ID, 1, 0,           // id = 1 (little-endian)
        FIELD_WIDTH, 80,         // width
        FIELD_HEIGHT, 20,        // height
        TML_START, TML_TEXT,     // start child: TEXT
            FIELD_ID, 2, 0,     // id = 2
            FIELD_TEXT, 'H','e','l','l','o', 0,
        TML_END,                // end TEXT
        TML_START, TML_BUTTON,   // start child: BUTTON
            FIELD_ID, 3, 0,     // id = 3
            FIELD_TEXT, 'O','K', 0,
        TML_END,                // end BUTTON
    TML_END,                   // end DIV
    0
};

// User-provided element storage
TmlElement pool[8];

// Parse into the pool; returns pointer to root element within pool
TmlElement* root = tml_parse(sample, pool, 8);
if (root == NULL) {
    // handle parse failure
}

// Modify an element discovered by id
TmlElement* btn = tml_find_by_id(root, 3);
if (btn) {
    tml_set_position(btn, 30, 10);
}

// Render tree
tml_render(root);
```

---

## Notes & Caveats
- `TML_MAX_DEPTH` prevents stack overflow during parse/render. If you need larger nesting, modify source and validate memory.
- Rotation arithmetic uses fast table-based trig approximations (sine/cos tables). Rotation precision is limited by table resolution.
- Image blits and `tui_draw_byte` are optimized for platform VRAM layout; ensure alignment when preparing bitmaps.
- All examples use `byte/word/sbyte` typedefs from `src/base.h`.

---

## Planned refinements (first-draft → final)
1. Add more complete, copy-pasteable examples (including image data generation).
2. Add a small reference table for `FIELD_*` tag encodings for the parser.
3. Add cross-references to `Docs/virtual-machine.md` where relevant and insert `Docs/tangent-ui.md` into main TOC.
4. Add microbenchmarks and performance notes for common operations (draw_text, draw_image).
5. Add diagrams for anchor/rotation and coordinate examples.

---

## Implementation notes
- Source files: `src/tui/glib.h`, `src/tui/glib.c`, `src/tui/graphics.h`, `src/tui/graphics.c`.

---

**Cross-link / TOC:** Add `Docs/tangent-ui.md` to the project TOC under UI or Developer docs.


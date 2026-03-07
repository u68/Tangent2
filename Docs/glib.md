# glib Reference

## Overview
`glib.h` exposes Tangent Markup Language, or TML, as a tree based UI description and rendering API.

The API provides:

- element type definitions
- alignment and field tag enums
- element data structures
- tree construction helpers
- element property setters
- a byte stream parser for serialized TML
- a splash helper for full screen images

This document covers the public declarations in `Tangent2/src/tui/glib.h` and the current behavior implemented in `Tangent2/src/tui/glib.c`.

## Table of contents
- [Integration And Configuration](#integration-and-configuration)
- [Enums And Macros](#enums-and-macros)
- [Structures And Data Types](#structures-and-data-types)
- [Rendering Functions](#rendering-functions)
- [Initialization Functions](#initialization-functions)
- [Tree Manipulation Functions](#tree-manipulation-functions)
- [Property Setter Functions](#property-setter-functions)
- [Parser And Utility Functions](#parser-and-utility-functions)
- [Operational Notes](#operational-notes)

## Integration And Configuration

### Section contents
- [Including glib](#including-glib)
- [Dependencies](#dependencies)
- [Shared Example Preamble](#shared-example-preamble)
- [General Notes](#general-notes)

### Including glib
Include `glib.h` in the translation unit that uses TML.

```c
#include "tui/glib.h"
```

### Dependencies
`glib.h` includes `libcw.h`, and `glib.c` depends heavily on libcw drawing and keyboard functions.

Current runtime requirements:

1. libcw graphics must be usable before calling `tml_render()`.
2. `CheckButtons()` is read inside `tml_render()` to update selection state.
3. `tml_splash()` uses low level timer registers through a private delay helper in `glib.c`.
4. `tml_parse()` requires caller supplied element storage.

### Shared Example Preamble
Unless a snippet states otherwise, the following setup is assumed.

```c
#include "tui/glib.h"

static TmlElement ui_root;

static void glib_setup(void)
{
    Write2RealScreen = 0;
    tml_init_root(&ui_root);
}
```

### General Notes
- Examples in this document do not use standard library headers.
- Examples omit repeated includes after the shared preamble.
- `tml_render()` performs tree traversal and also mutates selection state based on the current key.
- `tml_render_element()` is declared in the header, but there is no current implementation in `glib.c`.
- Child ordering is sorted by `z_index` when using `tml_add_child()` and most uses of `tml_add_sibling()`.

## Enums And Macros

### Section contents
- [TmlElementType](#tmlelementtype)
- [TmlAlignment](#tmlalignment)
- [TML_MAX_DEPTH](#tml_max_depth)
- [TML_START And TML_END](#tml_start-and-tml_end)
- [TmlField](#tmlfield)
- [Legacy Aliases](#legacy-aliases)

### TmlElementType
`TmlElementType` identifies which element variant a `TmlElement` stores.

| Value | Meaning |
| --- | --- |
| `TML_TYPE_ROOT` | root container node |
| `TML_TYPE_TEXT` | text node |
| `TML_TYPE_BUTTON` | button node |
| `TML_TYPE_DIV` | rectangular container or outline node |
| `TML_TYPE_INPUT` | text input node |
| `TML_TYPE_CHECKBOX` | checkbox node |
| `TML_TYPE_RADIO` | radio button node |
| `TML_TYPE_LINE` | line node |
| `TML_TYPE_WINDOW` | window node |
| `TML_TYPE_COUNT` | enum count sentinel |

Example:

```c
static byte is_visual_type(TmlElementType type)
{
    return type == TML_TYPE_TEXT || type == TML_TYPE_BUTTON || type == TML_TYPE_WINDOW;
}
```

### TmlAlignment
`TmlAlignment` is currently used by button text alignment and some stored layout values.

| Value |
| --- |
| `TML_ALIGN_TOP_LEFT` |
| `TML_ALIGN_CENTER_LEFT` |
| `TML_ALIGN_BOTTOM_LEFT` |
| `TML_ALIGN_TOP_RIGHT` |
| `TML_ALIGN_CENTER_RIGHT` |
| `TML_ALIGN_BOTTOM_RIGHT` |
| `TML_ALIGN_TOP_CENTER` |
| `TML_ALIGN_BOTTOM_CENTER` |
| `TML_ALIGN_MIDDLE_CENTER` |

Example:

```c
static TmlAlignment centered_alignment(void)
{
    return TML_ALIGN_MIDDLE_CENTER;
}
```

### TML_MAX_DEPTH

| Macro | Value | Meaning |
| --- | --- | --- |
| `TML_MAX_DEPTH` | `16` | maximum traversal and parser nesting depth |

Example:

```c
static byte depth_limit_example(void)
{
    return TML_MAX_DEPTH >= 8;
}
```

### TML_START And TML_END

| Macro | Value | Meaning |
| --- | --- | --- |
| `TML_START` | `'<'` | begin element marker in serialized TML |
| `TML_END` | `'>'` | end element marker in serialized TML |

Example:

```c
static byte has_expected_markers(void)
{
    return TML_START == '<' && TML_END == '>';
}
```

### TmlField
`TmlField` assigns numeric tags to serialized element fields.

| Value | Meaning |
| --- | --- |
| `FIELD_ID` | element id |
| `FIELD_X` | x position |
| `FIELD_Y` | y position |
| `FIELD_WIDTH` | width |
| `FIELD_HEIGHT` | height |
| `FIELD_COLOUR` | color |
| `FIELD_THICKNESS` | border or line thickness |
| `FIELD_STYLE` | border or line style |
| `FIELD_FONT` | font selection |
| `FIELD_TEXT` | string pointer in serialized data |
| `FIELD_ALIGN` | alignment value |
| `FIELD_CHECKED` | checkbox selected state |
| `FIELD_SELECTED` | radio selected state |
| `FIELD_SIZE` | checkbox or radio size |
| `FIELD_END_X` | line end x offset |
| `FIELD_END_Y` | line end y offset |
| `FIELD_ROTATION` | rotation word |
| `FIELD_ANCHOR_X` | anchor x |
| `FIELD_ANCHOR_Y` | anchor y |
| `FIELD_Z_INDEX` | z index |

Example:

```c
static byte uses_rotation_field(TmlField field)
{
    return field == FIELD_ROTATION;
}
```

### Legacy Aliases
The header also exports compatibility aliases.

Element aliases:

| Alias | Value source |
| --- | --- |
| `TML_ROOT` | `TML_TYPE_ROOT` |
| `TML_TEXT` | `TML_TYPE_TEXT` |
| `TML_BUTTON` | `TML_TYPE_BUTTON` |
| `TML_DIV` | `TML_TYPE_DIV` |
| `TML_INPUT` | `TML_TYPE_INPUT` |
| `TML_CHECKBOX` | `TML_TYPE_CHECKBOX` |
| `TML_RADIO` | `TML_TYPE_RADIO` |
| `TML_LINE` | `TML_TYPE_LINE` |

Alignment aliases:

| Alias | Value source |
| --- | --- |
| `ALIGN_TOP_LEFT` | `TML_ALIGN_TOP_LEFT` |
| `ALIGN_CENTER_LEFT` | `TML_ALIGN_CENTER_LEFT` |
| `ALIGN_BOTTOM_LEFT` | `TML_ALIGN_BOTTOM_LEFT` |
| `ALIGN_TOP_RIGHT` | `TML_ALIGN_TOP_RIGHT` |
| `ALIGN_CENTER_RIGHT` | `TML_ALIGN_CENTER_RIGHT` |
| `ALIGN_BOTTOM_RIGHT` | `TML_ALIGN_BOTTOM_RIGHT` |
| `ALIGN_TOP_CENTER` | `TML_ALIGN_TOP_CENTER` |
| `ALIGN_BOTTOM_CENTER` | `TML_ALIGN_BOTTOM_CENTER` |
| `ALIGN_MIDDLE_CENTER` | `TML_ALIGN_MIDDLE_CENTER` |

Example:

```c
static byte legacy_alias_example(void)
{
    return TML_BUTTON == TML_TYPE_BUTTON && ALIGN_MIDDLE_CENTER == TML_ALIGN_MIDDLE_CENTER;
}
```

## Structures And Data Types

### Section contents
- [struct TmlElement Forward Declaration](#struct-tmlelement-forward-declaration)
- [TmlTextData](#tmltextdata)
- [TmlButtonData](#tmlbuttondata)
- [TmlDivData](#tmldivdata)
- [TmlLineData](#tmllinedata)
- [TmlCheckboxData](#tmlcheckboxdata)
- [TmlRadioData](#tmlradiodata)
- [TmlInputData](#tmlinputdata)
- [TmlWindowData](#tmlwindowdata)
- [TmlSelect](#tmlselect)
- [TmlElement](#tmlelement)
- [TmlTransform](#tmltransform)

### struct TmlElement Forward Declaration
The header forward declares `struct TmlElement` so pointer fields can reference the full type.

Example:

```c
static struct TmlElement *forward_decl_example(struct TmlElement *elem)
{
    return elem;
}
```

### TmlTextData
Stores text element content.

```c
typedef struct {
    const char *text;
    byte font_size;
} TmlTextData;
```

Example:

```c
static void text_data_example(void)
{
    TmlTextData data;
    data.text = "Hello";
    data.font_size = TUI_FONT_SIZE_6x8;
}
```

### TmlButtonData
Stores button content, dimensions, border style, and text alignment.

```c
typedef struct {
    const char *text;
    byte font_size;
    byte width;
    byte height;
    byte border_thickness;
    byte border_style;
    byte text_align;
} TmlButtonData;
```

Example:

```c
static void button_data_example(void)
{
    TmlButtonData data;
    data.text = "OK";
    data.font_size = TUI_FONT_SIZE_6x8;
    data.width = 30;
    data.height = 12;
    data.border_thickness = 1;
    data.border_style = TUI_LINE_STYLE_SOLID;
    data.text_align = TML_ALIGN_MIDDLE_CENTER;
}
```

### TmlDivData
Stores rectangular element dimensions and border settings.

```c
typedef struct {
    byte width;
    byte height;
    byte border_thickness;
    byte border_style;
    byte child_align;
} TmlDivData;
```

Example:

```c
static void div_data_example(void)
{
    TmlDivData data;
    data.width = 60;
    data.height = 20;
    data.border_thickness = 1;
    data.border_style = TUI_LINE_STYLE_DASHED;
    data.child_align = TML_ALIGN_TOP_LEFT;
}
```

### TmlLineData
Stores the end offset and visual style of a line.

```c
typedef struct {
    byte x1;
    byte y1;
    byte thickness;
    byte style;
} TmlLineData;
```

Example:

```c
static void line_data_example(void)
{
    TmlLineData data;
    data.x1 = 20;
    data.y1 = 0;
    data.thickness = 1;
    data.style = TUI_LINE_STYLE_DOTTED;
}
```

### TmlCheckboxData
Stores checkbox size and border style.

```c
typedef struct {
    byte size;
    byte border_thickness;
    byte border_style;
} TmlCheckboxData;
```

Example:

```c
static void checkbox_data_example(void)
{
    TmlCheckboxData data;
    data.size = 8;
    data.border_thickness = 1;
    data.border_style = TUI_LINE_STYLE_SOLID;
}
```

### TmlRadioData
Stores radio button size and border thickness.

```c
typedef struct {
    byte size;
    byte border_thickness;
} TmlRadioData;
```

Example:

```c
static void radio_data_example(void)
{
    TmlRadioData data;
    data.size = 8;
    data.border_thickness = 1;
}
```

### TmlInputData
Stores input field text buffer and box settings.

```c
typedef struct {
    char *text;
    byte max_length;
    byte font_size;
    byte width;
    byte height;
    byte border_thickness;
    byte border_style;
} TmlInputData;
```

Example:

```c
static char input_buffer[16] = "Name";

static void input_data_example(void)
{
    TmlInputData data;
    data.text = input_buffer;
    data.max_length = 15;
    data.font_size = TUI_FONT_SIZE_6x8;
    data.width = 50;
    data.height = 12;
    data.border_thickness = 1;
    data.border_style = TUI_LINE_STYLE_SOLID;
}
```

### TmlWindowData
Stores window title and dimensions.

```c
typedef struct {
    const char *title;
    byte width;
    byte height;
} TmlWindowData;
```

Example:

```c
static void window_data_example(void)
{
    TmlWindowData data;
    data.title = "Settings";
    data.width = 80;
    data.height = 40;
}
```

### TmlSelect
Stores selectable, selected, and focused flags in a packed byte.

```c
typedef union {
    byte raw;
    struct {
        byte selectable:1;
        byte selected:1;
        byte focused:1;
    } field;
} TmlSelect;
```

Example:

```c
static void select_state_example(void)
{
    TmlSelect state;
    state.raw = 0;
    state.field.selectable = 1;
    state.field.focused = 1;
}
```

### TmlElement
`TmlElement` is the main tree node type.

```c
typedef struct TmlElement {
    struct TmlElement *parent;
    struct TmlElement *first_child;
    struct TmlElement *next_sibling;
    TmlElementType type;
    word id;
    byte x;
    byte y;
    byte z_index;
    byte anchor_x;
    byte anchor_y;
    word rotation;
    byte colour;
    TmlSelect select;
    union {
        TmlTextData text;
        TmlButtonData button;
        TmlDivData div;
        TmlLineData line;
        TmlCheckboxData checkbox;
        TmlRadioData radio;
        TmlInputData input;
        TmlWindowData window;
    } data;
} TmlElement;
```

Example:

```c
static void element_type_example(void)
{
    TmlElement element;
    tml_init_text(&element, 1, 5, 5, "Item", TUI_FONT_SIZE_6x8, TUI_COLOUR_BLACK);
    element.z_index = 2;
}
```

### TmlTransform
Stores world transform state used during traversal.

```c
typedef struct {
    byte x;
    byte y;
    word rotation;
} TmlTransform;
```

Example:

```c
static void transform_type_example(void)
{
    TmlTransform transform;
    transform.x = 10;
    transform.y = 12;
    transform.rotation = 15;
}
```

## Rendering Functions

### Section contents
- [tml_render](#tml_render)
- [tml_render_element](#tml_render_element)

### tml_render
Signature:

```c
void tml_render(TmlElement *root);
```

Traverses the tree from `root`, computes world transforms, handles a small selection navigation model, and renders each element except `TML_TYPE_ROOT`.

Current key handling in `glib.c`:

| Key | Effect |
| --- | --- |
| `SP_BACK` or `SP_UP` | move focus to parent when allowed |
| `SP_DOWN` | move focus to first selectable child |
| `SP_RIGHT` | move focus to next selectable sibling |
| `SP_EXE` | mark focused element as selected |
| `SP_AC` | clear selection and focus, then reset focus to root |

Example:

```c
static TmlElement title_text;

static void render_example(void)
{
    glib_setup();
    tml_init_text(&title_text, 2, 4, 4, "Menu", TUI_FONT_SIZE_6x8, TUI_COLOUR_BLACK);
    tml_add_child(&ui_root, &title_text);
    tml_render(&ui_root);
}
```

### tml_render_element
Signature:

```c
void tml_render_element(TmlElement *elem, TmlTransform *transform);
```

This was just a thing I had before but forgot to remove, so yeah.
It was basically the same as tml_render, as tml_render actually rendered a global root element, and was a wrapper for render_element, and then the implementation was made into render_tml and render element became obselete.

## Initialization Functions

### Section contents
- [tml_init_root](#tml_init_root)
- [tml_init_text](#tml_init_text)
- [tml_init_button](#tml_init_button)
- [tml_init_div](#tml_init_div)
- [tml_init_line](#tml_init_line)
- [tml_init_checkbox](#tml_init_checkbox)
- [tml_init_radio](#tml_init_radio)
- [tml_init_input](#tml_init_input)
- [tml_init_window](#tml_init_window)

### tml_init_root
Signature:

```c
void tml_init_root(TmlElement *elem);
```

Initializes a root element with zeroed tree links and default color state.

Example:

```c
static void init_root_example(void)
{
    TmlElement root;
    tml_init_root(&root);
}
```

### tml_init_text
Signature:

```c
void tml_init_text(TmlElement *elem, word id, byte x, byte y, const char *text, byte font_size, byte colour);
```

Initializes a text element.

Example:

```c
static void init_text_example(void)
{
    TmlElement text;
    tml_init_text(&text, 10, 6, 6, "Hello", TUI_FONT_SIZE_6x8, TUI_COLOUR_BLACK);
}
```

### tml_init_button
Signature:

```c
void tml_init_button(TmlElement *elem, word id, byte x, byte y, byte w, byte h, const char *text, byte font_size, byte colour);
```

Initializes a selectable button with a solid border and centered text by default.

Example:

```c
static void init_button_example(void)
{
    TmlElement button;
    tml_init_button(&button, 11, 6, 18, 40, 12, "Run", TUI_FONT_SIZE_6x8, TUI_COLOUR_BLACK);
}
```

### tml_init_div
Signature:

```c
void tml_init_div(TmlElement *elem, word id, byte x, byte y, byte w, byte h, byte colour, byte thickness, byte style);
```

Initializes a rectangular outline or container element.

Example:

```c
static void init_div_example(void)
{
    TmlElement box;
    tml_init_div(&box, 12, 2, 2, 80, 24, TUI_COLOUR_BLACK, 1, TUI_LINE_STYLE_SOLID);
}
```

### tml_init_line
Signature:

```c
void tml_init_line(TmlElement *elem, word id, byte x, byte y, byte x1, byte y1, byte colour, byte thickness, byte style);
```

Initializes a line element whose end point is stored as an offset.

Example:

```c
static void init_line_example(void)
{
    TmlElement line;
    tml_init_line(&line, 13, 4, 32, 60, 0, TUI_COLOUR_BLACK, 1, TUI_LINE_STYLE_DASHED);
}
```

### tml_init_checkbox
Signature:

```c
void tml_init_checkbox(TmlElement *elem, word id, byte x, byte y, byte size, byte colour, byte checked);
```

Initializes a selectable checkbox element.

Example:

```c
static void init_checkbox_example(void)
{
    TmlElement checkbox;
    tml_init_checkbox(&checkbox, 14, 6, 44, 8, TUI_COLOUR_BLACK, 1);
}
```

### tml_init_radio
Signature:

```c
void tml_init_radio(TmlElement *elem, word id, byte x, byte y, byte size, byte colour, byte selected);
```

Initializes a selectable radio button element.

Example:

```c
static void init_radio_example(void)
{
    TmlElement radio;
    tml_init_radio(&radio, 15, 20, 44, 8, TUI_COLOUR_BLACK, 0);
}
```

### tml_init_input
Signature:

```c
void tml_init_input(TmlElement *elem, word id, byte x, byte y, byte w, byte h, byte max_length, byte font_size, byte colour);
```

Initializes a selectable input box. The `text` pointer is set to `0` by default and is expected to be assigned later by the caller.

Example:

```c
static char name_buffer[16] = "User";

static void init_input_example(void)
{
    TmlElement input;
    tml_init_input(&input, 16, 4, 54, 60, 12, 15, TUI_FONT_SIZE_6x8, TUI_COLOUR_BLACK);
    input.data.input.text = name_buffer;
}
```

### tml_init_window
Signature:

```c
void tml_init_window(TmlElement *elem, word id, byte x, byte y, byte w, byte h, const char *title);
```

Initializes a selectable window element with a title.

Example:

```c
static void init_window_example(void)
{
    TmlElement window;
    tml_init_window(&window, 17, 10, 10, 100, 40, "Dialog");
}
```

## Tree Manipulation Functions

### Section contents
- [tml_add_child](#tml_add_child)
- [tml_add_sibling](#tml_add_sibling)
- [tml_find_by_id](#tml_find_by_id)

### tml_add_child
Signature:

```c
void tml_add_child(TmlElement *parent, TmlElement *child);
```

Assigns `child->parent` and inserts the child into the parent's child list, sorted by `z_index`.

Example:

```c
static TmlElement container_a;
static TmlElement child_a;

static void add_child_example(void)
{
    tml_init_div(&container_a, 20, 0, 0, 80, 20, TUI_COLOUR_BLACK, 1, TUI_LINE_STYLE_SOLID);
    tml_init_text(&child_a, 21, 2, 2, "A", TUI_FONT_SIZE_6x8, TUI_COLOUR_BLACK);
    tml_add_child(&container_a, &child_a);
}
```

### tml_add_sibling
Signature:

```c
void tml_add_sibling(TmlElement *elem, TmlElement *sibling);
```

Adds a sibling near `elem`. When a parent exists, the new element is inserted into the parent's child list with `z_index` ordering.

Example:

```c
static TmlElement sibling_a;
static TmlElement sibling_b;

static void add_sibling_example(void)
{
    tml_init_text(&sibling_a, 22, 0, 0, "One", TUI_FONT_SIZE_6x8, TUI_COLOUR_BLACK);
    tml_init_text(&sibling_b, 23, 0, 8, "Two", TUI_FONT_SIZE_6x8, TUI_COLOUR_BLACK);
    tml_add_sibling(&sibling_a, &sibling_b);
}
```

### tml_find_by_id
Signature:

```c
TmlElement *tml_find_by_id(TmlElement *root, word id);
```

Searches the tree iteratively and returns the first matching id.

Example:

```c
static TmlElement *find_by_id_example(void)
{
    TmlElement *found;
    glib_setup();
    tml_init_text(&title_text, 99, 2, 2, "Find", TUI_FONT_SIZE_6x8, TUI_COLOUR_BLACK);
    tml_add_child(&ui_root, &title_text);
    found = tml_find_by_id(&ui_root, 99);
    return found;
}
```

## Property Setter Functions

### Section contents
- [tml_set_position](#tml_set_position)
- [tml_set_rotation](#tml_set_rotation)
- [tml_set_anchor](#tml_set_anchor)
- [tml_set_colour](#tml_set_colour)
- [tml_set_z_index](#tml_set_z_index)

### tml_set_position
Signature:

```c
void tml_set_position(TmlElement *elem, byte x, byte y);
```

Updates an element's local position.

Example:

```c
static void set_position_example(void)
{
    tml_set_position(&title_text, 12, 10);
}
```

### tml_set_rotation
Signature:

```c
void tml_set_rotation(TmlElement *elem, word rotation);
```

Updates an element's rotation. The current implementation stores `rotation % 360`.

Example:

```c
static void set_rotation_example(void)
{
    tml_set_rotation(&title_text, 725);
}
```

### tml_set_anchor
Signature:

```c
void tml_set_anchor(TmlElement *elem, byte ax, byte ay);
```

Updates an element's anchor offsets.

Example:

```c
static void set_anchor_example(void)
{
    tml_set_anchor(&title_text, 3, 3);
}
```

### tml_set_colour
Signature:

```c
void tml_set_colour(TmlElement *elem, byte colour);
```

Updates an element's color field.

Example:

```c
static void set_colour_example(void)
{
    tml_set_colour(&title_text, TUI_COLOUR_BLACK);
}
```

### tml_set_z_index
Signature:

```c
void tml_set_z_index(TmlElement *elem, byte z_index);
```

Updates `z_index` and, when the element has a parent, removes and reinserts the element into the parent's child list so the new ordering is applied.

Example:

```c
static void set_z_index_example(void)
{
    tml_set_z_index(&title_text, 5);
}
```

## Parser And Utility Functions

### Section contents
- [tml_splash](#tml_splash)
- [tml_parse](#tml_parse)

### tml_splash
Signature:

```c
void tml_splash(const byte *image_data, word duration);
```

Temporarily switches drawing to the real screen, draws a full screen image in image mode, delays, then draws the same image in light grey and delays again.

Current implementation details:

- stores the old `Write2RealScreen` value and restores it before returning
- uses a private delay helper based on timer registers
- assumes `image_data` is compatible with `tui_draw_image(0, 1, 192, 63, ...)`

Example:

```c
static byte splash_image[1512];

static void splash_example(void)
{
    tml_splash(splash_image, 8000);
}
```

### tml_parse
Signature:

```c
TmlElement *tml_parse(const byte *data, TmlElement *elements, byte max_elems);
```

Parses a serialized TML byte stream into caller supplied `elements` storage and returns the root element pointer.

Storage rules:

- `elements` must point to writable `TmlElement` storage
- `max_elems` limits the number of parsed nodes
- nesting deeper than `TML_MAX_DEPTH` is not supported

Current parser behavior:

- the first parsed element becomes the root pointer returned to the caller
- child links are created using `tml_add_child()`
- top level siblings are linked by `z_index`
- type specific defaults are applied before fields are parsed
- text fields point into the input byte stream rather than being copied

Example:

```c
static const byte parsed_ui[] = {
    TML_START, TML_TEXT,
    FIELD_ID, 1, 0,
    FIELD_X, 4,
    FIELD_Y, 4,
    FIELD_TEXT, 'H', 'i', 0,
    TML_END,
    0
};

static TmlElement parsed_elements[4];

static TmlElement *parse_example(void)
{
    return tml_parse(parsed_ui, parsed_elements, 4);
}
```

More complete example with multiple elements:

```c
static const byte parsed_menu[] = {
    TML_START, TML_ROOT,
        TML_START, TML_BUTTON,
            FIELD_ID, 2, 0,
            FIELD_X, 8,
            FIELD_Y, 10,
            FIELD_WIDTH, 32,
            FIELD_HEIGHT, 12,
            FIELD_TEXT, 'O', 'K', 0,
        TML_END,
        TML_START, TML_TEXT,
            FIELD_ID, 3, 0,
            FIELD_X, 8,
            FIELD_Y, 26,
            FIELD_TEXT, 'R', 'e', 'a', 'd', 'y', 0,
        TML_END,
    TML_END,
    0
};

static TmlElement parsed_menu_storage[8];

static void parse_and_render_example(void)
{
    TmlElement *root = tml_parse(parsed_menu, parsed_menu_storage, 8);
    if (root)
    {
        tml_render(root);
    }
}
```

## Operational Notes

### Section contents
- [Selection Model](#selection-model)
- [Rendering Order](#rendering-order)
- [Current Drawing Behavior](#current-drawing-behavior)
- [Practical Workflow](#practical-workflow)

### Selection Model
Selection state is stored in `TmlSelect` and is updated by `tml_render()`.

Practical implications:

- button, checkbox, radio, input, and window initializers mark the element selectable
- text, div, and line initializers currently leave the selection byte cleared
- focused and selected elements are rendered with extra outlines in several element renderers

Example:

```c
static void selection_model_example(void)
{
    TmlElement checkbox;
    tml_init_checkbox(&checkbox, 30, 2, 2, 8, TUI_COLOUR_BLACK, 0);
    checkbox.select.field.focused = 1;
    checkbox.select.field.selected = 1;
}
```

### Rendering Order
Child ordering is based on `z_index`.

Current behavior:

- `tml_add_child()` inserts the child in ascending `z_index` order
- `tml_add_sibling()` does the same when the element has a parent
- `tml_set_z_index()` can reorder an already attached element when it has a parent

Example:

```c
static TmlElement low_z;
static TmlElement high_z;

static void rendering_order_example(void)
{
    tml_init_text(&low_z, 31, 0, 0, "low", TUI_FONT_SIZE_6x8, TUI_COLOUR_BLACK);
    tml_init_text(&high_z, 32, 0, 0, "high", TUI_FONT_SIZE_6x8, TUI_COLOUR_BLACK);
    low_z.z_index = 0;
    high_z.z_index = 5;
    tml_add_child(&ui_root, &low_z);
    tml_add_child(&ui_root, &high_z);
}
```

### Current Drawing Behavior
The current renderer implementations in `glib.c` map element types to libcw primitives.

Summary:

- text uses `tui_draw_text()`
- button and div use `tui_draw_rectangle()` plus optional text
- line uses `tui_draw_line()`
- checkbox uses a square and draws an X when selected
- radio uses `tui_draw_circle()` and `tui_circle()`
- input uses a rectangle plus text
- window uses rectangles and a small title bar

Example:

```c
static void mixed_ui_example(void)
{
    TmlElement button;
    TmlElement checkbox;

    glib_setup();
    tml_init_button(&button, 40, 4, 4, 40, 12, "Open", TUI_FONT_SIZE_6x8, TUI_COLOUR_BLACK);
    tml_init_checkbox(&checkbox, 41, 4, 20, 8, TUI_COLOUR_BLACK, 1);
    tml_add_child(&ui_root, &button);
    tml_add_child(&ui_root, &checkbox);
    tml_render(&ui_root);
}
```

### Practical Workflow
Typical usage flow:

1. Create static or stack `TmlElement` storage.
2. Initialize a root with `tml_init_root()`.
3. Initialize child elements with the matching initializer.
4. Attach them with `tml_add_child()`.
5. Update positions, anchors, colors, or z order with the setter helpers.
6. Call `tml_render()` in the render loop.

Example:

```c
static TmlElement main_window;
static TmlElement run_button;
static TmlElement status_text;

static void full_glib_workflow_example(void)
{
    glib_setup();

    tml_init_window(&main_window, 50, 8, 8, 96, 40, "Main");
    tml_init_button(&run_button, 51, 8, 18, 36, 12, "Run", TUI_FONT_SIZE_6x8, TUI_COLOUR_BLACK);
    tml_init_text(&status_text, 52, 8, 34, "Idle", TUI_FONT_SIZE_6x8, TUI_COLOUR_BLACK);

    tml_add_child(&ui_root, &main_window);
    tml_add_child(&main_window, &run_button);
    tml_add_child(&main_window, &status_text);

    tml_set_anchor(&main_window, 0, 0);
    tml_set_z_index(&main_window, 1);
    tml_render(&ui_root);
}
```
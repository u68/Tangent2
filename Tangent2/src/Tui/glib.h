/*
 * glib.h
 *
 *  Created on: Dec 2, 2025
 *      Author: harma
 *
 *  Tangent Markup Language (TML) Engine
 *  Cool UI library for "embedded" systems
 */

#ifndef GLIB_H_
#define GLIB_H_

#include "../libcw.h"

// Element types
typedef enum {
	TML_TYPE_ROOT,
	TML_TYPE_TEXT,
	TML_TYPE_BUTTON,
	TML_TYPE_DIV,
	TML_TYPE_INPUT,
	TML_TYPE_CHECKBOX,
	TML_TYPE_RADIO,
	TML_TYPE_LINE,
	TML_TYPE_COUNT
} TmlElementType;

// Alignment options
typedef enum {
	TML_ALIGN_TOP_LEFT,
	TML_ALIGN_CENTER_LEFT,
	TML_ALIGN_BOTTOM_LEFT,
	TML_ALIGN_TOP_RIGHT,
	TML_ALIGN_CENTER_RIGHT,
	TML_ALIGN_BOTTOM_RIGHT,
	TML_ALIGN_TOP_CENTER,
	TML_ALIGN_BOTTOM_CENTER,
	TML_ALIGN_MIDDLE_CENTER,
} TmlAlignment;

// Forward declaration
struct TmlElement;

// Element-specific data for TEXT elements
typedef struct {
	const char *text;
	byte         font_size;
} TmlTextData;

// Element-specific data for BUTTON elements
typedef struct {
	const char *text;
	byte         font_size;
	byte         width;
	byte         height;
	byte         border_thickness;
	byte         border_style;
	byte         text_align;
} TmlButtonData;

// Element-specific data for DIV elements
typedef struct {
	byte         width;
	byte         height;
	byte         border_thickness;
	byte         border_style;
	byte         child_align;      // Alignment for positioning children
} TmlDivData;

// Element-specific data for LINE elements
typedef struct {
	byte         x1;              // End X offset from start
	byte         y1;              // End Y offset from start
	byte         thickness;
	byte         style;
} TmlLineData;

// Element-specific data for CHECKBOX elements
typedef struct {
	byte         size;
	byte         border_thickness;
	byte         border_style;
} TmlCheckboxData;

// Element-specific data for RADIO elements
typedef struct {
	byte         size;
	byte         border_thickness;
} TmlRadioData;

// Element-specific data for INPUT elements
typedef struct {
	char*        text;            // Editable text buffer
	byte         max_length;
	byte         font_size;
	byte         width;
	byte         height;
	byte         border_thickness;
	byte         border_style;
} TmlInputData;

typedef union {
	byte raw;
	struct {
		byte selectable:1;
		byte selected:1;
		byte focused:1;
	} field;
} TmlSelect;

// Main element structure
typedef struct TmlElement {
	// Tree structure pointers
	struct TmlElement *parent;        // Parent element (NULL for root)
	struct TmlElement *first_child;   // First child element (NULL if leaf)
	struct TmlElement *next_sibling;  // Next sibling element (NULL if last)
	
	// Common properties
	byte         type;            // TmlElementType
	word         id;              // Unique element ID
	byte         x;               // X position relative to parent
	byte         y;               // Y position relative to parent
	byte         anchor_x;        // Anchor point X offset
	byte         anchor_y;        // Anchor point Y offset
	word         rotation;        // Rotation in degrees (0-359)
	byte         colour;          // Element colour
	TmlSelect    select;          // Selection state
	
	// Element-specific data
	union {
		TmlTextData      text;
		TmlButtonData    button;
		TmlDivData       div;
		TmlLineData      line;
		TmlCheckboxData  checkbox;
		TmlRadioData     radio;
		TmlInputData     input;
	} data;
} TmlElement;

#define TML_MAX_DEPTH  16   // Maximum nesting depth

typedef struct {
	byte    x;
	byte    y;
	word    rotation;
} TmlTransform;

// Rendering
void tml_render(TmlElement *root);
void tml_render_element(TmlElement *elem, TmlTransform *transform);

// Element initialization - user provides storage, library initializes it
void tml_init_root(TmlElement *elem);
void tml_init_text(TmlElement *elem, word id, byte x, byte y, const char *text, byte font_size, byte colour);
void tml_init_button(TmlElement *elem, word id, byte x, byte y, byte w, byte h, const char *text, byte font_size, byte colour);
void tml_init_div(TmlElement *elem, word id, byte x, byte y, byte w, byte h, byte colour, byte thickness, byte style);
void tml_init_line(TmlElement *elem, word id, byte x, byte y, byte x1, byte y1, byte colour, byte thickness, byte style);
void tml_init_checkbox(TmlElement *elem, word id, byte x, byte y, byte size, byte colour, byte checked);
void tml_init_radio(TmlElement *elem, word id, byte x, byte y, byte size, byte colour, byte selected);

// Tree manipulation
void tml_add_child(TmlElement *parent, TmlElement *child);
void tml_add_sibling(TmlElement *elem, TmlElement *sibling);
TmlElement *tml_find_by_id(TmlElement *root, word id);

// Element property setters
void tml_set_position(TmlElement *elem, byte x, byte y);
void tml_set_rotation(TmlElement *elem, word rotation);
void tml_set_anchor(TmlElement *elem, byte ax, byte ay);
void tml_set_colour(TmlElement *elem, byte colour);

// Splash screen utility
void tml_splash(const byte *image_data, word duration);

// Structure markers
#define TML_START      '<'
#define TML_END        '>'

// Field tags
typedef enum {
	FIELD_ID = 1,
	FIELD_X,
	FIELD_Y,
	FIELD_WIDTH,
	FIELD_HEIGHT,
	FIELD_COLOUR,
	FIELD_THICKNESS,
	FIELD_STYLE,
	FIELD_FONT,
	FIELD_TEXT,
	FIELD_ALIGN,
	FIELD_CHECKED,
	FIELD_SELECTED,
	FIELD_SIZE,
	FIELD_END_X,
	FIELD_END_Y,
	FIELD_ROTATION,
	FIELD_ANCHOR_X,
	FIELD_ANCHOR_Y
} TmlField;

// Parse data array into element tree, returns root element
TmlElement *tml_parse(const byte *data, TmlElement *elements, byte max_elems);

// Legacy stuff (probably will be removed later, after all, you shouldn't be manually making byte arrays)
#define TML_ROOT      TML_TYPE_ROOT
#define TML_TEXT      TML_TYPE_TEXT
#define TML_BUTTON    TML_TYPE_BUTTON
#define TML_DIV       TML_TYPE_DIV
#define TML_INPUT     TML_TYPE_INPUT
#define TML_CHECKBOX  TML_TYPE_CHECKBOX
#define TML_RADIO     TML_TYPE_RADIO
#define TML_LINE      TML_TYPE_LINE

#define ALIGN_TOP_LEFT      TML_ALIGN_TOP_LEFT
#define ALIGN_CENTER_LEFT   TML_ALIGN_CENTER_LEFT
#define ALIGN_BOTTOM_LEFT   TML_ALIGN_BOTTOM_LEFT
#define ALIGN_TOP_RIGHT     TML_ALIGN_TOP_RIGHT
#define ALIGN_CENTER_RIGHT  TML_ALIGN_CENTER_RIGHT
#define ALIGN_BOTTOM_RIGHT  TML_ALIGN_BOTTOM_RIGHT
#define ALIGN_TOP_CENTER    TML_ALIGN_TOP_CENTER
#define ALIGN_BOTTOM_CENTER TML_ALIGN_BOTTOM_CENTER
#define ALIGN_MIDDLE_CENTER TML_ALIGN_MIDDLE_CENTER

#endif /* GLIB_H_ */

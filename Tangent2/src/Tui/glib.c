/*
 * glib.c
 * Tangent Markup Language (TML) Engine Implementation
 * Cool UI library for "embedded" systems
 *  Created on: Dec 2, 2025
 *      Author: harma
 */

#include "glib.h"
#include "../debug/debug.h"

// Internal stuff
static void render_text(TmlElement *elem, byte world_x, byte world_y, word world_rot);
static void render_button(TmlElement *elem, byte world_x, byte world_y, word world_rot);
static void render_div(TmlElement *elem, byte world_x, byte world_y, word world_rot);
static void render_line(TmlElement *elem, byte world_x, byte world_y, word world_rot);
static void render_checkbox(TmlElement *elem, byte world_x, byte world_y, word world_rot);
static void render_radio(TmlElement *elem, byte world_x, byte world_y, word world_rot);

// Transform helper
static void apply_transform(byte parent_wx, byte parent_wy, word parent_rot,
                            byte local_x, byte local_y,
                            byte* out_wx, byte* out_wy) {
	byte rx, ry;
	
	if (parent_rot == 0) {
		// No rotation - simple offset
		*out_wx = parent_wx + local_x;
		*out_wy = parent_wy + local_y;
	} else {
		// Rotate local position around parent origin, then add parent position
		tui_rotate_point(0, 0, local_x, local_y, parent_rot, &rx, &ry);
		*out_wx = parent_wx + rx;
		*out_wy = parent_wy + ry;
	}
}

// Main render function - traverses tree and renders elements
void tml_render(TmlElement* root) {
	if (!root) return;
	
	// Stack for traversal state
	TmlElement* elem_stack[TML_MAX_DEPTH];
	TmlTransform transform_stack[TML_MAX_DEPTH];
	byte stack_ptr = 0;
	
	// Start with root
	TmlElement* current = root;
	TmlTransform current_transform = {0, 0, 0};

	TmlElement* focused_elem = root;
	byte current_key = CheckButtons();
	if (current_key != 0xff) {
		switch(current_key) {
			case SP_BACK:
			case SP_UP:
				if (focused_elem->parent && focused_elem->parent->select.field.selectable) {
					focused_elem->select.field.focused = 0;
					focused_elem->parent->select.field.focused = 1;
				}
				break;
			case SP_DOWN:
				if (focused_elem->first_child && focused_elem->first_child->select.field.selectable) {
					focused_elem->select.field.focused = 0;
					focused_elem = focused_elem->first_child;
					focused_elem->select.field.focused = 1;
				}
				break;
			case SP_RIGHT:
				if (focused_elem->next_sibling && focused_elem->next_sibling->select.field.selectable) {
					focused_elem->select.field.focused = 0;
					focused_elem = focused_elem->next_sibling;
					focused_elem->select.field.focused = 1;
				}
				break;
			case SP_EXE:
				if (focused_elem->select.field.selectable) {
					focused_elem->select.field.selected = 1;
				}
				break;
			case SP_AC:
				focused_elem->select.field.selected = 0;
				focused_elem->select.field.focused = 0;
				focused_elem = root;
		}
	}
	
	while (current || stack_ptr > 0) {
		// Go as deep as possible through first children
		while (current) {
			// Calculate this element's world transform
			byte world_x, world_y;
			word world_rot;
			
			if (stack_ptr > 0) {
				// Apply parent's transform to get our world position
				TmlTransform* parent_t = &transform_stack[stack_ptr - 1];
				apply_transform(parent_t->x, parent_t->y, parent_t->rotation,
				                current->x, current->y,
				                &world_x, &world_y);
				world_rot = (parent_t->rotation + current->rotation) % 360;
			} else {
				// Root element - local = world
				world_x = current->x;
				world_y = current->y;
				world_rot = current->rotation;
			}
			
			// Render this element (skip ROOT type)
			if (current->type != TML_TYPE_ROOT) {
				switch (current->type) {
				case TML_TYPE_TEXT:
					render_text(current, world_x, world_y, world_rot);
					break;
				case TML_TYPE_BUTTON:
					render_button(current, world_x, world_y, world_rot);
					break;
				case TML_TYPE_DIV:
					render_div(current, world_x, world_y, world_rot);
					break;
				case TML_TYPE_LINE:
					render_line(current, world_x, world_y, world_rot);
					break;
				case TML_TYPE_CHECKBOX:
					render_checkbox(current, world_x, world_y, world_rot);
					break;
				case TML_TYPE_RADIO:
					render_radio(current, world_x, world_y, world_rot);
					break;
				default:
					trigger_bsod(ERROR_TUI_INVALID_ELEMENT);
					break;
				}
			}
			
			// If has children, push current state and descend
			if (current->first_child) {
				// Push current element and its world transform
				elem_stack[stack_ptr] = current;
				transform_stack[stack_ptr].x = world_x;
				transform_stack[stack_ptr].y = world_y;
				transform_stack[stack_ptr].rotation = world_rot;
			if (stack_ptr >= TML_MAX_DEPTH) {
				trigger_bsod(ERROR_TUI_INVALID_ELEMENT);
				return;
			}
				
				current = current->first_child;
			} else {
				// No children - try sibling
				if (current->next_sibling) {
					current = current->next_sibling;
				} else {
					// No sibling - need to backtrack
					current = 0;
				}
			}
		}
		
		// Backtrack: pop stack and try sibling of popped element
		while (stack_ptr > 0 && !current) {
			stack_ptr--;
			TmlElement* parent = elem_stack[stack_ptr];
			
			if (parent->next_sibling) {
				current = parent->next_sibling;
			}
		}
	}
}

// Element renderers
static void render_text(TmlElement* elem, byte world_x, byte world_y, word world_rot) {
	if (!elem->data.text.text) return;
	if (elem->select.field.focused) {
		byte text_w, text_h;
		tui_get_text_size(elem->data.text.font_size, elem->data.text.text, &text_w, &text_h);
		tui_draw_rectangle(world_x - 1, world_y -1, 
						   text_w + 2, text_h + 2, 
						   elem->anchor_x, elem->anchor_y, 
						   world_rot, TUI_COLOUR_DARK_GREY, 1, 0x55);
	} else if (elem->select.field.selected) {
		byte text_w, text_h;
		tui_get_text_size(elem->data.text.font_size, elem->data.text.text, &text_w, &text_h);
		tui_draw_rectangle(world_x - 1, world_y -1, 
						   text_w + 2, text_h + 2, 
						   elem->anchor_x, elem->anchor_y, 
						   world_rot, TUI_COLOUR_BLACK, 1, TUI_LINE_STYLE_SOLID);
	}
	tui_draw_text(world_x, world_y, 
	          elem->data.text.text, 
	          elem->data.text.font_size,
	          elem->anchor_x, elem->anchor_y,
	          world_rot, elem->colour);
}

static void render_button(TmlElement* elem, byte world_x, byte world_y, word world_rot) {
	TmlButtonData* btn = &elem->data.button;
	
	if (elem->select.field.focused) {
		byte but_w = btn->width, but_h = btn->height;
		tui_draw_rectangle(world_x - 1, world_y -1, 
						   but_w + 2, but_h + 2, 
						   elem->anchor_x, elem->anchor_y, 
						   world_rot, TUI_COLOUR_DARK_GREY, 1, 0x55);
	} else if (elem->select.field.selected) {
		byte but_w = btn->width, but_h = btn->height;
		tui_draw_rectangle(world_x - 1, world_y -1, 
						   but_w + 2, but_h + 2, 
						   elem->anchor_x, elem->anchor_y, 
						   world_rot, TUI_COLOUR_BLACK, 1, TUI_LINE_STYLE_SOLID);
	}

	// Draw border rectangle
	tui_draw_rectangle(world_x, world_y,
	               btn->width, btn->height,
	               elem->anchor_x, elem->anchor_y,
	               world_rot, elem->colour,
	               btn->border_thickness, btn->border_style);
	
	// Draw text if present
	if (btn->text) {
		byte text_w, text_h;
		byte text_x = world_x;
		byte text_y = world_y;
		
		tui_get_text_size(btn->font_size, btn->text, &text_w, &text_h);
		
		// Calculate text position based on alignment
		switch (btn->text_align) {
		case TML_ALIGN_CENTER_LEFT:
			text_y += (btn->height >> 1) - (text_h >> 1);
			break;
		case TML_ALIGN_BOTTOM_LEFT:
			text_y += btn->height - text_h;
			break;
		case TML_ALIGN_TOP_RIGHT:
		case TML_ALIGN_CENTER_RIGHT:
			text_x += btn->width - text_w;
			text_y += (btn->height >> 1) - (text_h >> 1);
			break;
		case TML_ALIGN_BOTTOM_RIGHT:
			text_x += btn->width - text_w;
			text_y += btn->height - text_h;
			break;
		case TML_ALIGN_TOP_CENTER:
			text_x += (btn->width >> 1) - (text_w >> 1);
			break;
		case TML_ALIGN_BOTTOM_CENTER:
			text_x += (btn->width >> 1) - (text_w >> 1);
			text_y += btn->height - text_h;
			break;
		default:
			trigger_bsod(ERROR_TUI_INVALID_TEXT_ALIGNMENT);
			break;
		}
		
		// Apply rotation to text position
		byte rot_x, rot_y;
		tui_rotate_point(world_x, world_y, 
		             text_x - elem->anchor_x + 2, 
		             text_y - elem->anchor_y + 2, 
		             world_rot, &rot_x, &rot_y);
		
		tui_draw_text(rot_x, rot_y, btn->text, btn->font_size,
		          elem->anchor_x, elem->anchor_y, world_rot, elem->colour);
	}
}

static void render_div(TmlElement* elem, byte world_x, byte world_y, word world_rot) {
	TmlDivData* div = &elem->data.div;
	
	if (elem->select.field.focused) {
		byte div_w = div->width, div_h = div->height;
		tui_draw_rectangle(world_x - 1, world_y -1, 
						   div_w + 2, div_h + 2, 
						   elem->anchor_x, elem->anchor_y, 
						   world_rot, TUI_COLOUR_DARK_GREY, 1, 0x55);
	} else if (elem->select.field.selected) {
		byte div_w = div->width, div_h = div->height;
		tui_draw_rectangle(world_x - 1, world_y -1, 
						   div_w + 2, div_h + 2, 
						   elem->anchor_x, elem->anchor_y, 
						   world_rot, TUI_COLOUR_BLACK, 1, TUI_LINE_STYLE_SOLID);
	}

	tui_draw_rectangle(world_x, world_y,
	               div->width, div->height,
	               elem->anchor_x, elem->anchor_y,
	               world_rot, elem->colour,
	               div->border_thickness, div->border_style);
}

static void render_line(TmlElement* elem, byte world_x, byte world_y, word world_rot) {
	TmlLineData* line = &elem->data.line;
	TmlElement* parent = elem->parent;
	byte x0 = world_x - elem->anchor_x;
	byte y0 = world_y - elem->anchor_y;
	
	byte x1, y1;
	x1 = parent->x + line->x1 - elem->anchor_x;
	y1 = parent->y + line->y1 - elem->anchor_y;
	
	// Apply rotation if needed
	if (world_rot != 0) {
		byte rx0, ry0, rx1, ry1;
		tui_rotate_point(world_x, world_y, x0, y0, world_rot, &rx0, &ry0);
		tui_rotate_point(world_x, world_y, x1, y1, world_rot, &rx1, &ry1);
		x0 = rx0; y0 = ry0;
		x1 = rx1; y1 = ry1;
	}

	if (elem->select.field.focused) {
		tui_draw_rectangle(x0 - 1, y0 -1, 
						   x1 + 1, y1 + 1, 
						   elem->anchor_x, elem->anchor_y, 
						   world_rot, TUI_COLOUR_DARK_GREY, 1, 0x55);
	} else if (elem->select.field.selected) {
		tui_draw_rectangle(x0 - 1, y0 -1, 
						   x1 + 1, y1 + 1, 
						   elem->anchor_x, elem->anchor_y, 
						   world_rot, TUI_COLOUR_BLACK, 1, TUI_LINE_STYLE_SOLID);
	}
	
	tui_draw_line(x0, y0, x1, y1, elem->colour, line->thickness, line->style);
}

static void render_checkbox(TmlElement* elem, byte world_x, byte world_y, word world_rot) {
	TmlCheckboxData* cb = &elem->data.checkbox;
	
	// Draw square border
	tui_draw_rectangle(world_x, world_y,
	               cb->size, cb->size,
	               elem->anchor_x, elem->anchor_y,
	               world_rot, elem->colour,
	               cb->border_thickness, cb->border_style);
	
	if (elem->select.field.focused) {
		byte cb_size = cb->size;
		tui_draw_rectangle(world_x - 1, world_y -1, 
						   cb_size + 2, cb_size + 2, 
						   elem->anchor_x, elem->anchor_y, 
						   world_rot, TUI_COLOUR_DARK_GREY, 1, 0x55);
	}
	
	// Draw checkmark if checked
	if (elem->select.field.selected) {
		byte inner = cb->size - (cb->border_thickness << 1) - 2;
		byte ix = world_x + cb->border_thickness + 1;
		byte iy = world_y + cb->border_thickness + 1;
		
		// Simple X mark
		tui_draw_line(ix, iy, ix + inner, iy + inner, elem->colour, 1, TUI_LINE_STYLE_SOLID);
		tui_draw_line(ix + inner, iy, ix, iy + inner, elem->colour, 1, TUI_LINE_STYLE_SOLID);
	}
}

static void render_radio(TmlElement* elem, byte world_x, byte world_y, word world_rot) {
	TmlRadioData* radio = &elem->data.radio;
	byte radius = radio->size >> 1;
	
	tui_draw_circle(world_x, world_y, radius,
	            elem->anchor_x, elem->anchor_y,
	            radio->border_thickness, elem->colour);
	if (world_rot) {
		byte nx, ny;
		tui_rotate_point(world_x, world_y, world_x - elem->anchor_x + radio->size, world_y - elem->anchor_y + radio->size, world_rot, &nx, &ny);
		world_x = nx;
		world_y = ny;
	} else {
		world_x = world_x - elem->anchor_x + radio->size;
		world_y = world_y - elem->anchor_y + radio->size;
	}
	// Draw filled inner circle if selected
	if (elem->select.field.selected) {
		byte inner_r = radius - radio->border_thickness - 1;
		if (inner_r > 0) {
			tui_circle(world_x, world_y, inner_r, elem->colour);
		}
	}
}

// Initializers (not super required)
void tml_init_root(TmlElement* e) {
	e->type = TML_TYPE_ROOT;
	e->id = 0;
	e->x = 0;
	e->y = 0;
	e->rotation = 0;
	e->anchor_x = 0;
	e->anchor_y = 0;
	e->colour = TUI_COLOUR_BLACK;
	e->parent = 0;
	e->first_child = 0;
	e->next_sibling = 0;
	e->select = (TmlSelect){0};
}

void tml_init_text(TmlElement* e, word id, byte x, byte y, const char* text, byte font_size, byte colour) {
	e->type = TML_TYPE_TEXT;
	e->id = id;
	e->x = x;
	e->y = y;
	e->rotation = 0;
	e->anchor_x = 0;
	e->anchor_y = 0;
	e->colour = colour;
	e->parent = 0;
	e->first_child = 0;
	e->next_sibling = 0;
	e->data.text.text = text;
	e->data.text.font_size = font_size;
	e->select = (TmlSelect){0};
}

void tml_init_button(TmlElement* e, word id, byte x, byte y, byte w, byte h, const char* text, byte font_size, byte colour) {
	e->type = TML_TYPE_BUTTON;
	e->id = id;
	e->x = x;
	e->y = y;
	e->rotation = 0;
	e->anchor_x = 0;
	e->anchor_y = 0;
	e->colour = colour;
	e->parent = 0;
	e->first_child = 0;
	e->next_sibling = 0;
	e->data.button.text = text;
	e->data.button.font_size = font_size;
	e->data.button.width = w;
	e->data.button.height = h;
	e->data.button.border_thickness = 1;
	e->data.button.border_style = TUI_LINE_STYLE_SOLID;
	e->data.button.text_align = TML_ALIGN_MIDDLE_CENTER;
	e->select = (TmlSelect){.field.selectable = 1, .field.selected = 0, .field.focused = 0};
}

void tml_init_div(TmlElement* e, word id, byte x, byte y, byte w, byte h, byte colour, byte thickness, byte style) {
	e->type = TML_TYPE_DIV;
	e->id = id;
	e->x = x;
	e->y = y;
	e->rotation = 0;
	e->anchor_x = 0;
	e->anchor_y = 0;
	e->colour = colour;
	e->parent = 0;
	e->first_child = 0;
	e->next_sibling = 0;
	e->data.div.width = w;
	e->data.div.height = h;
	e->data.div.border_thickness = thickness;
	e->data.div.border_style = style;
	e->data.div.child_align = TML_ALIGN_TOP_LEFT;
	e->select = (TmlSelect){0};
}

void tml_init_line(TmlElement* e, word id, byte x, byte y, byte x1, byte y1, byte colour, byte thickness, byte style) {
	e->type = TML_TYPE_LINE;
	e->id = id;
	e->x = x;
	e->y = y;
	e->rotation = 0;
	e->anchor_x = 0;
	e->anchor_y = 0;
	e->colour = colour;
	e->parent = 0;
	e->first_child = 0;
	e->next_sibling = 0;
	e->data.line.x1 = x1;
	e->data.line.y1 = y1;
	e->data.line.thickness = thickness;
	e->data.line.style = style;
	e->select = (TmlSelect){0};
}

void tml_init_checkbox(TmlElement* e, word id, byte x, byte y, byte size, byte colour, byte checked) {
	e->type = TML_TYPE_CHECKBOX;
	e->id = id;
	e->x = x;
	e->y = y;
	e->rotation = 0;
	e->anchor_x = 0;
	e->anchor_y = 0;
	e->colour = colour;
	e->parent = 0;
	e->first_child = 0;
	e->next_sibling = 0;
	e->data.checkbox.size = size;
	e->data.checkbox.border_thickness = 1;
	e->data.checkbox.border_style = TUI_LINE_STYLE_SOLID;
	e->select = (TmlSelect){.field.selectable = 1, .field.selected = checked, .field.focused = 0};
}

void tml_init_radio(TmlElement* e, word id, byte x, byte y, byte size, byte colour, byte selected) {
	e->type = TML_TYPE_RADIO;
	e->id = id;
	e->x = x;
	e->y = y;
	e->rotation = 0;
	e->anchor_x = 0;
	e->anchor_y = 0;
	e->colour = colour;
	e->parent = 0;
	e->first_child = 0;
	e->next_sibling = 0;
	e->data.radio.size = size;
	e->data.radio.border_thickness = 1;
	e->select = (TmlSelect){.field.selectable = 1, .field.selected = selected, .field.focused = 0};
}

// Tree manipulation
void tml_add_child(TmlElement* parent, TmlElement* child) {
	if (!parent || !child) return;
	
	child->parent = parent;
	child->next_sibling = 0;
	
	if (!parent->first_child) {
		// First child
		parent->first_child = child;
	} else {
		// Find last sibling and append
		TmlElement* sibling = parent->first_child;
		while (sibling->next_sibling) {
			sibling = sibling->next_sibling;
		}
		sibling->next_sibling = child;
	}
}

// Add sibling after given element
void tml_add_sibling(TmlElement* elem, TmlElement* sibling) {
	if (!elem || !sibling) return;
	
	sibling->parent = elem->parent;
	sibling->next_sibling = elem->next_sibling;
	elem->next_sibling = sibling;
}

// Find element by ID using iterative traversal
TmlElement* tml_find_by_id(TmlElement* root, word id) {
	if (!root) return 0;
	
	TmlElement* stack[TML_MAX_DEPTH];
	byte sp = 0;
	
	if (sp >= TML_MAX_DEPTH) return 0;
		stack[sp++] = root;
	
	while (sp > 0) {
		TmlElement* current = stack[--sp];
		
		if (current->id == id) return current;
		
		// Push siblings first (so children are processed first)
		if (current->next_sibling) {
			if (sp >= TML_MAX_DEPTH) return 0;
			stack[sp++] = current->next_sibling;
		}
		if (current->first_child) {
			if (sp >= TML_MAX_DEPTH) return 0;
			stack[sp++] = current->first_child;
		}
	}
	
	return 0;
}

// Property setters
void tml_set_position(TmlElement* elem, byte x, byte y) {
	if (elem) {
		elem->x = x;
		elem->y = y;
	}
}

void tml_set_rotation(TmlElement* elem, word rotation) {
	if (elem) {
		elem->rotation = rotation % 360;
	}
}

void tml_set_anchor(TmlElement* elem, byte ax, byte ay) {
	if (elem) {
		elem->anchor_x = ax;
		elem->anchor_y = ay;
	}
}

void tml_set_colour(TmlElement* elem, byte colour) {
	if (elem) {
		elem->colour = colour;
	}
}

// Utilities (will be replaced with stuff from T2)

// Internal delay function using Timer0
static void tml_delay(ushort after_ticks) {
	if ((FCON & 2) != 0) {
		FCON &= 0xFD;
	}
	__DI();
	Timer0Interval = after_ticks;
	Timer0Counter = 0;
	Timer0Control = 0x0101;
	InterruptPending_W0 = 0;
	StopAcceptor = 0x50;
	StopAcceptor = 0xA0;
	StopControl = 2;
	__asm("nop");
	__asm("nop");
	__EI();
}

void tml_splash(const byte* image_data, word duration) {
	byte old_write_mode = Write2RealScreen;
	Write2RealScreen = 1;
	
	tui_draw_image(0, 1, 192, 63, image_data, 0, 0, 0, TUI_COLOUR_IMAGE);
	tml_delay(duration << 3);
	
	tui_draw_image(0, 1, 192, 63, image_data, 0, 0, 0, TUI_COLOUR_LIGHT_GREY);
	tml_delay(4000);
	
	Write2RealScreen = old_write_mode;
}

// Helper to read word from byte array (little-endian)
static word read_word(const byte* p) {
	return p[0] | (p[1] << 8);
}

// Parser for TML data format
TmlElement* tml_parse(const byte* data, TmlElement* elements, byte max_elems) {
	if (!data || !elements || max_elems == 0) return 0;
	
	TmlElement* root = 0;
	TmlElement* parent_stack[TML_MAX_DEPTH];
	byte stack_ptr = 0;
	
	const byte* p = data;
	byte elem_idx = 0;
	
	while (elem_idx < max_elems && *p != 0) {
		if (*p == TML_START) {
			p++;
			byte type = *p++;
			
			TmlElement* elem = &elements[elem_idx++];
			
			// Set defaults
			elem->type = type;
			elem->id = 0;
			elem->x = 0;
			elem->y = 0;
			elem->rotation = 0;
			elem->anchor_x = 0;
			elem->anchor_y = 0;
			elem->colour = TUI_COLOUR_BLACK;
			elem->parent = 0;
			elem->first_child = 0;
			elem->next_sibling = 0;
			
			// Type-specific defaults
			switch (type) {
			case TML_TYPE_TEXT:
				elem->data.text.text = 0;
				elem->data.text.font_size = TUI_FONT_SIZE_6x8;
				break;
			case TML_TYPE_BUTTON:
				elem->data.button.text = 0;
				elem->data.button.font_size = TUI_FONT_SIZE_6x8;
				elem->data.button.width = 20;
				elem->data.button.height = 12;
				elem->data.button.border_thickness = 1;
				elem->data.button.border_style = TUI_LINE_STYLE_SOLID;
				elem->data.button.text_align = TML_ALIGN_MIDDLE_CENTER;
				break;
			case TML_TYPE_DIV:
				elem->data.div.width = 10;
				elem->data.div.height = 10;
				elem->data.div.border_thickness = 1;
				elem->data.div.border_style = TUI_LINE_STYLE_SOLID;
				elem->data.div.child_align = TML_ALIGN_TOP_LEFT;
				break;
			case TML_TYPE_LINE:
				elem->data.line.x1 = 10;
				elem->data.line.y1 = 0;
				elem->data.line.thickness = 1;
				elem->data.line.style = TUI_LINE_STYLE_SOLID;
				break;
			case TML_TYPE_CHECKBOX:
				elem->data.checkbox.size = 8;
				elem->data.checkbox.border_thickness = 1;
				elem->data.checkbox.border_style = TUI_LINE_STYLE_SOLID;
				elem->select = (TmlSelect){.field.selectable = 1, .field.selected = 0, .field.focused = 0};
				break;
			case TML_TYPE_RADIO:
				elem->data.radio.size = 8;
				elem->data.radio.border_thickness = 1;
				elem->select = (TmlSelect){.field.selectable = 1, .field.selected = 0, .field.focused = 0};
				break;
			default:
				trigger_bsod(ERROR_TUI_INVALID_ELEMENT);
				break;
			}
			
			// Parse fields until we hit '<' (child) or '>' (end)
			while (*p != TML_END && *p != TML_START && *p != 0) {
				byte field = *p++;
				switch (field) {
				case FIELD_ID:
					elem->id = read_word(p); p += 2;
					break;
				case FIELD_X:
					elem->x = *p++;
					break;
				case FIELD_Y:
					elem->y = *p++;
					break;
				case FIELD_WIDTH:
					if (type == TML_TYPE_BUTTON) elem->data.button.width = *p++;
					else if (type == TML_TYPE_DIV) elem->data.div.width = *p++;
					else p++;
					break;
				case FIELD_HEIGHT:
					if (type == TML_TYPE_BUTTON) elem->data.button.height = *p++;
					else if (type == TML_TYPE_DIV) elem->data.div.height = *p++;
					else p++;
					break;
				case FIELD_COLOUR:
					elem->colour = *p++;
					break;
				case FIELD_THICKNESS:
					if (type == TML_TYPE_BUTTON) elem->data.button.border_thickness = *p++;
					else if (type == TML_TYPE_DIV) elem->data.div.border_thickness = *p++;
					else if (type == TML_TYPE_LINE) elem->data.line.thickness = *p++;
					else if (type == TML_TYPE_CHECKBOX) elem->data.checkbox.border_thickness = *p++;
					else if (type == TML_TYPE_RADIO) elem->data.radio.border_thickness = *p++;
					else p++;
					break;
				case FIELD_STYLE:
					if (type == TML_TYPE_BUTTON) elem->data.button.border_style = *p++;
					else if (type == TML_TYPE_DIV) elem->data.div.border_style = *p++;
					else if (type == TML_TYPE_LINE) elem->data.line.style = *p++;
					else if (type == TML_TYPE_CHECKBOX) elem->data.checkbox.border_style = *p++;
					else p++;
					break;
				case FIELD_FONT:
					if (type == TML_TYPE_TEXT) elem->data.text.font_size = *p++;
					else if (type == TML_TYPE_BUTTON) elem->data.button.font_size = *p++;
					else p++;
					break;
				case FIELD_TEXT:
					if (type == TML_TYPE_TEXT) {
						elem->data.text.text = (const char*)p;
					} else if (type == TML_TYPE_BUTTON) {
						elem->data.button.text = (const char*)p;
					}
					while (*p) p++;  // skip past string
					p++;  // skip null terminator
					break;
				case FIELD_ALIGN:
					if (type == TML_TYPE_BUTTON) elem->data.button.text_align = *p++;
					else p++;
					break;
				case FIELD_CHECKED:
					if (type == TML_TYPE_CHECKBOX) elem->select.field.selected = *p++;
					else p++;
					break;
				case FIELD_SELECTED:
					if (type == TML_TYPE_RADIO) elem->select.field.selected = *p++;
					else p++;
					break;
				case FIELD_SIZE:
					if (type == TML_TYPE_CHECKBOX) elem->data.checkbox.size = *p++;
					else if (type == TML_TYPE_RADIO) elem->data.radio.size = *p++;
					else p++;
					break;
				case FIELD_END_X:
					if (type == TML_TYPE_LINE) elem->data.line.x1 = *p++;
					else p++;
					break;
				case FIELD_END_Y:
					if (type == TML_TYPE_LINE) elem->data.line.y1 = *p++;
					else p++;
					break;
				case FIELD_ROTATION:
					elem->rotation = read_word(p); p += 2;
					break;
				case FIELD_ANCHOR_X:
					elem->anchor_x = *p++;
					break;
				case FIELD_ANCHOR_Y:
					elem->anchor_y = *p++;
					break;
				default:
					trigger_bsod(ERROR_TUI_INVALID_ELEMENT_FIELD);
					break;
				}
			}
			
			// Add to tree
			if (!root) {
				root = elem;
			} else if (stack_ptr > 0) {
				tml_add_child(parent_stack[stack_ptr - 1], elem);
			} else {
				// Top-level sibling - link after root
				TmlElement* sib = root;
				while (sib->next_sibling) sib = sib->next_sibling;
				sib->next_sibling = elem;
			}
			
			// If next is '<', this element has children - push onto stack
			if (*p == TML_START) {
				if (stack_ptr < TML_MAX_DEPTH) {
					parent_stack[stack_ptr++] = elem;
				}
			}
			// If next is '>', element is done with no children - skip it
			else if (*p == TML_END) {
				p++;
			}
		}
		else if (*p == TML_END) {
			// Closing '>' pops parent off stack
			p++;
			if (stack_ptr > 0) {
				stack_ptr--;
			}
		}
		else {
			p++;
		}
	}
	
	return root;
}

byte lastbutton = 0xff;
byte CheckButtons() {
	byte x;
	byte y;
	byte i = 0;
	for(x = 0x80; x != 0; x = x >> 1)
	{
		deref(0xf046) = x;
		for(y = 0x80; y != 0; y = y >> 1)
		{
			if((deref(0xf040) & y) == 0)
			{
				if(i != lastbutton)
				{
					lastbutton = i;
					return i;
				}
				return 0xff;
			}
			++i;
		}
	}
	lastbutton = 0x50;
	return 0xff;
}
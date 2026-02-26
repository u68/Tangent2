/*
 * text_edit.c
 * Implementation of text editor application
 * A simple text editor for Tangent, you can do stuff with it.
 *  Created on: Feb 21, 2026
 *      Author: harma
 */

#include "text_edit.h"

// State
static char *text_buffer = 0;
static word text_len = 0;
static word text_capacity = 0;
static word cursor_idx = 0;
static word cursor_x_target = 0;
static word top_line = 0;
static word cursor_line = 0;

static byte blink_state = 1;
static byte last_sec = 0;
static byte my_last_key = 0xff;

#define MAX_LINES 256
typedef struct {
    word start_idx;
    word end_idx;
    byte max_height;
    byte start_font;
    byte start_col;
} LineInfo;

static LineInfo lines[MAX_LINES];
static word num_lines = 0;

// Helpers
static void get_font_size(byte font_sentinel, byte *w, byte *h) {
    switch (font_sentinel) {
        case TE_FONT_6x7:   *w = 6;  *h = 7;  break;
        case TE_FONT_6x8:   *w = 6;  *h = 8;  break;
        case TE_FONT_6x10:  *w = 6;  *h = 10; break;
        case TE_FONT_7x10:  *w = 7;  *h = 10; break;
        case TE_FONT_8x8:   *w = 8;  *h = 8;  break;
        case TE_FONT_8x12:  *w = 8;  *h = 12; break;
        case TE_FONT_12x16: *w = 12; *h = 16; break;
        default:            *w = 6;  *h = 7;  break;
    }
}

static void do_layout(void) {
    num_lines = 0;
    byte cur_font = TE_DEFAULT_FONT;
    byte cur_col = TE_DEFAULT_COLOUR;
    byte cur_font_w, cur_font_h;
    get_font_size(cur_font, &cur_font_w, &cur_font_h);
    
    word i = 0;
    
    while (i <= text_len && num_lines < MAX_LINES) {
        byte scan_font = cur_font;
        byte scan_col = cur_col;
        byte scan_w = cur_font_w;
        byte scan_h = cur_font_h;
        byte max_h = scan_h;
        word x = 0;
        
        word start = i;
        byte break_line = 0;
        
        while (i < text_len && !break_line) {
            byte c = (byte)text_buffer[i];
            
            switch (c) {
                case TE_LINE_END:
                    i++;
                    break_line = 1;
                    break;
                case TE_FONT_6x7:
                case TE_FONT_6x8:
                case TE_FONT_6x10:
                case TE_FONT_7x10:
                case TE_FONT_8x8:
                case TE_FONT_8x12:
                case TE_FONT_12x16:
                    scan_font = c;
                    get_font_size(scan_font, &scan_w, &scan_h);
                    if (scan_h > max_h) max_h = scan_h;
                    i++;
                    break;
                case TE_COLOUR_LIGHT_GREY:
                    scan_col = TUI_COLOUR_LIGHT_GREY;
                    i++;
                    break;
                case TE_COLOUR_DARK_GREY:
                    scan_col = TUI_COLOUR_DARK_GREY;
                    i++;
                    break;
                case TE_COLOUR_BLACK:
                    scan_col = TUI_COLOUR_BLACK;
                    i++;
                    break;
                default:
                    if (c >= 32) {
                        if (x + scan_w > 192) {
                            break_line = 1;
                            break;
                        }
                        x += scan_w;
                    }
                    i++;
                    break;
            }
        }
        
        lines[num_lines].start_idx = start;
        lines[num_lines].end_idx = i;
        lines[num_lines].max_height = max_h;
        lines[num_lines].start_font = cur_font;
        lines[num_lines].start_col = cur_col;
        num_lines++;
        
        cur_font = scan_font;
        cur_col = scan_col;
        cur_font_w = scan_w;
        cur_font_h = scan_h;
        
        if (i >= text_len) {
            if (start == text_len) break;
            if (i == text_len && text_len > 0 && text_buffer[i - 1] == TE_LINE_END) {
                continue;
            }
            if (i == text_len) break;
        }
    }
}

static void update_cursor_line(void) {
    cursor_line = 0;
    for (word l = 0; l < num_lines; l++) {
        if (cursor_idx >= lines[l].start_idx && cursor_idx <= lines[l].end_idx) {
            if (cursor_idx == lines[l].end_idx && l + 1 < num_lines) {
                continue;
            }
            cursor_line = l;
            break;
        }
    }
}

static word get_x_for_index(word l, word target_idx) {
    byte scan_font = lines[l].start_font;
    byte scan_w, scan_h;
    get_font_size(scan_font, &scan_w, &scan_h);
    word x = 0;
    for (word i = lines[l].start_idx; i < target_idx && i < lines[l].end_idx; ) {
        byte c = (byte)text_buffer[i];
        switch (c) {
            case TE_LINE_END:
                i++;
                return x;
            case TE_FONT_6x7:
            case TE_FONT_6x8:
            case TE_FONT_6x10:
            case TE_FONT_7x10:
            case TE_FONT_8x8:
            case TE_FONT_8x12:
            case TE_FONT_12x16:
                scan_font = c;
                get_font_size(scan_font, &scan_w, &scan_h);
                i++;
                break;
            case TE_COLOUR_LIGHT_GREY:
            case TE_COLOUR_DARK_GREY:
            case TE_COLOUR_BLACK:
                i++;
                break;
            default:
                if (c >= 32) x += scan_w;
                i++;
                break;
        }
    }
    return x;
}

static word get_index_for_x(word l, word target_x) {
    byte scan_font = lines[l].start_font;
    byte scan_w, scan_h;
    get_font_size(scan_font, &scan_w, &scan_h);
    word x = 0;
    word last_valid = lines[l].start_idx;
    word i = lines[l].start_idx;
    byte done = 0;
    for (; i < lines[l].end_idx && !done; ) {
        last_valid = i;
        byte c = (byte)text_buffer[i];
        switch (c) {
            case TE_LINE_END:
                done = 1;
                break;
            case TE_FONT_6x7:
            case TE_FONT_6x8:
            case TE_FONT_6x10:
            case TE_FONT_7x10:
            case TE_FONT_8x8:
            case TE_FONT_8x12:
            case TE_FONT_12x16:
                scan_font = c;
                get_font_size(scan_font, &scan_w, &scan_h);
                i++;
                break;
            case TE_COLOUR_LIGHT_GREY:
            case TE_COLOUR_DARK_GREY:
            case TE_COLOUR_BLACK:
                i++;
                break;
            default:
                if (c >= 32) {
                    if (x + (scan_w / 2) > target_x) {
                        done = 1;
                        break;
                    }
                    x += scan_w;
                }
                i++;
                break;
        }
    }
    word res = i;
    if (res == lines[l].end_idx && l + 1 < num_lines) {
        if (res > 0 && text_buffer[res - 1] != TE_LINE_END) {
            res = last_valid;
        }
    }
    return res;
}

static void bring_cursor_into_view(void) {
    if (cursor_line < top_line) {
        top_line = cursor_line;
    } else {
        while (1) {
            int y = 1;
            for (word l = top_line; l <= cursor_line; l++) {
                y += lines[l].max_height;
            }
            if (y > 64) {
                top_line++;
            } else {
                break;
            }
        }
    }
}

static void cursor_left(void) {
    if (cursor_idx > 0) cursor_idx--;
    update_cursor_line();
    cursor_x_target = get_x_for_index(cursor_line, cursor_idx);
    bring_cursor_into_view();
}

static void cursor_right(void) {
    if (cursor_idx < text_len) cursor_idx++;
    update_cursor_line();
    cursor_x_target = get_x_for_index(cursor_line, cursor_idx);
    bring_cursor_into_view();
}

static void cursor_up(void) {
    if (cursor_line > 0) {
        cursor_idx = get_index_for_x(cursor_line - 1, cursor_x_target);
        update_cursor_line();
    } else {
        cursor_idx = 0;
        update_cursor_line();
    }
    bring_cursor_into_view();
}

static void cursor_down(void) {
    if (cursor_line + 1 < num_lines) {
        cursor_idx = get_index_for_x(cursor_line + 1, cursor_x_target);
        update_cursor_line();
    } else {
        cursor_idx = text_len;
        update_cursor_line();
    }
    bring_cursor_into_view();
}

static void insert_char(byte c) {
    if (text_len + 1 >= text_capacity) {
        word new_cap = text_capacity + TE_DEFAULT_BUF;
        char *new_buf = hrealloc(text_buffer, new_cap);
        if (!new_buf) return;
        text_buffer = new_buf;
        text_capacity = new_cap;
    }
    for (word i = text_len; i > cursor_idx; i--) {
        text_buffer[i] = text_buffer[i - 1];
    }
    text_buffer[cursor_idx] = c;
    text_len++;
    text_buffer[text_len] = TE_TEXT_END;
    cursor_idx++;
    
    do_layout();
    update_cursor_line();
    cursor_x_target = get_x_for_index(cursor_line, cursor_idx);
    bring_cursor_into_view();
}

static void backspace(void) {
    if (cursor_idx > 0) {
        for (word i = cursor_idx - 1; i < text_len - 1; i++) {
            text_buffer[i] = text_buffer[i + 1];
        }
        text_len--;
        text_buffer[text_len] = TE_TEXT_END;
        cursor_idx--;
        
        do_layout();
        update_cursor_line();
        cursor_x_target = get_x_for_index(cursor_line, cursor_idx);
        bring_cursor_into_view();
    }
}

static void render(void) {
    Write2RealScreen = 0;
    tui_clear_screen();
    
    int y = 1;
    word l = top_line;
    
    while (l < num_lines) {
        if (y + lines[l].max_height > 64) break;
        
        byte cur_font = lines[l].start_font;
        byte cur_col = lines[l].start_col;
        byte cur_w, cur_h;
        get_font_size(cur_font, &cur_w, &cur_h);
        
        word x = 0;
        word i = lines[l].start_idx;
        byte done = 0;
        
        while (i < lines[l].end_idx && !done) {
            byte c = (byte)text_buffer[i];
            
            if (i == cursor_idx && blink_state) {
                tui_draw_line(x, y + lines[l].max_height - cur_h, x, y + lines[l].max_height - 1, cur_col, 1, TUI_LINE_STYLE_SOLID);
            }
            
            switch (c) {
                case TE_LINE_END:
                    i++;
                    done = 1;
                    break;
                case TE_FONT_6x7:
                case TE_FONT_6x8:
                case TE_FONT_6x10:
                case TE_FONT_7x10:
                case TE_FONT_8x8:
                case TE_FONT_8x12:
                case TE_FONT_12x16:
                    cur_font = c;
                    get_font_size(cur_font, &cur_w, &cur_h);
                    i++;
                    break;
                case TE_COLOUR_LIGHT_GREY:
                    cur_col = TUI_COLOUR_LIGHT_GREY;
                    i++;
                    break;
                case TE_COLOUR_DARK_GREY:
                    cur_col = TUI_COLOUR_DARK_GREY;
                    i++;
                    break;
                case TE_COLOUR_BLACK:
                    cur_col = TUI_COLOUR_BLACK;
                    i++;
                    break;
                default:
                    if (c >= 32) {
                        tui_draw_char(x, y + lines[l].max_height - cur_h, (char)c, cur_font - 2, 0, 0, 0, cur_col);
                        x += cur_w;
                    }
                    i++;
                    break;
            }
        }
        
        if (cursor_idx == lines[l].end_idx && cursor_line == l) {
            if (blink_state) {
                tui_draw_line(x, y + lines[l].max_height - cur_h, x, y + lines[l].max_height - 1, cur_col, 1, TUI_LINE_STYLE_SOLID);
            }
        }
        
        y += lines[l].max_height;
        l++;
    }
    
    tui_render_buffer();
}

void start_text_edit(void) {
    text_editor_open(0);
}

void text_editor_open(const char *filename) {
    text_capacity = TE_DEFAULT_BUF;
    text_buffer = halloc(text_capacity);
    if (!text_buffer) return;
    
    text_len = 0;
    text_buffer[0] = TE_TEXT_END;
    cursor_idx = 0;
    cursor_x_target = 0;
    cursor_line = 0;
    top_line = 0;
    blink_state = 1;
    my_last_key = 0xff;
    last_sec = rtc_get_seconds();
    
    if (filename) {
        fs_node_t *file = fs_get_node_from_path(filename, FS_ROOT);
        if (file) {
            if (file->size >= text_capacity) {
                text_capacity = file->size + TE_DEFAULT_BUF;
                text_buffer = hrealloc(text_buffer, text_capacity);
                if (!text_buffer) return;
            }
            text_len = fs_read_file(file, text_buffer, text_capacity);
            text_buffer[text_len] = TE_TEXT_END;
        }
    }
    
    do_layout();
    update_cursor_line();
    bring_cursor_into_view();
    
    byte needs_render = 1;
    byte running = 1;
    
    while (running) {
        byte key = CheckButtons();
        
        if (key != my_last_key && key != 0xff) {
            switch (key) {
                case SP_UP: cursor_up(); break;
                case SP_DOWN: cursor_down(); break;
                case SP_LEFT: cursor_left(); break;
                case SP_RIGHT: cursor_right(); break;
                case SP_DEL: backspace(); break;
                case SP_EXE:
                case SP_OKAY:
                case SP_ANS:
                    insert_char(TE_LINE_END);
                    break;
                case SP_PGUP:
                    if (cursor_line > 6) cursor_idx = get_index_for_x(cursor_line - 6, cursor_x_target); 
                    else cursor_idx = 0;
                    update_cursor_line();
                    bring_cursor_into_view();
                    break;
                case SP_PGDOWN:
                    if (cursor_line + 6 < num_lines) cursor_idx = get_index_for_x(cursor_line + 6, cursor_x_target);
                    else cursor_idx = text_len;
                    update_cursor_line();
                    bring_cursor_into_view();
                    break;
                case SP_FUNC: {
                    byte f = TE_DEFAULT_FONT;
                    for (word i = lines[cursor_line].start_idx; i < cursor_idx; i++) {
                        if (text_buffer[i] >= TE_FONT_6x7 && text_buffer[i] <= TE_FONT_12x16) f = text_buffer[i];
                    }
                    f++;
                    if (f > TE_FONT_12x16) f = TE_FONT_6x7;
                    insert_char(f);
                    break;
                }
                case SP_HOME:
                    if (filename) {
                        fs_node_t *file = fs_touch(FS_ROOT, filename, PERMS_RWX);
                        if (file) {
                            fs_write_file(file, text_buffer, text_len);
                        }
                    }
                    running = 0;
                    break;
            }
            needs_render = 1;
        }
        my_last_key = key;
        
        if (!running) break;
        
        char tmp[2] = {0, 0};
        input_text(tmp, 2);
        if (tmp[0] != 0) {
            insert_char(tmp[0]);
            needs_render = 1;
        }
        
        byte post_key = CheckButtons();
        if (post_key != key) {
            my_last_key = post_key;
            needs_render = 1;
        }
        
        byte sec = rtc_get_seconds();
        if (sec != last_sec) {
            blink_state = !blink_state;
            last_sec = sec;
            needs_render = 1;
        }
        
        if (needs_render) {
            render();
            needs_render = 0;
        }
    }
    
    if (text_buffer) {
        hfree(text_buffer);
        text_buffer = 0;
    }
}

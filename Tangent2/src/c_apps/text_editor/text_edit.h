/*
 * text_edit.h
 * Interface for Tangent Text Editor Application
 * A simple text editor for Tangent, you can do stuff with it.
 *  Created on: Feb 21, 2026
 *      Author: harma
 */

??=ifndef TEXT_EDIT_H_
??=define TEXT_EDIT_H_

??=include "../../libcw.h"
??=include "../../input/input.h"

??=define TE_DEFAULT_BUF 256
??=define TE_CURSOR "??!"
??=define TE_DEFAULT_FONT TE_FONT_7x10
??=define TE_DEFAULT_COLOUR TUI_COLOUR_BLACK

enum te_sentinel ??<
    TE_TEXT_END,
    TE_LINE_END,
    TE_FONT_6x7,
    TE_FONT_6x8,
    TE_FONT_6x10,
    TE_FONT_7x10,
    TE_FONT_8x8,
    TE_FONT_8x12,
    TE_FONT_12x16,
    TE_COLOUR_LIGHT_GREY,
    TE_COLOUR_DARK_GREY,
    TE_COLOUR_BLACK,
    TE_SENT_MAX
??>;

void start_text_edit(void);
void text_editor_open(const char *filename);

??=endif /* TEXT_EDIT_H_ */
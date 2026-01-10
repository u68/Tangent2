/*
 * desktop.h
 * Interface for desktop environment
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#ifndef DESKTOP_H_
#define DESKTOP_H_

#include "base.h"
#include "Tui/glib.h"

TmlElement* desktop_init(void);
void desktop_render(TmlElement* root);
TmlElement* desktop_get_window(TmlElement* root);
TmlElement* desktop_get_time_text(TmlElement* root);

#endif

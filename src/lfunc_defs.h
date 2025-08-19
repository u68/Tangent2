#ifndef LFUNC_DEFS_H
#define LFUNC_DEFS_H
#include "lbase.h"


void linit_prog();
void lkill(word PID);
void lload_from_rom(word adr);
byte lread_byte(word PID, word addr);
word lread_word(word PID, word addr);
void lwrite_byte(word PID, word addr, byte data);
void lwrite_word(word PID, word addr, word data);
void lexc_instruction(word PID);

void lmemcpy(word dest, word src, word size);

#endif

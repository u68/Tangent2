#ifndef VM_H
#define VM_H

#include "base.h"

//I never made a vm.h on tangent 🥀🥀🥀

void invalid_instruction(word opc);
void init_prog();
void kill(word PID);  //pid is now just direct address instead of bit with magic math
void load_from_rom(word adr);
byte read_byte(word PID, word addr);
word read_word(word PID, word addr);
void write_byte(word PID, word addr, byte data);
void write_word(word PID, word addr, word data);
void exc_instruction(word PID);

//"unexpected end of file, sorry to scare you ccu8"
#endif

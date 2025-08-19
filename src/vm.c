//VIRTUAL MACHINE 😭😭😭😭😭😭😭😭😭😭

#include "base.h"
#include "vm.h"
#include "io.h"
#include "vm_config.h"

const byte error[] = "twin smth aint right";
const byte msg[] = "check 0x9000";
void invalid_instruction(word opc)
{
    print(error, 1, 1, 2);
    print(msg, 1, 2, 2);
    derefw(0x9000) = opc;
}

void memcpy(word dest, word src, word size)
{
    word i = 0;
    for(i = 0;i < size;i++)
    {
        deref(dest+i) = deref(src+i);
    }
}

void bubble_sortw(word arr[], byte size)
{
    byte i, j, temp;

    for(i = 0; i < size - 1; i++)
    {
        for(j = 0; j < size - i - 1; j++)
        {
            if(arr[j] > arr[j + 1])
            {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void init_prog()
{
    // I lowk dunno what this is for now so ill leave it here lol
}
void kill(word PID);  //pid is now just direct address instead of bit with magic math
void load_from_rom(word adr)
{
    //go through start of ram and check for next free program slot
    //TODO: allow loading from ANYWHERE in rom instead of rom window (let that sink in)
    word *program_array = (word *)MAXIMUS_PROGRAMUS_START;
    byte i = 0;
    word dadr = derefw(adr); //dadr is precalculated size of program determined by assembler
    bubble_sortw(program_array, MAXIMUS_PROGRAMUS);
    while(program_array[i]) (i++);
    program_array[i] = adr;
}
byte read_byte(word PID, word addr);
word read_word(word PID, word addr);
void write_byte(word PID, word addr, byte data);
void write_word(word PID, word addr, word data);
void exc_instruction(word PID);
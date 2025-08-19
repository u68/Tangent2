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

void del_index(word arr[], byte size, byte index)
{
    byte i;
    if(index >= size) return;
    for(i = index; i < size - 1; i++)
    {
        arr[i] = arr[i + 1];
    }
    arr[size - 1] = 0; 
}

word get_program_start(word PID)
{
    word *program_start_array = (word *)PROGRAMUS_START_START;
    byte i = 0;
    bubble_sortw(program_start_array, MAXIMUS_PROGRAMUS);
    while(program_start_array[i])
    {
        if(program_start_array[i] == PID) return program_start_array[i];
        i++;
    }
    return 0; //not found
}

void init_prog()
{
    // I lowk dunno what this is for now so ill leave it here lol
}
void kill(word PID)  //pid is now just direct address instead of bit with magic math but you use it as an index to an array as an index to an array to get the true ram adress of the program
{
    del_index((word *)MAXIMUS_PROGRAMUS_START, MAXIMUS_PROGRAMUS, PID);
    del_index((word *)PROGRAMUS_START_START, MAXIMUS_PROGRAMUS, PID);
}
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
byte read_byte(word PID, word addr)
{
    word mrams = derefw(PID + 2);
    if(addr > mrams) {return 0;} //out of bounds

}
word read_word(word PID, word addr);
void write_byte(word PID, word addr, byte data);
void write_word(word PID, word addr, word data);
void exc_instruction(word PID);
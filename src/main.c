#include "base.h"
#include "io.h"
#include "vm.h"
//#define LEGACY_VM // uncomment this line to run old tangent programs, but no further support will be made
void custom_break()
{
	while(1)
	{
		//debug stuff goes in here but my brain lowk broken rn so idk
	}

}
#ifdef LEGACY_VM
#include "lconfig.h"
void main()
{
    byte progs = 0;
    word PID = 0;
    word i = 0;

    word prog_size = PROGRAM_SIZE;
    word prog_count = PROGRAM_COUNT;
    for (i=0;i<0xEE00;i++)
    {
    	deref(i)=0;
    }
    deref(0xf037) = 0;
    for (i=0xF800;i<0xFFFF;i++)
    {
    	deref(i)=0;
    }
    deref(0xf037) = 4;
    for (i=0xF800;i<0xFFFF;i++)
    {
    	deref(i)=0;
    }
    deref(PROGRAM_COUNT) = 0; 
    derefw(0x9C06) = 30;
    load_from_rom(0x5000);
    while (1)
    {
        for (progs = 0;progs < 16;progs++)
        {

            if ((((derefw(prog_count) << progs) & 32768) >> 15) == 1)
            {
                PID = (prog_size*progs)+0x9C02; //program * size + prog adr + prog count + vram1/2
                exc_instruction(PID);
            }
        }
    }
}
#else
void main()
{
	invalid_instruction(0x1234);
}
#endif

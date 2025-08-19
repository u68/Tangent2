#include "lbase.h"
#include "lconfig.h"
#include "lfunc_defs.h"

void linit_prog()
{
    byte prog_index = 0;
    word progs = derefw(0x9000);
    for (prog_index = 0;prog_index < 16;prog_index++)
    {
        if((((progs << prog_index) & 32768) >> 15) == 0)
        {
            derefw(0x9000) |= (32768 >> prog_index);
            break;
        }
    }
}

void lkill(word PID)
{
    byte prog_index = (PID - 0x9C02) / PROGRAM_SIZE;
    derefw(PROGRAM_COUNT) &= ~(1 << prog_index);
}

void lmemcpy(word dest, word src, word size)
{
    word i = 0;
    for(i = 0;i < size;i++)
    {
        deref(dest+i) = deref(src+i);
    }
}

void lload_from_rom(word adr)
{
    byte prog_index = 0;
    word progs = derefw(PROGRAM_COUNT);
    word prog_size = PROGRAM_SIZE;
    for (prog_index = 0;prog_index < 16;prog_index++)
    {
        //deref(0x9000) = 2;
        if(((progs << prog_index) & 32768) == 0)
        {
            //deref(0x9000) = 4;
            //custom_break();
            memcpy((prog_index*prog_size)+0x9C02,adr,PROGRAM_SIZE);
            //derefw(0x9100) = (prog_index*prog_size)+0x9C02;
            init_prog();
            break;
        }
    }
}

byte lread_byte(word PID, word addr)
{
    if(addr < RAM_SIZE)
    {
        return deref(PID + 10 + NUM_REGS + addr);
    } else {
        custom_break();
    }
    return 0;
}

word lread_word(word PID, word addr)
{
    if(addr < RAM_SIZE)
    {
        return derefw(PID + 10 + NUM_REGS + addr);
    } else {
        custom_break();
    }
    return 0;
}

void lwrite_byte(word PID, word addr, byte data)
{
    if(addr < RAM_SIZE)
    {
        deref(PID + 10 + NUM_REGS + addr) = data;
    } else {
        custom_break();
    }
}

void lwrite_word(word PID, word addr, word data)
{
    if(addr < RAM_SIZE)
    {
        derefw(PID + 10 + NUM_REGS + addr) = data;
    } else {
        custom_break();
    }
}

void lcmp_flags_word(word val1, word val2, word flags_addr) {
    deref(flags_addr) = 0;
    if (val1 == val2) deref(flags_addr) |= 0x01;
    if (val1 >  val2) deref(flags_addr) |= 0x02;
    if (val1 <  val2) deref(flags_addr) |= 0x04;
    if (val1 >= val2) deref(flags_addr) |= 0x08;
    if (val1 <= val2) deref(flags_addr) |= 0x10;
}

void lcmp_flags_byte(byte val1, byte val2, word flags_addr) {
    deref(flags_addr) = 0;
    if (val1 == val2) deref(flags_addr) |= 0x01;
    if (val1 >  val2) deref(flags_addr) |= 0x02;
    if (val1 <  val2) deref(flags_addr) |= 0x04;
    if (val1 >= val2) deref(flags_addr) |= 0x08;
    if (val1 <= val2) deref(flags_addr) |= 0x10;
}

void lexc_instruction(word PID)
{
    word PC = PID + 2;
    word SP = PID + 4;
    word LR = PID + 6;
    word FLAGS = PID + 8;
    word regs = PID + 10;
    word ram = PID + 10 + NUM_REGS;
    word code = PID + 10 + NUM_REGS + RAM_SIZE;
    word instruction = derefw(derefw(PC)+code);
    byte opcode = (instruction & 0xFF00) >> 8;
    byte operand = instruction & 0x00FF;
    byte reg1 = (operand & 0xF0) >> 4;
    byte reg2 = operand & 0x0F;
    word cpc;

    //derefw(0x910A) = derefw(PC)+code;
    //derefw(0x910C) = derefw(derefw(PC)+code);
    //deref(0x910E) = opcode;
    //custom_break();
    derefw(PC) += 2;
    cpc = derefw(PC);
    switch(opcode)
    {
        //BYTE SIZE OPERATIONS
        case 0x00:
            //MOV
            deref(regs + reg1) = deref(regs + reg2);
            break;
        case 0x01:
            //ADD
            deref(regs + reg1) += deref(regs + reg2);
            break;
        case 0x02:
            //SUB
            deref(regs + reg1) -= deref(regs + reg2);
            break;
        case 0x03:
            //MUL
            deref(regs + reg1) *= deref(regs + reg2);
            break;
        case 0x04:
            //DIV
            deref(regs + reg1) /= deref(regs + reg2);
            break;
        case 0x05:
            //SLL
            deref(regs + reg1) = deref(regs + reg1) << deref(regs + reg2);
            break;
        case 0x06:
            //SRL
            deref(regs + reg1) = deref(regs + reg1) >> deref(regs + reg2);
            break;
        case 0x07:
            //L
            deref(regs + reg1) = lread_byte(PID, derefw(regs + reg2));
            break;
        case 0x08:
            //ST
            lwrite_byte(PID, derefw(regs + reg2), deref(regs + reg1));
            break;
        case 0x09:
            //AND
            deref(regs + reg1) &= deref(regs + reg2);
            break;
        case 0x0A:
            //OR
            deref(regs + reg1) |= deref(regs + reg2);
            break;
        case 0x0B:
            //XOR
            deref(regs + reg1) ^= deref(regs + reg2);
            break;
        case 0x0C:
            //INC
            deref(regs + reg1)+=1;
            //deref(regs) = 0xFF;
            break;
        case 0x0D:
            //DEC
            deref(regs + reg1)-=1;
            break;
        case 0x0E:
            //CMP
            lcmp_flags_byte(deref(regs + reg1), deref(regs + reg2), FLAGS);
            break;
        case 0x0F:
            //BAL
            derefw(PC) = derefw(regs + reg1);
            break;
        case 0x10:
            //BEQ
            if(deref(FLAGS) & 0x01)
            {
                derefw(PC) = derefw(regs + reg1);
            }
            break;
        case 0x11:
            //BNE
            if(!(deref(FLAGS) & 0x01))
            {
                derefw(PC) = derefw(regs + reg1);
            }
            break;
        case 0x12:
            //BGT
            if(deref(FLAGS) & 0x02)
            {
                derefw(PC) = derefw(regs + reg1);
            }
            break;
        case 0x13:
            //BLT
            if(deref(FLAGS) & 0x04)
            {
                derefw(PC) = derefw(regs + reg1);
            }
            break;
        case 0x14:
            //BGE
            if(deref(FLAGS) & 0x08)
            {
                derefw(PC) = derefw(regs + reg1);
            }
            break;
        case 0x15:
            //BLE
            if(deref(FLAGS) & 0x10)
            {
                derefw(PC) = derefw(regs + reg1);
            }
            break;
    //WORD SIZE OPERATIONS
        case 0x16:
            //MOV
            derefw(regs + reg1) = derefw(regs + reg2);
            break;
        case 0x17:
            //ADD
            derefw(regs + reg1) += derefw(regs + reg2);
            break;
        case 0x18:
            //SUB
            derefw(regs + reg1) -= derefw(regs + reg2);
            break;
        case 0x19:
            //MUL
            derefw(regs + reg1) *= derefw(regs + reg2);
            break;
        case 0x1A:
            //DIV
            derefw(regs + reg1) /= derefw(regs + reg2);
            break;
        case 0x1B:
            //SLL
            derefw(regs + reg1) = derefw(regs + reg1) << derefw(regs + reg2);
            break;
        case 0x1C:
            //SRL
            derefw(regs + reg1) = derefw(regs + reg1) >> derefw(regs + reg2);
            break;
        case 0x1D:
            //L
            derefw(regs + reg1) = lread_word(PID, derefw(regs + reg2));
            break;
        case 0x1E:
            //ST
            lwrite_word(PID, derefw(regs + reg2), derefw(regs + reg1));
            break;
        case 0x1F:
            //AND
            derefw(regs + reg1) &= derefw(regs + reg2);
            break;
        case 0x20:
            //OR
            derefw(regs + reg1) |= derefw(regs + reg2);
            break;
        case 0x21:
            //XOR
            derefw(regs + reg1) ^= derefw(regs + reg2);
            break;
        case 0x22:
            //INC
            derefw(regs + reg1)++;
            break;
        case 0x23:
            //DEC
            derefw(regs + reg1)--;
            break;
        case 0x24:
            //CMP
            lcmp_flags_word(derefw(regs + reg1), derefw(regs + reg2), FLAGS);
            break;
    //IMMEDIATE OPERATIONS
        case 0x30:
            //MOV
            deref(regs + reg1) = deref(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x31:
            //ADD
            deref(regs + reg1) += deref(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x32:
            //SUB
            deref(regs + reg1) -= deref(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x33:
            //MUL
            deref(regs + reg1) *= deref(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x34:
            //DIV
            deref(regs + reg1) /= deref(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x35:
            //SLL
            deref(regs + reg1) = deref(regs + reg1) << deref(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x36:
            //SRL
            deref(regs + reg1) = deref(regs + reg1) >> deref(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x37:
            //L
            deref(regs + reg1) = lread_byte(PID, derefw(code + cpc));
            derefw(PC) += 2;
            break;
        case 0x38:
            //ST
            lwrite_byte(PID, derefw(code + cpc), deref(regs + reg1));
            derefw(PC) += 2;
            break;
        case 0x39:
            //AND
            deref(regs + reg1) &= deref(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x3A:
            //OR
            deref(regs + reg1) |= deref(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x3B:
            //XOR
            deref(regs + reg1) ^= deref(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x3C:
            //INC
            deref(regs + reg1)++;
            break;
        case 0x3D:
            //DEC
            deref(regs + reg1)--;
            break;
        case 0x3E:
            //CMP
            lcmp_flags_byte(deref(regs + reg1), deref(cpc + code), FLAGS);
            derefw(PC) += 2;
            break;
    //WORD IMMEADIATE OPERATIONS
        case 0x40:
            //MOV
            derefw(regs + reg1) = derefw(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x41:
            //ADD
            derefw(regs + reg1) += derefw(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x42:
            //SUB
            derefw(regs + reg1) -= derefw(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x43:
            //MUL
            derefw(regs + reg1) *= derefw(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x44:
            //DIV
            derefw(regs + reg1) /= derefw(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x45:
            //SLL
            derefw(regs + reg1) = derefw(regs + reg1) << derefw(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x46:
            //SRL
            derefw(regs + reg1) = derefw(regs + reg1) >> derefw(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x47:
            //L
            derefw(regs + reg1) = lread_word(PID, derefw(code + cpc));
            derefw(PC) += 2;
            break;
        case 0x48:
            //ST
            lwrite_word(PID, derefw(code + cpc), derefw(regs + reg1));
            derefw(PC) += 2;
            break;
        case 0x49:
            //AND
            derefw(regs + reg1) &= derefw(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x4A:
            //OR
            derefw(regs + reg1) |= derefw(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x4B:
            //XOR
            derefw(regs + reg1) ^= derefw(code + cpc);
            derefw(PC) += 2;
            break;
        case 0x4C:
            //INC
            derefw(regs + reg1)++;
            break;
        case 0x4D:
            //DEC
            derefw(regs + reg1)--;
            break;
        case 0x4E:
            //CMP
            lcmp_flags_word(derefw(regs + reg1), derefw(cpc + code), FLAGS);
            derefw(PC) += 2;
            break;
    //STACK OPERATIONS
        case 0x50:
            //PUSH
            derefw(SP) -= 2;
            lwrite_word(PID, derefw(SP), derefw(regs + reg1));
            break;
        case 0x51:
            //POP
            derefw(regs + reg1) = lread_word(PID, derefw(SP));
            derefw(SP) += 2;
            break;
        case 0x52:
            //PUSH16
            derefw(SP) -= 2;
            lwrite_word(PID, derefw(SP), derefw(regs + reg1));
        case 0x53:
            //POP16
            derefw(regs + reg1) = lread_word(PID, derefw(SP));
            derefw(SP) += 2;
            break;
    //CONTROL OPERATIONS
        case 0x60:
            //B
            derefw(PC) = derefw(regs + reg1);
            break;
        case 0x61:
            //BL
            derefw(LR) = cpc;
            derefw(PC) = derefw(regs + reg1);
            break;
        case 0x62:
            //RT
            derefw(PC) = derefw(LR);
            break;
        case 0x70:
            //SYSCALL
            if(operand == 0)
            {
                //PRINT
                print((const byte*)derefw(regs + 0)+code+derefw(PC), deref(regs + 2), deref(regs + 3), deref(regs + 4));
            }
            else if(operand == 1)
            {
                //SETPIXEL
                set_pixel(deref(regs + 0), deref(regs + 1), deref(regs + 2));
                //custom_break();
            }
            else if(operand == 2)
            {
                //PLOTLINE
                plot_line(deref(regs + 0), deref(regs + 1), deref(regs + 2), deref(regs + 3), deref(regs + 4));
            }
            /*else if(operand == 3)
            {
                //MENU
                deref(regs + 0) = menu((const byte*)derefw(regs + 0), (const byte*)derefw(regs + 2), (const byte*)derefw(regs + 4), (const byte*)derefw(regs + 6));
            }
            else if(operand == 4)
            {
                //MENU2
                deref(regs + 0) = menu_2((const byte*)derefw(regs + 0), (const byte*)derefw(regs + 2));
            }*/
            else if(operand == 5)
            {
                //CHECKBUTTONS
                deref(regs + 0) = CheckButtons();
            }
            else if(operand == 6)
            {
                //RENDER
                render(vram1);
            }
            break;
    //BRANCH IMMIDIATE OPERATIONS
        case 0x80:
            //BAL
            derefw(PC) = derefw(code + cpc);
            break;
        case 0x81:
            //BEQ
            if(deref(FLAGS) & 0x01)
            {
                derefw(PC) = derefw(code + cpc);
            }
            break;
        case 0x82:
            //BNE
            if(!(deref(FLAGS) & 0x01))
            {
                derefw(PC) = derefw(code + cpc);
            }
            break;
        case 0x83:
            //BGT
            if(deref(FLAGS) & 0x02)
            {
                derefw(PC) = derefw(code + cpc);
            }
            break;
        case 0x84:
            //BLT
            if(deref(FLAGS) & 0x04)
            {
                derefw(PC) = derefw(code + cpc);
            }
            break;
        case 0x85:
            //BGE
            if(deref(FLAGS) & 0x08)
            {
                derefw(PC) = derefw(code + cpc);
            }
            break;
        case 0x86:
            //BLE
            if(deref(FLAGS) & 0x10)
            {
                derefw(PC) = derefw(code + cpc);
            }
            break;
        case 0x87:
            //B
            derefw(PC) = derefw(code + cpc);
            break;
        case 0x88:
            //BL
            derefw(LR) = cpc;
            derefw(PC) = derefw(code + cpc);
            break;

        case 0xFF:
            //break for testing instructions, do not include in code
            while (1)
            {
                reg1 = 0;
                //do nothing
            }
        default:
            invalid_instruction(instruction);
            break;
    }
}

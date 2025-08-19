;; Compile Options : /TML620909 /ML /near /SD /Oa /Ot /W 1 /Fasrc\ 
;; Version Number  : Ver.3.66.2
;; File Name       : legacy_vm.c

	type (ML620909) 
	model large, near
	$$NTABlegacy_vm segment table 2h #0h
	$$lcmp_flags_byte$legacy_vm segment code 2h any
	$$lcmp_flags_word$legacy_vm segment code 2h any
	$$lexc_instruction$legacy_vm segment code 2h any
	$$linit_prog$legacy_vm segment code 2h any
	$$lkill$legacy_vm segment code 2h any
	$$lload_from_rom$legacy_vm segment code 2h any
	$$lmemcpy$legacy_vm segment code 2h any
	$$lread_byte$legacy_vm segment code 2h any
	$$lread_word$legacy_vm segment code 2h any
	$$lwrite_byte$legacy_vm segment code 2h any
	$$lwrite_word$legacy_vm segment code 2h any
	$$NTABlexc_instruction$legacy_vm segment table 2h #0h
CVERSION 3.66.2
CGLOBAL 01H 03H 0000H "lexc_instruction" 08H 02H 07H 00H 83H 24H 00H 00H 07H
CGLOBAL 01H 02H 0000H "lread_word" 08H 02H 04H 00H 81H 06H 00H 00H 08H
CGLOBAL 01H 03H 0000H "linit_prog" 08H 02H 00H 00H 80H 04H 00H 00H 07H
CGLOBAL 01H 03H 0000H "lcmp_flags_word" 08H 02H 17H 00H 82H 08H 00H 00H 07H
CGLOBAL 01H 03H 0000H "lwrite_byte" 08H 02H 05H 00H 83H 08H 00H 00H 07H
CGLOBAL 01H 03H 0000H "lwrite_word" 08H 02H 06H 00H 83H 08H 00H 00H 07H
CGLOBAL 01H 03H 0000H "lcmp_flags_byte" 08H 02H 18H 00H 80H 04H 00H 00H 07H
CGLOBAL 01H 03H 0000H "lmemcpy" 08H 02H 08H 00H 82H 08H 00H 00H 07H
CGLOBAL 01H 02H 0000H "lread_byte" 08H 02H 03H 00H 81H 06H 00H 00H 00H
CGLOBAL 01H 03H 0000H "lload_from_rom" 08H 02H 02H 00H 81H 0cH 00H 00H 07H
CGLOBAL 01H 03H 0000H "lkill" 08H 02H 01H 00H 81H 04H 00H 00H 07H
CTYPEDEF 0000H 0000H 42H "ushort" 02H 00H 08H
CTYPEDEF 0000H 0000H 42H "byte" 02H 00H 00H
CTYPEDEF 0000H 0000H 42H "word" 02H 00H 08H
CFILE 0001H 0000006BH "..\\src\\lbase.h"
CFILE 0002H 00000013H "..\\src\\lconfig.h"
CFILE 0003H 00000012H "..\\src\\lfunc_defs.h"
CFILE 0000H 00000255H "..\\src\\legacy_vm.c"

	rseg $$linit_prog$legacy_vm
CFUNCTION 0

_linit_prog	:
CBLOCK 0 1 6

;;{
CLINEA 0000H 0001H 0006H 0001H 0001H
	push	xr4
CBLOCK 0 2 6
CLOCAL 46H 0001H 0018H 0002H "prog_index" 02H 00H 00H
CLOCAL 46H 0002H 0027H 0002H "progs" 02H 00H 08H

;;    byte prog_index = 0;
CLINEA 0000H 0001H 0007H 0005H 0018H
	mov	r4,	#00h

;;    word progs = derefw(0x9000);
CLINEA 0000H 0001H 0008H 0005H 0020H
	l	er6,	09000h

;;    for (prog_index = 0;prog_index < 16;prog_index++)
CLINEA 0000H 0000H 0009H 0005H 0035H
_$L3 :
CBLOCK 0 3 10

;;        if((((progs << prog_index) & 32768) >> 15) == 0)
CLINEA 0000H 0001H 000BH 0009H 0038H
	mov	er0,	er6
	mov	r2,	r4
_$M2 :
	cmp	r2,	#07h
	ble	_$M1
	sllc	r1,	#07h
	sll	r0,	#07h
	add	r2,	#0f9h
	bne	_$M2
_$M1 :
	sllc	r1,	r2
	mov	er2,	#0 
	and	r1,	#080h
	and	r2,	#00h
	and	r3,	#00h
	mov	r0,	r1
	mov	r1,	r2
	mov	r2,	r3
	srlc	r0,	#07h
	srlc	r1,	#07h
	sra	r2,	#07h
	extbw	er2
	cmp	r0,	#00h
	cmpc	r1,	#00h
	cmpc	r2,	#00h
	cmpc	r3,	#00h
	bne	_$L7
CBLOCK 0 4 12

;;            derefw(0x9000) |= (32768 >> prog_index);
CLINEA 0000H 0001H 000DH 000DH 0034H
	mov	r0,	#00h
	mov	r1,	#080h
	mov	er2,	#0 
_$M4 :
	cmp	r4,	#07h
	ble	_$M3
	srlc	r0,	#07h
	srlc	r1,	#07h
	srlc	r2,	#07h
	sra	r3,	#07h
	add	r4,	#0f9h
	bne	_$M4
_$M3 :
	srlc	r0,	r4
	srlc	r1,	r4
	l	er2,	09000h
	or	r2,	r0
	or	r3,	r1
	st	er2,	09000h

;;            break;
CLINEA 0000H 0001H 000EH 000DH 0012H
	bal	_$L2
CBLOCKEND 0 4 15

;;        }
CLINEA 0000H 0000H 000FH 0009H 0009H
_$L7 :
CBLOCKEND 0 3 16

;;    for (prog_index = 0;prog_index < 16;prog_index++)
CLINEA 0000H 0000H 0009H 0005H 0035H
	add	r4,	#01h
	cmp	r4,	#010h
	blt	_$L3
_$L2 :
CBLOCKEND 0 2 17

;;}
CLINEA 0000H 0001H 0011H 0001H 0001H
	pop	xr4
	rt
CBLOCKEND 0 1 17
CFUNCTIONEND 0


	rseg $$lkill$legacy_vm
CFUNCTION 1

_lkill	:
CBLOCK 1 1 20

;;{
CLINEA 0000H 0001H 0014H 0001H 0001H
	push	lr
CBLOCK 1 2 20
CRET 0000H
CARGUMENT 46H 0002H 0024H "PID" 02H 00H 01H
CLOCAL 4AH 0001H 0000H 0002H "prog_index" 02H 00H 00H

;;    derefw(PROGRAM_COUNT) &= ~(1 << prog_index);
CLINEA 0000H 0001H 0016H 0005H 0030H
	add	r0,	#0feh
	addc	r1,	#063h
	mov	er2,	#10
	bl	__uidivu8lw
	mov	r2,	r0
	add	r2,	#010h
	mov	er0,	#1 
_$M7 :
	cmp	r2,	#07h
	ble	_$M6
	sllc	r1,	#07h
	sll	r0,	#07h
	add	r2,	#0f9h
	bne	_$M7
_$M6 :
	sllc	r1,	r2
	sll	r0,	r2
	xor	r0,	#0ffh
	xor	r1,	#0ffh
	l	er2,	09000h
	and	r2,	r0
	and	r3,	r1
	st	er2,	09000h
CBLOCKEND 1 2 23

;;}
CLINEA 0000H 0001H 0017H 0001H 0001H
	pop	pc
CBLOCKEND 1 1 23
CFUNCTIONEND 1


	rseg $$lmemcpy$legacy_vm
CFUNCTION 8

_lmemcpy	:
CBLOCK 8 1 26

;;{
CLINEA 0000H 0001H 001AH 0001H 0001H
	push	fp
	mov	fp,	sp
	push	xr8
	push	er4
	mov	er10,	er2
	mov	er8,	er0
CBLOCK 8 2 26
CARGUMENT 46H 0002H 0028H "dest" 02H 00H 01H
CARGUMENT 46H 0002H 0029H "src" 02H 00H 01H
CARGUMENT 42H 0002H 0002H "size" 02H 00H 01H
CLOCAL 46H 0002H 0026H 0002H "i" 02H 00H 08H

;;    for(i = 0;i < size;i++)
CLINEA 0000H 0001H 001CH 0005H 001BH
	mov	er4,	#0 
	bal	_$L16
_$L13 :
CBLOCK 8 3 29

;;        deref(dest+i) = deref(src+i);
CLINEA 0000H 0001H 001EH 0009H 0025H
	mov	er0,	er10
	add	er0,	er4
	mov	er2,	er0
	mov	er0,	er8
	add	er0,	er4
	l	r2,	[er2]
	st	r2,	[er0]
CBLOCKEND 8 3 31

;;    for(i = 0;i < size;i++)
CLINEA 0000H 0000H 001CH 0005H 001BH
	add	er4,	#1 
_$L16 :
	l	er2,	2[fp]
	cmp	er4,	er2
	blt	_$L13
CBLOCKEND 8 2 32

;;}
CLINEA 0000H 0001H 0020H 0001H 0001H
	pop	er4
	pop	xr8
	mov	sp,	fp
	pop	fp
	rt
CBLOCKEND 8 1 32
CFUNCTIONEND 8


	rseg $$lload_from_rom$legacy_vm
CFUNCTION 2

_lload_from_rom	:
CBLOCK 2 1 35

;;{
CLINEA 0000H 0001H 0023H 0001H 0001H
	push	lr
	push	xr4
	push	er8
	mov	er8,	er0
CBLOCK 2 2 35
CRET 0006H
CARGUMENT 46H 0002H 0028H "adr" 02H 00H 01H
CLOCAL 46H 0001H 0018H 0002H "prog_index" 02H 00H 00H
CLOCAL 46H 0002H 0027H 0002H "progs" 02H 00H 08H
CLOCAL 4AH 0002H 0000H 0002H "prog_size" 02H 00H 08H

;;    byte prog_index = 0;
CLINEA 0000H 0001H 0024H 0005H 0018H
	mov	r4,	#00h

;;    word progs = derefw(PROGRAM_COUNT);
CLINEA 0000H 0001H 0025H 0005H 0027H
	l	er6,	09000h

;;    for (prog_index = 0;prog_index < 16;prog_index++)
CLINEA 0000H 0000H 0027H 0005H 0035H
_$L20 :
CBLOCK 2 3 40

;;        if(((progs << prog_index) & 32768) == 0)
CLINEA 0000H 0001H 002AH 0009H 0030H
	mov	er0,	er6
	mov	r2,	r4
_$M11 :
	cmp	r2,	#07h
	ble	_$M10
	sllc	r1,	#07h
	sll	r0,	#07h
	add	r2,	#0f9h
	bne	_$M11
_$M10 :
	sllc	r1,	r2
	tb	r1.7
	bne	_$L24
CBLOCK 2 4 43

;;            memcpy((prog_index*prog_size)+0x9C02,adr,PROGRAM_SIZE);
CLINEA 0000H 0001H 002EH 000DH 0043H
	mov	r0,	#01ah
	mov	r1,	#05h
	push	er0
	mov	er2,	er8
	mov	r0,	r4
	mov	r1,	#00h
	mov	er4,	er0
	sllc	r5,	#02h
	sll	r4,	#02h
	add	er4,	er0
	sllc	r5,	#04h
	sll	r4,	#04h
	add	er4,	er0
	add	er4,	er4
	add	er4,	er0
	sllc	r5,	#02h
	sll	r4,	#02h
	add	er4,	er0
	add	er4,	er4
	mov	er0,	er4
	add	r0,	#02h
	addc	r1,	#09ch
	bl	_memcpy
	add	sp,	#2 

;;            init_prog();
CLINEA 0000H 0001H 0030H 000DH 0018H
	bl	_init_prog

;;            break;
CLINEA 0000H 0001H 0031H 000DH 0012H
	bal	_$L19
CBLOCKEND 2 4 50

;;        }
CLINEA 0000H 0000H 0032H 0009H 0009H
_$L24 :
CBLOCKEND 2 3 51

;;    for (prog_index = 0;prog_index < 16;prog_index++)
CLINEA 0000H 0000H 0027H 0005H 0035H
	add	r4,	#01h
	cmp	r4,	#010h
	blt	_$L20
_$L19 :
CBLOCKEND 2 2 52

;;}
CLINEA 0000H 0001H 0034H 0001H 0001H
	pop	er8
	pop	xr4
	pop	pc
CBLOCKEND 2 1 52
CFUNCTIONEND 2


	rseg $$lread_byte$legacy_vm
CFUNCTION 3

_lread_byte	:
CBLOCK 3 1 55

;;{
CLINEA 0000H 0001H 0037H 0001H 0001H
	push	lr
	push	er8
	mov	er8,	er0
CBLOCK 3 2 55
CRET 0002H
CARGUMENT 46H 0002H 0028H "PID" 02H 00H 01H
CARGUMENT 46H 0002H 0025H "addr" 02H 00H 01H

;;    if(addr < RAM_SIZE)
CLINEA 0000H 0001H 0038H 0005H 0017H
	mov	er0,	er2
	cmp	r2,	#00h
	cmpc	r3,	#01h
	bge	_$L27
CBLOCK 3 3 57

;;        return deref(PID + 10 + NUM_REGS + addr);
CLINEA 0000H 0001H 003AH 0009H 0031H
	add	er0,	er8
	l	r0,	01ah[er0]
CBLOCKEND 3 3 59
CBLOCKEND 3 2 63

;;}
CLINEA 0000H 0001H 003FH 0001H 0001H
_$L26 :
	pop	er8
	pop	pc
_$L27 :

;;    } else {
CLINEA 0000H 0000H 003BH 0005H 000CH
CBLOCK 3 4 59

;;        custom_break();
CLINEA 0000H 0001H 003CH 0009H 0017H
	bl	_custom_break
CBLOCKEND 3 4 61

;;    return 0;
CLINEA 0000H 0001H 003EH 0005H 000DH
	mov	r0,	#00h
	bal	_$L26
CBLOCKEND 3 1 63
CFUNCTIONEND 3


	rseg $$lread_word$legacy_vm
CFUNCTION 4

_lread_word	:
CBLOCK 4 1 66

;;{
CLINEA 0000H 0001H 0042H 0001H 0001H
	push	lr
	push	er8
	mov	er8,	er0
CBLOCK 4 2 66
CRET 0002H
CARGUMENT 46H 0002H 0028H "PID" 02H 00H 01H
CARGUMENT 46H 0002H 0025H "addr" 02H 00H 01H

;;    if(addr < RAM_SIZE)
CLINEA 0000H 0001H 0043H 0005H 0017H
	mov	er0,	er2
	cmp	r2,	#00h
	cmpc	r3,	#01h
	bge	_$L31
CBLOCK 4 3 68

;;        return derefw(PID + 10 + NUM_REGS + addr);
CLINEA 0000H 0001H 0045H 0009H 0032H
	add	er0,	er8
	l	er0,	01ah[er0]
CBLOCKEND 4 3 70
CBLOCKEND 4 2 74

;;}
CLINEA 0000H 0001H 004AH 0001H 0001H
_$L30 :
	pop	er8
	pop	pc
_$L31 :

;;    } else {
CLINEA 0000H 0000H 0046H 0005H 000CH
CBLOCK 4 4 70

;;        custom_break();
CLINEA 0000H 0001H 0047H 0009H 0017H
	bl	_custom_break
CBLOCKEND 4 4 72

;;    return 0;
CLINEA 0000H 0001H 0049H 0005H 000DH
	mov	er0,	#0 
	bal	_$L30
CBLOCKEND 4 1 74
CFUNCTIONEND 4


	rseg $$lwrite_byte$legacy_vm
CFUNCTION 5

_lwrite_byte	:
CBLOCK 5 1 77

;;{
CLINEA 0000H 0001H 004DH 0001H 0001H
	push	lr
	push	fp
	mov	fp,	sp
	push	er8
	mov	er8,	er0
CBLOCK 5 2 77
CRET 0004H
CARGUMENT 46H 0002H 0028H "PID" 02H 00H 01H
CARGUMENT 46H 0002H 0025H "addr" 02H 00H 01H
CARGUMENT 42H 0001H 0006H "data" 02H 00H 00H

;;    if(addr < RAM_SIZE)
CLINEA 0000H 0001H 004EH 0005H 0017H
	mov	er0,	er2
	cmp	r2,	#00h
	cmpc	r3,	#01h
	bge	_$L35
CBLOCK 5 3 79

;;        deref(PID + 10 + NUM_REGS + addr) = data;
CLINEA 0000H 0001H 0050H 0009H 0031H
	add	er0,	er8
	l	r2,	6[fp]
	st	r2,	01ah[er0]
CBLOCKEND 5 3 81
	bal	_$L37
_$L35 :

;;    } else {
CLINEA 0000H 0000H 0051H 0005H 000CH
CBLOCK 5 4 81

;;        custom_break();
CLINEA 0000H 0001H 0052H 0009H 0017H
	bl	_custom_break
CBLOCKEND 5 4 83

;;    }
CLINEA 0000H 0000H 0053H 0005H 0005H
_$L37 :
CBLOCKEND 5 2 84

;;}
CLINEA 0000H 0001H 0054H 0001H 0001H
	pop	er8
	mov	sp,	fp
	pop	fp
	pop	pc
CBLOCKEND 5 1 84
CFUNCTIONEND 5


	rseg $$lwrite_word$legacy_vm
CFUNCTION 6

_lwrite_word	:
CBLOCK 6 1 87

;;{
CLINEA 0000H 0001H 0057H 0001H 0001H
	push	lr
	push	fp
	mov	fp,	sp
	push	er8
	mov	er8,	er0
CBLOCK 6 2 87
CRET 0004H
CARGUMENT 46H 0002H 0028H "PID" 02H 00H 01H
CARGUMENT 46H 0002H 0025H "addr" 02H 00H 01H
CARGUMENT 42H 0002H 0006H "data" 02H 00H 01H

;;    if(addr < RAM_SIZE)
CLINEA 0000H 0001H 0058H 0005H 0017H
	mov	er0,	er2
	cmp	r2,	#00h
	cmpc	r3,	#01h
	bge	_$L39
CBLOCK 6 3 89

;;        derefw(PID + 10 + NUM_REGS + addr) = data;
CLINEA 0000H 0001H 005AH 0009H 0032H
	add	er0,	er8
	l	er2,	6[fp]
	st	er2,	01ah[er0]
CBLOCKEND 6 3 91
	bal	_$L41
_$L39 :

;;    } else {
CLINEA 0000H 0000H 005BH 0005H 000CH
CBLOCK 6 4 91

;;        custom_break();
CLINEA 0000H 0001H 005CH 0009H 0017H
	bl	_custom_break
CBLOCKEND 6 4 93

;;    }
CLINEA 0000H 0000H 005DH 0005H 0005H
_$L41 :
CBLOCKEND 6 2 94

;;}
CLINEA 0000H 0001H 005EH 0001H 0001H
	pop	er8
	mov	sp,	fp
	pop	fp
	pop	pc
CBLOCKEND 6 1 94
CFUNCTIONEND 6


	rseg $$lcmp_flags_word$legacy_vm
CFUNCTION 23

_lcmp_flags_word	:
CBLOCK 23 1 96

;;void lcmp_flags_word(word val1, word val2, word flags_addr) {
CLINEA 0000H 0001H 0060H 0001H 003DH
	push	fp
	mov	fp,	sp
	push	xr8
	push	bp
	mov	er8,	er0
	mov	er10,	er2
CBLOCK 23 2 96
CARGUMENT 46H 0002H 0028H "val1" 02H 00H 01H
CARGUMENT 46H 0002H 0029H "val2" 02H 00H 01H
CARGUMENT 42H 0002H 0002H "flags_addr" 02H 00H 01H

;;    deref(flags_addr) = 0;
CLINEA 0000H 0001H 0061H 0005H 001AH
	l	bp,	2[fp]
	mov	r0,	#00h
	st	r0,	[bp]

;;    if (val1 == val2) deref(flags_addr) |= 0x01;
CLINEA 0000H 0001H 0062H 0005H 0030H
	cmp	er8,	er2
	bne	_$L43
	l	bp,	2[fp]
	l	r0,	[bp]
	or	r0,	#01h
	st	r0,	[bp]
_$L43 :

;;    if (val1 >  val2) deref(flags_addr) |= 0x02;
CLINEA 0000H 0001H 0063H 0005H 0030H
	cmp	er8,	er2
	ble	_$L45
	l	bp,	2[fp]
	l	r0,	[bp]
	or	r0,	#02h
	st	r0,	[bp]
_$L45 :

;;    if (val1 <  val2) deref(flags_addr) |= 0x04;
CLINEA 0000H 0001H 0064H 0005H 0030H
	cmp	er8,	er10
	bge	_$L47
	l	bp,	2[fp]
	l	r0,	[bp]
	or	r0,	#04h
	st	r0,	[bp]
_$L47 :

;;    if (val1 >= val2) deref(flags_addr) |= 0x08;
CLINEA 0000H 0001H 0065H 0005H 0030H
	cmp	er8,	er10
	blt	_$L49
	l	bp,	2[fp]
	l	r0,	[bp]
	or	r0,	#08h
	st	r0,	[bp]
_$L49 :

;;    if (val1 <= val2) deref(flags_addr) |= 0x10;
CLINEA 0000H 0001H 0066H 0005H 0030H
	cmp	er8,	er10
	bgt	_$L51
	l	bp,	2[fp]
	l	r0,	[bp]
	or	r0,	#010h
	st	r0,	[bp]
_$L51 :
CBLOCKEND 23 2 103

;;}
CLINEA 0000H 0001H 0067H 0001H 0001H
	pop	bp
	pop	xr8
	mov	sp,	fp
	pop	fp
	rt
CBLOCKEND 23 1 103
CFUNCTIONEND 23


	rseg $$lcmp_flags_byte$legacy_vm
CFUNCTION 24

_lcmp_flags_byte	:
CBLOCK 24 1 105

;;void lcmp_flags_byte(byte val1, byte val2, word flags_addr) {
CLINEA 0000H 0001H 0069H 0001H 003DH
	push	xr8
	mov	er10,	er2
	mov	r9,	r1
	mov	r8,	r0
CBLOCK 24 2 105
CARGUMENT 46H 0001H 001CH "val1" 02H 00H 00H
CARGUMENT 46H 0001H 001DH "val2" 02H 00H 00H
CARGUMENT 46H 0002H 0029H "flags_addr" 02H 00H 01H

;;    deref(flags_addr) = 0;
CLINEA 0000H 0001H 006AH 0005H 001AH
	mov	r2,	#00h
	st	r2,	[er10]

;;    if (val1 == val2) deref(flags_addr) |= 0x01;
CLINEA 0000H 0001H 006BH 0005H 0030H
	cmp	r1,	r0
	bne	_$L54
	mov	r0,	r2
	or	r0,	#01h
	st	r0,	[er10]
_$L54 :

;;    if (val1 >  val2) deref(flags_addr) |= 0x02;
CLINEA 0000H 0001H 006CH 0005H 0030H
	cmp	r8,	r1
	ble	_$L56
	l	r0,	[er10]
	or	r0,	#02h
	st	r0,	[er10]
_$L56 :

;;    if (val1 <  val2) deref(flags_addr) |= 0x04;
CLINEA 0000H 0001H 006DH 0005H 0030H
	cmp	r8,	r9
	bge	_$L58
	l	r0,	[er10]
	or	r0,	#04h
	st	r0,	[er10]
_$L58 :

;;    if (val1 >= val2) deref(flags_addr) |= 0x08;
CLINEA 0000H 0001H 006EH 0005H 0030H
	cmp	r8,	r9
	blt	_$L60
	l	r0,	[er10]
	or	r0,	#08h
	st	r0,	[er10]
_$L60 :

;;    if (val1 <= val2) deref(flags_addr) |= 0x10;
CLINEA 0000H 0001H 006FH 0005H 0030H
	cmp	r8,	r9
	bgt	_$L62
	l	r0,	[er10]
	or	r0,	#010h
	st	r0,	[er10]
_$L62 :
CBLOCKEND 24 2 112

;;}
CLINEA 0000H 0001H 0070H 0001H 0001H
	pop	xr8
	rt
CBLOCKEND 24 1 112
CFUNCTIONEND 24


	rseg $$lexc_instruction$legacy_vm
CFUNCTION 7

_lexc_instruction	:
CBLOCK 7 1 115

;;{
CLINEA 0000H 0001H 0073H 0001H 0001H
	push	lr
	push	fp
	mov	fp,	sp
	add	sp,	#-014
	push	xr8
	push	xr4
	push	bp
	mov	er8,	er0
CBLOCK 7 2 115
CRET 001AH
CARGUMENT 46H 0002H 0028H "PID" 02H 00H 01H
CLOCAL 42H 0002H 0002H 0002H "PC" 02H 00H 08H
CLOCAL 42H 0002H 0008H 0002H "SP" 02H 00H 08H
CLOCAL 42H 0002H 000AH 0002H "LR" 02H 00H 08H
CLOCAL 42H 0002H 0006H 0002H "FLAGS" 02H 00H 08H
CLOCAL 46H 0002H 0027H 0002H "regs" 02H 00H 08H
CLOCAL 4AH 0002H 0000H 0002H "ram" 02H 00H 08H
CLOCAL 42H 0002H 0004H 0002H "code" 02H 00H 08H
CLOCAL 42H 0002H 000EH 0002H "instruction" 02H 00H 08H
CLOCAL 42H 0001H 000BH 0002H "opcode" 02H 00H 00H
CLOCAL 46H 0001H 0017H 0002H "operand" 02H 00H 00H
CLOCAL 46H 0001H 001EH 0002H "reg1" 02H 00H 00H
CLOCAL 46H 0001H 001FH 0002H "reg2" 02H 00H 00H
CLOCAL 46H 0002H 0026H 0002H "cpc" 02H 00H 08H

;;    word PC = PID + 2;
CLINEA 0000H 0001H 0074H 0005H 0016H
	add	er0,	#2 
	st	er0,	-2[fp]

;;    word SP = PID + 4;
CLINEA 0000H 0001H 0075H 0005H 0016H
	mov	er0,	er8
	add	er0,	#4 
	st	er0,	-8[fp]

;;    word LR = PID + 6;
CLINEA 0000H 0001H 0076H 0005H 0016H
	mov	er0,	er8
	add	er0,	#6 
	st	er0,	-10[fp]

;;    word FLAGS = PID + 8;
CLINEA 0000H 0001H 0077H 0005H 0019H
	mov	er0,	er8
	add	er0,	#8 
	st	er0,	-6[fp]

;;    word regs = PID + 10;
CLINEA 0000H 0001H 0078H 0005H 0019H
	mov	er0,	er8
	add	er0,	#10
	mov	er6,	er0

;;    word code = PID + 10 + NUM_REGS + RAM_SIZE;
CLINEA 0000H 0001H 007AH 0005H 002FH
	mov	bp,	er8
	add	r12,	#01ah
	addc	r13,	#01h
	st	bp,	-4[fp]

;;    word instruction = derefw(derefw(PC)+code);
CLINEA 0000H 0001H 007BH 0005H 002FH
	l	er0,	02h[er8]
	add	er0,	bp
	l	er2,	[er0]
	st	er2,	-14[fp]

;;    byte opcode = (instruction & 0xFF00) >> 8;
CLINEA 0000H 0001H 007CH 0005H 002EH
	l	er2,	[er0]
	st	r3,	-11[fp]

;;    byte operand = instruction & 0x00FF;
CLINEA 0000H 0001H 007DH 0005H 0028H
	l	er0,	[er0]
	mov	r2,	r0
	mov	r3,	r0

;;    byte reg1 = (operand & 0xF0) >> 4;
CLINEA 0000H 0001H 007EH 0005H 0026H
	mov	r1,	#00h
	and	r0,	#0f0h
	and	r1,	#00h
	srlc	r0,	#04h
	mov	r10,	r0

;;    byte reg2 = operand & 0x0F;
CLINEA 0000H 0001H 007FH 0005H 001FH
	and	r2,	#0fh
	mov	r11,	r2

;;    derefw(PC) += 2;
CLINEA 0000H 0001H 0086H 0005H 0014H
	l	er0,	02h[er8]
	add	er0,	#2 
	st	er0,	02h[er8]

;;    cpc = derefw(PC);
CLINEA 0000H 0001H 0087H 0005H 0015H
	l	bp,	-2[fp]
	l	er0,	[bp]
	mov	er4,	er0

;;    switch(opcode)
CLINEA 0000H 0001H 0088H 0005H 0012H
	l	r0,	-11[fp]
	mov	r1,	#00h
CBLOCK 7 3 137
	cmp	r0,	#0ffh
	cmpc	r1,	#00h
	ble	_$M36
	b	_$L198
_$M36 :
	sllc	r1,	#01h
	sll	r0,	#01h
	l	er0,	NEAR _$M19[er0]
	b	er0

;;    }
_$L68 :
CBLOCKEND 7 2 596

;;}
CLINEA 0000H 0001H 0254H 0001H 0001H
	pop	bp
	pop	xr4
	pop	xr8
	mov	sp,	fp
	pop	fp
	pop	pc

;;        case 0x00:
CLINEA 0000H 0001H 008BH 0009H 0012H
_$S70 :

;;            deref(regs + reg1) = deref(regs + reg2);
CLINEA 0000H 0001H 008DH 000DH 0034H
	mov	r0,	r11
	mov	r1,	#00h
	add	er0,	er6
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	r2,	[er2]
	st	r2,	[er0]

;;            break;
CLINEA 0000H 0001H 008EH 000DH 0012H
	bal	_$L68

;;        case 0x01:
CLINEA 0000H 0001H 008FH 0009H 0012H
_$S71 :

;;            deref(regs + reg1) += deref(regs + reg2);
CLINEA 0000H 0001H 0091H 000DH 0035H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	r2,	r11
	mov	r3,	#00h
	add	er2,	er6
	l	r4,	[er0]
	l	r2,	[er2]
	add	r4,	r2
	st	r4,	[er0]

;;            break;
CLINEA 0000H 0001H 0092H 000DH 0012H
	bal	_$L68

;;        case 0x02:
CLINEA 0000H 0001H 0093H 0009H 0012H
_$S72 :

;;            deref(regs + reg1) -= deref(regs + reg2);
CLINEA 0000H 0001H 0095H 000DH 0035H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	r2,	r11
	mov	r3,	#00h
	add	er2,	er6
	l	r4,	[er0]
	l	r2,	[er2]
	sub	r4,	r2
	st	r4,	[er0]

;;            break;
CLINEA 0000H 0001H 0096H 000DH 0012H
	bal	_$L68

;;        case 0x03:
CLINEA 0000H 0001H 0097H 0009H 0012H
_$S73 :

;;            deref(regs + reg1) *= deref(regs + reg2);
CLINEA 0000H 0001H 0099H 000DH 0035H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	r2,	r11
	mov	r3,	#00h
	add	er2,	er6
	l	r4,	[er0]
	l	r2,	[er2]
	mul	er4,	r2
	st	r4,	[er0]

;;            break;
CLINEA 0000H 0001H 009AH 000DH 0012H
	bal	_$L68

;;        case 0x04:
CLINEA 0000H 0001H 009BH 0009H 0012H
_$S74 :

;;            deref(regs + reg1) /= deref(regs + reg2);
CLINEA 0000H 0001H 009DH 000DH 0035H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	r4,	[er0]
	mov	r5,	#00h
	mov	r2,	r11
	mov	r3,	#00h
	add	er2,	er6
	l	r2,	[er2]
	div	er4,	r2
	st	r4,	[er0]

;;            break;
CLINEA 0000H 0001H 009EH 000DH 0012H
	b	_$L68

;;        case 0x05:
CLINEA 0000H 0001H 009FH 0009H 0012H
_$S75 :

;;            deref(regs + reg1) = deref(regs + reg1) << deref(regs + reg2);
CLINEA 0000H 0001H 00A1H 000DH 004AH
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	er2,	er0
	mov	r0,	r11
	mov	r1,	#00h
	add	er0,	er6
	mov	er4,	er0
	l	r0,	[er2]
	l	r1,	[er4]
	cmp	r1,	#07h
	bgt	_$M20
	sll	r0,	r1
	b	_$M21
_$M20 :
	mov	r0,	#00h
_$M21 :
	mov	r2,	r10
	mov	r3,	#00h
	add	er2,	er6
	st	r0,	[er2]

;;            break;
CLINEA 0000H 0001H 00A2H 000DH 0012H
	b	_$L68

;;        case 0x06:
CLINEA 0000H 0001H 00A3H 0009H 0012H
_$S76 :

;;            deref(regs + reg1) = deref(regs + reg1) >> deref(regs + reg2);
CLINEA 0000H 0001H 00A5H 000DH 004AH
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	er2,	er0
	mov	r0,	r11
	mov	r1,	#00h
	add	er0,	er6
	mov	er4,	er0
	l	r0,	[er2]
	l	r1,	[er4]
	cmp	r1,	#07h
	bgt	_$M22
	srl	r0,	r1
	b	_$M23
_$M22 :
	mov	r0,	#00h
_$M23 :
	mov	r2,	r10
	mov	r3,	#00h
	add	er2,	er6
	st	r0,	[er2]

;;            break;
CLINEA 0000H 0001H 00A6H 000DH 0012H
	b	_$L68

;;        case 0x07:
CLINEA 0000H 0001H 00A7H 0009H 0012H
_$S77 :

;;            deref(regs + reg1) = lread_byte(PID, derefw(regs + reg2));
CLINEA 0000H 0000H 00A9H 000DH 0046H
	mov	r0,	r11
	mov	r1,	#00h
	add	er0,	er6
	l	er2,	[er0]
	mov	er0,	er8
	bl	_lread_byte
	mov	r4,	r0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	st	r4,	[er0]

;;            break;
CLINEA 0000H 0001H 00AAH 000DH 0012H
	b	_$L68

;;        case 0x08:
CLINEA 0000H 0001H 00ABH 0009H 0012H
_$S78 :

;;            lwrite_byte(PID, derefw(regs + reg2), deref(regs + reg1));
CLINEA 0000H 0001H 00ADH 000DH 0046H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	r0,	[er0]
	push	r0
	mov	r0,	r11
	mov	r1,	#00h
	add	er0,	er6
	l	er2,	[er0]
	mov	er0,	er8
	bl	_lwrite_byte
	add	sp,	#2 

;;            break;
CLINEA 0000H 0001H 00AEH 000DH 0012H
	b	_$L68

;;        case 0x09:
CLINEA 0000H 0001H 00AFH 0009H 0012H
_$S79 :

;;            deref(regs + reg1) &= deref(regs + reg2);
CLINEA 0000H 0001H 00B1H 000DH 0035H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	r2,	r11
	mov	r3,	#00h
	add	er2,	er6
	l	r4,	[er0]
	l	r2,	[er2]
	and	r4,	r2
	st	r4,	[er0]

;;            break;
CLINEA 0000H 0001H 00B2H 000DH 0012H
	b	_$L68

;;        case 0x0A:
CLINEA 0000H 0001H 00B3H 0009H 0012H
_$S80 :

;;            deref(regs + reg1) |= deref(regs + reg2);
CLINEA 0000H 0001H 00B5H 000DH 0035H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	r2,	r11
	mov	r3,	#00h
	add	er2,	er6
	l	r4,	[er0]
	l	r2,	[er2]
	or	r4,	r2
	st	r4,	[er0]

;;            break;
CLINEA 0000H 0001H 00B6H 000DH 0012H
	b	_$L68

;;        case 0x0B:
CLINEA 0000H 0001H 00B7H 0009H 0012H
_$S81 :

;;            deref(regs + reg1) ^= deref(regs + reg2);
CLINEA 0000H 0001H 00B9H 000DH 0035H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	r2,	r11
	mov	r3,	#00h
	add	er2,	er6
	l	r4,	[er0]
	l	r2,	[er2]
	xor	r4,	r2
	st	r4,	[er0]

;;            break;
CLINEA 0000H 0001H 00BAH 000DH 0012H
	b	_$L68

;;        case 0x0C:
CLINEA 0000H 0001H 00BBH 0009H 0012H
_$S82 :

;;            deref(regs + reg1)+=1;
CLINEA 0000H 0001H 00BDH 000DH 0022H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	lea	[er0]
	inc	[ea]

;;            break;
CLINEA 0000H 0001H 00BFH 000DH 0012H
	b	_$L68

;;        case 0x0D:
CLINEA 0000H 0001H 00C0H 0009H 0012H
_$S83 :

;;            deref(regs + reg1)-=1;
CLINEA 0000H 0001H 00C2H 000DH 0022H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	lea	[er0]
	dec	[ea]

;;            break;
CLINEA 0000H 0001H 00C3H 000DH 0012H
	b	_$L68

;;        case 0x0E:
CLINEA 0000H 0001H 00C4H 0009H 0012H
_$S84 :

;;            lcmp_flags_byte(deref(regs + reg1), deref(regs + reg2), FLAGS);
CLINEA 0000H 0001H 00C6H 000DH 004BH
	l	er2,	-6[fp]
	mov	r0,	r11
	mov	r1,	#00h
	add	er0,	er6
	l	r1,	[er0]
	mov	r4,	r10
	mov	r5,	#00h
	add	er4,	er6
	l	r0,	[er4]
	bl	_lcmp_flags_byte

;;            break;
CLINEA 0000H 0001H 00C7H 000DH 0012H
	b	_$L68

;;        case 0x0F:
CLINEA 0000H 0001H 00C8H 0009H 0012H
_$S85 :

;;            derefw(PC) = derefw(regs + reg1);
CLINEA 0000H 0001H 00CAH 000DH 002DH
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 00CBH 000DH 0012H
	b	_$L68

;;        case 0x10:
CLINEA 0000H 0001H 00CCH 0009H 0012H
_$S86 :

;;            if(deref(FLAGS) & 0x01)
CLINEA 0000H 0001H 00CEH 000DH 0023H
	l	bp,	-6[fp]
	l	r0,	[bp]
	tb	r0.0
	bne	_$M37
	b	_$L68
_$M37 :
CBLOCK 7 4 207

;;                derefw(PC) = derefw(regs + reg1);
CLINEA 0000H 0001H 00D0H 0011H 0031H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]
CBLOCKEND 7 4 209

;;            break;
CLINEA 0000H 0001H 00D2H 000DH 0012H
	b	_$L68

;;        case 0x11:
CLINEA 0000H 0001H 00D3H 0009H 0012H
_$S89 :

;;            if(!(deref(FLAGS) & 0x01))
CLINEA 0000H 0000H 00D5H 000DH 0026H
	l	bp,	-6[fp]
	l	r0,	[bp]
	tb	r0.0
	beq	_$M38
	b	_$L68
_$M38 :
CBLOCK 7 5 214

;;                derefw(PC) = derefw(regs + reg1);
CLINEA 0000H 0001H 00D7H 0011H 0031H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]
CBLOCKEND 7 5 216

;;            break;
CLINEA 0000H 0001H 00D9H 000DH 0012H
	b	_$L68

;;        case 0x12:
CLINEA 0000H 0001H 00DAH 0009H 0012H
_$S94 :

;;            if(deref(FLAGS) & 0x02)
CLINEA 0000H 0001H 00DCH 000DH 0023H
	l	bp,	-6[fp]
	l	r0,	[bp]
	tb	r0.1
	bne	_$M39
	b	_$L68
_$M39 :
CBLOCK 7 6 221

;;                derefw(PC) = derefw(regs + reg1);
CLINEA 0000H 0001H 00DEH 0011H 0031H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]
CBLOCKEND 7 6 223

;;            break;
CLINEA 0000H 0001H 00E0H 000DH 0012H
	b	_$L68

;;        case 0x13:
CLINEA 0000H 0001H 00E1H 0009H 0012H
_$S97 :

;;            if(deref(FLAGS) & 0x04)
CLINEA 0000H 0001H 00E3H 000DH 0023H
	l	bp,	-6[fp]
	l	r0,	[bp]
	tb	r0.2
	bne	_$M40
	b	_$L68
_$M40 :
CBLOCK 7 7 228

;;                derefw(PC) = derefw(regs + reg1);
CLINEA 0000H 0001H 00E5H 0011H 0031H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]
CBLOCKEND 7 7 230

;;            break;
CLINEA 0000H 0001H 00E7H 000DH 0012H
	b	_$L68

;;        case 0x14:
CLINEA 0000H 0001H 00E8H 0009H 0012H
_$S100 :

;;            if(deref(FLAGS) & 0x08)
CLINEA 0000H 0001H 00EAH 000DH 0023H
	l	bp,	-6[fp]
	l	r0,	[bp]
	tb	r0.3
	bne	_$M41
	b	_$L68
_$M41 :
CBLOCK 7 8 235

;;                derefw(PC) = derefw(regs + reg1);
CLINEA 0000H 0001H 00ECH 0011H 0031H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]
CBLOCKEND 7 8 237

;;            break;
CLINEA 0000H 0001H 00EEH 000DH 0012H
	b	_$L68

;;        case 0x15:
CLINEA 0000H 0001H 00EFH 0009H 0012H
_$S103 :

;;            if(deref(FLAGS) & 0x10)
CLINEA 0000H 0001H 00F1H 000DH 0023H
	l	bp,	-6[fp]
	l	r0,	[bp]
	tb	r0.4
	bne	_$M42
	b	_$L68
_$M42 :
CBLOCK 7 9 242

;;                derefw(PC) = derefw(regs + reg1);
CLINEA 0000H 0001H 00F3H 0011H 0031H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]
CBLOCKEND 7 9 244

;;            break;
CLINEA 0000H 0001H 00F5H 000DH 0012H
	b	_$L68

;;        case 0x16:
CLINEA 0000H 0001H 00F7H 0009H 0012H
_$S106 :

;;            derefw(regs + reg1) = derefw(regs + reg2);
CLINEA 0000H 0001H 00F9H 000DH 0036H
	mov	r0,	r11
	mov	r1,	#00h
	add	er0,	er6
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er2,	[er2]
	st	er2,	[er0]

;;            break;
CLINEA 0000H 0001H 00FAH 000DH 0012H
	b	_$L68

;;        case 0x17:
CLINEA 0000H 0001H 00FBH 0009H 0012H
_$S107 :

;;            derefw(regs + reg1) += derefw(regs + reg2);
CLINEA 0000H 0001H 00FDH 000DH 0037H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	r2,	r11
	mov	r3,	#00h
	add	er2,	er6
	l	er4,	[er0]
	l	er2,	[er2]
	add	er4,	er2
	st	er4,	[er0]

;;            break;
CLINEA 0000H 0001H 00FEH 000DH 0012H
	b	_$L68

;;        case 0x18:
CLINEA 0000H 0001H 00FFH 0009H 0012H
_$S108 :

;;            derefw(regs + reg1) -= derefw(regs + reg2);
CLINEA 0000H 0001H 0101H 000DH 0037H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	r2,	r11
	mov	r3,	#00h
	add	er2,	er6
	l	er4,	[er0]
	l	er2,	[er2]
	sub	r4,	r2
	subc	r5,	r3
	st	er4,	[er0]

;;            break;
CLINEA 0000H 0001H 0102H 000DH 0012H
	b	_$L68

;;        case 0x19:
CLINEA 0000H 0001H 0103H 0009H 0012H
_$S109 :

;;            derefw(regs + reg1) *= derefw(regs + reg2);
CLINEA 0000H 0001H 0105H 000DH 0037H
	mov	r4,	r10
	mov	r5,	#00h
	add	er4,	er6
	mov	r0,	r11
	mov	r1,	#00h
	add	er0,	er6
	mov	bp,	er0
	l	er0,	[er4]
	l	er2,	[bp]
	bl	__imulu8lw
	st	er0,	[er4]

;;            break;
CLINEA 0000H 0001H 0106H 000DH 0012H
	b	_$L68

;;        case 0x1A:
CLINEA 0000H 0001H 0107H 0009H 0012H
_$S110 :

;;            derefw(regs + reg1) /= derefw(regs + reg2);
CLINEA 0000H 0001H 0109H 000DH 0037H
	mov	r4,	r10
	mov	r5,	#00h
	add	er4,	er6
	mov	r0,	r11
	mov	r1,	#00h
	add	er0,	er6
	mov	bp,	er0
	l	er0,	[er4]
	l	er2,	[bp]
	bl	__uidivu8lw
	st	er0,	[er4]

;;            break;
CLINEA 0000H 0001H 010AH 000DH 0012H
	b	_$L68

;;        case 0x1B:
CLINEA 0000H 0001H 010BH 0009H 0012H
_$S111 :

;;            derefw(regs + reg1) = derefw(regs + reg1) << derefw(regs + reg2);
CLINEA 0000H 0001H 010DH 000DH 004DH
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	er2,	er0
	mov	r0,	r11
	mov	r1,	#00h
	add	er0,	er6
	mov	er4,	er0
	l	er0,	[er2]
	l	er2,	[er4]
_$M25 :
	cmp	r2,	#07h
	cmpc	r3,	#00h
	ble	_$M24
	sllc	r1,	#07h
	sll	r0,	#07h
	add	er2,	#-7
	bne	_$M25
_$M24 :
	sllc	r1,	r2
	sll	r0,	r2
	mov	r2,	r10
	mov	r3,	#00h
	add	er2,	er6
	st	er0,	[er2]

;;            break;
CLINEA 0000H 0001H 010EH 000DH 0012H
	b	_$L68

;;        case 0x1C:
CLINEA 0000H 0001H 010FH 0009H 0012H
_$S112 :

;;            derefw(regs + reg1) = derefw(regs + reg1) >> derefw(regs + reg2);
CLINEA 0000H 0001H 0111H 000DH 004DH
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	er2,	er0
	mov	r0,	r11
	mov	r1,	#00h
	add	er0,	er6
	mov	er4,	er0
	l	er0,	[er2]
	l	er2,	[er4]
_$M27 :
	cmp	r2,	#07h
	cmpc	r3,	#00h
	ble	_$M26
	srlc	r0,	#07h
	srl	r1,	#07h
	add	er2,	#-7
	bne	_$M27
_$M26 :
	srlc	r0,	r2
	srl	r1,	r2
	mov	r2,	r10
	mov	r3,	#00h
	add	er2,	er6
	st	er0,	[er2]

;;            break;
CLINEA 0000H 0001H 0112H 000DH 0012H
	b	_$L68

;;        case 0x1D:
CLINEA 0000H 0001H 0113H 0009H 0012H
_$S113 :

;;            derefw(regs + reg1) = lread_word(PID, derefw(regs + reg2));
CLINEA 0000H 0000H 0115H 000DH 0047H
	mov	r0,	r11
	mov	r1,	#00h
	add	er0,	er6
	l	er2,	[er0]
	mov	er0,	er8
	bl	_lread_word
	mov	er4,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	st	er4,	[er0]

;;            break;
CLINEA 0000H 0001H 0116H 000DH 0012H
	b	_$L68

;;        case 0x1E:
CLINEA 0000H 0001H 0117H 0009H 0012H
_$S114 :

;;            lwrite_word(PID, derefw(regs + reg2), derefw(regs + reg1));
CLINEA 0000H 0001H 0119H 000DH 0047H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er0,	[er0]
	push	er0
	mov	r0,	r11
	mov	r1,	#00h
	add	er0,	er6
	l	er2,	[er0]
	mov	er0,	er8
	bl	_lwrite_word
	add	sp,	#2 

;;            break;
CLINEA 0000H 0001H 011AH 000DH 0012H
	b	_$L68

;;        case 0x1F:
CLINEA 0000H 0001H 011BH 0009H 0012H
_$S115 :

;;            derefw(regs + reg1) &= derefw(regs + reg2);
CLINEA 0000H 0001H 011DH 000DH 0037H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	r2,	r11
	mov	r3,	#00h
	add	er2,	er6
	l	er4,	[er0]
	l	er2,	[er2]
	and	r4,	r2
	and	r5,	r3
	st	er4,	[er0]

;;            break;
CLINEA 0000H 0001H 011EH 000DH 0012H
	b	_$L68

;;        case 0x20:
CLINEA 0000H 0001H 011FH 0009H 0012H
_$S116 :

;;            derefw(regs + reg1) |= derefw(regs + reg2);
CLINEA 0000H 0001H 0121H 000DH 0037H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	r2,	r11
	mov	r3,	#00h
	add	er2,	er6
	l	er4,	[er0]
	l	er2,	[er2]
	or	r4,	r2
	or	r5,	r3
	st	er4,	[er0]

;;            break;
CLINEA 0000H 0001H 0122H 000DH 0012H
	b	_$L68

;;        case 0x21:
CLINEA 0000H 0001H 0123H 0009H 0012H
_$S117 :

;;            derefw(regs + reg1) ^= derefw(regs + reg2);
CLINEA 0000H 0001H 0125H 000DH 0037H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	mov	r2,	r11
	mov	r3,	#00h
	add	er2,	er6
	l	er4,	[er0]
	l	er2,	[er2]
	xor	r4,	r2
	xor	r5,	r3
	st	er4,	[er0]

;;            break;
CLINEA 0000H 0001H 0126H 000DH 0012H
	b	_$L68

;;        case 0x22:
CLINEA 0000H 0001H 0127H 0009H 0012H
_$S118 :

;;            derefw(regs + reg1)++;
CLINEA 0000H 0000H 0129H 000DH 0022H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er2,	[er0]
	add	er2,	#1 
	st	er2,	[er0]

;;            break;
CLINEA 0000H 0001H 012AH 000DH 0012H
	b	_$L68

;;        case 0x23:
CLINEA 0000H 0001H 012BH 0009H 0012H
_$S119 :

;;            derefw(regs + reg1)--;
CLINEA 0000H 0000H 012DH 000DH 0022H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er2,	[er0]
	add	er2,	#-1
	st	er2,	[er0]

;;            break;
CLINEA 0000H 0001H 012EH 000DH 0012H
	b	_$L68

;;        case 0x24:
CLINEA 0000H 0001H 012FH 0009H 0012H
_$S120 :

;;            lcmp_flags_word(derefw(regs + reg1), derefw(regs + reg2), FLAGS);
CLINEA 0000H 0001H 0131H 000DH 004DH
	l	er0,	-6[fp]
	push	er0
	mov	r0,	r11
	mov	r1,	#00h
	add	er0,	er6
	l	er2,	[er0]
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er0,	[er0]
	bl	_lcmp_flags_word
	add	sp,	#2 

;;            break;
CLINEA 0000H 0001H 0132H 000DH 0012H
	b	_$L68

;;        case 0x30:
CLINEA 0000H 0001H 0134H 0009H 0012H
_$S121 :

;;            deref(regs + reg1) = deref(code + cpc);
CLINEA 0000H 0001H 0136H 000DH 0033H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	r2,	[er2]
	st	r2,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 0137H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 0138H 000DH 0012H
	b	_$L68

;;        case 0x31:
CLINEA 0000H 0001H 0139H 0009H 0012H
_$S122 :

;;            deref(regs + reg1) += deref(code + cpc);
CLINEA 0000H 0001H 013BH 000DH 0034H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	r4,	[er0]
	l	r2,	[er2]
	add	r4,	r2
	st	r4,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 013CH 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 013DH 000DH 0012H
	b	_$L68

;;        case 0x32:
CLINEA 0000H 0001H 013EH 0009H 0012H
_$S123 :

;;            deref(regs + reg1) -= deref(code + cpc);
CLINEA 0000H 0001H 0140H 000DH 0034H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	r4,	[er0]
	l	r2,	[er2]
	sub	r4,	r2
	st	r4,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 0141H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 0142H 000DH 0012H
	b	_$L68

;;        case 0x33:
CLINEA 0000H 0001H 0143H 0009H 0012H
_$S124 :

;;            deref(regs + reg1) *= deref(code + cpc);
CLINEA 0000H 0001H 0145H 000DH 0034H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	mov	er4,	er6
	add	er0,	er6
	l	r4,	[er0]
	l	r2,	[er2]
	mul	er4,	r2
	st	r4,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 0146H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 0147H 000DH 0012H
	b	_$L68

;;        case 0x34:
CLINEA 0000H 0001H 0148H 0009H 0012H
_$S125 :

;;            deref(regs + reg1) /= deref(code + cpc);
CLINEA 0000H 0001H 014AH 000DH 0034H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	r4,	[er0]
	mov	r5,	#00h
	l	r2,	[er2]
	div	er4,	r2
	st	r4,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 014BH 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 014CH 000DH 0012H
	b	_$L68

;;        case 0x35:
CLINEA 0000H 0001H 014DH 0009H 0012H
_$S126 :

;;            deref(regs + reg1) = deref(regs + reg1) << deref(code + cpc);
CLINEA 0000H 0001H 014FH 000DH 0049H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	r0,	[er0]
	l	r1,	[er2]
	cmp	r1,	#07h
	bgt	_$M28
	sll	r0,	r1
	b	_$M29
_$M28 :
	mov	r0,	#00h
_$M29 :
	mov	r2,	r10
	mov	r3,	#00h
	add	er2,	er6
	st	r0,	[er2]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 0150H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 0151H 000DH 0012H
	b	_$L68

;;        case 0x36:
CLINEA 0000H 0001H 0152H 0009H 0012H
_$S127 :

;;            deref(regs + reg1) = deref(regs + reg1) >> deref(code + cpc);
CLINEA 0000H 0001H 0154H 000DH 0049H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	r0,	[er0]
	l	r1,	[er2]
	cmp	r1,	#07h
	bgt	_$M30
	srl	r0,	r1
	b	_$M31
_$M30 :
	mov	r0,	#00h
_$M31 :
	mov	r2,	r10
	mov	r3,	#00h
	add	er2,	er6
	st	r0,	[er2]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 0155H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 0156H 000DH 0012H
	b	_$L68

;;        case 0x37:
CLINEA 0000H 0001H 0157H 0009H 0012H
_$S128 :

;;            deref(regs + reg1) = lread_byte(PID, derefw(code + cpc));
CLINEA 0000H 0000H 0159H 000DH 0045H
	l	er0,	-4[fp]
	add	er0,	er4
	l	er2,	[er0]
	mov	er0,	er8
	bl	_lread_byte
	mov	r4,	r0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	st	r4,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 015AH 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 015BH 000DH 0012H
	b	_$L68

;;        case 0x38:
CLINEA 0000H 0001H 015CH 0009H 0012H
_$S129 :

;;            lwrite_byte(PID, derefw(code + cpc), deref(regs + reg1));
CLINEA 0000H 0001H 015EH 000DH 0045H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	r0,	[er0]
	push	r0
	l	er0,	-4[fp]
	add	er0,	er4
	l	er2,	[er0]
	mov	er0,	er8
	bl	_lwrite_byte
	add	sp,	#2 

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 015FH 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 0160H 000DH 0012H
	b	_$L68

;;        case 0x39:
CLINEA 0000H 0001H 0161H 0009H 0012H
_$S130 :

;;            deref(regs + reg1) &= deref(code + cpc);
CLINEA 0000H 0001H 0163H 000DH 0034H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	r4,	[er0]
	l	r2,	[er2]
	and	r4,	r2
	st	r4,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 0164H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 0165H 000DH 0012H
	b	_$L68

;;        case 0x3A:
CLINEA 0000H 0001H 0166H 0009H 0012H
_$S131 :

;;            deref(regs + reg1) |= deref(code + cpc);
CLINEA 0000H 0001H 0168H 000DH 0034H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	r4,	[er0]
	l	r2,	[er2]
	or	r4,	r2
	st	r4,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 0169H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 016AH 000DH 0012H
	b	_$L68

;;        case 0x3B:
CLINEA 0000H 0001H 016BH 0009H 0012H
_$S132 :

;;            deref(regs + reg1) ^= deref(code + cpc);
CLINEA 0000H 0001H 016DH 000DH 0034H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	r4,	[er0]
	l	r2,	[er2]
	xor	r4,	r2
	st	r4,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 016EH 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 016FH 000DH 0012H
	b	_$L68

;;        case 0x3C:
CLINEA 0000H 0001H 0170H 0009H 0012H
_$S133 :

;;            deref(regs + reg1)++;
CLINEA 0000H 0000H 0172H 000DH 0021H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	lea	[er0]
	inc	[ea]

;;            break;
CLINEA 0000H 0001H 0173H 000DH 0012H
	b	_$L68

;;        case 0x3D:
CLINEA 0000H 0001H 0174H 0009H 0012H
_$S134 :

;;            deref(regs + reg1)--;
CLINEA 0000H 0000H 0176H 000DH 0021H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	lea	[er0]
	dec	[ea]

;;            break;
CLINEA 0000H 0001H 0177H 000DH 0012H
	b	_$L68

;;        case 0x3E:
CLINEA 0000H 0001H 0178H 0009H 0012H
_$S135 :

;;            lcmp_flags_byte(deref(regs + reg1), deref(cpc + code), FLAGS);
CLINEA 0000H 0001H 017AH 000DH 004AH
	l	er2,	-6[fp]
	l	er0,	-4[fp]
	add	er0,	er4
	l	r1,	[er0]
	mov	r4,	r10
	mov	r5,	#00h
	add	er4,	er6
	l	r0,	[er4]
	bl	_lcmp_flags_byte

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 017BH 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 017CH 000DH 0012H
	b	_$L68

;;        case 0x40:
CLINEA 0000H 0001H 017EH 0009H 0012H
_$S136 :

;;            derefw(regs + reg1) = derefw(code + cpc);
CLINEA 0000H 0001H 0180H 000DH 0035H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er2,	[er2]
	st	er2,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 0181H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 0182H 000DH 0012H
	b	_$L68

;;        case 0x41:
CLINEA 0000H 0001H 0183H 0009H 0012H
_$S137 :

;;            derefw(regs + reg1) += derefw(code + cpc);
CLINEA 0000H 0001H 0185H 000DH 0036H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er4,	[er0]
	l	er2,	[er2]
	add	er4,	er2
	st	er4,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 0186H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 0187H 000DH 0012H
	b	_$L68

;;        case 0x42:
CLINEA 0000H 0001H 0188H 0009H 0012H
_$S138 :

;;            derefw(regs + reg1) -= derefw(code + cpc);
CLINEA 0000H 0001H 018AH 000DH 0036H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er4,	[er0]
	l	er2,	[er2]
	sub	r4,	r2
	subc	r5,	r3
	st	er4,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 018BH 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 018CH 000DH 0012H
	b	_$L68

;;        case 0x43:
CLINEA 0000H 0001H 018DH 0009H 0012H
_$S139 :

;;            derefw(regs + reg1) *= derefw(code + cpc);
CLINEA 0000H 0001H 018FH 000DH 0036H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r4,	r10
	mov	r5,	#00h
	add	er4,	er6
	l	er0,	[er4]
	l	er2,	[er2]
	bl	__imulu8lw
	st	er0,	[er4]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 0190H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 0191H 000DH 0012H
	b	_$L68

;;        case 0x44:
CLINEA 0000H 0001H 0192H 0009H 0012H
_$S140 :

;;            derefw(regs + reg1) /= derefw(code + cpc);
CLINEA 0000H 0001H 0194H 000DH 0036H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r4,	r10
	mov	r5,	#00h
	add	er4,	er6
	l	er0,	[er4]
	l	er2,	[er2]
	bl	__uidivu8lw
	st	er0,	[er4]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 0195H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 0196H 000DH 0012H
	b	_$L68

;;        case 0x45:
CLINEA 0000H 0001H 0197H 0009H 0012H
_$S141 :

;;            derefw(regs + reg1) = derefw(regs + reg1) << derefw(code + cpc);
CLINEA 0000H 0001H 0199H 000DH 004CH
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er0,	[er0]
	l	er2,	[er2]
_$M33 :
	cmp	r2,	#07h
	cmpc	r3,	#00h
	ble	_$M32
	sllc	r1,	#07h
	sll	r0,	#07h
	add	er2,	#-7
	bne	_$M33
_$M32 :
	sllc	r1,	r2
	sll	r0,	r2
	mov	r2,	r10
	mov	r3,	#00h
	add	er2,	er6
	st	er0,	[er2]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 019AH 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 019BH 000DH 0012H
	b	_$L68

;;        case 0x46:
CLINEA 0000H 0001H 019CH 0009H 0012H
_$S142 :

;;            derefw(regs + reg1) = derefw(regs + reg1) >> derefw(code + cpc);
CLINEA 0000H 0001H 019EH 000DH 004CH
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er0,	[er0]
	l	er2,	[er2]
_$M35 :
	cmp	r2,	#07h
	cmpc	r3,	#00h
	ble	_$M34
	srlc	r0,	#07h
	srl	r1,	#07h
	add	er2,	#-7
	bne	_$M35
_$M34 :
	srlc	r0,	r2
	srl	r1,	r2
	mov	r2,	r10
	mov	r3,	#00h
	add	er2,	er6
	st	er0,	[er2]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 019FH 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 01A0H 000DH 0012H
	b	_$L68

;;        case 0x47:
CLINEA 0000H 0001H 01A1H 0009H 0012H
_$S143 :

;;            derefw(regs + reg1) = lread_word(PID, derefw(code + cpc));
CLINEA 0000H 0000H 01A3H 000DH 0046H
	l	er0,	-4[fp]
	add	er0,	er4
	l	er2,	[er0]
	mov	er0,	er8
	bl	_lread_word
	mov	er4,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	st	er4,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 01A4H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 01A5H 000DH 0012H
	b	_$L68

;;        case 0x48:
CLINEA 0000H 0001H 01A6H 0009H 0012H
_$S144 :

;;            lwrite_word(PID, derefw(code + cpc), derefw(regs + reg1));
CLINEA 0000H 0001H 01A8H 000DH 0046H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er0,	[er0]
	push	er0
	l	er0,	-4[fp]
	add	er0,	er4
	l	er2,	[er0]
	mov	er0,	er8
	bl	_lwrite_word
	add	sp,	#2 

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 01A9H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 01AAH 000DH 0012H
	b	_$L68

;;        case 0x49:
CLINEA 0000H 0001H 01ABH 0009H 0012H
_$S145 :

;;            derefw(regs + reg1) &= derefw(code + cpc);
CLINEA 0000H 0001H 01ADH 000DH 0036H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er4,	[er0]
	l	er2,	[er2]
	and	r4,	r2
	and	r5,	r3
	st	er4,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 01AEH 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 01AFH 000DH 0012H
	b	_$L68

;;        case 0x4A:
CLINEA 0000H 0001H 01B0H 0009H 0012H
_$S146 :

;;            derefw(regs + reg1) |= derefw(code + cpc);
CLINEA 0000H 0001H 01B2H 000DH 0036H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er4,	[er0]
	l	er2,	[er2]
	or	r4,	r2
	or	r5,	r3
	st	er4,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 01B3H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 01B4H 000DH 0012H
	b	_$L68

;;        case 0x4B:
CLINEA 0000H 0001H 01B5H 0009H 0012H
_$S147 :

;;            derefw(regs + reg1) ^= derefw(code + cpc);
CLINEA 0000H 0001H 01B7H 000DH 0036H
	l	er0,	-4[fp]
	add	er0,	er4
	mov	er2,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er4,	[er0]
	l	er2,	[er2]
	xor	r4,	r2
	xor	r5,	r3
	st	er4,	[er0]

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 01B8H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 01B9H 000DH 0012H
	b	_$L68

;;        case 0x4C:
CLINEA 0000H 0001H 01BAH 0009H 0012H
_$S148 :

;;            derefw(regs + reg1)++;
CLINEA 0000H 0000H 01BCH 000DH 0022H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er2,	[er0]
	add	er2,	#1 
	st	er2,	[er0]

;;            break;
CLINEA 0000H 0001H 01BDH 000DH 0012H
	b	_$L68

;;        case 0x4D:
CLINEA 0000H 0001H 01BEH 0009H 0012H
_$S149 :

;;            derefw(regs + reg1)--;
CLINEA 0000H 0000H 01C0H 000DH 0022H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er2,	[er0]
	add	er2,	#-1
	st	er2,	[er0]

;;            break;
CLINEA 0000H 0001H 01C1H 000DH 0012H
	b	_$L68

;;        case 0x4E:
CLINEA 0000H 0001H 01C2H 0009H 0012H
_$S150 :

;;            lcmp_flags_word(derefw(regs + reg1), derefw(cpc + code), FLAGS);
CLINEA 0000H 0001H 01C4H 000DH 004CH
	l	er0,	-6[fp]
	push	er0
	l	er0,	-4[fp]
	add	er0,	er4
	l	er2,	[er0]
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er0,	[er0]
	bl	_lcmp_flags_word
	add	sp,	#2 

;;            derefw(PC) += 2;
CLINEA 0000H 0001H 01C5H 000DH 001CH
	l	bp,	-2[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 01C6H 000DH 0012H
	b	_$L68

;;        case 0x50:
CLINEA 0000H 0001H 01C8H 0009H 0012H
_$S151 :

;;            derefw(SP) -= 2;
CLINEA 0000H 0001H 01CAH 000DH 001CH
	l	bp,	-8[fp]
	l	er0,	[bp]
	add	er0,	#-2
	st	er0,	[bp]

;;            lwrite_word(PID, derefw(SP), derefw(regs + reg1));
CLINEA 0000H 0001H 01CBH 000DH 003EH
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er0,	[er0]
	push	er0
	l	bp,	-8[fp]
	l	er2,	[bp]
	mov	er0,	er8
	bl	_lwrite_word
	add	sp,	#2 

;;            break;
CLINEA 0000H 0001H 01CCH 000DH 0012H
	b	_$L68

;;        case 0x51:
CLINEA 0000H 0001H 01CDH 0009H 0012H
_$S152 :

;;            derefw(regs + reg1) = lread_word(PID, derefw(SP));
CLINEA 0000H 0000H 01CFH 000DH 003EH
	l	bp,	-8[fp]
	l	er2,	[bp]
	mov	er0,	er8
	bl	_lread_word
	mov	er4,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	st	er4,	[er0]

;;            derefw(SP) += 2;
CLINEA 0000H 0001H 01D0H 000DH 001CH
	l	bp,	-8[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 01D1H 000DH 0012H
	b	_$L68

;;        case 0x52:
CLINEA 0000H 0001H 01D2H 0009H 0012H
_$S153 :

;;            derefw(SP) -= 2;
CLINEA 0000H 0001H 01D4H 000DH 001CH
	l	bp,	-8[fp]
	l	er0,	[bp]
	add	er0,	#-2
	st	er0,	[bp]

;;            lwrite_word(PID, derefw(SP), derefw(regs + reg1));
CLINEA 0000H 0001H 01D5H 000DH 003EH
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	er0,	[er0]
	push	er0
	l	bp,	-8[fp]
	l	er2,	[bp]
	mov	er0,	er8
	bl	_lwrite_word
	add	sp,	#2 

;;        case 0x53:
CLINEA 0000H 0001H 01D6H 0009H 0012H
_$S154 :

;;            derefw(regs + reg1) = lread_word(PID, derefw(SP));
CLINEA 0000H 0000H 01D8H 000DH 003EH
	l	bp,	-8[fp]
	l	er2,	[bp]
	mov	er0,	er8
	bl	_lread_word
	mov	er4,	er0
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	st	er4,	[er0]

;;            derefw(SP) += 2;
CLINEA 0000H 0001H 01D9H 000DH 001CH
	l	bp,	-8[fp]
	l	er0,	[bp]
	add	er0,	#2 
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 01DAH 000DH 0012H
	b	_$L68

;;        case 0x60:
CLINEA 0000H 0001H 01DCH 0009H 0012H
_$S155 :

;;            derefw(PC) = derefw(regs + reg1);
CLINEA 0000H 0001H 01DEH 000DH 002DH
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 01DFH 000DH 0012H
	b	_$L68

;;        case 0x61:
CLINEA 0000H 0001H 01E0H 0009H 0012H
_$S156 :

;;            derefw(LR) = cpc;
CLINEA 0000H 0001H 01E2H 000DH 001DH
	l	bp,	-10[fp]
	st	er4,	[bp]

;;            derefw(PC) = derefw(regs + reg1);
CLINEA 0000H 0001H 01E3H 000DH 002DH
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er6
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 01E4H 000DH 0012H
	b	_$L68

;;        case 0x62:
CLINEA 0000H 0001H 01E5H 0009H 0012H
_$S157 :

;;            derefw(PC) = derefw(LR);
CLINEA 0000H 0001H 01E7H 000DH 0024H
	l	bp,	-10[fp]
	l	er0,	-2[fp]
	l	er2,	[bp]
	st	er2,	[er0]

;;            break;
CLINEA 0000H 0001H 01E8H 000DH 0012H
	b	_$L68

;;        case 0x70:
CLINEA 0000H 0001H 01E9H 0009H 0012H
_$S158 :

;;            if(operand == 0)
CLINEA 0000H 0001H 01EBH 000DH 001CH
	cmp	r3,	#00h
	bne	_$L159
CBLOCK 7 10 492

;;                print((const byte*)derefw(regs + 0)+code+derefw(PC), deref(regs + 2), deref(regs + 3), deref(regs + 4));
CLINEA 0000H 0001H 01EEH 0011H 0078H
	l	r0,	04h[er6]
	mov	r1,	#00h
	push	er0
	l	r0,	03h[er6]
	push	er0
	l	r2,	02h[er6]
	mov	r3,	#00h
	l	er0,	[er6]
	l	er4,	-4[fp]
	add	er0,	er4
	l	bp,	-2[fp]
	l	er4,	[bp]
	add	er0,	er4
	bl	_print
	add	sp,	#4 
CBLOCKEND 7 10 495
	bal	_$L171
_$L159 :

;;            else if(operand == 1)
CLINEA 0000H 0000H 01F0H 000DH 0021H
	cmp	r3,	#01h
	bne	_$L162
CBLOCK 7 11 497

;;                set_pixel(deref(regs + 0), deref(regs + 1), deref(regs + 2));
CLINEA 0000H 0001H 01F3H 0011H 004DH
	l	r0,	02h[er6]
	mov	r1,	#00h
	push	er0
	l	r2,	01h[er6]
	mov	r3,	#00h
	l	r0,	[er6]
	bl	_set_pixel
	add	sp,	#2 
CBLOCKEND 7 11 501
	bal	_$L171
_$L162 :

;;            else if(operand == 2)
CLINEA 0000H 0000H 01F6H 000DH 0021H
	cmp	r3,	#02h
	bne	_$L165
CBLOCK 7 12 503

;;                plot_line(deref(regs + 0), deref(regs + 1), deref(regs + 2), deref(regs + 3), deref(regs + 4));
CLINEA 0000H 0001H 01F9H 0011H 006FH
	l	r0,	04h[er6]
	mov	r1,	#00h
	push	er0
	l	r0,	03h[er6]
	push	er0
	l	r0,	02h[er6]
	push	er0
	l	r2,	01h[er6]
	mov	r3,	#00h
	l	r0,	[er6]
	bl	_plot_line
	add	sp,	#6 
CBLOCKEND 7 12 506
	bal	_$L171
_$L165 :

;;            else if(operand == 5)
CLINEA 0000H 0000H 0205H 000DH 0021H
	cmp	r3,	#05h
	bne	_$L168
CBLOCK 7 13 518

;;                deref(regs + 0) = CheckButtons();
CLINEA 0000H 0000H 0208H 0011H 0031H
	bl	_CheckButtons
	st	r0,	[er6]
CBLOCKEND 7 13 521
	bal	_$L171
_$L168 :

;;            else if(operand == 6)
CLINEA 0000H 0000H 020AH 000DH 0021H
	cmp	r3,	#06h
	bne	_$L171
CBLOCK 7 14 523

;;                render(vram1);
CLINEA 0000H 0001H 020DH 0011H 001EH
	mov	r0,	#02h
	mov	r1,	#090h
	bl	_render
CBLOCKEND 7 14 526

;;            }
CLINEA 0000H 0000H 020EH 000DH 000DH
_$L171 :

;;            break;
CLINEA 0000H 0001H 020FH 000DH 0012H
	b	_$L68

;;        case 0x80:
CLINEA 0000H 0001H 0211H 0009H 0012H
_$S173 :

;;            derefw(PC) = derefw(code + cpc);
CLINEA 0000H 0001H 0213H 000DH 002CH
	l	er0,	-4[fp]
	add	er0,	er4
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 0214H 000DH 0012H
	b	_$L68

;;        case 0x81:
CLINEA 0000H 0001H 0215H 0009H 0012H
_$S174 :

;;            if(deref(FLAGS) & 0x01)
CLINEA 0000H 0001H 0217H 000DH 0023H
	l	bp,	-6[fp]
	l	r0,	[bp]
	tb	r0.0
	bne	_$M43
	b	_$L68
_$M43 :
CBLOCK 7 15 536

;;                derefw(PC) = derefw(code + cpc);
CLINEA 0000H 0001H 0219H 0011H 0030H
	l	er0,	-4[fp]
	add	er0,	er4
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]
CBLOCKEND 7 15 538

;;            break;
CLINEA 0000H 0001H 021BH 000DH 0012H
	b	_$L68

;;        case 0x82:
CLINEA 0000H 0001H 021CH 0009H 0012H
_$S177 :

;;            if(!(deref(FLAGS) & 0x01))
CLINEA 0000H 0000H 021EH 000DH 0026H
	l	bp,	-6[fp]
	l	r0,	[bp]
	tb	r0.0
	beq	_$M44
	b	_$L68
_$M44 :
CBLOCK 7 16 543

;;                derefw(PC) = derefw(code + cpc);
CLINEA 0000H 0001H 0220H 0011H 0030H
	l	er0,	-4[fp]
	add	er0,	er4
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]
CBLOCKEND 7 16 545

;;            break;
CLINEA 0000H 0001H 0222H 000DH 0012H
	b	_$L68

;;        case 0x83:
CLINEA 0000H 0001H 0223H 0009H 0012H
_$S182 :

;;            if(deref(FLAGS) & 0x02)
CLINEA 0000H 0001H 0225H 000DH 0023H
	l	bp,	-6[fp]
	l	r0,	[bp]
	tb	r0.1
	bne	_$M45
	b	_$L68
_$M45 :
CBLOCK 7 17 550

;;                derefw(PC) = derefw(code + cpc);
CLINEA 0000H 0001H 0227H 0011H 0030H
	l	er0,	-4[fp]
	add	er0,	er4
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]
CBLOCKEND 7 17 552

;;            break;
CLINEA 0000H 0001H 0229H 000DH 0012H
	b	_$L68

;;        case 0x84:
CLINEA 0000H 0001H 022AH 0009H 0012H
_$S185 :

;;            if(deref(FLAGS) & 0x04)
CLINEA 0000H 0001H 022CH 000DH 0023H
	l	bp,	-6[fp]
	l	r0,	[bp]
	tb	r0.2
	bne	_$M46
	b	_$L68
_$M46 :
CBLOCK 7 18 557

;;                derefw(PC) = derefw(code + cpc);
CLINEA 0000H 0001H 022EH 0011H 0030H
	l	er0,	-4[fp]
	add	er0,	er4
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]
CBLOCKEND 7 18 559

;;            break;
CLINEA 0000H 0001H 0230H 000DH 0012H
	b	_$L68

;;        case 0x85:
CLINEA 0000H 0001H 0231H 0009H 0012H
_$S188 :

;;            if(deref(FLAGS) & 0x08)
CLINEA 0000H 0001H 0233H 000DH 0023H
	l	bp,	-6[fp]
	l	r0,	[bp]
	tb	r0.3
	bne	_$M47
	b	_$L68
_$M47 :
CBLOCK 7 19 564

;;                derefw(PC) = derefw(code + cpc);
CLINEA 0000H 0001H 0235H 0011H 0030H
	l	er0,	-4[fp]
	add	er0,	er4
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]
CBLOCKEND 7 19 566

;;            break;
CLINEA 0000H 0001H 0237H 000DH 0012H
	b	_$L68

;;        case 0x86:
CLINEA 0000H 0001H 0238H 0009H 0012H
_$S191 :

;;            if(deref(FLAGS) & 0x10)
CLINEA 0000H 0001H 023AH 000DH 0023H
	l	bp,	-6[fp]
	l	r0,	[bp]
	tb	r0.4
	bne	_$M48
	b	_$L68
_$M48 :
CBLOCK 7 20 571

;;                derefw(PC) = derefw(code + cpc);
CLINEA 0000H 0001H 023CH 0011H 0030H
	l	er0,	-4[fp]
	add	er0,	er4
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]
CBLOCKEND 7 20 573

;;            break;
CLINEA 0000H 0001H 023EH 000DH 0012H
	b	_$L68

;;        case 0x87:
CLINEA 0000H 0001H 023FH 0009H 0012H
_$S194 :

;;            derefw(PC) = derefw(code + cpc);
CLINEA 0000H 0001H 0241H 000DH 002CH
	l	er0,	-4[fp]
	add	er0,	er4
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 0242H 000DH 0012H
	b	_$L68

;;        case 0x88:
CLINEA 0000H 0001H 0243H 0009H 0012H
_$S195 :

;;            derefw(LR) = cpc;
CLINEA 0000H 0001H 0245H 000DH 001DH
	l	bp,	-10[fp]
	st	er4,	[bp]

;;            derefw(PC) = derefw(code + cpc);
CLINEA 0000H 0001H 0246H 000DH 002CH
	l	er0,	-4[fp]
	add	er0,	er4
	l	bp,	-2[fp]
	l	er0,	[er0]
	st	er0,	[bp]

;;            break;
CLINEA 0000H 0001H 0247H 000DH 0012H
	b	_$L68

;;        case 0xFF:
CLINEA 0000H 0001H 0249H 0009H 0012H
_$L196 :
_$S196 :

;;            while (1)
CLINEA 0000H 0000H 024BH 0000H 0000H
	bal	_$L196
_$L198 :

;;            invalid_instruction(instruction);
CLINEA 0000H 0001H 0251H 000DH 002DH
	l	er0,	-14[fp]
	bl	_invalid_instruction

;;            break;
CLINEA 0000H 0001H 0252H 000DH 0012H
	b	_$L68
CBLOCKEND 7 3 595
CBLOCKEND 7 1 596
CFUNCTIONEND 7

	extrn code far : _init_prog
	extrn code far : _memcpy
	extrn code far : _custom_break
	extrn code far : _print
	extrn code far : _set_pixel
	extrn code far : _plot_line
	extrn code far : _CheckButtons
	extrn code far : _render
	extrn code far : _invalid_instruction
	public _lexc_instruction
	public _lread_word
	public _linit_prog
	public _lcmp_flags_word
	public _lwrite_byte
	public _lwrite_word
	public _lcmp_flags_byte
	public _lmemcpy
	public _lread_byte
	public _lload_from_rom
	public _lkill
	extrn code far : _main

	rseg $$NTABlexc_instruction$legacy_vm
_$M19 :
	dw	_$S70
	dw	_$S71
	dw	_$S72
	dw	_$S73
	dw	_$S74
	dw	_$S75
	dw	_$S76
	dw	_$S77
	dw	_$S78
	dw	_$S79
	dw	_$S80
	dw	_$S81
	dw	_$S82
	dw	_$S83
	dw	_$S84
	dw	_$S85
	dw	_$S86
	dw	_$S89
	dw	_$S94
	dw	_$S97
	dw	_$S100
	dw	_$S103
	dw	_$S106
	dw	_$S107
	dw	_$S108
	dw	_$S109
	dw	_$S110
	dw	_$S111
	dw	_$S112
	dw	_$S113
	dw	_$S114
	dw	_$S115
	dw	_$S116
	dw	_$S117
	dw	_$S118
	dw	_$S119
	dw	_$S120
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$S121
	dw	_$S122
	dw	_$S123
	dw	_$S124
	dw	_$S125
	dw	_$S126
	dw	_$S127
	dw	_$S128
	dw	_$S129
	dw	_$S130
	dw	_$S131
	dw	_$S132
	dw	_$S133
	dw	_$S134
	dw	_$S135
	dw	_$L198
	dw	_$S136
	dw	_$S137
	dw	_$S138
	dw	_$S139
	dw	_$S140
	dw	_$S141
	dw	_$S142
	dw	_$S143
	dw	_$S144
	dw	_$S145
	dw	_$S146
	dw	_$S147
	dw	_$S148
	dw	_$S149
	dw	_$S150
	dw	_$L198
	dw	_$S151
	dw	_$S152
	dw	_$S153
	dw	_$S154
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$S155
	dw	_$S156
	dw	_$S157
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$S158
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$S173
	dw	_$S174
	dw	_$S177
	dw	_$S182
	dw	_$S185
	dw	_$S188
	dw	_$S191
	dw	_$S194
	dw	_$S195
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$L198
	dw	_$S196
	extrn code : __imulu8lw
	extrn code : __uidivu8lw

	end

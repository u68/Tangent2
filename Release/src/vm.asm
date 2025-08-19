;; Compile Options : /TML620909 /ML /near /SD /Oa /Ot /W 1 /Fasrc\ 
;; Version Number  : Ver.3.66.2
;; File Name       : vm.c

	type (ML620909) 
	model large, near
	$$TABerror$vm segment table 2h #0h
	$$TABmsg$vm segment table 2h #0h
	$$bubble_sortw$vm segment code 2h any
	$$del_index$vm segment code 2h any
	$$get_program_start$vm segment code 2h any
	$$init_prog$vm segment code 2h any
	$$invalid_instruction$vm segment code 2h any
	$$kill$vm segment code 2h any
	$$load_from_rom$vm segment code 2h any
	$$memcpy$vm segment code 2h any
	$$read_byte$vm segment code 2h any
CVERSION 3.66.2
CGLOBAL 01H 02H 0000H "read_byte" 08H 02H 05H 00H 80H 04H 00H 00H 00H
CGLOBAL 01H 03H 0000H "bubble_sortw" 08H 02H 17H 00H 80H 08H 00H 00H 07H
CGLOBAL 01H 03H 0000H "del_index" 08H 02H 18H 00H 80H 06H 00H 00H 07H
CGLOBAL 01H 03H 0000H "kill" 08H 02H 03H 00H 81H 06H 00H 00H 07H
CGLOBAL 01H 03H 0000H "invalid_instruction" 08H 02H 01H 00H 81H 08H 00H 00H 07H
CGLOBAL 01H 03H 0000H "memcpy" 08H 02H 00H 00H 82H 08H 00H 00H 07H
CGLOBAL 01H 03H 0000H "init_prog" 08H 02H 02H 00H 80H 00H 00H 00H 07H
CGLOBAL 01H 03H 0000H "load_from_rom" 08H 02H 04H 00H 81H 08H 00H 00H 07H
CGLOBAL 01H 02H 0000H "get_program_start" 08H 02H 19H 00H 81H 08H 00H 00H 08H
CENUMTAG 0000H 0000H 0001H 0017H "SPECIAL_CHARS"
CENUMMEM 0000000AH "SP_EXE"
CENUMMEM 00000009H "SP_TAB"
CENUMMEM 00000020H "SP_SPACE"
CENUMMEM 00000008H "SP_BACKSPACE"
CENUMMEM 0000002AH "SP_HOME"
CENUMMEM 0000001AH "SP_END"
CENUMMEM 0000003AH "SP_YES"
CENUMMEM 00000012H "SP_NO"
CENUMMEM 00000021H "SP_OK"
CENUMMEM 00000020H "SP_UP"
CENUMMEM 00000022H "SP_DOWN"
CENUMMEM 00000029H "SP_LEFT"
CENUMMEM 00000019H "SP_RIGHT"
CENUMMEM 00000010H "SP_PLUS"
CENUMMEM 00000011H "SP_MINUS"
CENUMMEM 00000031H "SP_ALT"
CENUMMEM 00000030H "SP_ABC"
CENUMMEM 00000039H "SP_ESC"
CENUMMEM 0000003AH "SP_SELECTLEFT"
CENUMMEM 0000003BH "SP_SELECTRIGHT"
CENUMMEM 0000003CH "SP_PASTE"
CENUMMEM 0000003DH "SP_COPY"
CENUMMEM 0000003EH "SPECIAL_MAX"
CENUMTAG 0000H 0000H 0000H 0025H "BUTTON"
CENUMMEM 0000000BH "B_0"
CENUMMEM 0000003FH "B_1"
CENUMMEM 00000037H "B_2"
CENUMMEM 0000002FH "B_3"
CENUMMEM 0000003EH "B_4"
CENUMMEM 00000036H "B_5"
CENUMMEM 0000002EH "B_6"
CENUMMEM 0000003DH "B_7"
CENUMMEM 00000035H "B_8"
CENUMMEM 0000002DH "B_9"
CENUMMEM 0000003CH "B_A"
CENUMMEM 00000034H "B_B"
CENUMMEM 0000002CH "B_C"
CENUMMEM 00000024H "B_D"
CENUMMEM 0000001CH "B_E"
CENUMMEM 00000014H "B_F"
CENUMMEM 0000003DH "B_G"
CENUMMEM 00000035H "B_H"
CENUMMEM 0000002DH "B_I"
CENUMMEM 00000025H "B_J"
CENUMMEM 0000001DH "B_K"
CENUMMEM 0000003EH "B_L"
CENUMMEM 00000036H "B_M"
CENUMMEM 0000002EH "B_N"
CENUMMEM 00000026H "B_O"
CENUMMEM 0000001EH "B_P"
CENUMMEM 0000003FH "B_Q"
CENUMMEM 00000037H "B_R"
CENUMMEM 0000002FH "B_S"
CENUMMEM 00000027H "B_T"
CENUMMEM 0000001FH "B_U"
CENUMMEM 0000000BH "B_V"
CENUMMEM 0000000CH "B_W"
CENUMMEM 0000000DH "B_X"
CENUMMEM 0000000EH "B_Y"
CENUMMEM 0000000FH "B_Z"
CENUMMEM 00000040H "BUTTON_COUNT"
CTYPEDEF 0000H 0000H 42H "ushort" 02H 00H 08H
CTYPEDEF 0000H 0000H 42H "byte" 02H 00H 00H
CTYPEDEF 0000H 0000H 42H "word" 02H 00H 08H
CGLOBAL 01H 00H 000DH "msg" 05H 01H 0DH 00H 00H 00H
CGLOBAL 01H 00H 0015H "error" 05H 01H 15H 00H 00H 00H
CFILE 0001H 00000070H "..\\src\\base.h"
CFILE 0002H 00000014H "..\\src\\vm.h"
CFILE 0003H 00000072H "..\\src\\io.h"
CFILE 0004H 00000008H "..\\src\\vm_config.h"
CFILE 0000H 00000061H "..\\src\\vm.c"

	rseg $$invalid_instruction$vm
CFUNCTION 1

_invalid_instruction	:
CBLOCK 1 1 11

;;{
CLINEA 0000H 0001H 000BH 0001H 0001H
	push	lr
	push	er8
	mov	er8,	er0
CBLOCK 1 2 11
CRET 0002H
CARGUMENT 46H 0002H 0028H "opc" 02H 00H 01H

;;    print(error, 1, 1, 2);
CLINEA 0000H 0001H 000CH 0005H 001AH
	mov	r0,	#02h
	push	r0
	mov	r3,	#01h
	mov	r2,	#01h
	mov	r0,	#BYTE1 OFFSET _error
	mov	r1,	#BYTE2 OFFSET _error
	bl	_print
	add	sp,	#2 

;;    print(msg, 1, 2, 2);
CLINEA 0000H 0001H 000DH 0005H 0018H
	mov	r0,	#02h
	push	r0
	mov	r3,	#02h
	mov	r2,	#01h
	mov	r0,	#BYTE1 OFFSET _msg
	mov	r1,	#BYTE2 OFFSET _msg
	bl	_print
	add	sp,	#2 

;;    derefw(0x9000) = opc;
CLINEA 0000H 0001H 000EH 0005H 0019H
	st	er8,	09000h
CBLOCKEND 1 2 15

;;}
CLINEA 0000H 0001H 000FH 0001H 0001H
	pop	er8
	pop	pc
CBLOCKEND 1 1 15
CFUNCTIONEND 1


	rseg $$memcpy$vm
CFUNCTION 0

_memcpy	:
CBLOCK 0 1 18

;;{
CLINEA 0000H 0001H 0012H 0001H 0001H
	push	fp
	mov	fp,	sp
	push	xr8
	push	er4
	mov	er10,	er2
	mov	er8,	er0
CBLOCK 0 2 18
CARGUMENT 46H 0002H 0028H "dest" 02H 00H 01H
CARGUMENT 46H 0002H 0029H "src" 02H 00H 01H
CARGUMENT 42H 0002H 0002H "size" 02H 00H 01H
CLOCAL 46H 0002H 0026H 0002H "i" 02H 00H 08H

;;    for(i = 0;i < size;i++)
CLINEA 0000H 0001H 0014H 0005H 001BH
	mov	er4,	#0 
	bal	_$L9
_$L6 :
CBLOCK 0 3 21

;;        deref(dest+i) = deref(src+i);
CLINEA 0000H 0001H 0016H 0009H 0025H
	mov	er0,	er10
	add	er0,	er4
	mov	er2,	er0
	mov	er0,	er8
	add	er0,	er4
	l	r2,	[er2]
	st	r2,	[er0]
CBLOCKEND 0 3 23

;;    for(i = 0;i < size;i++)
CLINEA 0000H 0000H 0014H 0005H 001BH
	add	er4,	#1 
_$L9 :
	l	er2,	2[fp]
	cmp	er4,	er2
	blt	_$L6
CBLOCKEND 0 2 24

;;}
CLINEA 0000H 0001H 0018H 0001H 0001H
	pop	er4
	pop	xr8
	mov	sp,	fp
	pop	fp
	rt
CBLOCKEND 0 1 24
CFUNCTIONEND 0


	rseg $$bubble_sortw$vm
CFUNCTION 23

_bubble_sortw	:
CBLOCK 23 1 27

;;{
CLINEA 0000H 0001H 001BH 0001H 0001H
	push	xr8
	push	xr4
	mov	er8,	er0
	mov	r10,	r2
CBLOCK 23 2 27
CARGUMENT 46H 0002H 0028H "arr" 04H 03H 00H 00H 01H
CARGUMENT 46H 0001H 001EH "size" 02H 00H 00H
CLOCAL 46H 0001H 0019H 0002H "i" 02H 00H 00H
CLOCAL 46H 0001H 0018H 0002H "j" 02H 00H 00H
CLOCAL 46H 0001H 001AH 0002H "temp" 02H 00H 00H

;;    for(i = 0; i < size - 1; i++)
CLINEA 0000H 0001H 001EH 0005H 0021H
	mov	r5,	#00h
	bal	_$L16
_$L13 :
CBLOCK 23 3 31

;;        for(j = 0; j < size - i - 1; j++)
CLINEA 0000H 0001H 0020H 000DH 0012H
	mov	r4,	#00h

;;        for(j = 0; j < size - i - 1; j++)
CLINEA 0000H 0000H 0020H 0014H 0024H
	bal	_$L22

;;        for(j = 0; j < size - i - 1; j++)
CLINEA 0000H 0000H 0020H 0026H 0028H
_$L19 :
CBLOCK 23 4 33

;;            if(arr[j] > arr[j + 1])
CLINEA 0000H 0001H 0022H 000DH 0023H
	mov	r0,	r4
	mov	r1,	#00h
	add	er0,	er0
	add	er0,	er8
	l	er2,	[er0]
	l	er0,	02h[er0]
	cmp	er2,	er0
	ble	_$L23
CBLOCK 23 5 35

;;                temp = arr[j];
CLINEA 0000H 0001H 0024H 0011H 001EH
	mov	r0,	r4
	mov	r1,	#00h
	add	er0,	er0
	add	er0,	er8
	l	r6,	[er0]

;;                arr[j] = arr[j + 1];
CLINEA 0000H 0001H 0025H 0011H 0024H
	mov	er2,	er0
	l	er0,	02h[er0]
	st	er0,	[er2]

;;                arr[j + 1] = temp;
CLINEA 0000H 0001H 0026H 0011H 0022H
	mov	r0,	r4
	mov	r1,	#00h
	add	er0,	er0
	add	er0,	er8
	mov	r2,	r6
	mov	r3,	#00h
	st	er2,	02h[er0]
CBLOCKEND 23 5 39

;;            }
CLINEA 0000H 0000H 0027H 000DH 000DH
_$L23 :
CBLOCKEND 23 4 40

;;        for(j = 0; j < size - i - 1; j++)
CLINEA 0000H 0000H 0020H 0026H 0028H
	add	r4,	#01h

;;        for(j = 0; j < size - i - 1; j++)
CLINEA 0000H 0000H 0020H 0014H 0024H
_$L22 :
	mov	r0,	r10
	mov	r1,	#00h
	mov	r3,	#00h
	sub	r0,	r5
	subc	r1,	r3
	add	er0,	#-1
	mov	r2,	r4
	cmp	er2,	er0
	blts	_$L19
CBLOCKEND 23 3 41

;;    for(i = 0; i < size - 1; i++)
CLINEA 0000H 0000H 001EH 0005H 0021H
	add	r5,	#01h
_$L16 :

;;    for(i = 0; i < size - 1; i++)
CLINEA 0000H 0000H 001EH 0014H 0024H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	#-1
	mov	r2,	r5
	mov	r3,	#00h
	cmp	er2,	er0
	blts	_$L13
CBLOCKEND 23 2 42

;;}
CLINEA 0000H 0001H 002AH 0001H 0001H
	pop	xr4
	pop	xr8
	rt
CBLOCKEND 23 1 42
CFUNCTIONEND 23


	rseg $$del_index$vm
CFUNCTION 24

_del_index	:
CBLOCK 24 1 45

;;{
CLINEA 0000H 0001H 002DH 0001H 0001H
	push	xr8
	push	er4
	mov	r10,	r2
	mov	er8,	er0
CBLOCK 24 2 45
CARGUMENT 46H 0002H 0028H "arr" 04H 03H 00H 00H 01H
CARGUMENT 46H 0001H 001EH "size" 02H 00H 00H
CARGUMENT 46H 0001H 0017H "index" 02H 00H 00H
CLOCAL 46H 0001H 0018H 0002H "i" 02H 00H 00H

;;    if(index >= size) return;
CLINEA 0000H 0001H 002FH 0005H 001DH
	cmp	r3,	r2
	bge	_$L25

;;    for(i = index; i < size - 1; i++)
CLINEA 0000H 0001H 0030H 0009H 0012H
	mov	r4,	r3

;;    for(i = index; i < size - 1; i++)
CLINEA 0000H 0000H 0030H 0014H 0020H
	bal	_$L33

;;    for(i = index; i < size - 1; i++)
CLINEA 0000H 0000H 0030H 0022H 0024H
_$L30 :
CBLOCK 24 3 49

;;        arr[i] = arr[i + 1];
CLINEA 0000H 0001H 0032H 0009H 001CH
	mov	r0,	r4
	mov	r1,	#00h
	add	er0,	er0
	add	er0,	er8
	mov	er2,	er0
	l	er0,	02h[er0]
	st	er0,	[er2]
CBLOCKEND 24 3 51

;;    for(i = index; i < size - 1; i++)
CLINEA 0000H 0000H 0030H 0022H 0024H
	add	r4,	#01h

;;    for(i = index; i < size - 1; i++)
CLINEA 0000H 0000H 0030H 0014H 0020H
_$L33 :
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	#-1
	mov	r2,	r4
	mov	r3,	#00h
	cmp	er2,	er0
	blts	_$L30

;;    arr[size - 1] = 0; 
CLINEA 0000H 0001H 0034H 0005H 0017H
	mov	r0,	r10
	mov	r1,	#00h
	add	er0,	er0
	add	er0,	er8
	mov	er2,	#0 
	st	er2,	0fffeh[er0]
CBLOCKEND 24 2 53

;;}
CLINEA 0000H 0001H 0035H 0001H 0001H
_$L25 :
	pop	er4
	pop	xr8
	rt
CBLOCKEND 24 1 53
CFUNCTIONEND 24


	rseg $$get_program_start$vm
CFUNCTION 25

_get_program_start	:
CBLOCK 25 1 56

;;{
CLINEA 0000H 0001H 0038H 0001H 0001H
	push	lr
	push	er8
	push	er4
	mov	er8,	er0
CBLOCK 25 2 56
CRET 0004H
CARGUMENT 46H 0002H 0028H "PID" 02H 00H 01H
CLOCAL 4AH 0002H 0000H 0002H "program_start_array" 04H 03H 00H 00H 08H
CLOCAL 46H 0001H 0018H 0002H "i" 02H 00H 00H

;;    byte i = 0;
CLINEA 0000H 0001H 003AH 0005H 000FH
	mov	r4,	#00h

;;    bubble_sortw(program_start_array, MAXIMUS_PROGRAMUS);
CLINEA 0000H 0001H 003BH 0005H 0039H
	mov	r2,	#043h
	mov	r0,	#086h
	mov	r1,	#09ch
	bl	_bubble_sortw

;;    while(program_start_array[i])
CLINEA 0000H 0001H 003CH 0005H 0021H
	bal	_$L35
_$L37 :
CBLOCK 25 3 61

;;        if(program_start_array[i] == PID) return program_start_array[i];
CLINEA 0000H 0001H 003EH 0009H 0048H
	mov	r0,	r4
	mov	r1,	#00h
	add	er0,	er0
	l	er0,	09c86h[er0]
	cmp	er0,	er8
	bne	_$L39
	mov	r0,	r4
	mov	r1,	#00h
	add	er0,	er0
	l	er0,	09c86h[er0]
CBLOCKEND 25 2 66

;;}
CLINEA 0000H 0001H 0042H 0001H 0001H
_$L34 :
	pop	er4
	pop	er8
	pop	pc

;;        if(program_start_array[i] == PID) return program_start_array[i];
CLINEA 0000H 0000H 003EH 0009H 0048H
_$L39 :

;;        i++;
CLINEA 0000H 0000H 003FH 0009H 000CH
	add	r4,	#01h
CBLOCKEND 25 3 64

;;    }
CLINEA 0000H 0000H 0040H 0005H 0005H
_$L35 :

;;    while(program_start_array[i])
CLINEA 0000H 0000H 003CH 0014H 0020H
	mov	r0,	r4
	mov	r1,	#00h
	add	er0,	er0
	l	er0,	09c86h[er0]
	bne	_$L37

;;    return 0; //not found
CLINEA 0000H 0001H 0041H 0005H 0019H
	mov	er0,	#0 
	bal	_$L34
CBLOCKEND 25 1 66
CFUNCTIONEND 25


	rseg $$init_prog$vm
CFUNCTION 2

_init_prog	:
CBLOCK 2 1 69

;;}
CLINEA 0000H 0001H 0047H 0001H 0001H
	rt
CBLOCKEND 2 1 71
CFUNCTIONEND 2


	rseg $$kill$vm
CFUNCTION 3

_kill	:
CBLOCK 3 1 73

;;{
CLINEA 0000H 0001H 0049H 0001H 0001H
	push	lr
	push	er8
	mov	er8,	er0
CBLOCK 3 2 73
CRET 0002H
CARGUMENT 46H 0002H 0028H "PID" 02H 00H 01H

;;    del_index((word *)MAXIMUS_PROGRAMUS_START, MAXIMUS_PROGRAMUS, PID);
CLINEA 0000H 0001H 004AH 0005H 0047H
	mov	r3,	r0
	mov	r2,	#043h
	mov	r0,	#00h
	mov	r1,	#09ch
	bl	_del_index

;;    del_index((word *)PROGRAMUS_START_START, MAXIMUS_PROGRAMUS, PID);
CLINEA 0000H 0001H 004BH 0005H 0045H
	mov	r3,	r8
	mov	r2,	#043h
	mov	r0,	#086h
	mov	r1,	#09ch
	bl	_del_index
CBLOCKEND 3 2 76

;;}
CLINEA 0000H 0001H 004CH 0001H 0001H
	pop	er8
	pop	pc
CBLOCKEND 3 1 76
CFUNCTIONEND 3


	rseg $$load_from_rom$vm
CFUNCTION 4

_load_from_rom	:
CBLOCK 4 1 78

;;{
CLINEA 0000H 0001H 004EH 0001H 0001H
	push	lr
	push	er8
	push	er4
	mov	er8,	er0
CBLOCK 4 2 78
CRET 0004H
CARGUMENT 46H 0002H 0028H "adr" 02H 00H 01H
CLOCAL 4AH 0002H 0000H 0002H "program_array" 04H 03H 00H 00H 08H
CLOCAL 46H 0001H 0018H 0002H "i" 02H 00H 00H
CLOCAL 4AH 0002H 0000H 0002H "dadr" 02H 00H 08H

;;    byte i = 0;
CLINEA 0000H 0001H 0052H 0005H 000FH
	mov	r0,	#00h
	mov	r4,	#00h

;;    bubble_sortw(program_array, MAXIMUS_PROGRAMUS);
CLINEA 0000H 0001H 0054H 0005H 0033H
	mov	r2,	#043h
	mov	r1,	#09ch
	bl	_bubble_sortw

;;    while(program_array[i]) (i++);
CLINEA 0000H 0001H 0055H 0005H 0022H
	bal	_$L44
_$L46 :
	add	r4,	#01h
_$L44 :

;;    while(program_array[i]) (i++);
CLINEA 0000H 0000H 0055H 0014H 0020H
	mov	r0,	r4
	mov	r1,	#00h
	add	er0,	er0
	l	er0,	09c00h[er0]
	bne	_$L46

;;    program_array[i] = adr;
CLINEA 0000H 0001H 0056H 0005H 001BH
	mov	r0,	r4
	mov	r1,	#00h
	add	er0,	er0
	st	er8,	09c00h[er0]
CBLOCKEND 4 2 87

;;}
CLINEA 0000H 0001H 0057H 0001H 0001H
	pop	er4
	pop	er8
	pop	pc
CBLOCKEND 4 1 87
CFUNCTIONEND 4


	rseg $$read_byte$vm
CFUNCTION 5

_read_byte	:
CBLOCK 5 1 89

;;{
CLINEA 0000H 0001H 0059H 0001H 0001H
	push	er10
	push	er4
	mov	er10,	er2
CBLOCK 5 2 89
CARGUMENT 46H 0002H 0024H "PID" 02H 00H 01H
CARGUMENT 46H 0002H 0029H "addr" 02H 00H 01H
CLOCAL 4AH 0002H 0000H 0002H "mrams" 02H 00H 08H

;;    if(addr > mrams) {return 0;} //out of bounds
CLINEA 0000H 0001H 005BH 0005H 0030H
	l	er2,	02h[er0]
	cmp	er10,	er2
	ble	_$L49
CBLOCK 5 3 91
	mov	r4,	#00h
CBLOCKEND 5 3 91
_$L49 :
CBLOCKEND 5 2 93

;;}
CLINEA 0000H 0001H 005DH 0001H 0001H
	mov	r0,	r4
	pop	er4
	pop	er10
	rt
CBLOCKEND 5 1 93
CFUNCTIONEND 5

	public _read_byte
	public _bubble_sortw
	public _del_index
	public _msg
	public _error
	public _kill
	public _invalid_instruction
	public _memcpy
	public _init_prog
	public _load_from_rom
	public _get_program_start
	extrn code far : _print
	extrn code far : _main

	rseg $$TABerror$vm
_error :
	DB	"twin smth aint right", 00H

	rseg $$TABmsg$vm
_msg :
	DB	"check 0x9000", 00H

	end

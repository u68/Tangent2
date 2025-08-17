;; Compile Options : /TML620909 /ML /near /SD /Oa /Ot /W 1 /Fasrc\ 
;; Version Number  : Ver.3.66.2
;; File Name       : io.c

	type (ML620909) 
	model large, near
	$$NINITVAR segment data 2h #0h
	$$NINITTAB segment table 2h any
	$$CheckButtons$io segment code 2h any
	$$TABbutton_to_char$io segment table 2h #0h
	$$TABbutton_to_char_abc$io segment table 2h #0h
	$$TABbutton_to_char_alt$io segment table 2h #0h
	$$TABimage_raw$io segment table 2h #0h
	$$charprint1$io segment code 2h any
	$$charprint2$io segment code 2h any
	$$draw_byte1$io segment code 2h any
	$$draw_byte2$io segment code 2h any
	$$drawbitmap$io segment code 2h any
	$$plot_line$io segment code 2h any
	$$print$io segment code 2h any
	$$render$io segment code 2h any
	$$set_pixel$io segment code 2h any
CVERSION 3.66.2
CGLOBAL 01H 03H 0000H "set_pixel" 08H 02H 08H 00H 80H 06H 00H 00H 07H
CGLOBAL 01H 02H 0000H "CheckButtons" 08H 02H 0BH 00H 80H 00H 00H 00H 00H
CGLOBAL 01H 03H 0000H "render" 08H 02H 07H 00H 80H 04H 00H 00H 07H
CGLOBAL 01H 03H 0000H "plot_line" 08H 02H 0AH 00H 83H 16H 00H 00H 07H
CGLOBAL 01H 03H 0000H "drawbitmap" 08H 02H 06H 00H 82H 0eH 00H 00H 07H
CGLOBAL 01H 02H 0000H "print" 08H 02H 05H 00H 83H 18H 00H 00H 08H
CGLOBAL 01H 03H 0000H "charprint1" 08H 02H 02H 00H 81H 0aH 00H 00H 07H
CGLOBAL 01H 03H 0000H "charprint2" 08H 02H 04H 00H 81H 0aH 00H 00H 07H
CGLOBAL 01H 03H 0000H "draw_byte1" 08H 02H 01H 00H 80H 08H 00H 00H 07H
CGLOBAL 01H 03H 0000H "draw_byte2" 08H 02H 03H 00H 80H 0aH 00H 00H 07H
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
CGLOBAL 01H 00H 1000H "image_raw" 05H 01H 00H 10H 00H 00H
CGLOBAL 01H 42H 0001H "lastbutton" 02H 00H 00H
CGLOBAL 01H 00H 0040H "button_to_char_abc" 05H 01H 40H 00H 00H 00H
CGLOBAL 01H 00H 0040H "button_to_char_alt" 05H 01H 40H 00H 00H 00H
CGLOBAL 01H 00H 0040H "button_to_char" 05H 01H 40H 00H 00H 00H
CFILE 0001H 00000070H "..\\src\\base.h"
CFILE 0002H 00000072H "..\\src\\io.h"
CFILE 0003H 00000408H "..\\src\\thefont.h"
CFILE 0000H 000000F8H "..\\src\\io.c"

	rseg $$draw_byte1$io
CFUNCTION 1

_draw_byte1	:
CBLOCK 1 1 43

;;void draw_byte1(byte x, byte y, byte sprite) {
CLINEA 0000H 0001H 002BH 0001H 002EH
	push	xr8
	push	xr4
	mov	r9,	r1
	mov	r10,	r2
CBLOCK 1 2 43
CARGUMENT 46H 0001H 0014H "x" 02H 00H 00H
CARGUMENT 46H 0001H 001DH "y" 02H 00H 00H
CARGUMENT 46H 0001H 001EH "sprite" 02H 00H 00H
CLOCAL 46H 0001H 001BH 0002H "bit_index" 02H 00H 00H
CLOCAL 46H 0002H 0026H 0002H "index" 02H 00H 08H

;;    byte bit_index = x & 7;
CLINEA 0000H 0001H 002CH 0005H 001BH
	mov	r4,	r0
	mov	r5,	#00h
	mov	er0,	er4
	and	r0,	#07h
	mov	r6,	r0

;;    word index = ((y << 4) + (y << 3)) + x;
CLINEA 0000H 0001H 002DH 0005H 002BH
	mov	r2,	r9
	mov	r3,	#00h
	mov	er0,	er2
	sllc	r1,	#04h
	sll	r0,	#04h
	sllc	r3,	#03h
	sll	r2,	#03h
	add	er2,	er0
	mov	er0,	er2
	add	er0,	er4
	mov	er4,	er0

;;    deref(vram1 + index) |= (sprite >> bit_index);
CLINEA 0000H 0001H 002EH 0005H 0032H
	mov	r2,	r10
	cmp	r6,	#07h
	bgt	_$M1
	srl	r2,	r6
	b	_$M2
_$M1 :
	mov	r2,	#00h
_$M2 :
	l	r3,	09000h[er0]
	or	r3,	r2
	st	r3,	09000h[er0]

;;    deref(vram1 + index + 1) |= (sprite << (8 - bit_index));
CLINEA 0000H 0001H 002FH 0005H 003CH
	mov	er0,	#8 
	sub	r0,	r6
	mov	r2,	r10
	cmp	r0,	#07h
	bgt	_$M3
	sll	r2,	r0
	b	_$M4
_$M3 :
	mov	r2,	#00h
_$M4 :
	l	r0,	09001h[er4]
	or	r0,	r2
	st	r0,	09001h[er4]
CBLOCKEND 1 2 48

;;}
CLINEA 0000H 0001H 0030H 0001H 0001H
	pop	xr4
	pop	xr8
	rt
CBLOCKEND 1 1 48
CFUNCTIONEND 1


	rseg $$draw_byte2$io
CFUNCTION 3

_draw_byte2	:
CBLOCK 3 1 50

;;void draw_byte2(byte x, byte y, byte sprite) {
CLINEA 0000H 0001H 0032H 0001H 002EH
	push	xr8
	push	xr4
	push	bp
	mov	er8,	er0
	mov	r10,	r2
CBLOCK 3 2 50
CARGUMENT 46H 0001H 001CH "x" 02H 00H 00H
CARGUMENT 46H 0001H 001DH "y" 02H 00H 00H
CARGUMENT 46H 0001H 001EH "sprite" 02H 00H 00H
CLOCAL 46H 0002H 002AH 0002H "bit_index" 02H 00H 08H
CLOCAL 46H 0002H 0027H 0002H "index" 02H 00H 08H

;;    word bit_index = x & 7;
CLINEA 0000H 0001H 0033H 0005H 001BH
	mov	r4,	r0
	mov	r5,	#00h
	mov	er0,	er4
	and	r0,	#07h
	and	r1,	#00h
	mov	bp,	er0

;;    word index = y * 24 + x;
CLINEA 0000H 0001H 0034H 0005H 001CH
	mov	r2,	r9
	mov	r3,	#00h
	mov	er6,	er2
	add	er6,	er2
	add	er6,	er2
	sllc	r7,	#03h
	sll	r6,	#03h
	mov	er2,	er6
	add	er2,	er4
	mov	er6,	er2

;;    deref(vram2 + index) |= sprite >> bit_index;
CLINEA 0000H 0001H 0035H 0005H 0030H
	mov	r4,	r10
	srl	r4,	r0
	l	r0,	09600h[er2]
	or	r0,	r4
	st	r0,	09600h[er2]

;;    if (bit_index != 0 && x + 1 < 24) {
CLINEA 0000H 0001H 0036H 0005H 0027H
	mov	bp,	bp
	beq	_$L2
	mov	r0,	r8
	mov	r1,	#00h
	add	er0,	#1 
	cmp	r0,	#018h
	cmpc	r1,	#00h
	bges	_$L2
CBLOCK 3 3 54

;;    	deref(vram2 + index + 1) |= sprite << (8 - bit_index);
CLINEA 0000H 0001H 0037H 0006H 003BH
	mov	er0,	#8 
	sub	r0,	r12
	mov	r2,	r10
	cmp	r0,	#07h
	bgt	_$M6
	sll	r2,	r0
	b	_$M7
_$M6 :
	mov	r2,	#00h
_$M7 :
	l	r0,	09601h[er6]
	or	r0,	r2
	st	r0,	09601h[er6]
CBLOCKEND 3 3 56

;;    }
CLINEA 0000H 0000H 0038H 0005H 0005H
_$L2 :
CBLOCKEND 3 2 57

;;}
CLINEA 0000H 0001H 0039H 0001H 0001H
	pop	bp
	pop	xr4
	pop	xr8
	rt
CBLOCKEND 3 1 57
CFUNCTIONEND 3


	rseg $$charprint1$io
CFUNCTION 2

_charprint1	:
CBLOCK 2 1 60

;;{
CLINEA 0000H 0001H 003CH 0001H 0001H
	push	lr
	push	xr8
	push	er4
	mov	er10,	er2
	mov	r9,	r1
	mov	r8,	r0
CBLOCK 2 2 60
CRET 0006H
CARGUMENT 46H 0001H 001CH "x" 02H 00H 00H
CARGUMENT 46H 0001H 001DH "y" 02H 00H 00H
CARGUMENT 46H 0002H 0029H "smth" 02H 00H 01H
CLOCAL 46H 0002H 0026H 0002H "i" 02H 00H 08H

;;	for(i = 0;i < 8;i++)
CLINEA 0000H 0001H 003EH 0002H 0015H
	mov	er4,	#0 
_$L12 :
CBLOCK 2 3 63

;;		draw_byte1(x,y+i,image_raw[i+(smth<<4)]);
CLINEA 0000H 0001H 0040H 0003H 002BH
	mov	er0,	er10
	sllc	r1,	#04h
	sll	r0,	#04h
	add	er0,	er4
	l	r2,	NEAR _image_raw[er0]
	mov	r1,	r9
	add	r1,	r4
	mov	r0,	r8
	bl	_draw_byte1
CBLOCKEND 2 3 65

;;	for(i = 0;i < 8;i++)
CLINEA 0000H 0000H 003EH 0002H 0015H
	add	er4,	#1 
	cmp	r4,	#08h
	cmpc	r5,	#00h
	blt	_$L12
CBLOCKEND 2 2 66

;;}
CLINEA 0000H 0001H 0042H 0001H 0001H
	pop	er4
	pop	xr8
	pop	pc
CBLOCKEND 2 1 66
CFUNCTIONEND 2


	rseg $$charprint2$io
CFUNCTION 4

_charprint2	:
CBLOCK 4 1 69

;;{
CLINEA 0000H 0001H 0045H 0001H 0001H
	push	lr
	push	xr8
	push	er4
	mov	er10,	er2
	mov	r9,	r1
	mov	r8,	r0
CBLOCK 4 2 69
CRET 0006H
CARGUMENT 46H 0001H 001CH "x" 02H 00H 00H
CARGUMENT 46H 0001H 001DH "y" 02H 00H 00H
CARGUMENT 46H 0002H 0029H "smth" 02H 00H 01H
CLOCAL 46H 0002H 0026H 0002H "i" 02H 00H 08H

;;	for(i = 0;i < 8;i++)
CLINEA 0000H 0001H 0047H 0002H 0015H
	mov	er4,	#0 
_$L19 :
CBLOCK 4 3 72

;;		draw_byte2(x,y+i,image_raw[i+(smth<<4)]);
CLINEA 0000H 0001H 0049H 0003H 002BH
	mov	er0,	er10
	sllc	r1,	#04h
	sll	r0,	#04h
	add	er0,	er4
	l	r2,	NEAR _image_raw[er0]
	mov	r1,	r9
	add	r1,	r4
	mov	r0,	r8
	bl	_draw_byte2
CBLOCKEND 4 3 74

;;	for(i = 0;i < 8;i++)
CLINEA 0000H 0000H 0047H 0002H 0015H
	add	er4,	#1 
	cmp	r4,	#08h
	cmpc	r5,	#00h
	blt	_$L19
CBLOCKEND 4 2 75

;;}
CLINEA 0000H 0001H 004BH 0001H 0001H
	pop	er4
	pop	xr8
	pop	pc
CBLOCKEND 4 1 75
CFUNCTIONEND 4


	rseg $$drawbitmap$io
CFUNCTION 6

_drawbitmap	:
CBLOCK 6 1 80

;;{
CLINEA 0000H 0001H 0050H 0001H 0001H
	push	fp
	mov	fp,	sp
	add	sp,	#-02
	push	xr8
	push	xr4
	push	bp
	mov	er10,	er2
	mov	er8,	er0
CBLOCK 6 2 80
CARGUMENT 44H 0002H 0028H "ptr" 04H 03H 00H 00H 00H
CARGUMENT 46H 0002H 0029H "offset" 02H 00H 01H
CARGUMENT 42H 0001H 0002H "width" 02H 00H 00H
CARGUMENT 42H 0001H 0004H "height" 02H 00H 00H
CARGUMENT 42H 0001H 0006H "color" 02H 00H 00H
CLOCAL 46H 0002H 002AH 0002H "x" 02H 00H 08H
CLOCAL 42H 0002H 0002H 0002H "y" 02H 00H 08H
CLOCAL 46H 0002H 0027H 0002H "i" 02H 00H 08H
CLOCAL 46H 0002H 0026H 0002H "j" 02H 00H 08H

;;	word i = 0;
CLINEA 0000H 0001H 0053H 0002H 000CH
	mov	er6,	#0 

;;	word j = 0;
CLINEA 0000H 0001H 0054H 0002H 000CH
	mov	er4,	#0 

;;	deref(0xF037) = 0; //magic buffer switch to make gr -> e <- yscale
CLINEA 0000H 0001H 0055H 0002H 0043H
	mov	r0,	#00h
	st	r0,	0f037h

;;	if(color == 1)
CLINEA 0000H 0001H 0056H 0002H 000FH
	l	r0,	6[fp]
	cmp	r0,	#01h
	bne	_$L24
CBLOCK 6 3 87

;;		deref(0xF037) = 4;
CLINEA 0000H 0001H 0058H 0003H 0014H
	mov	r0,	#04h
	st	r0,	0f037h
CBLOCKEND 6 3 89

;;	}
CLINEA 0000H 0000H 0059H 0002H 0002H
_$L24 :

;;	for(y = 0; y < offset; y++)
CLINEA 0000H 0001H 005AH 0006H 000BH
	mov	er0,	#0 

;;	for(y = 0; y < offset; y++)
CLINEA 0000H 0000H 005AH 000DH 0017H
	bal	_$L31

;;	for(y = 0; y < offset; y++)
CLINEA 0000H 0000H 005AH 0019H 001BH
_$L28 :
CBLOCK 6 4 91

;;		++j;
CLINEA 0000H 0001H 005CH 0003H 0006H
	add	er4,	#1 
	mov	er0,	er4

;;		if((j&0x1f) == 0x18)
CLINEA 0000H 0001H 005DH 0003H 0016H
	and	r0,	#01fh
	and	r1,	#00h
	cmp	r0,	#018h
	cmpc	r1,	#00h
	bne	_$L32
CBLOCK 6 5 94

;;			j += 0x8;
CLINEA 0000H 0001H 005FH 0004H 000CH
	add	er4,	#8 
CBLOCKEND 6 5 96

;;		}
CLINEA 0000H 0000H 0060H 0003H 0003H
_$L32 :
CBLOCKEND 6 4 97

;;	for(y = 0; y < offset; y++)
CLINEA 0000H 0000H 005AH 0019H 001BH
	l	er0,	-2[fp]
	add	er0,	#1 

;;	for(y = 0; y < offset; y++)
CLINEA 0000H 0000H 005AH 000DH 0017H
_$L31 :
	st	er0,	-2[fp]
	cmp	er0,	er10
	blt	_$L28

;;	for(y = 0; y < height; y++)
CLINEA 0000H 0001H 0063H 0006H 000BH
	mov	er0,	#0 

;;	for(y = 0; y < height; y++)
CLINEA 0000H 0000H 0063H 000DH 0017H
	bal	_$L39

;;	for(y = 0; y < height; y++)
CLINEA 0000H 0000H 0063H 0019H 001BH
_$L36 :
CBLOCK 6 6 100

;;		for(x = 0; x < width; x++)
CLINEA 0000H 0001H 0065H 0007H 000CH
	mov	bp,	#0 

;;		for(x = 0; x < width; x++)
CLINEA 0000H 0000H 0065H 000EH 0017H
	bal	_$L45

;;		for(x = 0; x < width; x++)
CLINEA 0000H 0000H 0065H 0019H 001BH
_$L42 :
CBLOCK 6 7 102

;;			if(color == 2)
CLINEA 0000H 0001H 0067H 0004H 0011H
	l	r0,	6[fp]
	cmp	r0,	#02h
	bne	_$L46
CBLOCK 6 8 104

;;				deref(0xF037) = 0;
CLINEA 0000H 0001H 0069H 0005H 0016H
	mov	r0,	#00h
	st	r0,	0f037h

;;				deref(0xf800+j) = ptr[i]; //the screen is f800
CLINEA 0000H 0001H 006AH 0005H 0032H
	mov	er0,	er8
	add	er0,	er6
	l	r0,	[er0]
	st	r0,	0f800h[er4]

;;				deref(0xF037) = 4;
CLINEA 0000H 0001H 006BH 0005H 0016H
	mov	r0,	#04h
	st	r0,	0f037h
CBLOCKEND 6 8 108

;;			}
CLINEA 0000H 0000H 006CH 0004H 0004H
_$L46 :

;;			deref(0xf800+j) = ptr[i];
CLINEA 0000H 0001H 006DH 0004H 001CH
	mov	er0,	er8
	add	er0,	er6
	l	r0,	[er0]
	st	r0,	0f800h[er4]

;;			++j;
CLINEA 0000H 0001H 006EH 0004H 0007H
	add	er4,	#1 
	mov	er0,	er4

;;			if((j&0x1f) == 0x18)
CLINEA 0000H 0001H 006FH 0004H 0017H
	and	r0,	#01fh
	and	r1,	#00h
	cmp	r0,	#018h
	cmpc	r1,	#00h
	bne	_$L48
CBLOCK 6 9 112

;;				j += 0x8;
CLINEA 0000H 0001H 0071H 0005H 000DH
	add	er4,	#8 
CBLOCKEND 6 9 114

;;			}
CLINEA 0000H 0000H 0072H 0004H 0004H
_$L48 :

;;			++i;
CLINEA 0000H 0001H 0073H 0004H 0007H
	add	er6,	#1 
CBLOCKEND 6 7 116

;;		for(x = 0; x < width; x++)
CLINEA 0000H 0000H 0065H 0019H 001BH
	add	bp,	#1 

;;		for(x = 0; x < width; x++)
CLINEA 0000H 0000H 0065H 000EH 0017H
_$L45 :
	l	r0,	2[fp]
	mov	r1,	#00h
	cmp	bp,	er0
	blt	_$L42

;;		j += 32-width;
CLINEA 0000H 0001H 0075H 0003H 0010H
	mov	r2,	r0
	mov	r3,	#00h
	mov	er0,	#32
	sub	r0,	r2
	subc	r1,	r3
	add	er4,	er0

;;		if((j&0x1f) == 0x18)
CLINEA 0000H 0001H 0076H 0003H 0016H
	mov	er0,	er4
	and	r0,	#01fh
	and	r1,	#00h
	cmp	r0,	#018h
	cmpc	r1,	#00h
	bne	_$L50
CBLOCK 6 10 119

;;			j += 0x8;
CLINEA 0000H 0001H 0078H 0004H 000CH
	add	er4,	#8 
CBLOCKEND 6 10 121

;;		}
CLINEA 0000H 0000H 0079H 0003H 0003H
_$L50 :
CBLOCKEND 6 6 122

;;	for(y = 0; y < height; y++)
CLINEA 0000H 0000H 0063H 0019H 001BH
	l	er0,	-2[fp]
	add	er0,	#1 

;;	for(y = 0; y < height; y++)
CLINEA 0000H 0000H 0063H 000DH 0017H
_$L39 :
	st	er0,	-2[fp]

;;	for(y = 0; y < height; y++)
CLINEA 0000H 0000H 0063H 000EH 0017H
	l	r0,	4[fp]
	mov	r1,	#00h
	l	er2,	-2[fp]
	cmp	er2,	er0
	blt	_$L36
CBLOCKEND 6 2 123

;;}
CLINEA 0000H 0001H 007BH 0001H 0001H
	pop	bp
	pop	xr4
	pop	xr8
	mov	sp,	fp
	pop	fp
	rt
CBLOCKEND 6 1 123
CFUNCTIONEND 6


	rseg $$render$io
CFUNCTION 7

_render	:
CBLOCK 7 1 128

;;{
CLINEA 0000H 0001H 0080H 0001H 0001H
	push	xr4
	mov	er2,	er0
CBLOCK 7 2 128
CARGUMENT 46H 0002H 0025H "addr" 02H 00H 01H
CLOCAL 46H 0002H 0027H 0002H "i" 02H 00H 08H
CLOCAL 46H 0002H 0026H 0002H "j" 02H 00H 08H

;;    word j = 0;
CLINEA 0000H 0001H 0082H 0005H 000FH
	mov	er0,	#0 
	mov	er4,	#0 

;;    deref(0xf037) = 0;
CLINEA 0000H 0001H 0083H 0005H 0016H
	st	r0,	0f037h

;;    for(i = 0; i < 0x0800; i++)
CLINEA 0000H 0001H 0084H 0005H 001FH
	mov	er6,	er0
_$L55 :
CBLOCK 7 3 133

;;        j++;
CLINEA 0000H 0000H 0086H 0009H 000CH
	add	er4,	#1 
	mov	er0,	er4

;;        if((j & 0x001F) == 0x18)
CLINEA 0000H 0001H 0087H 0009H 0020H
	and	r0,	#01fh
	and	r1,	#00h
	cmp	r0,	#018h
	cmpc	r1,	#00h
	bne	_$L59
CBLOCK 7 4 136

;;            j+=8;
CLINEA 0000H 0001H 0089H 000DH 0011H
	add	er4,	#8 
CBLOCKEND 7 4 138

;;        }
CLINEA 0000H 0000H 008AH 0009H 0009H
_$L59 :

;;        *((word *)(0xf800 + j)) = *((word *)(addr + i));
CLINEA 0000H 0001H 008BH 0009H 0038H
	mov	er0,	er2
	add	er0,	er6
	l	er0,	[er0]
	st	er0,	0f800h[er4]
CBLOCKEND 7 3 140

;;    for(i = 0; i < 0x0800; i++)
CLINEA 0000H 0000H 0084H 0005H 001FH
	add	er6,	#1 

;;    for(i = 0; i < 0x0800; i++)
CLINEA 0000H 0000H 0084H 000EH 0017H
	cmp	r6,	#00h
	cmpc	r7,	#08h
	blt	_$L55

;;    j = 0;
CLINEA 0000H 0001H 008DH 0005H 000AH
	mov	er4,	#0 

;;    deref(0xf037) = 4;
CLINEA 0000H 0001H 008EH 0005H 0016H
	mov	r0,	#04h
	st	r0,	0f037h

;;    for(i = 0; i < 0x0800; i++)
CLINEA 0000H 0001H 008FH 0005H 001FH
	mov	er6,	#0 
_$L63 :
CBLOCK 7 5 144

;;        j++;
CLINEA 0000H 0000H 0091H 0009H 000CH
	add	er4,	#1 
	mov	er0,	er4

;;        if((j & 0x001F) == 0x18)
CLINEA 0000H 0001H 0092H 0009H 0020H
	and	r0,	#01fh
	and	r1,	#00h
	cmp	r0,	#018h
	cmpc	r1,	#00h
	bne	_$L67
CBLOCK 7 6 147

;;            j+=8;
CLINEA 0000H 0001H 0094H 000DH 0011H
	add	er4,	#8 
CBLOCKEND 7 6 149

;;        }
CLINEA 0000H 0000H 0095H 0009H 0009H
_$L67 :

;;        *((word *)(0xf800 + j)) = *((word *)(addr + i + 0x600));
CLINEA 0000H 0001H 0096H 0009H 0040H
	mov	er0,	er2
	add	er0,	er6
	l	er0,	0600h[er0]
	st	er0,	0f800h[er4]
CBLOCKEND 7 5 151

;;    for(i = 0; i < 0x0800; i++)
CLINEA 0000H 0000H 008FH 0005H 001FH
	add	er6,	#1 

;;    for(i = 0; i < 0x0800; i++)
CLINEA 0000H 0000H 008FH 000EH 0017H
	cmp	r6,	#00h
	cmpc	r7,	#08h
	blt	_$L63
CBLOCKEND 7 2 152

;;}
CLINEA 0000H 0001H 0098H 0001H 0001H
	pop	xr4
	rt
CBLOCKEND 7 1 152
CFUNCTIONEND 7


	rseg $$print$io
CFUNCTION 5

_print	:
CBLOCK 5 1 156

;;{
CLINEA 0000H 0001H 009CH 0001H 0001H
	push	lr
	push	fp
	mov	fp,	sp
	add	sp,	#-02
	push	xr8
	push	xr4
	push	bp
	mov	r11,	r3
CBLOCK 5 2 156
CRET 000EH
CARGUMENT 44H 0002H 0024H "str" 04H 03H 00H 00H 00H
CARGUMENT 46H 0001H 0016H "x" 02H 00H 00H
CARGUMENT 46H 0001H 001FH "y" 02H 00H 00H
CARGUMENT 42H 0001H 0006H "color" 02H 00H 00H
CLOCAL 44H 0002H 002AH 0002H "what" 04H 03H 00H 00H 00H
CLOCAL 46H 0002H 0027H 0002H "curoffset" 02H 00H 08H
CLOCAL 42H 0002H 0002H 0002H "i" 02H 00H 08H

;;	const byte* what = str;
CLINEA 0000H 0001H 009DH 0002H 0018H
	mov	bp,	er0

;;	word curoffset = (word)x+((word)y<<8)+((word)y<<7);
CLINEA 0000H 0001H 009EH 0002H 0034H
	mov	r1,	r3
	mov	r0,	#00h
	mov	r4,	r2
	mov	r5,	#00h
	mov	er2,	er0
	add	er2,	er4
	mov	r0,	r11
	mov	r1,	#00h
	sllc	r1,	#07h
	sll	r0,	#07h
	add	er0,	er2
	mov	er6,	er0

;;	word i = 0;
CLINEA 0000H 0001H 00A0H 0002H 000CH
	mov	er0,	#0 

;;	while(*what)
CLINEA 0000H 0001H 00A1H 0002H 000DH
	bal	_$L70
_$L72 :
CBLOCK 5 3 162

;;		drawbitmap(image_raw+((word)(*what)<<4),curoffset,1,16,color);
CLINEA 0000H 0001H 00A5H 0003H 0040H
	l	r0,	6[fp]
	push	r0
	mov	r0,	#010h
	push	r0
	mov	r0,	#01h
	push	r0
	mov	er2,	er6
	l	r0,	[bp]
	mov	r1,	#00h
	sllc	r1,	#04h
	sll	r0,	#04h
	mov	r4,	#BYTE1 OFFSET _image_raw
	mov	r5,	#BYTE2 OFFSET _image_raw
	add	er4,	er0
	mov	er0,	er4
	bl	_drawbitmap
	add	sp,	#6 

;;		++curoffset;
CLINEA 0000H 0001H 00A6H 0003H 000EH
	add	er6,	#1 

;;		++what;
CLINEA 0000H 0001H 00A7H 0003H 0009H
	add	bp,	#1 

;;		++i;
CLINEA 0000H 0001H 00A8H 0003H 0006H
	mov	er0,	er8
	add	er0,	#1 
CBLOCKEND 5 3 170

;;	}
CLINEA 0000H 0000H 00AAH 0002H 0002H
_$L70 :
	mov	er8,	er0

;;	while(*what)
CLINEA 0000H 0000H 00A1H 000EH 0017H
	l	r0,	[bp]
	bne	_$L72

;;	return i;
CLINEA 0000H 0001H 00ABH 0002H 000AH
	mov	er0,	er8
CBLOCKEND 5 2 172

;;}
CLINEA 0000H 0000H 00ACH 0001H 0001H
	pop	bp
	pop	xr4
	pop	xr8
	mov	sp,	fp
	pop	fp
	pop	pc
CBLOCKEND 5 1 172
CFUNCTIONEND 5


	rseg $$set_pixel$io
CFUNCTION 8

_set_pixel	:
CBLOCK 8 1 175

;;{
CLINEA 0000H 0001H 00AFH 0001H 0001H
	push	xr8
	push	er4
	mov	er8,	er0
	mov	r10,	r2
CBLOCK 8 2 175
CARGUMENT 46H 0001H 001CH "x" 02H 00H 00H
CARGUMENT 46H 0001H 001DH "y" 02H 00H 00H
CARGUMENT 46H 0001H 001EH "color" 02H 00H 00H
CLOCAL 46H 0002H 0026H 0002H "byteIndex" 02H 00H 08H
CLOCAL 46H 0001H 0017H 0002H "bitIndex" 02H 00H 00H

;;	word byteIndex = (y*24) + (x >> 3);
CLINEA 0000H 0001H 00B0H 0002H 0024H
	mov	r0,	r1
	mov	r1,	#00h
	mov	er2,	er0
	add	er2,	er0
	add	er2,	er0
	sllc	r3,	#03h
	sll	r2,	#03h
	mov	r0,	r8
	srlc	r0,	#03h
	sra	r1,	#03h
	add	er0,	er2
	mov	er4,	er0

;;	byte bitIndex = x & 7;
CLINEA 0000H 0001H 00B1H 0002H 0017H
	mov	r0,	r8
	and	r0,	#07h
	mov	r3,	r0

;;	if (x > 191)
CLINEA 0000H 0001H 00B2H 0002H 000DH
	cmp	r8,	#0bfh
	ble	_$M24
	b	_$L74
_$M24 :
CBLOCK 8 3 179

;;		return;
CLINEA 0000H 0001H 00B4H 0003H 0009H
CBLOCKEND 8 3 181

;;	if (y > 63)
CLINEA 0000H 0001H 00B6H 0002H 000CH
	cmp	r9,	#03fh
	ble	_$M25
	b	_$L74
_$M25 :
CBLOCK 8 4 183

;;		return;
CLINEA 0000H 0001H 00B8H 0003H 0009H
CBLOCKEND 8 4 185

;;	if (byteIndex + vram2 > 0x9c02)
CLINEA 0000H 0001H 00BAH 0002H 0020H
	mov	er0,	er4
	add	r0,	#00h
	addc	r1,	#096h
	cmp	r0,	#02h
	cmpc	r1,	#09ch
	ble	_$M26
	b	_$L74
_$M26 :
CBLOCK 8 5 187

;;		return;
CLINEA 0000H 0001H 00BCH 0003H 0009H
CBLOCKEND 8 5 189

;;	deref(byteIndex + vram1) &= ~(0x80>>bitIndex);
CLINEA 0000H 0001H 00BEH 0002H 002FH
	mov	r0,	#080h
	mov	r1,	#00h
	mov	r2,	r3
_$M15 :
	cmp	r2,	#07h
	ble	_$M14
	srlc	r0,	#07h
	sra	r1,	#07h
	add	r2,	#0f9h
	bne	_$M15
_$M14 :
	srlc	r0,	r2
	xor	r0,	#0ffh
	l	r1,	09000h[er4]
	and	r1,	r0
	st	r1,	09000h[er4]

;;	deref(byteIndex + vram2) &= ~(0x80>>bitIndex);
CLINEA 0000H 0001H 00BFH 0002H 002FH
	mov	r0,	#080h
	mov	r1,	#00h
	mov	r2,	r3
_$M17 :
	cmp	r2,	#07h
	ble	_$M16
	srlc	r0,	#07h
	sra	r1,	#07h
	add	r2,	#0f9h
	bne	_$M17
_$M16 :
	srlc	r0,	r2
	xor	r0,	#0ffh
	l	r1,	09600h[er4]
	and	r1,	r0
	st	r1,	09600h[er4]

;;    if(color == 1)
CLINEA 0000H 0001H 00C0H 0005H 0012H
	cmp	r10,	#01h
	bne	_$L81
CBLOCK 8 6 193

;;    	deref(byteIndex + vram1) |= (0x80>>bitIndex);
CLINEA 0000H 0001H 00C2H 0006H 0032H
	mov	r0,	#080h
	mov	r1,	#00h
	mov	r2,	r3
_$M19 :
	cmp	r2,	#07h
	ble	_$M18
	srlc	r0,	#07h
	sra	r1,	#07h
	add	r2,	#0f9h
	bne	_$M19
_$M18 :
	srlc	r0,	r2
	l	r1,	09000h[er4]
	or	r1,	r0
	st	r1,	09000h[er4]
CBLOCKEND 8 6 195

;;    }
CLINEA 0000H 0000H 00C3H 0005H 0005H
_$L81 :

;;    if(color > 1)
CLINEA 0000H 0001H 00C4H 0005H 0011H
	cmp	r10,	#01h
	ble	_$L83
CBLOCK 8 7 197

;;    	deref(byteIndex + vram2) |= (0x80>>bitIndex);
CLINEA 0000H 0001H 00C6H 0006H 0032H
	mov	r0,	#080h
	mov	r1,	#00h
	mov	r2,	r3
_$M21 :
	cmp	r2,	#07h
	ble	_$M20
	srlc	r0,	#07h
	sra	r1,	#07h
	add	r2,	#0f9h
	bne	_$M21
_$M20 :
	srlc	r0,	r2
	l	r1,	09600h[er4]
	or	r1,	r0
	st	r1,	09600h[er4]
CBLOCKEND 8 7 199

;;    }
CLINEA 0000H 0000H 00C7H 0005H 0005H
_$L83 :

;;    if(color == 3)
CLINEA 0000H 0001H 00C8H 0005H 0012H
	cmp	r10,	#03h
	bne	_$L85
CBLOCK 8 8 201

;;        deref(byteIndex + vram1) |= (0x80>>bitIndex);
CLINEA 0000H 0001H 00CAH 0009H 0035H
	mov	r0,	#080h
	mov	r1,	#00h
	mov	r2,	r3
_$M23 :
	cmp	r2,	#07h
	ble	_$M22
	srlc	r0,	#07h
	sra	r1,	#07h
	add	r2,	#0f9h
	bne	_$M23
_$M22 :
	srlc	r0,	r2
	l	r1,	09000h[er4]
	or	r1,	r0
	st	r1,	09000h[er4]
CBLOCKEND 8 8 203

;;    }
CLINEA 0000H 0000H 00CBH 0005H 0005H
_$L85 :
CBLOCKEND 8 2 204

;;}
CLINEA 0000H 0001H 00CCH 0001H 0001H
_$L74 :
	pop	er4
	pop	xr8
	rt
CBLOCKEND 8 1 204
CFUNCTIONEND 8


	rseg $$plot_line$io
CFUNCTION 10

_plot_line	:
CBLOCK 10 1 207

;;{
CLINEA 0000H 0001H 00CFH 0001H 0001H
	push	lr
	push	fp
	mov	fp,	sp
	add	sp,	#-06
	push	xr8
	push	xr4
	push	bp
	mov	er8,	er0
	mov	er10,	er2
CBLOCK 10 2 207
CRET 0012H
CARGUMENT 47H 0002H 0028H "x0" 02H 00H 01H
CARGUMENT 47H 0002H 0029H "y0" 02H 00H 01H
CARGUMENT 43H 0002H 0006H "x1" 02H 00H 01H
CARGUMENT 43H 0002H 0008H "y1" 02H 00H 01H
CARGUMENT 42H 0001H 000AH "color" 02H 00H 00H
CLOCAL 47H 0002H 0026H 0002H "dx" 02H 00H 01H
CLOCAL 43H 0002H 0002H 0002H "sx" 02H 00H 01H
CLOCAL 47H 0002H 0027H 0002H "dy" 02H 00H 01H
CLOCAL 43H 0002H 0006H 0002H "sy" 02H 00H 01H
CLOCAL 47H 0002H 002AH 0002H "err" 02H 00H 01H
CLOCAL 43H 0002H 0004H 0002H "e2" 02H 00H 01H

;;  int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
CLINEA 0000H 0001H 00D0H 0003H 0031H
	l	er0,	6[fp]
	sub	r0,	r8
	subc	r1,	r9
	bl	_abs
	mov	er4,	er0
	l	er2,	6[fp]
	cmp	er8,	er2
	bges	_$L88
	mov	er0,	#1 
	bal	_$L90
_$L88 :
	mov	er0,	#-1
_$L90 :
	st	er0,	-2[fp]

;;  int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
CLINEA 0000H 0001H 00D1H 0003H 0031H
	l	er0,	8[fp]
	sub	r0,	r10
	subc	r1,	r11
	bl	_abs
	mov	er2,	er0
	mov	er0,	#0 
	sub	r0,	r2
	subc	r1,	r3
	mov	er6,	er0
	l	er2,	8[fp]
	cmp	er10,	er2
	bges	_$L91
	mov	er0,	#1 
	bal	_$L93
_$L91 :
	mov	er0,	#-1
_$L93 :
	st	er0,	-6[fp]

;;  int err = dx + dy, e2;
CLINEA 0000H 0001H 00D2H 0003H 0018H
	mov	er0,	er4
	add	er0,	er6
	mov	bp,	er0

;;  for (;;){
CLINEA 0000H 0000H 00D4H 0003H 000BH
_$L96 :
CBLOCK 10 3 212

;;    set_pixel (x0,y0,color);
CLINEA 0000H 0001H 00D5H 0005H 001CH
	l	r2,	10[fp]
	mov	r1,	r10
	mov	r0,	r8
	bl	_set_pixel

;;    if (x0 == x1 && y0 == y1) break;
CLINEA 0000H 0001H 00D6H 0005H 0024H
	l	er2,	6[fp]
	cmp	er8,	er2
	bne	_$L100
	l	er2,	8[fp]
	cmp	er10,	er2
	bne	_$L100
CBLOCKEND 10 2 219

;;}
CLINEA 0000H 0001H 00DBH 0001H 0001H
	pop	bp
	pop	xr4
	pop	xr8
	mov	sp,	fp
	pop	fp
	pop	pc

;;    if (x0 == x1 && y0 == y1) break;
CLINEA 0000H 0000H 00D6H 0005H 0024H
_$L100 :

;;    e2 = 2 * err;
CLINEA 0000H 0001H 00D7H 0005H 0011H
	mov	er0,	bp
	add	er0,	bp
	st	er0,	-4[fp]

;;    if (e2 >= dy) { err += dy; x0 += sx; }
CLINEA 0000H 0001H 00D8H 0005H 002AH
	cmp	er0,	er6
	blts	_$L107
CBLOCK 10 4 216
	add	bp,	er6
	l	er2,	-2[fp]
	add	er8,	er2
CBLOCKEND 10 4 216
_$L107 :

;;    if (e2 <= dx) { err += dx; y0 += sy; }
CLINEA 0000H 0001H 00D9H 0005H 002AH
	l	er0,	-4[fp]
	cmp	er0,	er4
	bgts	_$L96
CBLOCK 10 5 217
	add	bp,	er4
	l	er2,	-6[fp]
	add	er10,	er2
CBLOCKEND 10 5 217
CBLOCKEND 10 3 218
	bal	_$L96

;;  }
CLINEA 0000H 0000H 00DAH 0003H 0003H
CBLOCKEND 10 1 219
CFUNCTIONEND 10


	rseg $$CheckButtons$io
CFUNCTION 11

_CheckButtons	:
CBLOCK 11 1 225

;;{
CLINEA 0000H 0001H 00E1H 0001H 0001H
CBLOCK 11 2 225
CLOCAL 46H 0001H 0017H 0002H "x" 02H 00H 00H
CLOCAL 46H 0001H 0015H 0002H "y" 02H 00H 00H
CLOCAL 46H 0001H 0016H 0002H "i" 02H 00H 00H

;;	byte i = 0;
CLINEA 0000H 0001H 00E4H 0002H 000CH
	mov	r2,	#00h

;;	for(x = 0x80; x != 0; x = x >> 1)
CLINEA 0000H 0001H 00E5H 0002H 0022H
	mov	r3,	#080h
_$L114 :
CBLOCK 11 3 230

;;		deref(0xf046) = x;
CLINEA 0000H 0001H 00E7H 0003H 0014H
	st	r3,	0f046h

;;		for(y = 0x80; y != 0; y = y >> 1)
CLINEA 0000H 0001H 00E8H 0003H 0023H
	mov	r1,	#080h
_$L120 :
CBLOCK 11 4 233

;;			if((deref(0xf040) & y) == 0)
CLINEA 0000H 0001H 00EAH 0004H 001FH
	l	r0,	0f040h
	and	r0,	r1
	bne	_$L124
CBLOCK 11 5 235

;;				if(i != lastbutton)
CLINEA 0000H 0001H 00ECH 0005H 0017H
	l	r0,	NEAR _lastbutton
	cmp	r0,	r2
	beq	_$L126
CBLOCK 11 6 237

;;					lastbutton = i;
CLINEA 0000H 0001H 00EEH 0006H 0014H
	mov	r0,	r2
	st	r2,	NEAR _lastbutton

;;					return i;
CLINEA 0000H 0001H 00EFH 0006H 000EH
CBLOCKEND 11 6 240
CBLOCKEND 11 2 248

;;}
CLINEA 0000H 0001H 00F8H 0001H 0001H
	rt

;;				}
CLINEA 0000H 0000H 00F0H 0005H 0005H
_$L126 :

;;				return 0xff;
CLINEA 0000H 0001H 00F1H 0005H 0010H
	mov	r0,	#0ffh
	rt
CBLOCKEND 11 5 242

;;			}
CLINEA 0000H 0000H 00F2H 0004H 0004H
_$L124 :

;;			++i;
CLINEA 0000H 0001H 00F3H 0004H 0007H
	add	r2,	#01h
CBLOCKEND 11 4 244

;;		for(y = 0x80; y != 0; y = y >> 1)
CLINEA 0000H 0000H 00E8H 0003H 0023H
	srl	r1,	#01h
	mov	r0,	r1

;;		for(y = 0x80; y != 0; y = y >> 1)
CLINEA 0000H 0000H 00E8H 000EH 0017H
	bne	_$L120
CBLOCKEND 11 3 245

;;	for(x = 0x80; x != 0; x = x >> 1)
CLINEA 0000H 0000H 00E5H 0002H 0022H
	srl	r3,	#01h
	mov	r0,	r3

;;	for(x = 0x80; x != 0; x = x >> 1)
CLINEA 0000H 0000H 00E5H 000EH 0017H
	bne	_$L114

;;	lastbutton = 0x50;
CLINEA 0000H 0001H 00F6H 0002H 0013H
	mov	r0,	#050h
	st	r0,	NEAR _lastbutton

;;	return 0xff;
CLINEA 0000H 0001H 00F7H 0002H 000DH
	mov	r0,	#0ffh
	rt
CBLOCKEND 11 1 248
CFUNCTIONEND 11

	public _set_pixel
	public _CheckButtons
	public _render
	public _image_raw
	public _lastbutton
	public _plot_line
	public _drawbitmap
	public _button_to_char_abc
	public _print
	public _button_to_char_alt
	public _charprint1
	public _charprint2
	public _button_to_char
	public _draw_byte1
	public _draw_byte2
	extrn code far : _abs
	extrn code far : _main

	rseg $$NINITTAB
	db	0ffh

	rseg $$TABimage_raw$io
_image_raw :
	db	055h
	db	0aah
	db	055h
	db	0aah
	db	055h
	db	0aah
	db	055h
	db	0aah
	db	055h
	db	0aah
	db	055h
	db	0aah
	db	055h
	db	0aah
	db	055h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	0ffh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ffh
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	0ffh
	db	0aah
	db	054h
	db	0aah
	db	054h
	db	0aah
	db	054h
	db	0aah
	db	054h
	db	0aah
	db	054h
	db	0aah
	db	054h
	db	0aah
	db	054h
	db	00h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	0ffh
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	0ffh
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ffh
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	0ffh
	db	0ffh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ffh
	db	0ffh
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	0ffh
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	0ffh
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	0ffh
	db	0ffh
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	0ffh
	db	0ffh
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	081h
	db	0ffh
	db	01h
	db	01h
	db	02h
	db	02h
	db	04h
	db	04h
	db	08h
	db	08h
	db	010h
	db	010h
	db	020h
	db	020h
	db	040h
	db	040h
	db	080h
	db	080h
	db	080h
	db	080h
	db	040h
	db	040h
	db	020h
	db	020h
	db	010h
	db	010h
	db	08h
	db	08h
	db	04h
	db	04h
	db	02h
	db	02h
	db	01h
	db	01h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0fh
	db	0fh
	db	0fh
	db	0fh
	db	0fh
	db	0fh
	db	0fh
	db	0fh
	db	0f0h
	db	0f0h
	db	0f0h
	db	0f0h
	db	0f0h
	db	0f0h
	db	0f0h
	db	0f0h
	db	0ffh
	db	0aah
	db	055h
	db	0aah
	db	055h
	db	0aah
	db	055h
	db	0aah
	db	055h
	db	0aah
	db	055h
	db	0aah
	db	055h
	db	0aah
	db	055h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	081h
	db	081h
	db	081h
	db	081h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	07eh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	07eh
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ffh
	db	0aah
	db	0d5h
	db	0aah
	db	0d5h
	db	0aah
	db	0d5h
	db	0aah
	db	0d5h
	db	0aah
	db	0d5h
	db	0aah
	db	0d5h
	db	0aah
	db	0d5h
	db	080h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	080h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	01h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	080h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	01h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ffh
	db	0ffh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	00h
	db	08h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	022h
	db	022h
	db	022h
	db	022h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	012h
	db	012h
	db	012h
	db	07eh
	db	024h
	db	024h
	db	07eh
	db	048h
	db	048h
	db	048h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	08h
	db	03eh
	db	049h
	db	048h
	db	038h
	db	0eh
	db	09h
	db	049h
	db	03eh
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	031h
	db	04ah
	db	04ah
	db	034h
	db	08h
	db	08h
	db	016h
	db	029h
	db	029h
	db	046h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	01ch
	db	022h
	db	022h
	db	014h
	db	018h
	db	029h
	db	045h
	db	042h
	db	046h
	db	039h
	db	00h
	db	00h
	db	00h
	db	00h
	db	08h
	db	08h
	db	08h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	04h
	db	08h
	db	08h
	db	010h
	db	010h
	db	010h
	db	010h
	db	010h
	db	010h
	db	08h
	db	08h
	db	04h
	db	00h
	db	00h
	db	00h
	db	00h
	db	020h
	db	010h
	db	010h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	010h
	db	010h
	db	020h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	08h
	db	049h
	db	02ah
	db	01ch
	db	02ah
	db	049h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	08h
	db	08h
	db	08h
	db	07fh
	db	08h
	db	08h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	08h
	db	08h
	db	010h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	018h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	02h
	db	02h
	db	04h
	db	08h
	db	08h
	db	010h
	db	010h
	db	020h
	db	040h
	db	040h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	024h
	db	042h
	db	046h
	db	04ah
	db	052h
	db	062h
	db	042h
	db	024h
	db	018h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	08h
	db	018h
	db	028h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	03eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	02h
	db	0ch
	db	010h
	db	020h
	db	040h
	db	040h
	db	07eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	02h
	db	01ch
	db	02h
	db	02h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	04h
	db	0ch
	db	014h
	db	024h
	db	044h
	db	044h
	db	07eh
	db	04h
	db	04h
	db	04h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	07eh
	db	040h
	db	040h
	db	040h
	db	07ch
	db	02h
	db	02h
	db	02h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	01ch
	db	020h
	db	040h
	db	040h
	db	07ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	07eh
	db	02h
	db	02h
	db	04h
	db	04h
	db	04h
	db	08h
	db	08h
	db	08h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	03eh
	db	02h
	db	02h
	db	02h
	db	04h
	db	038h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	018h
	db	00h
	db	00h
	db	00h
	db	018h
	db	018h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	018h
	db	00h
	db	00h
	db	00h
	db	018h
	db	08h
	db	08h
	db	010h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	02h
	db	04h
	db	08h
	db	010h
	db	020h
	db	010h
	db	08h
	db	04h
	db	02h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	07eh
	db	00h
	db	00h
	db	00h
	db	07eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	040h
	db	020h
	db	010h
	db	08h
	db	04h
	db	08h
	db	010h
	db	020h
	db	040h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	02h
	db	04h
	db	08h
	db	08h
	db	00h
	db	08h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	01ch
	db	022h
	db	04ah
	db	056h
	db	052h
	db	052h
	db	052h
	db	04eh
	db	020h
	db	01eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	024h
	db	024h
	db	042h
	db	042h
	db	07eh
	db	042h
	db	042h
	db	042h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	07ch
	db	042h
	db	042h
	db	042h
	db	07ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	07ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	040h
	db	040h
	db	040h
	db	040h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	078h
	db	044h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	044h
	db	078h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	07eh
	db	040h
	db	040h
	db	040h
	db	07ch
	db	040h
	db	040h
	db	040h
	db	040h
	db	07eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	07eh
	db	040h
	db	040h
	db	040h
	db	07ch
	db	040h
	db	040h
	db	040h
	db	040h
	db	040h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	040h
	db	040h
	db	04eh
	db	042h
	db	042h
	db	046h
	db	03ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	07eh
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03eh
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	03eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	01fh
	db	04h
	db	04h
	db	04h
	db	04h
	db	04h
	db	04h
	db	044h
	db	044h
	db	038h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	044h
	db	048h
	db	050h
	db	060h
	db	060h
	db	050h
	db	048h
	db	044h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	040h
	db	040h
	db	040h
	db	040h
	db	040h
	db	040h
	db	040h
	db	040h
	db	040h
	db	07eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	042h
	db	066h
	db	066h
	db	05ah
	db	05ah
	db	042h
	db	042h
	db	042h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	062h
	db	062h
	db	052h
	db	052h
	db	04ah
	db	04ah
	db	046h
	db	046h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	07ch
	db	042h
	db	042h
	db	042h
	db	07ch
	db	040h
	db	040h
	db	040h
	db	040h
	db	040h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	05ah
	db	066h
	db	03ch
	db	03h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	07ch
	db	042h
	db	042h
	db	042h
	db	07ch
	db	048h
	db	044h
	db	044h
	db	042h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	040h
	db	030h
	db	0ch
	db	02h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	07fh
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	041h
	db	041h
	db	041h
	db	022h
	db	022h
	db	022h
	db	014h
	db	014h
	db	08h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	05ah
	db	05ah
	db	066h
	db	066h
	db	042h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	042h
	db	024h
	db	024h
	db	018h
	db	018h
	db	024h
	db	024h
	db	042h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	041h
	db	041h
	db	022h
	db	022h
	db	014h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	07eh
	db	02h
	db	02h
	db	04h
	db	08h
	db	010h
	db	020h
	db	040h
	db	040h
	db	07eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0eh
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	0eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	040h
	db	040h
	db	020h
	db	010h
	db	010h
	db	08h
	db	08h
	db	04h
	db	02h
	db	02h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	070h
	db	010h
	db	010h
	db	010h
	db	010h
	db	010h
	db	010h
	db	010h
	db	010h
	db	010h
	db	010h
	db	070h
	db	00h
	db	00h
	db	00h
	db	018h
	db	024h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	07fh
	db	00h
	db	00h
	db	020h
	db	010h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	02h
	db	03eh
	db	042h
	db	042h
	db	046h
	db	03ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	040h
	db	040h
	db	040h
	db	05ch
	db	062h
	db	042h
	db	042h
	db	042h
	db	042h
	db	062h
	db	05ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	040h
	db	040h
	db	040h
	db	040h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	02h
	db	02h
	db	02h
	db	03ah
	db	046h
	db	042h
	db	042h
	db	042h
	db	042h
	db	046h
	db	03ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	07eh
	db	040h
	db	040h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ch
	db	010h
	db	010h
	db	010h
	db	07ch
	db	010h
	db	010h
	db	010h
	db	010h
	db	010h
	db	010h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	02h
	db	03ah
	db	044h
	db	044h
	db	044h
	db	038h
	db	020h
	db	03ch
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	040h
	db	040h
	db	040h
	db	05ch
	db	062h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	08h
	db	08h
	db	00h
	db	018h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	03eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	04h
	db	04h
	db	00h
	db	0ch
	db	04h
	db	04h
	db	04h
	db	04h
	db	04h
	db	04h
	db	04h
	db	048h
	db	030h
	db	00h
	db	00h
	db	00h
	db	040h
	db	040h
	db	040h
	db	044h
	db	048h
	db	050h
	db	060h
	db	050h
	db	048h
	db	044h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	03eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	076h
	db	049h
	db	049h
	db	049h
	db	049h
	db	049h
	db	049h
	db	049h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	05ch
	db	062h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	05ch
	db	062h
	db	042h
	db	042h
	db	042h
	db	042h
	db	062h
	db	05ch
	db	040h
	db	040h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ah
	db	046h
	db	042h
	db	042h
	db	042h
	db	042h
	db	046h
	db	03ah
	db	02h
	db	02h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	05ch
	db	062h
	db	042h
	db	040h
	db	040h
	db	040h
	db	040h
	db	040h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	040h
	db	030h
	db	0ch
	db	02h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	010h
	db	010h
	db	010h
	db	07ch
	db	010h
	db	010h
	db	010h
	db	010h
	db	010h
	db	0ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	046h
	db	03ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	024h
	db	024h
	db	024h
	db	018h
	db	018h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	041h
	db	049h
	db	049h
	db	049h
	db	049h
	db	049h
	db	049h
	db	036h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	042h
	db	024h
	db	018h
	db	018h
	db	024h
	db	042h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	026h
	db	01ah
	db	02h
	db	02h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	07eh
	db	02h
	db	04h
	db	08h
	db	010h
	db	020h
	db	040h
	db	07eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ch
	db	010h
	db	010h
	db	08h
	db	08h
	db	010h
	db	020h
	db	010h
	db	08h
	db	08h
	db	010h
	db	010h
	db	0ch
	db	00h
	db	00h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	00h
	db	00h
	db	00h
	db	030h
	db	08h
	db	08h
	db	010h
	db	010h
	db	08h
	db	04h
	db	08h
	db	010h
	db	010h
	db	08h
	db	08h
	db	030h
	db	00h
	db	00h
	db	00h
	db	031h
	db	049h
	db	046h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	01h
	db	01h
	db	03h
	db	03h
	db	07h
	db	07h
	db	0fh
	db	0fh
	db	01fh
	db	01fh
	db	03fh
	db	03fh
	db	07fh
	db	07fh
	db	0ffh
	db	00h
	db	080h
	db	080h
	db	0c0h
	db	0c0h
	db	0e0h
	db	0e0h
	db	0f0h
	db	0f0h
	db	0f8h
	db	0f8h
	db	0fch
	db	0fch
	db	0feh
	db	0feh
	db	0ffh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ffh
	db	0ffh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ffh
	db	0ffh
	db	0ffh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	01h
	db	03h
	db	07h
	db	0fh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	080h
	db	0c0h
	db	0e0h
	db	0f0h
	db	0f0h
	db	0e0h
	db	0c0h
	db	080h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0fh
	db	07h
	db	03h
	db	01h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	01fh
	db	01fh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	01fh
	db	01fh
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0f8h
	db	0f8h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	0f8h
	db	0f8h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	0ffh
	db	0ffh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	01fh
	db	01fh
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ffh
	db	0ffh
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	0f8h
	db	0f8h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	0ffh
	db	0ffh
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	018h
	db	0ffh
	db	00h
	db	07eh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	07eh
	db	00h
	db	0ffh
	db	0ffh
	db	00h
	db	00h
	db	00h
	db	00h
	db	07eh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	07eh
	db	00h
	db	0ffh
	db	0e0h
	db	018h
	db	04h
	db	02h
	db	02h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	01h
	db	02h
	db	02h
	db	04h
	db	018h
	db	0e0h
	db	07h
	db	018h
	db	020h
	db	040h
	db	040h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	080h
	db	040h
	db	040h
	db	020h
	db	018h
	db	07h
	db	0e0h
	db	0f8h
	db	0fch
	db	0feh
	db	0feh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0feh
	db	0feh
	db	0fch
	db	0f8h
	db	0e0h
	db	07h
	db	01fh
	db	03fh
	db	07fh
	db	07fh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	07fh
	db	07fh
	db	03fh
	db	01fh
	db	07h
	db	088h
	db	022h
	db	088h
	db	022h
	db	088h
	db	022h
	db	088h
	db	022h
	db	088h
	db	022h
	db	088h
	db	022h
	db	088h
	db	022h
	db	088h
	db	022h
	db	080h
	db	00h
	db	00h
	db	00h
	db	08h
	db	00h
	db	00h
	db	00h
	db	080h
	db	00h
	db	00h
	db	00h
	db	08h
	db	00h
	db	00h
	db	00h
	db	0e0h
	db	0f8h
	db	0fch
	db	0feh
	db	0feh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	07h
	db	01fh
	db	03fh
	db	07fh
	db	07fh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	07fh
	db	07fh
	db	03fh
	db	01fh
	db	07h
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0feh
	db	0feh
	db	0fch
	db	0f8h
	db	0e0h
	db	0c0h
	db	030h
	db	0ch
	db	03h
	db	0c0h
	db	030h
	db	0ch
	db	03h
	db	0c0h
	db	030h
	db	0ch
	db	03h
	db	0c0h
	db	030h
	db	0ch
	db	03h
	db	011h
	db	011h
	db	022h
	db	022h
	db	044h
	db	044h
	db	088h
	db	088h
	db	011h
	db	011h
	db	022h
	db	022h
	db	044h
	db	044h
	db	088h
	db	088h
	db	00h
	db	00h
	db	00h
	db	00h
	db	08h
	db	08h
	db	00h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	08h
	db	08h
	db	03eh
	db	049h
	db	048h
	db	048h
	db	049h
	db	03eh
	db	08h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	0eh
	db	010h
	db	010h
	db	010h
	db	07ch
	db	010h
	db	010h
	db	010h
	db	03eh
	db	061h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	03ch
	db	024h
	db	042h
	db	042h
	db	024h
	db	03ch
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	041h
	db	022h
	db	014h
	db	08h
	db	07fh
	db	08h
	db	07fh
	db	08h
	db	08h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	08h
	db	08h
	db	08h
	db	08h
	db	00h
	db	00h
	db	08h
	db	08h
	db	08h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	040h
	db	03ch
	db	042h
	db	042h
	db	03ch
	db	02h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	024h
	db	024h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	099h
	db	0a5h
	db	0a1h
	db	0a1h
	db	0a5h
	db	099h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	01ch
	db	02h
	db	01eh
	db	022h
	db	01eh
	db	00h
	db	03eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	012h
	db	012h
	db	024h
	db	024h
	db	048h
	db	024h
	db	024h
	db	012h
	db	012h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	07eh
	db	02h
	db	02h
	db	02h
	db	00h
	db	00h
	db	0ffh
	db	0aah
	db	0d4h
	db	0aah
	db	0d4h
	db	0aah
	db	0d4h
	db	0aah
	db	0d4h
	db	0aah
	db	0d4h
	db	0aah
	db	0d4h
	db	0aah
	db	0d4h
	db	080h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	0b9h
	db	0a5h
	db	0a5h
	db	0b9h
	db	0a9h
	db	0a5h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	07eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	024h
	db	024h
	db	018h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	08h
	db	08h
	db	08h
	db	07fh
	db	08h
	db	08h
	db	08h
	db	00h
	db	07fh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	038h
	db	044h
	db	04h
	db	018h
	db	020h
	db	040h
	db	07ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	038h
	db	044h
	db	04h
	db	038h
	db	04h
	db	044h
	db	038h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	04h
	db	08h
	db	010h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	066h
	db	059h
	db	040h
	db	080h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03fh
	db	07ah
	db	07ah
	db	07ah
	db	03ah
	db	0ah
	db	0ah
	db	0ah
	db	0ah
	db	0ah
	db	0ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	018h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	08h
	db	030h
	db	00h
	db	00h
	db	00h
	db	010h
	db	030h
	db	050h
	db	010h
	db	010h
	db	010h
	db	07ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	01ch
	db	022h
	db	022h
	db	022h
	db	01ch
	db	00h
	db	03eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	048h
	db	048h
	db	024h
	db	024h
	db	012h
	db	024h
	db	024h
	db	048h
	db	048h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	022h
	db	062h
	db	024h
	db	028h
	db	028h
	db	012h
	db	016h
	db	02ah
	db	04eh
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	022h
	db	062h
	db	024h
	db	028h
	db	028h
	db	014h
	db	01ah
	db	022h
	db	044h
	db	04eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	062h
	db	012h
	db	024h
	db	018h
	db	068h
	db	012h
	db	016h
	db	02ah
	db	04eh
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	010h
	db	010h
	db	00h
	db	010h
	db	010h
	db	020h
	db	040h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	030h
	db	0ch
	db	00h
	db	00h
	db	018h
	db	024h
	db	024h
	db	042h
	db	042h
	db	07eh
	db	042h
	db	042h
	db	042h
	db	042h
	db	00h
	db	00h
	db	0ch
	db	030h
	db	00h
	db	00h
	db	018h
	db	024h
	db	024h
	db	042h
	db	042h
	db	07eh
	db	042h
	db	042h
	db	042h
	db	042h
	db	00h
	db	00h
	db	018h
	db	024h
	db	00h
	db	00h
	db	018h
	db	024h
	db	024h
	db	042h
	db	042h
	db	07eh
	db	042h
	db	042h
	db	042h
	db	042h
	db	00h
	db	00h
	db	032h
	db	04ch
	db	00h
	db	00h
	db	018h
	db	024h
	db	024h
	db	042h
	db	042h
	db	07eh
	db	042h
	db	042h
	db	042h
	db	042h
	db	00h
	db	00h
	db	024h
	db	024h
	db	00h
	db	00h
	db	018h
	db	024h
	db	024h
	db	042h
	db	042h
	db	07eh
	db	042h
	db	042h
	db	042h
	db	042h
	db	00h
	db	00h
	db	018h
	db	024h
	db	018h
	db	00h
	db	018h
	db	024h
	db	024h
	db	042h
	db	042h
	db	07eh
	db	042h
	db	042h
	db	042h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	01fh
	db	028h
	db	048h
	db	048h
	db	07fh
	db	048h
	db	048h
	db	048h
	db	048h
	db	04fh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	040h
	db	040h
	db	040h
	db	040h
	db	042h
	db	042h
	db	03ch
	db	08h
	db	030h
	db	030h
	db	0ch
	db	00h
	db	00h
	db	07eh
	db	040h
	db	040h
	db	040h
	db	07ch
	db	040h
	db	040h
	db	040h
	db	040h
	db	07eh
	db	00h
	db	00h
	db	0ch
	db	030h
	db	00h
	db	00h
	db	07eh
	db	040h
	db	040h
	db	040h
	db	07ch
	db	040h
	db	040h
	db	040h
	db	040h
	db	07eh
	db	00h
	db	00h
	db	018h
	db	024h
	db	00h
	db	00h
	db	07eh
	db	040h
	db	040h
	db	040h
	db	07ch
	db	040h
	db	040h
	db	040h
	db	040h
	db	07eh
	db	00h
	db	00h
	db	024h
	db	024h
	db	00h
	db	00h
	db	07eh
	db	040h
	db	040h
	db	040h
	db	07ch
	db	040h
	db	040h
	db	040h
	db	040h
	db	07eh
	db	00h
	db	00h
	db	018h
	db	06h
	db	00h
	db	00h
	db	03eh
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	03eh
	db	00h
	db	00h
	db	0ch
	db	030h
	db	00h
	db	00h
	db	03eh
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	03eh
	db	00h
	db	00h
	db	018h
	db	024h
	db	00h
	db	00h
	db	03eh
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	03eh
	db	00h
	db	00h
	db	024h
	db	024h
	db	00h
	db	00h
	db	03eh
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	03eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	078h
	db	044h
	db	042h
	db	042h
	db	0f2h
	db	042h
	db	042h
	db	042h
	db	044h
	db	078h
	db	00h
	db	00h
	db	032h
	db	04ch
	db	00h
	db	00h
	db	042h
	db	062h
	db	062h
	db	052h
	db	052h
	db	04ah
	db	04ah
	db	046h
	db	046h
	db	042h
	db	00h
	db	00h
	db	030h
	db	0ch
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	0ch
	db	030h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	018h
	db	024h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	032h
	db	04ch
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	024h
	db	024h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	024h
	db	018h
	db	024h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	02h
	db	03ah
	db	044h
	db	046h
	db	04ah
	db	04ah
	db	052h
	db	052h
	db	062h
	db	022h
	db	05ch
	db	040h
	db	00h
	db	030h
	db	0ch
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	0ch
	db	030h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	018h
	db	024h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	024h
	db	024h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	0ch
	db	030h
	db	00h
	db	00h
	db	041h
	db	041h
	db	022h
	db	022h
	db	014h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	040h
	db	040h
	db	078h
	db	044h
	db	042h
	db	042h
	db	044h
	db	078h
	db	040h
	db	040h
	db	040h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	038h
	db	044h
	db	044h
	db	048h
	db	058h
	db	044h
	db	042h
	db	042h
	db	052h
	db	04ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	030h
	db	0ch
	db	00h
	db	00h
	db	03ch
	db	042h
	db	02h
	db	03eh
	db	042h
	db	042h
	db	046h
	db	03ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ch
	db	030h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	02h
	db	03eh
	db	042h
	db	042h
	db	046h
	db	03ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	024h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	02h
	db	03eh
	db	042h
	db	042h
	db	046h
	db	03ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	032h
	db	04ch
	db	00h
	db	00h
	db	03ch
	db	042h
	db	02h
	db	03eh
	db	042h
	db	042h
	db	046h
	db	03ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	024h
	db	024h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	02h
	db	03eh
	db	042h
	db	042h
	db	046h
	db	03ah
	db	00h
	db	00h
	db	00h
	db	018h
	db	024h
	db	018h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	02h
	db	03eh
	db	042h
	db	042h
	db	046h
	db	03ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03eh
	db	049h
	db	09h
	db	03fh
	db	048h
	db	048h
	db	049h
	db	03eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	040h
	db	040h
	db	040h
	db	040h
	db	042h
	db	03ch
	db	08h
	db	030h
	db	00h
	db	00h
	db	030h
	db	0ch
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	07eh
	db	040h
	db	040h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ch
	db	030h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	07eh
	db	040h
	db	040h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	024h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	07eh
	db	040h
	db	040h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	024h
	db	024h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	07eh
	db	040h
	db	040h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	030h
	db	0ch
	db	00h
	db	00h
	db	018h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	03eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ch
	db	030h
	db	00h
	db	00h
	db	018h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	03eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	024h
	db	00h
	db	00h
	db	018h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	03eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	024h
	db	024h
	db	00h
	db	00h
	db	018h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	08h
	db	03eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	032h
	db	0ch
	db	014h
	db	022h
	db	02h
	db	03eh
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	032h
	db	04ch
	db	00h
	db	00h
	db	05ch
	db	062h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	00h
	db	00h
	db	00h
	db	00h
	db	030h
	db	0ch
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ch
	db	030h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	024h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	032h
	db	04ch
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	024h
	db	024h
	db	00h
	db	00h
	db	03ch
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	00h
	db	00h
	db	07eh
	db	00h
	db	00h
	db	018h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	02h
	db	03ch
	db	046h
	db	04ah
	db	04ah
	db	052h
	db	052h
	db	062h
	db	03ch
	db	040h
	db	00h
	db	00h
	db	00h
	db	030h
	db	0ch
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	046h
	db	03ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ch
	db	030h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	046h
	db	03ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	018h
	db	024h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	046h
	db	03ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	024h
	db	024h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	046h
	db	03ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	0ch
	db	030h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	026h
	db	01ah
	db	02h
	db	02h
	db	03ch
	db	00h
	db	00h
	db	00h
	db	040h
	db	040h
	db	040h
	db	05ch
	db	062h
	db	042h
	db	042h
	db	042h
	db	042h
	db	062h
	db	05ch
	db	040h
	db	040h
	db	00h
	db	00h
	db	024h
	db	024h
	db	00h
	db	00h
	db	042h
	db	042h
	db	042h
	db	042h
	db	042h
	db	026h
	db	01ah
	db	02h
	db	02h
	db	03ch
	db	00h

	rseg $$TABbutton_to_char$io
_button_to_char :
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	030h
	db	020h
	db	02ch
	db	02eh
	db	0ah
	db	010h
	db	011h
	db	012h
	db	05dh
	db	029h
	db	00h
	db	00h
	db	00h
	db	00h
	db	019h
	db	01ah
	db	05bh
	db	028h
	db	08h
	db	023h
	db	025h
	db	020h
	db	021h
	db	022h
	db	027h
	db	02fh
	db	021h
	db	040h
	db	024h
	db	00h
	db	029h
	db	02ah
	db	03bh
	db	05ch
	db	039h
	db	036h
	db	033h
	db	030h
	db	031h
	db	02ah
	db	02dh
	db	03dh
	db	038h
	db	035h
	db	032h
	db	00h
	db	039h
	db	03ah
	db	09h
	db	060h
	db	037h
	db	034h
	db	031h

	rseg $$TABbutton_to_char_abc$io
_button_to_char_abc :
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	056h
	db	057h
	db	058h
	db	059h
	db	05ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	046h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	045h
	db	04bh
	db	050h
	db	055h
	db	00h
	db	00h
	db	00h
	db	00h
	db	044h
	db	04ah
	db	04fh
	db	054h
	db	00h
	db	00h
	db	00h
	db	00h
	db	043h
	db	049h
	db	04eh
	db	053h
	db	00h
	db	00h
	db	00h
	db	00h
	db	042h
	db	048h
	db	04dh
	db	052h
	db	00h
	db	00h
	db	00h
	db	00h
	db	041h
	db	047h
	db	04ch
	db	051h

	rseg $$TABbutton_to_char_alt$io
_button_to_char_alt :
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ch
	db	03eh
	db	00h
	db	00h
	db	00h
	db	03ch
	db	07dh
	db	05eh
	db	00h
	db	00h
	db	00h
	db	00h
	db	03bh
	db	00h
	db	07bh
	db	02ah
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	022h
	db	03fh
	db	00h
	db	00h
	db	00h
	db	00h
	db	03ah
	db	00h
	db	03ah
	db	07ch
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	026h
	db	05fh
	db	02bh
	db	00h
	db	00h
	db	00h
	db	00h
	db	00h
	db	03dh
	db	00h
	db	07eh
	db	00h
	db	00h
	db	00h

	rseg $$NINITVAR
_lastbutton :
	ds	01h

	rseg $$NINITTAB
	align

	rseg $$NINITVAR
	align

	end

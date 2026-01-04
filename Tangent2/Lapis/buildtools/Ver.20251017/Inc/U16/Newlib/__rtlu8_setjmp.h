/*============================================================================
-   setjmp.h

-   Definitions/Declarations for setjmp/longjmp routines.
	RTLU8 Original version.

-   Copyright (C) 2008-2011 LAPIS Semiconductor Co., Ltd.

============================================================================ */
#ifndef _SETJMP
#define _SETJMP

#ifndef _YVALS
#include <yvals.h>
#endif


		/* macros */
#ifdef __LAPISOMF__

#ifdef	__MS__
								/* Small Model		*/
//#asm
					/* subscript number of jmp_buf			*/
__asm("_SP_STOCK		EQU		0	\n");		/* save sp			*/
__asm("_FP_STOCK		EQU		2	\n");		/* save fp			*/
__asm("_PSW_STOCK		EQU		4	\n");		/* save psw			*/
__asm("_PC_STOCK		EQU		6	\n");		/* save return address		*/
__asm("_DSR_STOCK		EQU		8	\n");		/* save DSR			*/
__asm("_VAL_STOCK		EQU		10	\n");		/* save val			*/
__asm("_ER4_STOCK		EQU		12	\n");		/* save ER4			*/
__asm("_ER6_STOCK		EQU		14	\n");		/* save ER6			*/
__asm("_ER8_STOCK		EQU		16	\n");		/* save ER8			*/
__asm("_ER10_STOCK		EQU		18	\n");		/* save ER10		*/
__asm("_ER12_STOCK		EQU		20	\n");		/* save ER12		*/
//#endasm
#endif

#ifdef	__ML__
								/* Large Model		*/
//#asm
					/* subscript number of jmp_buf			*/
__asm("_SP_STOCK		EQU		0	\n");		/* save sp			*/
__asm("_FP_STOCK		EQU		2	\n");		/* save fp			*/
__asm("_PSW_STOCK		EQU		4	\n");		/* save psw			*/
__asm("_PC_STOCK		EQU		6	\n");		/* save return address		*/
__asm("_CSR_STOCK		EQU		8	\n");		/* save CSR			*/
__asm("_DSR_STOCK		EQU		10	\n");		/* save DSR			*/
__asm("_VAL_STOCK		EQU		12	\n");		/* save val			*/
__asm("_ER4_STOCK		EQU		14	\n");		/* save ER4			*/
__asm("_ER6_STOCK		EQU		16	\n");		/* save ER6			*/
__asm("_ER8_STOCK		EQU		18	\n");		/* save ER8			*/
__asm("_ER10_STOCK		EQU		20	\n");		/* save ER10		*/
__asm("_ER12_STOCK		EQU		22	\n");		/* save ER12		*/
//#endasm
#endif

#else

#ifdef	__MS__
								/* Small Model		*/
//#asm
					/* subscript number of jmp_buf			*/
__asm(".equ _SP_STOCK		,		0	\n");	/* save sp			*/
__asm(".equ _FP_STOCK		,		2	\n");	/* save fp			*/
__asm(".equ _PSW_STOCK		,		4	\n");	/* save psw			*/
__asm(".equ _PC_STOCK		,		6	\n");	/* save return address		*/
__asm(".equ _DSR_STOCK		,		8	\n");	/* save DSR			*/
__asm(".equ _VAL_STOCK		,		10	\n");	/* save val			*/
__asm(".equ _ER4_STOCK		,		12	\n");	/* save ER4			*/
__asm(".equ _ER6_STOCK		,		14	\n");	/* save ER6			*/
__asm(".equ _ER8_STOCK		,		16	\n");	/* save ER8			*/
__asm(".equ _ER10_STOCK		,		18	\n");	/* save ER10		*/
__asm(".equ _ER12_STOCK		,		20	\n");	/* save ER12		*/
//#endasm
#endif

#ifdef	__ML__
								/* Large Model		*/
//#asm
					/* subscript number of jmp_buf			*/
__asm(".equ _SP_STOCK		,		0	\n");	/* save sp			*/
__asm(".equ _FP_STOCK		,		2	\n");	/* save fp			*/
__asm(".equ _PSW_STOCK		,		4	\n");	/* save psw			*/
__asm(".equ _PC_STOCK		,		6	\n");	/* save return address		*/
__asm(".equ _CSR_STOCK		,		8	\n");	/* save CSR			*/
__asm(".equ _DSR_STOCK		,		10	\n");	/* save DSR			*/
__asm(".equ _VAL_STOCK		,		12	\n");	/* save val			*/
__asm(".equ _ER4_STOCK		,		14	\n");	/* save ER4			*/
__asm(".equ _ER6_STOCK		,		16	\n");	/* save ER6			*/
__asm(".equ _ER8_STOCK		,		18	\n");	/* save ER8			*/
__asm(".equ _ER10_STOCK		,		20	\n");	/* save ER10		*/
__asm(".equ _ER12_STOCK		,		22	\n");	/* save ER12		*/
//#endasm
#endif

#endif /* __LAPISOMF__ */

#ifdef __NEAR__
#define	 jmp_buf		jmp_buf_n
#define  setjmp(env)		_Setjmp_n(env)
#define  longjmp(env,ret)	longjmp_n(env,ret)
#endif

#ifdef __FAR__
#define	 jmp_buf		jmp_buf_f
#define  setjmp(env)		_Setjmp_f(env)
#define  longjmp(env,ret)	longjmp_f(env,ret)
#endif

		/* type definitions */
//typedef int __near jmp_buf_n[_NSETJMP];
typedef int jmp_buf_n[_NSETJMP];
#ifndef __NOFAR__
//typedef int __far jmp_buf_f[_NSETJMP];
typedef int jmp_buf_f[_NSETJMP];
#endif

		/* declarations */
//void	longjmp_n(jmp_buf_n, int);
void	longjmp_n(int __near *, int);
#ifndef __NOFAR__
//void	longjmp_f(jmp_buf_f, int);
void	longjmp_f(int __far *, int);
#endif
//int	_Setjmp_n(jmp_buf_n);
int	_Setjmp_n(int __near *);
#ifndef __NOFAR__
//int	_Setjmp_f(jmp_buf_f);
int	_Setjmp_f(int __far *);
#endif

		/* macro overrides */
#define setjmp_n(env)	_Setjmp_n(env)

#ifndef __NOFAR__
#define setjmp_f(env)	_Setjmp_f(env)
#endif

#endif

/* End of File */

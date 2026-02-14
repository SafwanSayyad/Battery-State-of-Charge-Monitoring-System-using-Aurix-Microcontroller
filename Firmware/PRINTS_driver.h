#ifndef _P_R_I_N_T_S__D_R_I_V_E_R__H
#define _P_R_I_N_T_S__D_R_I_V_E_R__H


#include<_Impl/IfxAsclin_cfg.h>
#include <Asclin/Asc/IfxAsclin_Asc.h>
#include <Asclin/Std/IfxAsclin.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
//#include <global.h>
#include "_Reg/IfxAsclin_reg.h"
#include <_PinMap/IfxAsclin_PinMap.h>


#define CARRIAGERETURN         0x0D
#define LINE_FEED              0x0A
#define ESCAPE_KEY             0x1B

#define beep()      put_char(0x07)
#define clreol()    print_f("\033[K")
#define clrscr()    print_f("\033[2J")
#define gotoxy(x,y) print_f("\033[%d;%dH",y,x)
#define newline()   put_char(0x0A)
#define PAR_TYP_DEC 0
#define PAR_TYP_HEX 1

#define SUCCESS 0
#define FAILURE	1

#define ASC_TX_BUFFER_SIZE 256
#define ASC_RX_BUFFER_SIZE 256

#define JAZZPRINTSCORE0 0x7000F500
#define JAZZPRINTSCORE1 0x6000F500
#define JAZZPRINTSCORE2 0x5000F500

#ifdef VERBOSE1
	#define PRINTLOGLEVEL  1
#elif VERBOSE2
	#define PRINTLOGLEVEL  2
#else
	#define PRINTLOGLEVEL  0
#endif
#ifdef PRINTSONJAZZTERMINAL
	#undef PRINTLOGLEVEL 
	#define PRINTLOGLEVEL  3
#endif
#define BLOCKED_TERMINAL   1

#define is_digit(c)        ((c >= '0') && (c <= '9'))


IFX_EXTERN void ASC_init( void );
IFX_EXTERN void usr_printf(unsigned short int verboseValue, const char *p_frm, ...);
IFX_EXTERN void buf_to_screen(char *text_buf);
IFX_EXTERN void put_char(char z);
IFX_EXTERN int usr_vsprintf(char *dest, const char *fmt, va_list ap);
IFX_EXTERN void terminalPuts(const char * buffer);

#endif /* _P_R_I_N_T_S__D_R_I_V_E_R__H */


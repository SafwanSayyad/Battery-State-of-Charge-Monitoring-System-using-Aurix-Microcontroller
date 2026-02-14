/****************************************************************************
* Copyright(c) INFINEON TECHNOLOGIES 2016 all rights reserved               *
*                                                                           *
* \group    IP PSV                                                          *
*                                                                           *
* \platform Aurix TC3xxx                                                    *
*                                                                           *
* \file     PRINTS_driver.c                                                 *
*                                                                           *
* \brief    ASC PRINT function                                              *
*           usage: usr_printf(Verbosex,"print the statement");              *
*                  Verbosex - x-> 1,2,3                                     *
*                  Ex. usr_printf(3,"print the statement");                 *
*           Verbose 1 - Print all the strings                               *
*           Verbose 2 - User defined required prints                        *
*           Verbose 3 - Only for Automation                                 *
*                                                                           *
* \author   Suchitra H                                                      *
*                                                                           *
* \version  Intial version 0.1                                              *
*                                                                           *
* \date     2016-02-10                                                      *
*                                                                           *
* Change history                                                            *
* Version no./Date    Author                    Description                 *
* ----------------    ---------------------     --------------------------  *
* 0.1 / 2016-02-10    Suchitra H                Initial development         *
*                                                                           *
*                                                                           *
****************************************************************************/

#include "PRINTS_driver.h"
#include "BugInjection.h"

char g_AsclintxBuffer[ASC_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
char g_AsclinrxBuffer[ASC_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];

//uint16 *g_asclinTxBuffer = NULL_PTR;
//uint16 *g_asclinRxBuffer = NULL_PTR;

void ASC_init( void )
{
	/* disable interrupts */
    boolean              interruptState = IfxCpu_disableInterrupts();

    /* create module config */
    IfxAsclin_Asc_Config asc0Config;
	IfxAsclin_Asc asc0; 
	asc0.asclin = &MODULE_ASCLIN0;
    IfxAsclin_Asc_initModuleConfig(&asc0Config, &MODULE_ASCLIN0);

	asc0Config.txBuffer     = g_AsclintxBuffer;
	asc0Config.txBuffer     = g_AsclintxBuffer;
    asc0Config.txBufferSize = ASC_TX_BUFFER_SIZE;

    asc0Config.baudrate.baudrate = 115200; //set this baudrate value in the teraterm too see the print statement
    asc0Config.rxBuffer     = g_AsclinrxBuffer;
    asc0Config.rxBufferSize = ASC_RX_BUFFER_SIZE;
    asc0Config.fifo.txFifoInterruptLevel = IfxAsclin_TxFifoInterruptLevel_1;  

    /* pin configuration */
    // Pin written as eg.
    // RX : &Ifxasclin0_RXA_P32_2_IN
    // TX: Infer from above

    //Hey there... Look for me, if you are not able to see the print statements. Probably the pin connection is wrong
    IfxAsclin_Rx_In  *rxPin = &IfxAsclin0_RXA_P14_1_IN;
    IfxAsclin_Tx_Out *txPin = &IfxAsclin0_TX_P14_0_OUT;

    const IfxAsclin_Asc_Pins pins = {
           NULL,    IfxPort_InputMode_pullUp,        /* CTS pin not used */
           rxPin, IfxPort_InputMode_pullUp,     /* Rx pin */
           NULL,    IfxPort_OutputMode_pushPull,     /* RTS pin not used */
           txPin, IfxPort_OutputMode_pushPull,     /* Tx pin */
           IfxPort_PadDriver_cmosAutomotiveSpeed1
        };

    asc0Config.pins = &pins;

    /* initialize module */
   // IfxAsclin_Asc_initModule(&MODULE_ASCLIN0, &ascConfig);
     IfxAsclin_Asc_initModule(&asc0,&asc0Config);
    /* enable interrupts again */
    IfxCpu_restoreInterrupts(interruptState);
}

/*******************************************************************************
** Syntax : terminalPutBlock                                                  **
** 2 modes                                                                    **
**   1 - BLOCKED_TERMINAL  - works with CW exchange                           **
**   2 - NONBLOCKED_TERMINAL - Read char by char in JAZZ                      **
*******************************************************************************/

void terminalPutBlock(const char * data, int size)
{
	volatile char * pout = NULL; 
	
	if( 0 == IfxCpu_getCoreId())       /* Core 0 */
		pout = (char *)JAZZPRINTSCORE0;
	else if ( 1 == IfxCpu_getCoreId()) /* Core 1 */
		pout = (char *)JAZZPRINTSCORE1;
	#if !defined(DERIVATIVE_TC26x)
	else if ( 2 == IfxCpu_getCoreId()) /* Core 2 */
		pout = (char *)JAZZPRINTSCORE2;
	#endif
	
	#ifdef BLOCKED_TERMINAL
    int i, j,k;

    //replace with if statement
    while (size > 0)
    {
        i = size;

        if ( i > 252)
        i = 252;

        if (*pout);    /* wait for ready */

        for (j = 0 , k=0; j < i; j++,k++)
        {
            pout[k+4] = data[j];

            if (data[j]==0xA)
            {
                pout[k+4] = 0xD;
                k+=1;
                pout[k+4] = 0xA;
            }
        }
        *pout = k;
        data += i;
        size -= i;
    }
    #else
    while (size > 0)
    {
        while (*pout);    /* wait for ready */
        *pout = *data;
        data++;
        size--;
    }
    #endif
}
/*******************************************************************************
** Syntax : terminalPuts(const char *p_frm, ...)                              **
**                                                                            **
** Parameters (in):                                                           **
**                                                                            **
** Return value: none                                                         **
**                                                                            **
** Description : This function Prints a formatted string to the               **
**               Communication Port.                                          **
*******************************************************************************/
void terminalPuts(const char * buffer)
{
    terminalPutBlock((const char *) buffer, strlen(buffer));
}
/*******************************************************************************
** formatted output into buffer                                               **
*******************************************************************************/

static int _cvt(unsigned long val, char *buf, long radix, char *digits)
{
    char temp[80];
    char *cp = temp;
    int length = 0;

    if (val == 0)
    {
        /* Special case */
        *cp++ = '0';
    }
    else
    {
        while (val)
        {
        *cp++ = digits[val % radix];
        val /= radix;
        }
    }
    while (cp != temp)
    {
        *buf++ = *--cp;
        length++;
    }
    *buf = '\0';
    return length;
}


/*******************************************************************************
** simplified formatted output into buffer                                    **
*******************************************************************************/
int usr_vsprintf(char *dest, const char *fmt, va_list ap)
{
    char c, sign, *cp, *dp = dest;
    int left_prec, right_prec, zero_fill, length, pad, pad_on_right;
    char buf[32];
    long val;

    while ((c = *fmt++) != 0)
    {
        cp = buf;
        length = 0;
        if (c == '%')
        {
            c = *fmt++;
            left_prec = right_prec = pad_on_right = 0;
            if (c == '-')
            {
                c = *fmt++;
                pad_on_right++;
            }
            if (c == '0')
            {
                zero_fill = TRUE;
                c = *fmt++;
            }
            else
            {
                zero_fill = FALSE;
            }
            while (is_digit(c))
            {
                left_prec = (left_prec * 10) + (c - '0');
                c = *fmt++;
            }
            if (c == '.')
            {
                c = *fmt++;
                zero_fill++;
                while (is_digit(c))
                {
                    right_prec = (right_prec * 10) + (c - '0');
                    c = *fmt++;
                }
            }
            else
            {
                right_prec = left_prec;
            }
            sign = '\0';
            /* handle type modifier */
            if (c == 'l' || c == 'h')
            {
                c = *fmt++;
            }
            switch (c)
            {
                case 'd' :
                case 'u' :
                case 'x' :
                case 'X' :
                    val = va_arg(ap, long);
                    switch (c)
                    {
                        case 'd' :
                        if (val < 0)
                        {
                            sign = '-';
                            val = -val;
                        }
                        /* fall through */
                        case 'u' :
                            length = _cvt(val, buf, 10, "0123456789");
                            break;
                        case 'x' :
                            length = _cvt(val, buf, 16, "0123456789abcdef");
                            break;
                        case 'X' :
                            length = _cvt(val, buf, 16, "0123456789ABCDEF");
                            break;
                    }
                    break;
                case 's' :
                    cp = va_arg(ap, char *);
                    length = strlen(cp);
                    break;
                case 'c' :
                    c = (char)va_arg(ap, long);
                    *dp++ = c;
                    continue;
                default:
                    *dp++ = '?';
            }
            pad = left_prec - length;
            if (sign != '\0')
            {
                pad--;
            }
            if (zero_fill)
            {
                c = '0';
                if (sign != '\0')
                {
                    *dp++ = sign;
                    sign = '\0';
                }
            }
            else
            {
                c = ' ';
            }
            if (!pad_on_right)
            {
                while (pad-- > 0)
                {
                    *dp++ = c;
                }
            }
            if (sign != '\0')
            {
                *dp++ = sign;
            }
            while (length-- > 0)
            {
                c = *cp++;
                if (c == '\n')
                {
                    *dp++ = '\r';
                }
                *dp++ = c;
            }
            if (pad_on_right)
            {
                while (pad-- > 0)
                {
                    *dp++= ' ';
                }
            }
        }
        else
        {
            if (c == '\n')
            {
                *dp++= '\r';
            }
            *dp++ = c;
        }
    }
    *dp = '\0';

    return ((int)dp - (int)dest);
}
/*******************************************************************************
** Syntax : void put_char(char z)                                             **
**                                                                            **
** InputParameters                                                            **
**                                                                            **
** Return value: none                                                         **
**                                                                            **
** Description : This function sends a character to Communication Port.       **
*******************************************************************************/
void put_char(char z)
{
  	//asclin_sfrs_t* asc_Regs;
	Ifx_ASCLIN      *asclinSFR = &MODULE_ASCLIN0 ;

    /* send character */
	 asclinSFR->FLAGSCLEAR.U = 0xFFFFFFFFU;
	 
	 //A2G
     asclinSFR->TXDATA.U = (uint32)z;
	 //asclinSFR->TXDATA[0].U = (uint32)z;

    if (z == LINE_FEED)
    {
        while ((asclinSFR->TXFIFOCON.B.FILL) != 0)
        {
            ;
        }
        /* send CR */
        asclinSFR->FLAGSCLEAR.U = 0xFFFFFFFF;
		
        //A2G
		asclinSFR->TXDATA.I = CARRIAGERETURN;
		//asclinSFR->TXDATA[0].I = CARRIAGERETURN;
    }
    while ( (asclinSFR->TXFIFOCON.B.FILL) != 0 )
    {
        ;
	}
}

/*******************************************************************************
** Syntax : void buf_to_screen(char *text_buf)                                **
**                                                                            **
** InputParameters                                                            **
**                                                                            **
** Return value: none                                                         **
**                                                                            **
** Description : This function holds the String to be buffered to the         **
**               Communication Port.                                          **
*******************************************************************************/

void buf_to_screen(char *text_buf)
{
    while (*text_buf != 0)
    {
        /* send character */
		 put_char(*text_buf);
		 text_buf++;
    }
}

void usr_printf(unsigned short int verboseValue, const char *p_frm, ...)
{
    va_list p_arg;
	
    if((PRINTLOGLEVEL <= verboseValue) || (verboseValue == 3))
	{
		va_start(p_arg, p_frm);

		/* Optimized code */
		usr_vsprintf(g_AsclintxBuffer,p_frm,p_arg);

		va_end(p_arg);

		#ifndef PRINTSONJAZZTERMINAL
			buf_to_screen(g_AsclintxBuffer);
		#endif
		#ifdef PRINTSONJAZZTERMINAL
			terminalPuts(g_AsclintxBuffer);
		#endif
		
	}
}

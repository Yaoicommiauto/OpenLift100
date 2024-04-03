/*
*********************************************************************************************************
*                                     Embedded Systems Building Blocks
*                                  Complete and Ready-to-Use Modules in C
*
*                                    Asynchronous Serial Communications
*                                          Buffered Serial I/O
*                                     (Foreground/Background Systems)
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : COMMBGND.C
* Programmer : Jean J. Labrosse
*
* Notes      : The functions (actually macros) OS_ENTER_CRITICAL() and OS_EXIT_CRITICAL() are used to
*              disable and enable interrupts, respectively.  If using the Borland C++ compiler V3.1,
*              all you need to do is to define these macros as follows:
*
*                   #define OS_ENTER_CRITICAL()  disable()
*                   #define OS_EXIT_CRITICAL()   enable()
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*/
#define  COMM_GLOBALS
#include "includes.h"

/*$PAGE*/
/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

/*$PAGE*/


void     CommPutRxChar(INT8U c);
INT8U    CommGetTxChar(void);

INT8U    CommGetChar(void);
INT8U    CommPutChar(INT8U c);

unsigned char   status_temp;                  /*   image of sp_stat */

INT8U  RingBufRxInPtr;                 /* Pointer to where next character will be inserted        */
INT8U  RingBufRxOutPtr;                /* Pointer from where next character will be extracted     */
INT8U  RingBufTxInPtr;                 /* Pointer to where next character will be inserted        */
INT8U  RingBufTxOutPtr;                /* Pointer from where next character will be extracted     */

/*
*********************************************************************************************************
*                                    REMOVE CHARACTER FROM RING BUFFER
*
*
* Description : This function is called by your application to obtain a character from the communications
*               channel.
* Arguments   : 'ch'    is the COMM port channel number and can either be:
*                           COMM1
*                           COMM2
*               'err'   is a pointer to where an error code will be placed:
*                           *err is set to COMM_NO_ERR   if a character is available
*                           *err is set to COMM_RX_EMPTY if the Rx buffer is empty
*                           *err is set to COMM_BAD_CH   if you have specified an invalid channel
* Returns     : The character in the buffer (or NUL if the buffer is empty)
*********************************************************************************************************
*/

INT8U  CommGetChar (void)
{
    INT8U          c;


    if (CommBuf.RingBufRxCtr > 0) {                          /* See if buffer is empty                   */
        CommBuf.RingBufRxCtr--;                              /* No, decrement character count            */
        
        c = CommBuf.RingBufRx[RingBufRxOutPtr];                      /* Get character from buffer                */
        RingBufRxOutPtr++;        
        RingBufRxOutPtr %= COMM_RX_BUF_SIZE;        

        COMM_ERR = COMM_NO_ERR;
        return (c);
    } else {
        COMM_ERR = COMM_RX_EMPTY;
        c    = NUL;                                        /* Buffer is empty, return NUL              */
        return (c);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                  GET TX CHARACTER FROM RING BUFFER
*
*
* Description : This function is called by the Tx ISR to extract the next character from the Tx buffer.
*               The function returns FALSE if the buffer is empty after the character is extracted from
*               the buffer.  This is done to signal the Tx ISR to disable interrupts because this is the
*               last character to send.
* Arguments   : 'ch'    is the COMM port channel number and can either be:
*                           COMM1
*                           COMM2
*               'err'   is a pointer to where an error code will be deposited:
*                           *err is set to COMM_NO_ERR         if at least one character was left in the
*                                                              buffer.
*                           *err is set to COMM_TX_EMPTY       if the Tx buffer is empty.
*                           *err is set to COMM_BAD_CH         if you have specified an incorrect channel
* Returns     : The next character in the Tx buffer or NUL if the buffer is empty.
*********************************************************************************************************
*/

 INT8U  CommGetTxChar (void)
{
    INT8U c;

    if (CommBuf.RingBufTxCtr > 0) {                          /* See if buffer is empty                   */
        CommBuf.RingBufTxCtr--;                              /* No, decrement character count            */
        c = CommBuf.RingBufTx[RingBufTxOutPtr];                      /* Get character from buffer                */
        RingBufTxOutPtr++;
        RingBufTxOutPtr %= COMM_TX_BUF_SIZE;

        COMM_ERR = COMM_NO_ERR;
        return (c);                                        /* Characters are still available           */
    } else {
        COMM_ERR = COMM_TX_EMPTY;
		RingBufTxOutPtr;
		CommBuf.RingBufTxCtr = 0;
        return (NUL);                                      /* Buffer is empty                          */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                  INITIALIZE COMMUNICATIONS MODULE
*
*
* Description : This function is called by your application to initialize the communications module.  You
*               must call this function before calling any other functions.
* Arguments   : none
*********************************************************************************************************
*/

void  CommInit (void)
{

    unsigned char save_wsr = wsr;

    wsr = 0;
    baud_rate = ((unsigned char) BAUD_REG);             /* Write baud_rate twice, LS byte first */
    baud_rate = ((unsigned char) (BAUD_REG >> 8));      /* Next MS byte */
    sp_con = 0x09;                                      /* mode 1, receive enabled, * no 9th bit */
    status_temp = sp_stat | TI_BIT;

    CommBuf.RingBufRxCtr    = 0;
    RingBufRxInPtr  = 0;
    RingBufRxOutPtr = 0;
    
    CommBuf.RingBufTxCtr    = 0;
    RingBufTxInPtr  = 0;
    RingBufTxOutPtr = 0;

    EN_RI;
    EN_TI;
    DI_SER;
    wsr = save_wsr;                                     /* restore wsr */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                  SEE IF RX CHARACTER BUFFER IS EMPTY
*
*
* Description : This function is called by your application to see if any character is available from the
*               communications channel.  If at least one character is available, the function returns
*               FALSE otherwise, the function returns TRUE.
* Arguments   : 'ch'    is the COMM port channel number and can either be:
*                           COMM1
*                           COMM2
* Returns     : TRUE    if the buffer IS empty.
*               FALSE   if the buffer IS NOT empty or you have specified an incorrect channel
*********************************************************************************************************
*/

BOOLEAN  CommIsEmpty (void)
{
    BOOLEAN       empty;

    if (CommBuf.RingBufRxCtr > 0) {                        /* See if buffer is empty                   */
        empty = FALSE;                                     /* Buffer is NOT empty                      */
    } 
    else {
        empty = TRUE;                                      /* Buffer is empty                          */
    }


    return (empty);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                   SEE IF TX CHARACTER BUFFER IS FULL
*
*
* Description : This function is called by your application to see if any more characters can be placed
*               in the Tx buffer.  In other words, this function check to see if the Tx buffer is full.
*               If the buffer is full, the function returns TRUE otherwise, the function returns FALSE.
* Arguments   : 'ch'    is the COMM port channel number and can either be:
*                           COMM1
*                           COMM2
* Returns     : TRUE    if the buffer IS full.
*               FALSE   if the buffer IS NOT full or you have specified an incorrect channel
*********************************************************************************************************
*/

BOOLEAN  CommIsFull (void)
{
    BOOLEAN        full;

    if (CommBuf.RingBufTxCtr < COMM_TX_BUF_SIZE) {           /* See if buffer is full                    */
        full = FALSE;                                      /* Buffer is NOT full                       */
    } else {
        full = TRUE;                                       /* Buffer is full                           */
    }


    return (full);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                            OUTPUT CHARACTER
*
*
* Description : This function is called by your application to send a character on the communications
*               channel.  The character to send is first inserted into the Tx buffer and will be sent by
*               the Tx ISR.  If this is the first character placed into the buffer, the Tx ISR will be
*               enabled.  If the Tx buffer is full, the character will not be sent (i.e. it will be lost)
* Arguments   : 'ch'    is the COMM port channel number and can either be:
*                           COMM1
*                           COMM2
*               'c'     is the character to send.
* Returns     : COMM_NO_ERR   if the function was successful (the buffer was not full)
*               COMM_TX_FULL  if the buffer was full
*               COMM_BAD_CH   if you have specified an incorrect channel
*********************************************************************************************************
*/

INT8U  CommPutChar ( INT8U c)
{

    if (CommBuf.RingBufTxCtr < COMM_TX_BUF_SIZE) {           /* See if buffer is full                    */
        CommBuf.RingBufTxCtr++;                              /* No, increment character count            */
        CommBuf.RingBufTx[RingBufTxInPtr] = c;                       /* Put character into buffer                */
        
        RingBufTxInPtr++;
  		RingBufTxInPtr %= COMM_TX_BUF_SIZE;

        if (status_temp & TI_BIT)
            int_pend1 |= 0x01;                              /*  If transmitt buffer transmitting.  */
        
//        if (CommBuf.RingBufTxCtr = 1)                      /* See if this is the first character       */
  //          EN_TI;                       

        return (COMM_NO_ERR);
    } 
    else {
        return (COMM_TX_FULL);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                    INSERT CHARACTER INTO RING BUFFER
*
*
* Description : This function is called by the Rx ISR to insert a character into the receive ring buffer.
* Arguments   : 'ch'    is the COMM port channel number and can either be:
*                           COMM1
*                           COMM2
*               'c'     is the character to insert into the ring buffer.  If the buffer is full, the
*                       character will not be inserted, it will be lost.
*********************************************************************************************************
*/

void  CommPutRxChar (INT8U c)
{

    if (CommBuf.RingBufRxCtr < COMM_RX_BUF_SIZE) {           /* See if buffer is full                    */
        CommBuf.RingBufRxCtr++;                              /* No, increment character count            */
        CommBuf.RingBufRx[RingBufRxInPtr] = c;                       /* Put character into buffer                */
        
        RingBufRxInPtr++;
        RingBufRxInPtr %= COMM_RX_BUF_SIZE;
    }
}


void CommTxCharISR(void)
{
    INT8U   c;
    wsr = 0;       
    status_temp |= sp_stat;                             /* image sp_stat into status_temp  */

    
	c = CommGetTxChar();             /* Get next character to send.               */
	if(COMM_ERR != COMM_TX_EMPTY) {              /* Do we have anymore characters to send ?   */
		sbuf = c;            /* transmit character  */
   	 	status_temp &= ~TI_BIT;                         /*  clear TI bit in status_temp.   */
	}
}

/*  serial interrupt routine  */
/*  see pragma interrupt above */
void CommRxISR(void)
{
    INT8U   c;
    wsr = 0;                                            /* Disable vertical windowing */
    status_temp |= sp_stat;                             /* image sp_stat into status_temp  */

 	c= sbuf;
    CommPutRxChar(c);                    /* Insert received character into buffer     */

    if(status_temp & FE_BIT){
 		status_temp &= ~FE_BIT;
   	} 
    if(status_temp & OE_BIT){
		status_temp &= ~OE_BIT;
   	} 
    status_temp &= ~RI_BIT;                             /*  clear RI bit in status_temp.  */
}

int putch(int c){
  return (int) CommPutChar((int) c);
}

int getch(void){
  return (int) CommGetChar();
}

void SetCommDir(INT8U d)
{
	CommPtr = 0;
	if(d == DIR_OUT){ 		// send
		CommDir = DIR_OUT;
		DI_RI;
		//EN_TI;
	}
	else if(d == DIR_IN){
		CommDir = DIR_IN;
		EN_RI;
		//DI_TI;
	}
}
void CommFlush(void)
{
	DI_RI;
    CommBuf.RingBufRxCtr    = 0;
    RingBufRxInPtr  = 0;
    RingBufRxOutPtr = 0;
    
    CommBuf.RingBufTxCtr    = 0;
    RingBufTxInPtr  = 0;
    RingBufTxOutPtr = 0;
	EN_RI;
}

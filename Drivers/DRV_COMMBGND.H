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
* Filename   : COMMBGND.H
* Programmer : Jean J. Labrosse
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       CONFIGURATION CONSTANTS
*********************************************************************************************************
*/

#ifndef  CFG_H

#define  COMM_RX_BUF_SIZE    20                /* Number of characters in Rx ring buffer             */
#define  COMM_TX_BUF_SIZE    20               /* Number of characters in Tx ring buffer             */

#endif

#ifdef   COMM_GLOBALS
   #define  COMM_EXT
#else
   #define  COMM_EXT  extern
#endif

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

/* declare interrupt routines */
#pragma interrupt(CommRxISR = 25, CommTxCharISR = 24)


#define FREQUENCY       16000000L       /* 16 MHz  */
#define BAUD_RATE_VALUE 19200
#define BAUD_REG        ((unsigned int)(FREQUENCY/((long)BAUD_RATE_VALUE*16)-1)+0x8000)

#define RI_BIT          0x40
#define TI_BIT          0x20

#define	OE_BIT			0x04
#define	FE_BIT			0x10

#define TI_EN           0x01
#define RI_EN           0x02

#define SER_EN          0x40


#define EN_RI  (int_mask1 |= RI_EN);  
#define DI_RI  (int_mask1 &= ~RI_EN);  

#define EN_TI  (int_mask1 |= TI_EN);  
#define DI_TI  (int_mask1 &= ~TI_EN);  

#define EN_SER (int_mask |= SER_EN);  
#define DI_SER (int_mask &= ~SER_EN);  

#ifndef  NUL
#define  NUL                 0x00
#endif

                                                 /* ERROR CODES                                        */
#define  COMM_NO_ERR            0                /* Function call was successful                       */
//#define  COMM_BAD_CH            1                /* Invalid communications port channel                */
#define  COMM_RX_EMPTY          2                /* Rx buffer is empty, no character available         */
#define  COMM_TX_FULL           3                /* Tx buffer is full, could not deposit character     */
#define  COMM_TX_EMPTY          4                /* If the Tx buffer is empty.                         */


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/


typedef struct {
    INT8U  RingBufRxCtr;                   /* Number of characters in the Rx ring buffer              */
    INT8U  RingBufRx[COMM_RX_BUF_SIZE];    /* Ring buffer character storage (Rx)                      */
    INT8U  RingBufTxCtr;                   /* Number of characters in the Tx ring buffer              */
    INT8U  RingBufTx[COMM_TX_BUF_SIZE];    /* Ring buffer character storage (Tx)                      */
} COMM_RING_BUF;

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

COMM_EXT COMM_RING_BUF  CommBuf;
COMM_EXT INT8U          COMM_ERR;


#define DIR_IN			0
#define DIR_OUT			1
COMM_EXT	INT8U	CommDir;


/*$PAGE*/
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

int      getch(void);
int      putch(int c);

void     CommInit(void);

BOOLEAN  CommIsEmpty(void);
BOOLEAN  CommIsFull(void);
void 	 SetCommDir(INT8U d);
INT8U    CommGetChar(void);
INT8U    CommPutChar(INT8U c);
void 	CommFlush(void);

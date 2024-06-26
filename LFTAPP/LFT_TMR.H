/*
*********************************************************************************************************
*                                  Embedded Systems Building Blocks
*                               Complete and Ready-to-Use Modules in C
*
*                                             Timer Manager
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : TMR.H
* Programmer : Jean J. Labrosse
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#ifndef  CFG_H

#define  TMR_DLY_TICKS        2		//(OS_TICKS_PER_SEC / 10)
#define  TMR_TASK_PRIO        45

#define  TMR_MAX_TMR          2

#define  TMR_USE_SEM          0

#endif

#ifdef  TMR_GLOBALS
#define TMR_EXT
#else
#define TMR_EXT  extern
#endif

/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

typedef struct tmr {                             /* TIMER DATA STRUCTURE                               */
    BOOLEAN   TmrEn;                             /* Flag indicating whether timer is enabled           */
    INT16U    TmrCtr;                            /* Current value of timer (counting down)             */
    INT16U    TmrInit;                           /* Initial value of timer (i.e. when timer is set)    */
    void    (*TmrFnct)(void *);                  /* Function to execute when timer times out           */
    void     *TmrFnctArg;                        /* Arguments supplied to user defined function        */
//    BOOLEAN   TmrEvent;   
}TMR;
TMR_EXT  TMR	TmrTbl[TMR_MAX_TMR];          /* Table of timers managed by this module             */

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/


void    TmrFormat(INT8U n, char *s);

void    TmrInit(void);

void    TmrCfgFnct(INT8U n, void (*fnct)(void *), void *arg);
void    TmrSetMST(INT8U n, INT8U min, INT8U sec, INT8U tenths);
void    TmrSignalTmr(void);

INT16U  TmrChk(INT8U n);
void    TmrReset(INT8U n);
void    TmrSetT(INT8U n, INT16U tenths);
void    TmrStart(INT8U n);
void    TmrStop(INT8U n);



void  TmrTask (void);


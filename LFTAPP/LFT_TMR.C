/*
*********************************************************************************************************
*                                  Embedded Systems Building Blocks
*                               Complete and Ready-to-Use Modules in C
*
*                                            Timer Manager
*
*                           (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                          All Rights Reserved
*
* Filename   : TMR.C
* Programmer : Jean J. Labrosse
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           INCLUDE FILES
*********************************************************************************************************
*/

#define  TMR_GLOBALS
#include "includes.h"

/*
*********************************************************************************************************
*                                          LOCAL VARIABLES
*********************************************************************************************************
*/
/*
static  OS_EVENT   *TmrSemTenths;
static  OS_STK      TmrTaskStk[TMR_TASK_STK_SIZE];
*/
static  INT8U       TmrTickCtr;

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

//static  void        TmrTask(void *data);

/*$PAGE*/

/*
*********************************************************************************************************
*                                   CONFIGURE TIMER TIMEOUT FUNCTION
*
* Description : Set the user defined function when the timer expires.
* Arguments   : n         is the timer number 0..TMR_MAX_TMR-1
*               fnct      is a pointer to a function that will be executed when the timer expires
*               arg       is a pointer to an argument that is passed to 'fnct'
* Returns     : None.
*********************************************************************************************************
*/

void  TmrCfgFnct(INT8U n, void (*fnct)(void *), void *arg)
{

    if (n < TMR_MAX_TMR) {
        OS_ENTER_CRITICAL();
        TmrTbl[n].TmrFnct    = fnct;                 /* Store pointer to user function into timer          */
        TmrTbl[n].TmrFnctArg = arg;                  /* Store user's function arguments pointer            */
        OS_EXIT_CRITICAL();
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             CHECK TIMER
*
* Description : This function checks to see if a timer has expired
* Arguments   : n      is the timer to check
* Returns     : 0      if the timer has expired
*               TmrCtr the remaining time before the timer expires in 1/10 second
*********************************************************************************************************
*/

INT16U  TmrChk (INT8U n)
{
    INT16U val;


    val = 0;
    if (n < TMR_MAX_TMR) {
        OS_ENTER_CRITICAL();
        val = TmrTbl[n].TmrCtr;
        OS_EXIT_CRITICAL();
    }
    return (val);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       FORMAT TIMER INTO STRING
*
* Description : Formats a timer into an ASCII string.
* Arguments   : n      is the desired timer
*               s      is a pointer to the destination string.  The destination string must be large
*                      enough to hold the formatted timer value which will have the following format:
*                          "MM:SS.T"
*********************************************************************************************************
*/

void  TmrFormat (INT8U n, char *s)
{
    INT8U   min;
    INT8U   sec;
    INT8U   tenths;
    INT16U  val;


    if (n < TMR_MAX_TMR) {
        OS_ENTER_CRITICAL();
        val    = TmrTbl[n].TmrCtr;                    /* Get local copy of timer for conversion        */
        OS_EXIT_CRITICAL();
        min    = (INT8U)(val / 600);
        sec    = (INT8U)((val - min * 600) / 10);
        tenths = (INT8U)(val % 10);
        s[0]   = min / 10 + '0';                      /* Convert TIMER to ASCII                        */
        s[1]   = min % 10 + '0';
        s[2]   = ':';
        s[3]   = sec / 10 + '0';
        s[4]   = sec % 10 + '0';
        s[5]   = '.';
        s[6]   = tenths   + '0';
        s[7]   = 0x00;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                     TIMER MANAGER INITIALIZATION
*
* Description : This function initializes the timer manager module.
* Arguments   : None
* Returns     : None.
*********************************************************************************************************
*/

void  TmrInit (void)
{
    INT8U  err;
    INT8U  i;

    err  = err;
    for (i = 0; i < TMR_MAX_TMR; i++) {          /* Clear and disable all timers                       */
        TmrTbl[i].TmrEn   = FALSE;
        TmrTbl[i].TmrCtr  = 0;
        TmrTbl[i].TmrInit = 0;
        TmrTbl[i].TmrFnct = NULL;
    }
    TmrTickCtr   = 0;
    
   /*
    TmrSemTenths = OSSemCreate(0);               /* Create counting semaphore to signal 1/10 second    */
    
   /* OSTaskCreateExt(TmrTask, 
                   (void *)0, 
                   &TmrTaskStk[TMR_TASK_STK_SIZE], 
                   TMR_TASK_PRIO,
                   TMR_TASK_ID,  
                   &TmrTaskStk[0],  
                   TMR_TASK_STK_SIZE,  
                   (void *)0,  
                   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
*/
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             RESET TIMER
*
* Description : This function reloads a timer with its initial value
* Arguments   : n         is the timer to reset
* Returns     : None.
*********************************************************************************************************
*/

void  TmrReset (INT8U n)
{

    if (n < TMR_MAX_TMR) {
        OS_ENTER_CRITICAL();
        TmrTbl[n].TmrCtr = TmrTbl[n].TmrInit;       /* Reload the counter                                      */
        OS_EXIT_CRITICAL();
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                        SET TIMER (SPECIFYING MINUTES, SECONDS and TENTHS)
*
* Description : Set the timer with the specified number of minutes, seconds and 1/10 seconds.  The
*               function converts the minutes, seconds and tenths into tenths.
* Arguments   : n         is the timer number 0..TMR_MAX_TMR-1
*               min       is the number of minutes
*               sec       is the number of seconds
*               tenths    is the number of tenths of a second
* Returns     : None.
*********************************************************************************************************
*/

void  TmrSetMST (INT8U n, INT8U min, INT8U sec, INT8U tenths)
{
    INT16U  val;

    if (n < TMR_MAX_TMR) {
        val = (INT16U)min * 600 + (INT16U)sec * 10 + (INT16U)tenths;
        OS_ENTER_CRITICAL();
        TmrTbl[n].TmrInit = val;
        TmrTbl[n].TmrCtr  = val;
        OS_EXIT_CRITICAL();
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                               SET TIMER (SPECIFYING TENTHS OF SECOND)
*
* Description : Set the timer with the specified number of 1/10 seconds.
* Arguments   : n         is the timer number 0..TMR_MAX_TMR-1
*               tenths    is the number of 1/10 second to load into the timer
* Returns     : None.
*********************************************************************************************************
*/

void  TmrSetT (INT8U n, INT16U tenths)
{

    if (n < TMR_MAX_TMR) {
        OS_ENTER_CRITICAL();
        TmrTbl[n].TmrInit = tenths;
        TmrTbl[n].TmrCtr  = tenths;
        OS_EXIT_CRITICAL();
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                     SIGNAL TIMER MANAGER MODULE THAT A 'CLOCK TICK' HAS OCCURRED
*
* Description : This function is called by the 'clock tick' ISR on every tick.  This function is thus
*               responsible for counting the number of clock ticks per 1/10 second.  When 1/10 second
*               elapses, this function will signal the timer manager task.
* Arguments   : None.
* Returns     : None.
* Notes       : TMR_DLY_TICKS must be set to produce 1/10 second delays.  
*               This can be set to OS_TICKS_PER_SEC / 10 if you use uC/OS-II.
*********************************************************************************************************
*/

/*
void  TmrSignalTmr (void)
{
    TmrTickCtr++;
    if (TmrTickCtr >= TMR_DLY_TICKS) {
        TmrTickCtr = 0;
        OSSemPost(TmrSemTenths);
    }
}
*/

/*$PAGE*/
/*
*********************************************************************************************************
*                                             START TIMER
*
* Description : This function start a timer
* Arguments   : n         is the timer to start
* Returns     : None.
*********************************************************************************************************
*/

void  TmrStart (INT8U n)
{
    if (n < TMR_MAX_TMR) {
        OS_ENTER_CRITICAL();
        TmrTbl[n].TmrEn = TRUE;
        OS_EXIT_CRITICAL();
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             STOP TIMER
*
* Description : This function stops a timer
* Arguments   : n         is the timer to stop
* Returns     : None.
*********************************************************************************************************
*/

void  TmrStop (INT8U n)
{
    if (n < TMR_MAX_TMR) {
        OS_ENTER_CRITICAL();
        TmrTbl[n].TmrEn = FALSE;
        OS_EXIT_CRITICAL();
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          TIMER MANAGER TASK
*
* Description : This task is created by TmrInit() and is responsible for updating the timers.
*               TmrTask() executes every 1/10 of a second.
* Arguments   : None.
* Returns     : None.
* Note(s)     : 1) The function to execute when a timer times out is executed outside the critical
*                  section.
*********************************************************************************************************
*/

void  TmrTask (void)
{
    INT8U   i;
    void  (*pfnct)(void *);                           /* Function to execute when timer times out      */
    void   *parg;                                     /* Arguments to pass to above function           */
    
    for (i = 0; i < TMR_MAX_TMR; i++) {
        OS_ENTER_CRITICAL();
        if (TmrTbl[i].TmrEn == TRUE) {                /* Decrement timer only if it is enabled         */
            if (TmrTbl[i].TmrCtr > 0) {
                TmrTbl[i].TmrCtr--;
                if (TmrTbl[i].TmrCtr == 0) {               /* See if timer expired                     */
                    TmrTbl[i].TmrEn = FALSE;               /* Yes, stop timer                          */
                    pfnct = TmrTbl[i].TmrFnct;       /* Get pointer to function to execute ...   */
                    parg  = TmrTbl[i].TmrFnctArg;    /* ... and its argument                     */
                }
            }
        }
        OS_EXIT_CRITICAL();
        if (pfnct != (void (*)(void *))0) {       /* See if we need to execute function for ...    */
            (*pfnct)(parg);                       /* ... timed out timer.                          */
            pfnct = (void (*)(void *))0;
        }
    }
}




/*
*********************************************************************************************************
*                    SJA1000 CAN standlone controller driver routines for CANKing
*                          
*                                                       
*                                                                 
*                           Author:           
*                           Contact: 
*                           Version: 1.0                                     
*                           Modification date:                               
*                           Description:                                     
* This is the header file for memory mapping definition of the SJA1000 and compilance 
* with...
*********************************************************************************************************
*/
#define  CAN_GLOBALS
#include "includes.h"
INT8U *PCAN_Addr;
typedef struct	// general structure of 8 bit register allowing bit access 
	{
	unsigned  bit0 : 1;
	unsigned  bit1 : 1;
	unsigned  bit2 : 1;
	unsigned  bit3 : 1;
	unsigned  bit4 : 1;
 	unsigned  bit5 : 1;
	unsigned  bit6 : 1;
	unsigned  bit7 : 1;
	} Register;    

typedef union	// allow bit or byte access to registers 
	{
	char byte;
	Register bits;
	} Mix_Reg;
const INT16U PCAN_Base[2] = {0x300, 0x400};

const INT8U SJA_BTR_CODETAB[2][2] = {		// 0 = 12M 1 = 16M
//    0x08E, 0x0DC,		// 20k	12M 2016m
//    0x08F, 0x0C8,    	// 25k	12M	1182m
    0x089, 0x0C8,		// 40k	12M 728m
//    0x087, 0x0D7,		// 50k	12M	455m
//    0x084, 0x0C8,		// 80k	12M 349m
//    0x084, 0x0C5,		// 100k	12M 122m
//    0x083, 0x0C5,		// 125k	12M 91m

//    0x093, 0x0CD,		// 20k  16M 2243m 
//    0x08F, 0x0EB,    	// 25K	16M 1425m
    0x089, 0x0EB,		// 40k  16M 879m
//    0x087, 0x0EB,		// 50k  16M 697m
//    0x084, 0x0EB,		// 80k  16M 425m
//    0x083, 0x0EB,		// 100k 16M 334m
//    0x083, 0x0E7,		// 125k 16M 152m
};


static void PCAN_Overrun_ISR(char chan);
static void PCAN_Error_ISR(char chan);


/*$PAGE*/
/*
*********************************************************************************************************
* FUNCTION PROTOTYPE:bit BCAN_Enter_RetMode ( char channelNumber )
*
* DESCRIPTION:       this function is used to let the CAN Controller enter the RESET Mode.
*
* INPUT:             NONE
*
* RETURN:            0:    Enter RESET Mode successfully
*                    1:    FAILED
*********************************************************************************************************
*/
INT8U canEnterRet(INT8U chan)
{
    INT8U *BaseAddr;
    INT16U i;
    INT8U TempData, errflag;

    // Reset the circuit... (Reset Mode)
    BaseAddr = (INT8U *) PCAN_Base[chan];
    i = TestTimes;
    errflag = 1;
    while(i--){
        PCAN_Addr = BaseAddr + PCAN_MOD;
        TempData = *PCAN_Addr;
        *PCAN_Addr = (TempData | PCAN_RM);
        if ((*PCAN_Addr & PCAN_RM) == PCAN_RM){
             errflag = 0;
             break;
        }
    }
//    if (errflag)
        ////// // // //printf(" Enter Reset Failure!");

     return errflag;
}
/*
*********************************************************************************************************
* FUNCTION PROTOTYPE:bit BCAN_Quit_RetMode ( char channelNumber )
*
* DESCRIPTION:       this function is used to let the CAN Controller quit the RESET Mode.
*
* INPUT:             NONE
*
* RETURN:            0:    Quit RESET Mode successfully
*                    1:    FAILED
*********************************************************************************************************
*/
char canQuitRet ( INT8U channelNumber )
{
    INT8U *BaseAddr;
    INT16U i;
    INT8U TempData, errflag;

    BaseAddr = (INT8U *) PCAN_Base[channelNumber];
    i = TestTimes;
    errflag = 1;
    while(i--){
        PCAN_Addr = BaseAddr + PCAN_MOD;
        TempData = *PCAN_Addr;
        *PCAN_Addr = (TempData & ~PCAN_RM);
        if ((*PCAN_Addr & PCAN_RM) == 0){
             errflag = 0;
             break;
        }
    }

    return errflag;
}

/*$PAGE*/
/*
*********************************************************************************************************
* FUNCTION PROTOTYPE:bit BCAN_SetBaud_Rate(INT8U CAN_ByteRate)
*
* DESCRIPTION:       settint the baud rate of SJA1000 and OSC is 16MHZ.
*                    this routine must run in RESET MODE
*
* RETURN:            0:    OK
*                    1:    FAIL
*********************************************************************************************************
*/
char canSetBaudRate(INT16U chan, INT8U CAN_ByteRate)
{
   INT8U btr0, btr1;

   INT8U *BaseAddr;
   INT8U TempData, tmp;		   

    btr0 = SJA_BTR_CODETAB[chan][CAN_ByteRate *2];
    btr1 = SJA_BTR_CODETAB[chan][CAN_ByteRate *2 + 1];

    BaseAddr = (INT8U *)PCAN_Base[chan];
    // Put the circuit in Reset Mode
    canEnterRet(chan);

    PCAN_Addr = BaseAddr + PCAN_BTR0;
	*PCAN_Addr = btr0;                  // BTR0
    PCAN_Addr = BaseAddr + PCAN_BTR1;
    *PCAN_Addr = btr1;                  // BTR1

/*
    PCAN_Addr = BaseAddr + PCAN_BTR0;
    *PCAN_Addr = 0x8f;
    PCAN_Addr = BaseAddr + PCAN_BTR1;
    if(chan == 0)
        *PCAN_Addr = 0xC8;
    else
        *PCAN_Addr = 0xEB;
*/
       
	canQuitRet(chan);
    return 0;
}

/*
*********************************************************************************************************
* FUNCTION PROTOTYPE: char caSetOutClk(INT8U Out_Control, INT8U Clock_Out)
*
* DESCRIPTION:       settint the Out Mode & Clock Division.
*                    this routine must run in RESET MODE
*
* INPUT:             Out_Control: the OC register setting 
*                    Clock_Out: the CDR register setting 
*
*                    this routine must run in RESET MODE
*
* RETURN:            0:    OK
*                    1:    FAIL
*********************************************************************************************************
*/
char caSetOutClk(INT16U chan, INT8U Out_Control, INT8U Clock_Out)
{
	
    INT8U *BaseAddr,i;
    INT8U reset;
		   
    BaseAddr = (INT8U *)PCAN_Base[chan];
    // ...goto Pelican mode...
    PCAN_Addr = BaseAddr + PCAN_CDR;
    *PCAN_Addr = Clock_Out;

    // Output conrol
    PCAN_Addr = BaseAddr + PCAN_OCR;
    *PCAN_Addr = Out_Control;

    return 0;
}

/*$PAGE*/
/*
*********************************************************************************************************
*                          
* char canOpenChannel(INT8U channelNumber,  INT16U flags);
*
* Description
* Opens a CAN channel (circuit) and returns a handle which is used in subsequent calls to CANLIB. 
*********************************************************************************************************
*/
/*
    CAN_CONTROL = 1;
    CALL TIME(1);

    CAN_CLOCK = 6EH;  
	
    CAN_COMMAND = 0;
    CAN_ACR = 0;
    CAN_ACM = 0FFH;
    CAN_BUS_TIMING0 = 8FH;  
    CAN_BUS_TIMING1 = 0C8H; 
    CAN_OUTPUT_CONTROL = 0DAH; 
    CAN_TEST = 0;
    CAN_CONTROL = 1EH; 
    INIT_CAN_REQ = 0;
*/
char canOpenChannel(INT8U channelNumber)
{
	
    INT8U *BaseAddr,i;
    INT8U reset;
		   
    BaseAddr = (INT8U *)PCAN_Base[channelNumber];
       
    if(canEnterRet(channelNumber))
        return -1;  			//ERR_OPENCAN; 
        
    // ...goto Pelican mode...
    PCAN_Addr = BaseAddr + PCAN_CDR;
    *PCAN_Addr = PCAN_PELICAN|PCAN_CBP|PCAN_CLOCKOFF;

    // ...and set the filter mode.
    PCAN_Addr = BaseAddr + PCAN_MOD;
    *PCAN_Addr |= PCAN_AFM;
    
    // Activate almost all interrupt sources.
    PCAN_Addr = BaseAddr + PCAN_IER;
    *PCAN_Addr =  PCAN_BEIE|PCAN_EPIE|PCAN_DOIE|PCAN_EIE|PCAN_RIE | PCAN_TIE|PCAN_RIE;
//    *PCAN_Addr =  PCAN_RIE;

    PCAN_Addr = BaseAddr + PCAN_ACR0;
    *PCAN_Addr =  0;
    PCAN_Addr = BaseAddr + PCAN_ACR1;
    *PCAN_Addr =  0;
    PCAN_Addr = BaseAddr + PCAN_ACR2;
    *PCAN_Addr =  0;
    PCAN_Addr = BaseAddr + PCAN_ACR3;
    *PCAN_Addr =  0;

    PCAN_Addr = BaseAddr + PCAN_AMR0;
    *PCAN_Addr =  0x0FF;                                        // all Not Care
    PCAN_Addr = BaseAddr + PCAN_AMR1;
    *PCAN_Addr =  0x0FF;
    PCAN_Addr = BaseAddr + PCAN_AMR2;
    *PCAN_Addr =  0x0FF;
    PCAN_Addr = BaseAddr + PCAN_AMR3;
    *PCAN_Addr =  0x0FF;
   
    canSetBaudRate(channelNumber, ByteRate_40k);
    // Output conrol
    PCAN_Addr = BaseAddr + PCAN_OCR;
    *PCAN_Addr = 0x0DA;
    
    canQuitRet(channelNumber);
    return 0; //PCAN_OK;
}

/*
*********************************************************************************************************
*                          
* canStatus canClose(char handle);
*
* Description
* Closes the channel associated with the handle. If no other threads are using the circuit, it is taken off bus. The handle can not be used for further references to the channel. 
*********************************************************************************************************
*/

char  canCloseChannel(INT8U channelNumber)
{
    INT8U  *BaseAddr;
		   
    BaseAddr =(INT8U *) PCAN_Base[channelNumber];
    
    PCAN_Addr  = BaseAddr + PCAN_MOD;
    *PCAN_Addr = PCAN_RM;
    PCAN_Addr  = BaseAddr + PCAN_CDR;
    *PCAN_Addr = PCAN_CBP|PCAN_CLOCKOFF;
    PCAN_Addr  = BaseAddr + PCAN_MOD;
    *PCAN_Addr = 0x21;

    return 0;// PCAN_OK;
}

/*
*********************************************************************************************************
* FUNCTION PROTOTYPE:static void ResetErrorCounter (INT8U channelNumber)
*
* DESCRIPTION:       Enable bus error interrupts, and reset the counters 
*                    which keep track of the error rate
*
* INPUT:             
*
* RETURN:            
*********************************************************************************************************
*/
static void ResetErrorCounter (INT8U channelNumber)
{
	
    INT8U *BaseAddr;
    INT8U TempData;		   

    BaseAddr = (INT8U *) PCAN_Base[channelNumber];

    PCAN_Addr = BaseAddr + PCAN_IER;
    TempData = *PCAN_Addr;
    *PCAN_Addr = TempData | PCAN_BEIE;
}
/*$PAGE*/
/*
*********************************************************************************************************
* Go on bus.
* canStatus canBusOn(char handle);
* Description:  Takes the specified circuit on-bus. 
* Input Parameters 
* handle : An open handle to a CAN circuit.
* Output Parameters
* None. 
* Return Value
* canOK (zero) if success
* canERR_xxx (negative) if failure.
*********************************************************************************************************
 */
char canBusOn(INT8U channelNumber)
{
	
    INT8U *BaseAddr;
    INT8U TempData, tmp;		   

    BaseAddr = (INT8U *) PCAN_Base[channelNumber];

//    ARGUSED(e);
//    pCtx->C[chan].overrun = 0;

    ResetErrorCounter(channelNumber);
    
    // Go on bus
    PCAN_Addr = BaseAddr + PCAN_MOD;
    TempData = *PCAN_Addr;

    PCAN_Addr = BaseAddr + PCAN_CMR;
    *PCAN_Addr = PCAN_CDO;                       // Clear Data-Overrun 
      
    PCAN_Addr = BaseAddr + PCAN_MOD;
    *PCAN_Addr = TempData|PCAN_RM;               

    PCAN_Addr = BaseAddr + PCAN_TXERR;
    *PCAN_Addr = 0;

    PCAN_Addr = BaseAddr + PCAN_RXERR;
    *PCAN_Addr = 0;

    PCAN_Addr = BaseAddr + PCAN_ECC;
    tmp = *PCAN_Addr;                     // Read Error Capture to clear it

    PCAN_Addr = BaseAddr + PCAN_MOD;
    *PCAN_Addr = TempData & ~PCAN_RM;
   // PCC1000_GetChipState(pCtx, chan, NULL);
    
    return 0; //PCAN_OK;
}

/*
*********************************************************************************************************
* Go off bus.
* canStatus canBusOff(char handle);
* Description: Takes the specified circuit off-bus. 
* Input Parameters
* handle 
* An open handle to a CAN circuit.
* Output Parameters
* None. 
*
* Return Value
* canOK (zero) if success
* canERR_xxx (negative) if failure.
*********************************************************************************************************
*/
char canBusOff(INT8U channelNumber)
{
	
    INT8U *BaseAddr;
    INT8U TempData, tmp;		   

    BaseAddr = (INT8U *)PCAN_Base[channelNumber];

    PCAN_Addr = BaseAddr + PCAN_MOD;
    TempData = *PCAN_Addr;

    *PCAN_Addr = TempData|PCAN_RM;

//    PCC1000_GetChipState(pCtx, chan, NULL);

    return 0;//CAN_OK;
}



/*
*********************************************************************************************************
* Set driver output type.
* canStatus canSetBusOutputControl(char handle, INT16U drivertype);
* Description
* This function sets the driver type for a CAN controller. This corresponds loosely to the bus output control register in the CAN controller, hence the name of this function. CANLIB32 does not allow for direct manipulation of the bus output control register; instead, symbolic constants are used to select the desired driver type. 
*
* Input Parameters
* drivertype 
* Any one of the following values: Value Meaning 
* canDRIVER_NORMAL The "normal" driver type (push-pull). This is the default. 
* canDRIVER_SILENT Sets the CAN controller in Silent Mode; that is, it doesn't send anything, not even ACK bits, on the bus. Reception works as usual. 
* Output Parameters
* None. 
*
* Return Value
* canOK (zero) if success
* canERR_xxx (negative) if failure.
* 
* Notes
* Don't confuse the CAN controller driver type with the bus driver type. The CAN controller is not connected directly to the CAN bus; instead, it is connected to a bus transceiver circuit which interfaces directly to the bus. The "CAN controller driver type" we are talking about here refers to the mode which the CAN controller uses to drive the bus transceiver circuit. CANLIB supports two different modes:
*
* Normal mode, which is.. well.. the normal mode. 
* Silent mode, where the CAN controller does not send anything on the bus, not even ACK bits. Reception, however, works as usual. 
* Note that Silent Mode is not supported by all CAN controllers. 
*********************************************************************************************************
*/
char canSetBusOutputControl(INT8U channelNumber, INT16U drivertype)
{
    INT8U *BaseAddr;
    INT8U TempData, tmp;
    char stat, driver;		   

    BaseAddr = (INT8U*) PCAN_Base[channelNumber];

    canEnterRet(channelNumber);

    PCAN_Addr = BaseAddr + PCAN_MOD;
    tmp = *PCAN_Addr;
    switch (drivertype) {
        case COMMUNICATE:
            driver = 0xDA;            // Pushpull  (OCTP1|OCTN1|OCTP0|OCTN0|OCM1)
            tmp &= ~PCAN_LOM;
            break;
            
        case SILENT:
            driver = 0xDA;
            tmp |= PCAN_LOM;
            break;
        default:
            driver = -1;
            break;
    }

    if (driver == -1) {
        stat = -3;//PCAN_WRONG_PARAMETER;
    } 
    else {
        PCAN_Addr = BaseAddr + PCAN_OCR; 		// Set the output control
        *PCAN_Addr = (INT8U)(driver & 0xff);
        PCAN_Addr = BaseAddr + PCAN_MOD;         // Restore control register
        *PCAN_Addr = tmp;
        stat = 0; //PCAN_OK;
    }
    return stat;
}

/*$PAGE*/
/*
*********************************************************************************************************
canStatus canAccept(char handle, long envelope, char flag);

Description
This routine sets the message acceptance filters on a CAN channel. 

Parameters

handle 
An open handle to a CAN circuit.
envelope 
The mask or code to set.
flag 
Any one of the following values: Value Meaning 
canFILTER_SET_CODE_STD Sets the code for standard (11-bit) identifiers. 
canFILTER_SET_MASK_STD Sets the mask for standard (11-bit) identifiers. 
canFILTER_SET_CODE_EXT Sets the code for extended (29-bit) identifiers. 
canFILTER_SET_MASK_EXT Sets the mask for extended (29-bit) identifiers. 

Return Value
canOK (zero) if success
canERR_xxx (negative) if failure.


Code and Mask format
A binary 1 in a mask means "the corresponding bit in the code is relevant"

A binary 0 in a mask means "the corresponding bit in the code is not relevant"

A relevant binary 1 in a code means "the corresponding bit in the identifier must be 1"

A relevant binary 0 in a code means "the corresponding bit in the identifier must be 0"

In other words, the message is accepted if ((code XOR id) AND mask) == 0. 

Notes
You can set the extended code and mask only on CAN boards that support extended identifiers. 
Not all CAN boards support different masks for standard and extended CAN identifiers. 
If you want to remove a filter, call canAccept with the mask set to 0. 
On some boards the acceptance filtering is done by the CAN hardware; on other boards (typically those with an embedded CPU,) the acceptance filtering is done by software. canAccept behaves in the same way for all boards, however. 
Related Topics
*********************************************************************************************************
*/
char canSetHwFilter(INT8U channelNumber, INT32U acr, INT32U amr)
{
    INT8U *BaseAddr;
    INT8U TempData;
    char tmp;
    LREG  acc;
    
    BaseAddr =(INT8U *) PCAN_Base[channelNumber];   
    
    tmp = canEnterRet(channelNumber);
    if(tmp )
        return tmp;
    
     // Set the acceptance code
    acc.ulint = acr;
    if(acc.uchar.b3 & PCAN_FF_EXT){                       //EXTENDed
        acc.uchar.b3 &= ~PCAN_FF_EXT;
	acc.ulint <<= 3;
        // Set the acceptance mask
        PCAN_Addr = BaseAddr + PCAN_ACR0;
        *PCAN_Addr = (INT8U) (acc.uchar.b3);
        PCAN_Addr++;
        *PCAN_Addr = (INT8U) (acc.uchar.b2);
        PCAN_Addr++;
        *PCAN_Addr = (INT8U) (acc.uchar.b1);
        PCAN_Addr++;
        *PCAN_Addr = (INT8U) (acc.uchar.b0);
        PCAN_Addr++;

        acc.ulint = amr;
		acc.ulint |= 0x0007;
        // Set the acceptance mask
        *PCAN_Addr = (INT8U) (acc.uchar.b3);
        PCAN_Addr++;
        *PCAN_Addr = (INT8U) (acc.uchar.b2);
        PCAN_Addr++;
        *PCAN_Addr = (INT8U) (acc.uchar.b1);
        PCAN_Addr++;
        *PCAN_Addr = (INT8U)(acc.uchar.b0);
    } else {
        //
        // Standard CAN filter.
        acc.ulint = 0;
        acc.wint.lsw = (INT16U)(acr <<= 5);

        // Set the acceptance mask
        PCAN_Addr = BaseAddr + PCAN_ACR0;
        *PCAN_Addr = (INT8U) (acc.uchar.b1);
        PCAN_Addr++;
        *PCAN_Addr = (INT8U) (acc.uchar.b0);
        PCAN_Addr++;
        *PCAN_Addr = (INT8U) (0x0FF);
        PCAN_Addr++;
        *PCAN_Addr = (INT8U) (0x0FF);
        PCAN_Addr++;
        acc.wint.lsw = (INT16U)(amr <<= 5);
        // Set the acceptance mask
        *PCAN_Addr = (INT8U) (acc.uchar.b1);
        PCAN_Addr++;
        *PCAN_Addr = (INT8U) (acc.uchar.b0);
        PCAN_Addr++;
        *PCAN_Addr = (INT8U) (0x0FF);
        PCAN_Addr++;
        *PCAN_Addr = (INT8U) (0x0FF);
    }
    tmp  = canQuitRet(channelNumber);

    return tmp;
}


/*
*********************************************************************************************************
* FUNCTION PROTOTYPE:BOOL PCAN_TXAvailable(INT8U channelNumber)
*
* DESCRIPTION:       this function is used to detect the Transmit Buffer if is available.
*
* INPUT:             NONE
*
* RETURN:            0:    Unavailable
*                    1:    Available
*********************************************************************************************************
*/
char PCAN_TXAvailable(INT8U channelNumber)
{
	
    INT8U *BaseAddr;
    INT8U TempData;

    BaseAddr = (INT8U *) PCAN_Base[channelNumber];

    PCAN_Addr = BaseAddr + PCAN_SR;           
    TempData = *PCAN_Addr;

    return TempData & PCAN_TBS;         
}
/*$PAGE*/
/*
*********************************************************************************************************
* (Start Write) Start a transmission. May be called at interupt level.
*
*

canStatus canWrite(char handle, long id,  void* msg, INT16U dlc, INT16U flag);

Description
This function sends a CAN message. The call returns immediately after queuing the message to the driver. 

Input Parameters

handle 
A handle to an open CAN circuit. 
id 
The identifier of the CAN message to send. 
msg 
A pointer to the message data, or NULL. 
dlc 
The length of the message. Can be at most 8. 
flag 
A combination of message flags, canMSG_xxx. Use this parameter to send extended (29-bit) frames and/or remote frames. Use canMSG_EXT and/or canMSG_RTR for this purpose. 
Output Parameters
None. 

Return Value
canOK (zero) if success
canERR_xxx (negative) if failure.


Notes

The message has been queued for transmission when this calls return. It has not necessarily been sent. 
*********************************************************************************************************
*/
char canWrite(INT8U chan, letter *tbuf)
{
    INT8U *BaseAddr;
    INT8U i;
    INT8U dlc, TempData;
    LREG id;

	////// // // //printf("start writing Envelope %x %x ", chan, Folder[folderNo].Envelope);
    BaseAddr = (INT8U *) PCAN_Base[chan];
    PCAN_Addr = BaseAddr + PCAN_SR;           
    TempData = *PCAN_Addr;
	i = TestTimes;
    while((TempData & PCAN_TBS) ==0){
		WDTClear();
	    if(i > 0)
	    	i--;
		else 
			break;
	    TempData = *PCAN_Addr;
	}
	if (i == 0){
	  return -4 ;         
 	}	

    dlc = (tbuf->msginfo) & PCAN_FF_DLC;       // DLC
	if(dlc > 8 )
		dlc = 8;
		   
    PCAN_Addr = BaseAddr + PCAN_MSGBUF;
    *PCAN_Addr = tbuf->msginfo;
    if (tbuf->msginfo & PCAN_FF_EXT){        // Extended CAN  (all compressed)
    	id.filter.hfilter = (INT16U) tbuf->ident;
		id.msgid.cline0   = (INT8U)tbuf->cline[0];
		id.msgid.cline1 =   (INT8U)tbuf->cline[1];
		id.ulint <<= 3;	
        
        PCAN_Addr = BaseAddr + PCAN_XID0;
        *PCAN_Addr = id.uchar.b3;
        PCAN_Addr = BaseAddr + PCAN_XID1;
        *PCAN_Addr = id.uchar.b2;
        PCAN_Addr = BaseAddr + PCAN_XID2;
        *PCAN_Addr = id.uchar.b1;
        PCAN_Addr = BaseAddr + PCAN_XID3;
        *PCAN_Addr = id.uchar.b0;

        PCAN_Addr = BaseAddr + PCAN_XDATA;
    } else {											// Standard CAN
    	id.ulint = tbuf->ident;

        PCAN_Addr = BaseAddr+ PCAN_SID0;
        *PCAN_Addr = (INT8U) (id.ulint >> 3); 
        PCAN_Addr = BaseAddr+ PCAN_SID1;
        *PCAN_Addr = (INT8U) (id.ulint << 5); 

        PCAN_Addr = BaseAddr + PCAN_SDATA;
    }
	for (i = 0; i < dlc; i++) {
		*PCAN_Addr++ = tbuf->line[i];
	}
    

    PCAN_Addr = BaseAddr + PCAN_CMR;       // 
    *PCAN_Addr = PCAN_TR;
    return 0;
}


/*$PAGE*/
/*
*********************************************************************************************************
* FUNCTION PROTOTYPE:static void PCAN_Transmit_ISR(char chan)
*
* DESCRIPTION:      *
* INPUT:             NONE
*
* RETURN:            
*                    
*********************************************************************************************************
*/


/*
 ** Handle error interrupts. Happens when the bus status or error status
 ** bits in the status register changes.
 */
static void PCAN_Error_ISR(char chan)
{
//    PCC1000_GetChipState(pCtx, chan, NULL);
    ////// // // //printf("CAN %d Error ISR" ,chan);
}



/*
*********************************************************************************************************
* FUNCTION PROTOTYPE:static void PCAN_Receive_ISR(INT8U channelNumber)
*
* DESCRIPTION:      
* INPUT:             NONE
*
* RETURN:            
*                    
*********************************************************************************************************
*/

INT8U canReceive(INT8U chan)
{
	INT8U *BaseAddr;
    INT8U flags, SR;
	INT8U i;
    INT8U k, errflag;
    INT8U dlc;
    LREG id;
		 
    BaseAddr = (INT8U *) PCAN_Base[chan];   

    PCAN_Addr = BaseAddr + PCAN_SR;
    SR = *PCAN_Addr;
    id.ulint = 0;
	
	errflag = 0;
    while(SR & PCAN_RBS) {
		PCAN_Addr = BaseAddr + PCAN_MSGBUF;
        flags = *PCAN_Addr;
        dlc =  (flags & PCAN_FF_DLC);       			// DLC.3 - DLC.0
		// // // //printf(" R message = %x DLC = %d ", flags, dlc);
        id.ulint = 0;
        if (flags & PCAN_FF_EXT) {
			errflag = 2;								// Ext msg received
			// Extended CAN
            PCAN_Addr 	= BaseAddr + PCAN_XID0;
            id.uchar.b3 = *PCAN_Addr;
            PCAN_Addr 	= BaseAddr + PCAN_XID1;
            id.uchar.b2 = *PCAN_Addr;
            PCAN_Addr 	= BaseAddr + PCAN_XID2;
            id.uchar.b1 = *PCAN_Addr;
            PCAN_Addr 	= BaseAddr + PCAN_XID3;
            id.uchar.b0 = *PCAN_Addr;
            id.ulint >>= 3;
            PCAN_Addr = BaseAddr + PCAN_XDATA;
			if(chan == SHAFT_CAN){
				Folder[HCU_SFLD].Letter.cline[0] = id.msgid.cline0;
				Folder[HCU_SFLD].Letter.cline[1] = id.msgid.cline1;
				for(i = 0; i < dlc; i++){
					Folder[HCU_SFLD].Letter.line[i] = *PCAN_Addr++;
				}
				Folder[HCU_SFLD].NewMsg = 1;
			}
			else if(chan == CAR_CAN){
				if(id.msgid.envelope == Folder[CCU_SFLD].Letter.ident){
					Folder[CCU_SFLD].Letter.cline[0] = id.msgid.cline0;
					Folder[CCU_SFLD].Letter.cline[1] = id.msgid.cline1;
					for(i = 0; i < dlc; i++){
						Folder[CCU_SFLD].Letter.line[i] = *PCAN_Addr++;
					}
					Folder[CCU_SFLD].NewMsg = 1;
				}
			}
        } else {
			errflag = 1;								// Standard msg received
			//	//// // // //printf(" CAN %d Rx Std %d bytes", chan, dlc);
            PCAN_Addr = BaseAddr + PCAN_SID0;
            id.uchar.b1 = *PCAN_Addr;
            PCAN_Addr = BaseAddr + PCAN_SID1;
            id.uchar.b0 = *PCAN_Addr;
            id.ulint >>= 5;
            PCAN_Addr = BaseAddr + PCAN_SDATA;
			if(chan == CAR_CAN){
				if(id.msgid.envelope == Folder[CCU_SFLD].Letter.ident){
					for(i = 0; i < dlc; i++){
						Folder[CCU_SFLD].Letter.line[i] = *PCAN_Addr++;
					}
					Folder[CCU_SFLD].NewMsg = 1;
				}
			}
        }
		// // // //printf("");
        // Release receive buffer
        PCAN_Addr = BaseAddr + PCAN_CMR;
        *PCAN_Addr = PCAN_RRB;
    	PCAN_Addr = BaseAddr + PCAN_SR;
    	SR = *PCAN_Addr;
    }
	return errflag;
}


/*
*********************************************************************************************************
*********************************************************************************************************
*/
static void PCAN_Overrun_ISR(char chan)
{
    INT8U *BaseAddr, reval;
	
    BaseAddr = (INT8U *)PCAN_Base[chan];   
//    //// // // //printf("CAN %d Overrun ISR" ,chan);
    PCAN_Addr = BaseAddr + PCAN_CMR;
    *PCAN_Addr =  PCAN_CDO;
  	canReceive(chan);

}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
static void PCAN_BusError_ISR(char chan)
{
//    //// // // //printf("CAN %d Bus Error ISR" ,chan);
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
static void PCAN_ErrorPassive_ISR(char chan)
{
//    //// // // //printf("CAN %d Error Passive ISR" ,chan);
//    PCAN_GetChipState(chan, NULL);
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/

static void PCAN_Wakeup_ISR(chan)
{

//    //// // // //printf("CAN %d Wakeup ISR" ,chan);

}

/*
*********************************************************************************************************
*                     EXTERNAL  INTERRUPT 0 SERVICE FUNCTION
*
*                             CAN CONTROLLER INTERRUPT
*********************************************************************************************************
*/


void PCANISR(void)
{
    INT8U *BaseAddr;
    INT8U ireg, chan;		   
	char loopmax = 100;
	
	for(chan = 0; chan < 2; chan++){
	    BaseAddr = (INT8U *)PCAN_Base[chan];   
		 
	    PCAN_Addr = BaseAddr + PCAN_IR;             // Read Interrupt Register
	    ireg = *PCAN_Addr;
	    while(ireg) {
			
			//	WDTClear();
	        if (--loopmax == 0) {
	        	// report erroe PCCAN_EmergencyExit(chan);
	            return;
	        }

	        if (ireg & PCAN_RI) {                   // Receive Interrupt 
	            canReceive(chan);
				//printf("CAN RI ISR");
	        }
	        if (ireg & PCAN_TI) {                   // Transmit Interrupt
				//printf("CAN TI ISR");
			    PCAN_Addr = BaseAddr + PCAN_IR;     // Read Interrupt Register
			    ireg = *PCAN_Addr;
	        }

	        if (ireg & PCAN_EI) {                   // Error Warning Interrupt
				//printf("CAN EI ISR");
				canBusOff(chan);  canBusOn(chan);
	            PCAN_Error_ISR(chan);
	        }
	        if (ireg & PCAN_DOI) {                  // Data Overrun Interrupt
				//printf("CAN DOI ISR");
	            canReceive(chan);
	            PCAN_Overrun_ISR(chan);
				canBusOff(chan);  canBusOn(chan);
	        }

	        if (ireg & PCAN_WUI) {                  // Wake-Up Interrupt
				//printf("CAN WUI ISR");
	            PCAN_Wakeup_ISR(chan);
	        }

	        if (ireg & PCAN_BEI) {                  // Bus Error Interrupt
				//printf("CAN BEI ISR");
	            PCAN_BusError_ISR(chan);
				canBusOff(chan); canBusOn(chan);
	        }

	        if (ireg & PCAN_EPI) {                  // Error Passive Interrupt
				//printf("CAN EPI ISR");
				canBusOff(chan); canBusOn(chan);
	            PCAN_ErrorPassive_ISR(chan);
	        }
			PCAN_Addr = BaseAddr + PCAN_IR;             // Read Interrupt Register
		    ireg = *PCAN_Addr;
		}  
	}
}


void canAllOpen(void)
{
	INT8U i;

	for(i = 0; i < 2; i++){
	  	canOpenChannel(i);
	    //canSetBusOutputControl(i, COMMUNICATE);
	    //canBusOff(i);
	    //canBusOn(i);
	}
}


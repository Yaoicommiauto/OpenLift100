/*
*********************************************************************************************************
*            The Low Level Functions definition of FUNCTION PROTOTYPES, Including As  Follows:
*
*            CAN_GET
*			 CAN_PUT
*			 CAN_STATUS
*			 CAN_IR
*			 CAN_(UN)INIT
*			 ...
*********************************************************************************************************
*/

/* CAN Lib*/

/* declare interrupt routines */

//#pragma interrupt(PCANISR = 7)


#ifdef   CAN_GLOBALS
   #define  CAN_EXT
#else
   #define  CAN_EXT  extern
#endif


/* Mode Register */
#define PCAN_MOD             0		// Mode Register
#define PCAN_SM              0x10	// Sleep Mode
#define PCAN_AFM             0x08	// Acceptance Filter Mode
#define PCAN_STM             0x04	// Self Test Mode
#define PCAN_LOM             0x02	// Listen Only Mode
#define PCAN_RM              0x01	// Reset Mode

/* Command Register */
#define PCAN_CMR             1
#define PCAN_SSR             0x10	// Self Reception Request
#define PCAN_CDO             0x08	// Clear Data Overrun
#define PCAN_RRB             0x04	// Release Receive Buffer
#define PCAN_AT              0x02	// Abort Transmit
#define PCAN_TR              0x01  	// Tramsmission Reuest

#define PCAN_CMD_SINGLESHOT       (PCAN_AT|PCAN_TR)  // Transmit Shut-Down
#define PCAN_CMD_SINGLESHOT_SELF  (PCAN_SRR|PCAN_AT) // Transmit & Sleep

/* Status Register */
#define PCAN_SR              2		// Status Register
#define PCAN_BS              0x80	// Bus Status
#define PCAN_ES              0x40	// Error Status
#define PCAN_TS              0x20	// Trnsmit Status
#define PCAN_RS              0x10	// Receive Status
#define PCAN_TCS             0x08	// Transmission Complete Status
#define PCAN_TBS             0x04	// Transmit Buffer Status
#define PCAN_DOS             0x02	// Data Overrun Status
#define PCAN_RBS             0x01	// Receive Overflow Status

/* Interrupt Register */
#define PCAN_IR              3		// Interrup Register
#define PCAN_BEI             0x80	// Bus Error Interrupt
#define PCAN_ALI             0x40	// Arbitration Lost Interrupt
#define PCAN_EPI             0x20	// Error Passive Interrupt

#define PCAN_WUI             0x10	// Wake-Up Interrupt
#define PCAN_DOI             0x08	// Data Overrun Interrupt
#define PCAN_EI              0x04	// Error Warning Interrupt
#define PCAN_TI              0x02	// Transmit Interrupt
#define PCAN_RI              0x01	// Receive Interrupt

/* Interrupt Enable Register */
#define PCAN_IER             4
#define PCAN_BEIE            0x80	// Bus Erroor Interrupt Enable
#define PCAN_ALIE            0x40	// Arbitration Lost Interrupt Enable
#define PCAN_EPIE            0x20	// Error Passive Interrupt Enable
#define PCAN_WUIE            0x10	// Wake-Up Interrupt Enable
#define PCAN_DOIE            0x08	// Data Overrun Interrupt Enable
#define PCAN_EIE             0x04	// Error Warning Interrupt Enable
#define PCAN_TIE             0x02	// Transmit Interrupt Enable
#define PCAN_RIE             0x01	// Receive Interrupt Enable

/* Bus Timing Register 0 */
#define PCAN_BTR0            6		
/* Bus Timing Register 1 */
#define PCAN_BTR1            7

/* Output Control Register */
#define PCAN_OCR             8
#define PCAN_OCTP1           0x80	// Transistor P1
#define PCAN_OCTN1           0x40	// Transistor N1
#define PCAN_OCPOL1          0x20	// Output Control Polarity 1
#define PCAN_OCTP0           0x10	// Transistor P0
#define PCAN_OCTN0           0x08	// Transistor N0
#define PCAN_OCPOL0          0x04	// Output Control Polarity 0
#define PCAN_OCM1            0x02	// Output Control Mode 1
#define PCAN_OCM0            0x01	// Output Control Mode 0

#define PCAN_ALC             11		// Arbitration Lost Capture

#define PCAN_ECC             12		// Error Code Capture
#define PCAN_ECC_ERROR_MASK  0xC0
#define PCAN_ECC_BIT_ERROR   0x00
#define PCAN_ECC_FORM_ERROR  0x40
#define PCAN_ECC_STUFF_ERROR 0x80
#define PCAN_ECC_OTHER_ERROR 0xc0
#define PCAN_ECC_DIR_MASK    0x20
#define PCAN_ECC_DIR_TX      0x00
#define PCAN_ECC_DIR_RX      0x20
#define PCAN_ECC_SEGMENT_MASK 0x1F

#define PCAN_EWLR            13		// Error Warning Limit
#define PCAN_RXERR           14		// Receive Error Counter
#define PCAN_TXERR           15		// Transmit Error Counter

#define PCAN_ACR0            16		// Acceptance Code Registers0
#define PCAN_ACR1            17		// Acceptance Code Registers1
#define PCAN_ACR2            18		// Acceptance Code Registers2
#define PCAN_ACR3            19	        // Acceptance Code Registers3
#define PCAN_AMR0            20		// Acceptance Mask Registers0
#define PCAN_AMR1            21		// Acceptance Mask Registers1
#define PCAN_AMR2            22		// Acceptance Mask Registers2
#define PCAN_AMR3            23		// Acceptance Mask Registers3

#define PCAN_RMC             29		// RX Message Counter
#define PCAN_RBSA            30		// RX Buffer Start Address

#define PCAN_CDR             31		// Clock Divider Register
#define PCAN_PELICAN         0x80
#define PCAN_CBP             0x40
#define PCAN_RXINTEN         0x20
#define PCAN_CLOCKOFF        0x08


#define PCAN_MSGBUF          16		//  Message Buffer
/* Standard Frame Format */
#define PCAN_SID0            17
#define PCAN_SID1            18
#define PCAN_SDATA           19

/* Extended Frame Format */
#define PCAN_XID0            17
#define PCAN_XID1            18
#define PCAN_XID2            19
#define PCAN_XID3            20
#define PCAN_XDATA           21

#define PCAN_RXFIFO          32		// Rx FIFO Address
#define PCAN_RXFIFO_SIZE     64         // Rx FIFO Size

#define TestTimes             0x20
/*
*********************************************************************************************************
*                            CAN BUS baudrate under 16M OSC
*********************************************************************************************************
*/

#define  ByteRate_40k         0x00

//CAN_EXT folder Folder[3];     // 0 KP(T)  1 CP(R control)

//CAN_EXT CAN_Over[2];

//#define CANBLK0 	 (*(volatile sja1000*)(0x0300))
//#define CANBLK1 	 (*(volatile sja1000*)(0x0400))


INT8U canEnterRet(INT8U channelNumber);
char  canQuitRet(INT8U channelNumber);
char   canSetBaudRate(INT16U chan, INT8U CAN_ByteRate);
char   caSetOutClk(INT16U chan, INT8U Out_Control, INT8U Clock_Out);
char   canOpenChannel(INT8U channelNumber);
char   canCloseChannel(INT8U channelNumber);
char   canBusOn(INT8U channelNumber);
char   canBusOff(INT8U channelNumber);
char   canSetBusOutputControl(INT8U channelNumber, INT16U drivertype);
char   canSetHwFilter(INT8U channelNumber, INT32U acr, INT32U amr);
char  PCAN_TXAvailable(INT8U channelNumber);
char  canWrite(INT8U chan, letter *buf);

INT8U canReceive(INT8U chan);
void  PCAN_Receive_ISR(INT8U ch);
void  PCANISR(void);

void  canAllOpen(void);

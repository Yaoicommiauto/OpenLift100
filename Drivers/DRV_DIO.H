#define  DIO_MAX_DI                 32      /* Maximum number of Discrete Input  Channels (1..255)     */
#define  DIO_MAX_DO                 16       /* Maximum number of Discrete Output Channels (1..255)     */

#ifdef   DIO_GLOBALS
   #define  DIO_EXT
#else
   #define  DIO_EXT  extern
#endif

/*
*********************************************************************************************************
*                                       DISCRETE INPUT CONSTANTS
*********************************************************************************************************
*/
                                            /* DI MODE SELECTOR VALUES                                 */
/*$PAGE*/
#define  DI_INSP	             	0			// 1
#define  DI_ICU                     1			// 0
#define  DI_ICD                     2			// 0
#define  DI_PLU2                    3		   	// 1		1

#define  DI_PLD2                    4			// 1
#define  DI_UL                      5			// 1		 0
#define  DI_DL                      6			// 1
#define  DI_PLU                     7			// 1

#define  DI_PLD                     8			// 1		  0
#define  DI_ULVL                    9			// 0
#define  DI_DLVL                    10			// 0
#define  DI_DRVFAULT                11			// 1

#define  DI_FIRE                    12			// 1
#define  DI_SRELAY                  13			// 1		   4
#define  DI_DRELAY                  14			// 1
#define  DI_MC1                     15			// 0

#define  DI_MC2                     16			// 0
#define  DI_BRC                     17			// 0
#define  DI_FDZONE                  18			// 0
#define  DI_DRVRDY                  19			// 0

#define  DI_FIREMAN                 20			// 1
#define  DI_EPWRDY                  21			// 0
#define  DI_X22                     22			// 0
#define  DI_EQUK                    23			// 1

#define  DI_RDZONE                  24			// 1
#define  DI_PRELAY                  25			// 0
#define  DI_SCOIL                   26			// 0
#define  DI_DCOIL                   27			// 0
#define  DI_X28                     28

#define  DI_X29                     29
#define  DI_X30                     30
#define  DI_X31                     31

#define  DO_BRC                     0
#define  DO_BRCx                    1
#define  DO_MC1                     2
#define  DO_MC2                     3

#define  DO_FOP                     4
#define  DO_FCL                     5
#define  DO_ROP                     6
#define  DO_RCL                     7
				     
#define  DO_PROP                    8		// pre-open
#define  DO_EPW                     9		// Emergeny PoWer run enable

#define  DO_FWD                     10      
#define  DO_REV                     11
#define  DO_DRVEN                   12		// For CPI ect.

#define  DO_M0	                    13
#define  DO_M1                      14
#define  DO_M2                      15

/*$PAGE*/
#define  DI_MODE_DIRECT             0x00       /* Input is based on state of physical input               */
#define  DI_MODE_INV                0x10       /* Input is based on the complement of the physical input  */
#define  DI_MODE_LOW                0x20       /* Input is forced low                                     */
#define  DI_MODE_HIGH               0x30       /* Input is forced high                                    */
                                            	/* DO MODE SELECTOR VALUES                                 */
#define  DO_MODE_DIRECT             0x00       /* Output is based on state of user supplied state         */
#define  DO_MODE_LOW                0x10       /* Output will be low                                      */
#define  DO_MODE_HIGH               0x20       /* Output will be high                                     */

// For all DI Channels
DIO_EXT struct{
	INT16U	DIIn 	  :	1;                      /* Current state of sensor input (Input Physical State)                                */
	INT16U	DIVal 	  :	1;                      /* State of discrete input channel (or # of transitions)        */
	INT16U  DIPrev 	  :	1;                      /* Previous state of DIIn for edge detection                    */
	INT16U  DIModeSel : 13;
}DITbl[DIO_MAX_DI];

// For all DO Channels
DIO_EXT struct{
	INT16U 	DOOut	  :	1;                 		/* Current state of discrete output channel                     */
	INT16U	DOCtrl	  : 1;                    		/* Discrete output control request                              */
	INT16U  DOModeSel : 14;
}DOTbl[DIO_MAX_DO];

DIO_EXT INT8U   out[2];
DIO_EXT INT8U   in[4];

// Modbus Address
//#define CCUDO_MOD	0x0000



/*
*********************************************************************************************************
*                            FUNCTION PROTOTYPES (DIO Module Application Interface)
*********************************************************************************************************
*/
void     DIOInit (void);
void     DICfgMode (INT8U n, INT8U mode);
void     DOCfgMode(INT8U n, INT8U mode);
INT8U  	 DIGet(INT8U n);
INT8U    DIGetP(INT8U n);
INT8U    DIGetN(INT8U n);
void     DOSet(INT8U n, BOOLEAN state);
BOOLEAN  DOGet (INT8U n);
void     DIOTask();
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                           HARDWARE SPECIFIC
*********************************************************************************************************
*/
void     DIOInitIO(void);
void     DIRd(void);
void     DOWr(void);
void  	 DIConfig(void);

#ifdef  DOOR_GLOBALS
   #define  DOOR_EXT
#else
   #define  DOOR_EXT  extern
#endif
/*
*********************************************************************************************************
*                                       Lift Door Control Block
* CtrReg:	Bit3 - Bit0: Mode(0 - Normal, 1 - Hold-Close(Attendant),
*							  2 - Hand-Close/Open (Inspection), 
*					          3 - Reversal open/Close(Fire)
*			Bit4 - Bit7: Door Type
*							  F - No door(Disable
* DoorSig:
*			BIT0:	Request Open Signal
*			BIT1:	Request Close Signal
*			BIT7:	Door state Shift
* StaReg:	BIT0:	Car InterLock Switch
*			BIT1:	Open Limt switch
*			BIT2:	Close Limt switch
*			BIT3:	Open Button	
*			BIT4:	Close Button
*			BIT5:	Photo Sensor
*			Bit6:	Over Torque Signal
*			Bit7:	
 *********************************************************************************************************
*/
// Type
#define DOOR_DISABLE		0x00
#define DOOR_DIO			0x01
#define DOOR_SER			0x02

//Door Mode		// Close Mode | Open Mode
#define AOP_KILL			0x01		// Park auto open the door disable
#define AOP_REV				0x02
#define AOP_BTNONLY			0x04


#define ACL_KILL			0x10	   // OT auto close the dorr disable
#define ACL_REV				0x20
#define ACL_NOWAIT			0x40

// preopen reg
#define  POPEN_EN			0x01
#define  LOPEN_EN			0x02


//Door Status Register
#define DOOR_OL				0x01
#define DOOR_CL				0x02

// State Code > 0x80 (CODE - 0x80 is the Fault code)
#define  DOOR_INIT			0x00
#define  DOOR_OPEN			0x10
#define  DOOR_OPENED		0x11
#define  DOOR_CLOSE			0x20
#define  DOOR_CLOSED		0x21

typedef struct {
	INT8U 	Type[2];

	LFT_STATE State[2];		// Running State or Fault Code

	INT8U 	Mode;
	INT8U 	PreReg;			// preopen control

	INT8U 	StaReg[2];
	INT8U	DWait[2];

	INT8U   Dop;
  	INT8U   Dcp;
}LFT_DCB;

// Post Open Parameter
#define 	NOCHECK		0
#define 	CHCEKCALL	1

// Door open device
#define     DOP_DOB		0x01;
#define     DOP_DEE		0x02;
#define     DOP_CALL	0x04;

DOOR_EXT LFT_DCB LFT_Dcb;

void 	Set_DoorMode(INT8U mode);
BOOLEAN isDoorClosed(void);

void 	setup_Doors(void);
void 	task_RunDoor(void);


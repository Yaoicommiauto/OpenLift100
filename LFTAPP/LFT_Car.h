#ifdef   CAR_GLOBALS
   #define  CAR_EXT
#else
   #define  CAR_EXT  extern
#endif

//Car Mode
#define		MAX_MODE	13

#define		NORMAL_Mode 1
#define		HOMING		2
#define 	ATTENDANT	3
#define		FULL		4
#define 	VIP			4
#define		PARKING		6	
#define		PARKED		7
#define 	FSTFIRE		8
#define 	FSTFIRED	9
#define 	SNDFIRE		10
#define 	EARTHQK		11
#define 	INSTALL		12
//Car Load
#define		NOLOAD		0
#define		LIGHTLOAD	1
#define		FULLLOAD	2
#define		OVERLOAD	3

// Car State
//M0, M1, M2						//MMxx xxxx
#define 	CAR_MSTATE  0xC0
#define 	MINIT		0x00
#define 	M0			0x40
#define 	M1			0x80	
#define 	M2			0xC0

#define		M_INIT		0x00

	
// Auto run State
#define		A_INIT		0x50
#define		A_WAIT		0x40

#define		A_DOOR		0x4A


#define		AU_DRIVE	0x51
#define		AU_START	0x52
#define		AU_VACC		0x53
//#define		AU_VCON		0x54
#define		AU_VDEC		0x55
//#define		AU_DEST		0x56

#define		AU_FIND		0x57

//#define		AU_CREEP	0x58

#define		AU_STOP		0x5E
#define		AU_PARK		0x5F
//#define		AU_OPEN		0x58
#define		AD_DRIVE	0x41
#define		AD_START	0x42
#define		AD_VACC		0x43
//#define		AD_VCON		0x44
#define		AD_VDEC		0x45
#define		AD_DEST		0x46

#define		AD_FIND		0x47

//#define		AD_CREEP	0x48

#define		AD_STOP		0x4E
#define		AD_PARK		0x4F


//	TEACH	States
#define		T_CHECK		0x60

#define		TF_START	0x61
#define		TF_RUNH		0x62
#define		TF_RUNL		0x63
#define		TF_STOP		0x64

#define		TU_START	0x71
#define		TU_RUNH		0x72
#define		TU_RUNL		0x73
#define		TU_STOP		0x74

#define		TD_START	0x6B
#define		TD_RUNH		0x6C
#define		TD_RUNL		0x6D
#define		TD_STOP		0x6E
// Inspection
#define		I_INIT		0x80
#define		I_CHECK		0x81

#define		IU_START	0x92 
#define		IU_RUN		0x93
#define		IU_STOP		0x94

#define		ID_START	0x82
#define		ID_RUN		0x83
#define		ID_STOP		0x84

// Emergency
#define		E_INIT		0xC0


//direct
#define		CUR_NDIR	0x00
#define		CUR_UP		0x01
#define		CUR_DN		0x02

// status Register
#define 	TEACHED		0x80
#define 	TOINSTALL	0x40
#define 	RELEVING	0x01

typedef struct{
	LFT_STATE	State;				//.Timer 	unit = 0.1s

	INT8U		StaReg;				//.7 Teched
	INT8U		Direct;				// = 0 / 1 / 2
	INT8U		Mode;
	INT8U		Load;

}LFT_CARCB;

	//INT8U 		GCAddr;		???
typedef struct{
	INT8U 		StartFlr;
	INT8U 		DestFlr;

	INT32U 		StartLav;	// Leave point
	INT32U 		DestEnt;	// Ener pointer
	INT32U 		DestPos;	// Dest pointer
	INT16U 		DestLve;	// Level offset value
}LFT_CARCB1;

// Inverter type
#define DRV_FUJI		0
#define DRV_CPI			1

typedef struct{
	INT8U 		LobbyFlr;		// Car data 4 bytes
	INT8U		HomeFlr;
	INT8U		ParkFlr;
	INT8U		FireFlr;
}LFT_CARSET;


typedef struct{
	INT8U			Type;
	INT8U			reserved;

	INT8U			RAM_BPT;
	INT8U			RAM_BPH;

	INT8U			RAM_BDT;
	INT8U			RAM_MDT;
}
LFT_DRVCB;

#define	EMG_ON		0x80

typedef struct{
	INT16U		CtrReg;
}
LFT_FAULTCB;

//CITY_EXT INT8U		LFT_FlrMask[8];
//CITY_EXT INT8U		LFT_FlrFrontMask[8];
//CITY_EXT INT8U		LFT_FlrRearMask[8];
CAR_EXT LFT_CARSET		LFT_Carset;
CAR_EXT LFT_CARCB		LFT_Carcb;
CAR_EXT LFT_CARCB1		LFT_Carcb1;

CAR_EXT LFT_DRVCB		LFT_Drvcb;
CAR_EXT LFT_FAULTCB		LFT_Faultcb;

#define CAR_signal	0
#define			SIG_TECH		0x01

#define 	DIO_Signal	1
#define 	VELO_Signal 1
#define 	FLR_Signal	1
#define 	RTU_Signal	1
#define 	M012_Signal 1
#define 	CAN_Signal	1
#define		DOOR_Signal 1
#define 	MNG_Signal	1
#define 	CALL_Signal	1


#define 		SIG_DIOTICK		0x0001
#define			SIG_VELOTICK	0x0002
#define 		SIG_FLRTICK		0x0004
#define 		SIG_RTUTICK		0x0008
#define 		SIG_M012TICK	0x0010
#define 		SIG_CANTICK		0x0020
#define			SIG_DOORTICK	0x0040
#define 		SIG_MNGTICK		0x0080
#define 		SIG_CALLTICK 	0x0100


#define Slct_Signal	2
// SLCT_Signal
#define SIG_UENTER		0x0001
#define SIG_DENTER		0x0002
#define SIG_ULEAVE		0x0004
#define SIG_DLEAVE		0x0008
#define SIG_TUENTER		0x0010
#define SIG_TULEAVE		0x0020
#define SIG_TDENTER		0x0040
#define SIG_TDLEAVE		0x0080
#define SIG_PLDENTER	0x0100
#define SIG_PLDLEAVE	0x0200
#define SIG_PLD2ENTER	0x0400
#define SIG_PLD2LEAVE	0x0800
#define SIG_PLUENTER	0x1000
#define SIG_PLULEAVE	0x2000
#define SIG_PLU2ENTER	0x4000
#define SIG_PLU2LEAVE	0x8000

#define time_signal 3
#define  SIG_10MS 	0x0002
#define  SIG_15MS  	0x0001
#define  SIG_25MS 	0x0004
#define  SIG_50MS 	0x0008
#define  SIG_M8002 	0x0100

CAR_EXT INT16U	signal[4]; 		// 0 - 5

CAR_EXT INT16U	CAR_M012;
CAR_EXT INT8U	RAM_SSW[16];				//EEPROM Register group Update



void    StateShift(INT8U newState); 
void    PostSignal(INT16U sno, INT16U event);
INT16U 	WaitSignal(INT16U sno, INT16U event);
INT16U  InquirySignal(INT16U sno, INT16U event);
void    ClearSignal(INT16U sno, INT16U event);

void 	SetStartFlr(INT8U FlrNo);
void 	SetDestFlr(INT8U FlrNo);

INT8U 	Get_CState(void);

void 	Set_CarMode(INT8U mode);
void 	Car_Load(void);
void 	Car_Mode(void);

void 	Car_Dir(void);
void 	Car_Position(void);



#ifdef   CAR_GLOBALS
   #define  CAR_EXT
#else
   #define  CAR_EXT  extern
#endif

//Car Mode
#define		NORMAL		0
#define		HOMING		1
#define 	ATTENDANT	2
#define		FULL		3
#define 	VIP			4
#define		PARKING		5	
#define		PARKED		6
#define 	1STFIRE		7
#define 	2NDFIRE		8
#define 	EARTHQK		10
#define 	INSTALL		11
typedef struct{
	//setup phase
	INT8U 		Number_Floors;
	INT8U 		Number_Groups;
	INT8U 		GCAddr;
	//run phase
	INT8U 		curFlrNo;
	INT8U 		DestFlr;
	INT8U		Direct;
	LFT_STATE	State;
	INT8U		Mode;
	INT8U		isOverLoad;
}LFT_CARCB;

//CITY_EXT INT8U		LFT_FlrMask[8];
//CITY_EXT INT8U		LFT_FlrFrontMask[8];
//CITY_EXT INT8U		LFT_FlrRearMask[8];
CITY_EXT LFT_CARCB		LFT_Carcb;

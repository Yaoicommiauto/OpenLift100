#ifdef   CALL_GLOBALS
   #define  CALL_EXT
#else
   #define  CALL_EXT  extern
#endif

#define 	CCU_MAX_DI	16
#define 	CCU_MAX_DO	8

#define		CDI_DOL		0
#define		CDI_DCL		1
#define		CDI_OPNSR	2

#define		CDI_OVLD	3
#define		CDI_FULL	4
#define		CDI_HVYLD	5
#define		CDI_MIDLD	6
#define		CDI_LGTLD	7

#define		CDI_ATT		8
#define		CDI_VIP		9

#define		CDI_BYPASS	10

#define		CDI_TX11	11
#define		CDI_TX12	12
#define		CDI_TX13	13
#define		CDI_TX14	14
#define		CDI_TX15	15
#define		CDI_TX16	16

#define		CDI_OPENX	17
#define		CDI_CLOSEX	18


#define		CDI_OPEN	19
#define		CDI_CLOSE	20


//Lift Call Control Block
//#define 	CCB_ENABLE	0x01
//#define 	CALL_EVENT	0x80		// Call(s) added or Canceled


//mode
#define		NORMAL		0x00		// Normal
#define 	PPAANG		0x01		// Car Call PingPang Enable
#define		FROZEN		0x02		// Read disable
#define 	LASTONE		0x03		// Only Last one event effective
#define 	CONHOLD		0x04		// (car button Hold-Pressed effective for Installation
#define 	KILL		0x05		//

typedef struct {
	INT8U 	CCBMode;	

	INT8U 	CallGrp;	
	INT8U	LFT_CallBuf[4];
//	INT8U	LFT_LightBuf[4];
}LFT_CCB;
/*
*************************************************
*************************************************
*/
typedef struct {
	INT8U	CCU_DI[CCU_MAX_DI / 8];		
	INT8U	CCU_DO[CCU_MAX_DO / 8];
	LFT_CCB	LFT_Icb;
}LFT_ICCB;	   
//HCU_DI
#define 	HCU_LOCK		0x01

typedef struct {
	INT8U	HCU_DI;
	INT8U	HCU_DO;
	LFT_CCB	LFT_Hcb[2];
}LFT_HCCB;
/*
*************************************************
*************************************************
*/



#define CCALL	0x01
#define HUCALL	0x04
#define HDCALL	0x10
#define VCALL	0x40
#define NOCALL 	0x00


// Front/Rear U/D
CALL_EXT LFT_HCCB 		LFT_HCU[2];
CALL_EXT LFT_ICCB 		LFT_CCU[2];
CALL_EXT LFT_CCB		LFT_VCU[2];

//BOOLEAN  isCCB_Work(LFT_CCB *CallCB);
void     CCB_Clear(LFT_CCB *CallCB);
void 	 ClearCCall(void);
void 	 ClearHCall(void);

void 	 CCB_AddOne(LFT_CCB *CallCB, INT8U FlrNo);
void     CCB_ClearOne(LFT_CCB *CallCB, INT8U FlrNo);
BOOLEAN  CCB_JudgeOne(LFT_CCB *CallCB, INT8U FlrNo);
BOOLEAN  CCB_JudgeAbove(LFT_CCB *CallCB, INT8U FlrNo);
BOOLEAN  CCB_JudgeUnder(LFT_CCB *CallCB, INT8U FlrNo);

INT16U   AnyCall_Above(INT8U FlrNo);
INT16U   AnyCall_Under(INT8U FlrNo);

INT16U 	 AnyCall_ThisFlr(INT8U fr);
void 	ClearCall_ThisFlr(INT8U fr);

void     Call_Event(LFT_CCB *CallCB, INT8U FlrNo, INT8U CallV);

BOOLEAN	 CCU_DIGet(INT8U FR, INT8U chn);
BOOLEAN	 HCU_DIGet(INT8U fr, INT8U n);

void 	 Set_CallMode(LFT_CCB *CallCB, INT8U mode);
void 	 Set_CCallMode(INT8U mode);
void 	 Set_VCallMode(INT8U mode);
void 	 Set_HUCallMode(INT8U mode);
void 	 Set_HDCallMode(INT8U mode);

void setup_Call(void);
void 	 task_Call(void);


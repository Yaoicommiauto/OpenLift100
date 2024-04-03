#ifdef   VELO_GLOBALS
   #define  VELO_EXT 
   #else
   #define  VELO_EXT  extern
#endif

#define TKR		10				// reference tick ms 
#define TKF		100				// feedback tick ms

#define 	VSTOP 	0x00

#define 	VJSR	0x01
#define 	VACR	0x02
#define 	VJTS	0x03
#define 	VCON	0x10

#define 	VJDR	0x20
#define 	VDER	0x21
#define 	VLTG	0x22
#define 	VCRP	0x23
#define 	VFSR	0x24

typedef struct{
	INT16U	curA;		// 2  a6a7
	INT16U	curV;		// 2
	INT16U  Vm;		    // 2

	INT32U  Sacr;		// 4  ac
	INT32U  Sder;		// 4

}rvel;

typedef struct{
	INT16U  Vmacr;
	INT16U  Sacr;		// 4

	INT16U  Vmder;
	INT16U  Sder;		// 4
}rvmin;


VELO_EXT long P100ms[2];
VELO_EXT INT32U  curPReach;
VELO_EXT LFT_STATE vstate;
VELO_EXT INT16U   FbV;


BOOLEAN ifOpVel(void);
BOOLEAN ifZVel(void);

void 	isrPoisition(void);
void 	setPosition(INT32U position);
INT32U 	getPosition(void);


//INT16U 	getVFbk(void);
//INT8U 	getFbk_Dir(void);
INT8U 	getVDegree(void);

//INT32U 	getPJerk(void);

void 	isr_VGenPtn(void);
void 	setup_VPtn(void);
void 	task_VPtn(void);


INT32U getSLeft(void);


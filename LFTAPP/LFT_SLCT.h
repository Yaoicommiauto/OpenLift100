#ifdef   SLCT_GLOBALS
   #define  SLCT_EXT
#else
   #define  SLCT_EXT  extern
#endif

//Level State				DLVL     ULVL	 
#define LVL_NONE	0	  //  0		  0	
#define LVL_ABOVE	1	  //  0		  1
#define LVL_UNDER	2	  //  1       0	
#define LVL_LEVEL	3	  //  1		  1

// direct
#define	SLCT_NDIR		0x00
#define	SLCT_UP			0x01
#define	SLCT_DN			0x02
// Shaft Zone					// UL PLU PLU2 PLD2 PLD DL
#define SHFT_MID		0x00	// 0   0   0    0    0  0
#define SHFT_DL			0x07	// 0   0   0    1    1  1
#define SHFT_PLD		0x06	// 0   0   0    1    1  0 
#define SHFT_PLD2		0x04	// 0   0   0    1    0  0
#define SHFT_PLU2		0x08	// 0   0   1    0    0  0
#define SHFT_PLU		0x18	// 0   1   1    0    0  0
#define SHFT_UL			0x38	// 1   1   1    0    0  0
 
typedef struct {
	INT16U 	CtrReg	: 10;
	INT16U 	Direct	: 6;

	INT8U 	ZShaft;			// shaft zone
	BOOLEAN	DZone;
	INT8U 	Level;

	INT16U 	Timer;
	INT32U	DPPcnt;	
	INT8U 	FlrNo;
	LREG   LChar;
}LFT_SLCTCB;

SLCT_EXT LFT_SLCTCB		LFT_Slctcb;

void setup_selector(void);
void task_selector(void);

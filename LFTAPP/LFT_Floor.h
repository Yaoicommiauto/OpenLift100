#ifdef   FLR_GLOBALS
   #define  FLR_EXT
#else
   #define  FLR_EXT  extern
#endif
#define	 MAX_FLOORS	32
// Offset 
#define  UL_OFFSET	0
#define  DL_OFFSET	1
//point
#define  P_DLEAVE	0
#define  P_DENTER	1
#define  P_ULEAVE	2
#define  P_UENTER	3
//control register
//Status Register
  
typedef union{
	INT16U wreg;
	struct{
		INT16U FDOOR_EN		: 1;
		INT16U FCC_EN		: 1;
		INT16U FHU_EN		: 1;
		INT16U FHD_EN		: 1;
		
		INT16U RDOOR_EN		: 1;
		INT16U RCC_EN		: 1;
		INT16U RHU_EN		: 1;
		INT16U RHD_EN		: 1;

		INT16U FLR_ENABLE 	: 1;

		INT16U DFWait		: 2;
		INT16U DRWait		: 2;

		INT16U HAVECAR		: 1;
		INT16U UINTER		: 1;
		INT16U DINTER		: 1;
	}breg;

	struct{
		INT16U F_EN			: 4;
		INT16U R_EN			: 4;
		INT16U FLR_EN		: 4;
		INT16U RunBits		: 4;
	}hbreg;
}LFT_FCB;

FLR_EXT LFT_FCB 	LFT_Fcb[MAX_FLOORS]; 

void setup_Floor(void);
void task_Floor(void);

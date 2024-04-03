#ifdef   TEACH_GLOBALS
   #define  TEACH_EXT
#else
   #define  TEACH_EXT  extern
#endif

#define T_FIND	1
#define T_UP	2
#define T_DN	3
#define T_STOP	4

// TEACH IN Finish Flag
#define	TEACH_OK	0xa537

#define	TEACH_CMD	0xFEFE
#define	TEACH_RESET	0xFFFF

#define PLD_ERR0	0x0001
#define PLD_ERR1	0x0002
#define PLU_ERR0	0x0003
#define PLU_ERR1	0x0004

#define PLDD_ERR0	0x0005
#define PLDD_ERR1	0x0006
#define PLUD_ERR0	0x0007
#define PLUD_ERR1	0x0008

#define PLDM_ERR0	0x0009
#define PLDM_ERR1	0x000A
#define PLUM_ERR0	0x000B
#define PLUM_ERR1	0x000C

#define FLR_ERR0	0x0010
#define SLCT_ERR0	0x0011
#define POSI_ERR0	0x0012
#define TEACH_ABORT	0x0013



INT8U Teach_Phase(void);
void STeach_Run(void);

#ifdef   AUTO_GLOBALS
   #define  AUTO_EXT
#else
   #define  AUTO_EXT  extern
#endif
 
#define AU_UP		1
#define AU_DN		2
#define A_STOP		3


INT8U Auto_Phase(void);
BOOLEAN PDoorPhase(INT8U relevel);

void SFunc_A_Init(void);
void SAuto_Run(void);

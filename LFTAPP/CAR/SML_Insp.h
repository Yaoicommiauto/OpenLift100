#ifdef   INSP_GLOBALS
   #define  INSP_EXT
#else
   #define  INSP_EXT  extern
#endif

#define I_UP	1
#define I_DN	2

INT8U Insp_Phase(void);

void SFunc_I_Init(void);
void SInsp_Run(void);



#ifdef   TIMING_GLOBALS
   #define  TIME_EXT
#else
   #define  TIME_EXT  extern
#endif
 
void 		init_swtmrs(void);
void 		Init_timer(void);

void  TmrTask(void);




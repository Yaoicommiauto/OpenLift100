#ifdef   PWM_GLOBALS
   #define  PWM_EXT
#else
   #define  PWM_EXT  extern
#endif

#define	PWMVELO		0
#define	PWMTQR		1
#define	PWMLCD		2

void 	init_pwm (INT8U pwmch);
void 	pwm_write(INT8U pwmch, INT8U pwmval);

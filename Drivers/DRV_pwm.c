/* ----------------------------------------------------------------------- 
*/
#define  PWM_GLOBALS
#include "includes.h"
void init_pwm(INT8U pwmch)
{
	if(pwmch == 0){
		wsr = 0;
		pwm0_control = 0x0;
	}
	if(pwmch == 1){
		wsr = 1;
		pwm1_control = 0x0;
		wsr = 0;
	}
	if(pwmch == 2) {
		wsr = 1;
		pwm2_control = 0x0;
		wsr = 0;
	}
}

INT8U preval;
void pwm_write(INT8U pwmch, INT8U pwmval)
{   
	if(pwmch == 0) {
		wsr = 0;
		preval = pwmval;

		pwm0_control = pwmval;
	}
	if(pwmch == 1){
		wsr = 1;
		pwm1_control = pwmval;
		wsr = 0;
	}
}
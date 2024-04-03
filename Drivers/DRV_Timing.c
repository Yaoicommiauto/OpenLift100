#define   TIMING_GLOBALS

#include "includes.h"

#define 	HSO_INT		3
#define     SW_TIME_INT 5
#pragma 	interrupt(timer1_isr   = 0)
#pragma 	interrupt(sw_timer_isr = 5)

extern INT8U const BitMapTbl[];

INT8U cnt50ms = 0;
INT8U isr50ms = 0;
INT8U cnt25ms  = 0;
INT8U cnt15ms  = 0;
INT8U cnt10ms = 0;

/* LOCAL functions */
//static  void Init_timer(void);
static  void timer1_isr(void);
void timer1_isr(void)
{
}


void Init_timer(void)
{       
    timer1 		= 0x0000;                                  /* Initialize timer1 */
    ioc1 		|= 0x24;                                   /* Enable timer overflow */
    int_mask 	|= 0x01;                                   /* Enable timer overflow interrupt */
}


void init_swtmrs(void)
{
	int_mask &= 0xf7;
  	int_mask |= 0x20;
  	hso_command = 0x18;
  	hso_time = timer1 + 50000;	//		50ms
  	hso_command = 0x19;
  	hso_time = timer1 + 25000;	// 		25ms
  	hso_command = 0x1A;
  	hso_time = timer1 + 10000;	//		10ms
  	hso_command = 0x1B;
  	hso_time = timer1 + 15000;	//		15ms 
}

void sw_timer_isr(void)
{
	INT8U temp;

	disable();
   	wsr = 0;
	temp = ios1;	
	if(temp & 0x01){ 					//50ms
		hso_command = 0x18;
		hso_time = timer1 + 50000;
		isr50ms++;
		if((isr50ms & 0x03) == 0x00){
			P100ms[0] = P100ms[1];
			P100ms[1] = getPosition();
			FbV = (INT16U)((labs(P100ms[1] - P100ms[0])) * 5);

		}
		PostSignal(time_signal, SIG_50MS);
	}
	if(temp & 0x02){ 					//25ms
		hso_command = 0x19;
		hso_time = timer1 + 25000;
		PostSignal(time_signal, SIG_25MS);
	}
	if (temp & 0x04){					// 10ms
		hso_command = 0x1A;
		hso_time = timer1 + 10000;
		isrPoisition();
	   	isr_VGenPtn();
		pwm_write(PWMVELO, getVDegree());
		PostSignal(time_signal, SIG_10MS);
	}
	if (temp & 0x08) {
		hso_command = 0x1B;
		hso_time = timer1 + 15000;		// 15ms
		PostSignal(time_signal, SIG_15MS);
	}
	enable();
}


void  TmrTask(void)
{
	INT16U event;

	event = WaitSignal(time_signal, SIG_15MS | SIG_10MS | SIG_25MS | SIG_50MS);

	if(event & SIG_10MS){
		cnt10ms++;	
		PostSignal(VELO_Signal, SIG_VELOTICK);  	// 10ms interval from VPattern isr routine
	}
	if(event & SIG_15MS){
		cnt15ms++;
		PostSignal(DIO_Signal, SIG_DIOTICK);  	  	// 15ms interval
		if((cnt15ms & 0x03) == 0x02){					// 01
			PostSignal(FLR_Signal, SIG_FLRTICK);  		// 60ms interval	 
		}
	  	if((cnt15ms & 0x01) == 0x01){					// 010
		   	PostSignal(CAN_Signal, SIG_CANTICK);  	// 30ms interval	 
		}
	  	if((cnt15ms & 0x3)){					// 010
			PostSignal(RTU_Signal, SIG_RTUTICK);  		// 30 ms interval	 
	   }
	}
	if(event & SIG_25MS){
		cnt25ms++;
		PostSignal(MNG_Signal,  SIG_MNGTICK);  			   // 25ms interval
		if((cnt25ms & 0x03) == 0x01)
			PostSignal(DOOR_Signal, SIG_DOORTICK);  	   // 1SECms interval
	}
	if(event & SIG_50MS){
		cnt50ms++;
		if(cnt50ms & 0x01){
			PostSignal(time_signal, SIG_M8002);
			PostSignal(M012_Signal, SIG_M012TICK);  		// 100ms interval	 
		}
	}
}





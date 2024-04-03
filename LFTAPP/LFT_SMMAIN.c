#include "includes.h"

const unsigned int ccr[2] = {0x20cc, 0x20dc};
#pragma locate (ccr = 0x2018)


/*
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*/
extern INT8U const BitMapTbl[];
void sysreset(void)
{
	INT16U event;

    disable();
    InitIOPORT(); 	    
    InitPSD();

	WDTClear();
	CommInit();	

	Init_timer();		init_swtmrs();
	init_pwm(0); 		init_pwm(1);	init_pwm(2);
	init_eerom();
   	wsr = 0;	
   	ioc0 = 1;	ioc1 = 0x75;	ioc2 = 0x43;
  	wsr = 1;	
  	ioc3 = 0x0C;	
  	wsr = 0;
	int_mask  = 0x21;
   	int_mask1 = 0x03;
    enable();
	while(1){
		WDTClear();
	 //	event = WaitSignal(time_signal, SIG_M8002);
	   //	if(event & SIG_M8002)
			break;
	}

}
void LFT_Setup(void)
{
	INT8U  status;
   	status = Eeprom_Init();
   	if(status){
		WDTClear();
  		status = EEPROM_Format(EEPROM_RECORD_NUM); // 0# - 99# record  	
	}
	if(status == 0){
		status = VerifyEE();
		if(status){
			status = RestoreEE();
		}
	}

	DIOInit();				// Drive initilized
	setup_VPtn();
	setup_PostOffice();		// Module setup
	setup_RTUPoffice();
	setup_selector();
	setup_RunCar();
	setup_Doors();
	setup_Call();
}

void scheduler(void)
{
	WDTClear();				// Drive tasks
	TmrTask();
	WDTClear();				// Drive tasks
   	DIOTask(); 			
	WDTClear();				// Drive tasks
	task_VPtn();			// Lift Application tasks

	WDTClear();				// Drive tasks
	task_selector();
	WDTClear();				// Drive tasks

	task_Floor();
	WDTClear();				// Drive tasks
	task_CAN_ProcMail();
	WDTClear();				// Drive tasks
	task_RTUProcMail();

	WDTClear();				// Drive tasks
  	task_LFT_Mng();
	WDTClear();				// Drive tasks
	task_RunCar();
    task_RunDoor();
}

INT8U k;
void main(void)
{
	INT16U i, event;
	disable();
	sysreset();      	// Reset all hardwares
	WDTClear();
   	LFT_Setup();
	//printf("Started!\n");
    WDTClear();

	enable();
    WDTClear();
    WDTClear();

    while(1){			// Main loop
		WDTClear();
		scheduler();
		 event = WaitSignal(time_signal, SIG_M8002);
 		 if(event & SIG_M8002){
			k++;
			k %= 16;
		  //	if(DIGet(1))
				//printf("running\n");
		}
	}
}

  


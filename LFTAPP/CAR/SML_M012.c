#define  M012_GLOBALS
#include "includes.h"
extern INT8U const BitMapTbl[];

/*
*********************************************************************************************************
//	M0, M1, M2						//MMxx xxxx
// #define 	M0			0x00	
// #define 	M1			0x40
// #define 	M2			0x80
*********************************************************************************************************
*/
void SFunc_M_Init(void)
{
	if(CAR_M012 == M0){
		StateShift(A_INIT);
	}
	else if(CAR_M012 == M1){
		StateShift(I_INIT);
	}
	else if(CAR_M012 == M2){
		StateShift(E_INIT);
	}
	else{
		//sys error code = ?
	}
}


void setup_RunCar(void)			// Power Up State 00
{
	LFT_Carcb.State.curState = 0x00;
	StateShift(M_INIT);

	Set_CarMode(0);
}

void task_RunCar(void)
{
	static INT8U M012, event;

	Car_Position();
	event = WaitSignal(M012_Signal, SIG_M012TICK);  		// 25ms interval	 
	
	if(event & SIG_M012TICK){
	   	Car_Load();		Car_Mode();		
		Car_Dir();
		if(LFT_Carcb.State.Timer < 0xFFFF)
			LFT_Carcb.State.Timer++;

		if(LFT_Faultcb.CtrReg &	EMG_ON){				// Emergency Alarm ON
			CAR_M012 = M2;
		}
		else{
			if(DIGet(DI_INSP)){
				CAR_M012 = M1;
			}
			else{
				CAR_M012 = M0;
			}
		}

		M012 = LFT_Carcb.State.curState & CAR_MSTATE;
		if(M012 == MINIT){
			SFunc_M_Init();
		}				
		else if(M012 == M0){
			if(CAR_M012 != M0){
				StateShift(M_INIT);
				return;
			}
			else{
				SAuto_Run( );
			}
		}
		else if(M012 == M1){
			if(CAR_M012 != M1){
				StateShift(M_INIT);
				return;
			}
			else{
				SInsp_Run( );
			}
		}
		else if(M012 == M2){
			if(CAR_M012 != M2){
				StateShift(M_INIT);
				return;
			}
			else{
				SEmg_Run( );
			}
		}
		else{
			StateShift(M_INIT);
		}
	}
}

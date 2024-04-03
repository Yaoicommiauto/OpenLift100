#define  CAR_GLOBALS
#include "includes.h"
extern INT8U const BitMapTbl[];

void Car_Load(void)
{
	if(Get_CCUDI(CDI_OVLD, FRONT) || Get_CCUDI(CDI_OVLD, REAR)){
		LFT_Carcb.Load = OVERLOAD;		
	}
	else if(Get_CCUDI(CDI_FULL, FRONT) || Get_CCUDI(CDI_FULL, REAR)){
		LFT_Carcb.Load = FULLLOAD;		
	}
	else if(Get_CCUDI(CDI_LLD, FRONT) || Get_CCUDI(CDI_LLD, REAR)){
		LFT_Carcb.Load = LIGHTLOAD;		
	}
	else{
		LFT_Carcb.Load = NOLOAD;		
	}
}

void CAR_ShiftMode(INT8U mode)
{
	if(LFT_Carcb.Mode < mode){
		LFT_Carcb.Mode = mode;
	}
}


/*
*********************************************************************************************************
*								 
*********************************************************************************************************
*/
void StateShift(INT8U newState)
{
	LFT_Carcb.State.Timer = 0;								//Timer restart
	LFT_Carcb.State.PreState = LFT_Carcb.State.curState;
	LFT_Carcb.State.curState = newState;
	LFT_Carcb.State.isShift = TRUE;
}


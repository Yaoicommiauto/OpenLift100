#define  DOOR_GLOBALS
#include "includes.h"
extern INT8U const BitMapTbl[];
#pragma locate(LFT_Dcb	= 0x010A)

void Set_DoorMode(INT8U mode)
{
	LFT_Dcb.Mode = 0;
	LFT_Dcb.Mode = mode;
}


BOOLEAN anyCLSw(INT8U fr)
{
	INT16U event;

	if(LFT_Dcb.Mode & ACL_KILL){
		event = 0;
		event |= AnyCall_Above(LFT_Slctcb.FlrNo);
		event |= AnyCall_Under(LFT_Slctcb.FlrNo);
	}
	else
		event = 1;
	
	if(CCU_DIGet(fr, CDI_CLOSE) && event)
		return TRUE;
	else
		return FALSE;

}
BOOLEAN anyOPSw(INT8U fr)
{
	if(LFT_Dcb.Mode & AOP_BTNONLY){
		return CCU_DIGet(fr, CDI_OPEN);
	}
	else{
		if(CCU_DIGet(fr, CDI_OPEN) || CCU_DIGet(fr, CDI_OPNSR)){
			return TRUE;
		}
		else{
			return FALSE;
		}
	}
}

BOOLEAN isDoorClosed(void)
{
	INT16U event, i;

	event = TRUE;
	if(DIGet(DI_DRELAY) == 0){
		event = FALSE;
	}
	else{
		for(i = 0; i < 2; i++){	
			if(LFT_Dcb.State[i].curState != DOOR_CLOSED){ 
				event = FALSE;
			}
		}
	}
	return event;
}




/*
*********************************************************************************************************
*								 
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*								 
*********************************************************************************************************
*/
void DoorShift(INT8U fr, INT8U newState)
{
	LFT_Dcb.State[fr].Timer = 0;								//Timer restart
	LFT_Dcb.State[fr].PreState = LFT_Dcb.State[fr].curState;
	LFT_Dcb.State[fr].curState = newState;
	LFT_Dcb.State[fr].isShift = TRUE;
}


/*
*********************************************************************************************************
*								 
*********************************************************************************************************
*/
void SDoor_Wait(INT8U fr)								
{
	if(LFT_Dcb.State[fr].isShift){
		LFT_Dcb.State[fr].isShift = FALSE;
	}
	if(LFT_Dcb.State[fr].Timer < 10)	return;

	if(LFT_Slctcb.DZone == FALSE || CAR_M012 != M0){
		DoorShift(fr, DOOR_CLOSE);	
	}
}

void closeState(INT8U fr)
{
	INT16U event;
	if(LFT_Dcb.State[(fr + 1) % 2].curState == DOOR_OPEN ||
	   LFT_Dcb.State[(fr + 1) % 2].curState == DOOR_OPENED)
	   return;

	if(LFT_Dcb.State[fr].Timer >= (INT16U)(fr + 1)){
		event = CCALL | VCALL;
		if((AnyCall_ThisFlr(fr) & event) || anyOPSw(fr))
			DoorShift(fr, DOOR_OPEN);
	}
	else if(LFT_Dcb.State[fr].Timer >= (INT16U)(fr + 3)){
		event = 0;
		if(LFT_Carcb.Direct != CUR_UP){
			event = HDCALL;
		}
		if(LFT_Carcb.Direct != CUR_DN){
			event = HUCALL;
		}
		if(AnyCall_ThisFlr(fr) & event)
			DoorShift(fr, DOOR_OPEN);
	}
}

void SDoor_Closed(INT8U fr)								
{
	if(LFT_Dcb.State[fr].isShift){
		LFT_Dcb.State[fr].isShift = FALSE;
	}
	if(LFT_Dcb.Type[fr] == DOOR_DISABLE)
		return;

	if((LFT_Dcb.StaReg[fr] & DOOR_CL) == 0){
		DoorShift(fr, DOOR_CLOSE);	
		return;
	}

	if(LFT_Fcb[LFT_Slctcb.FlrNo].breg.HAVECAR){
		closeState(fr);
	}
	else{
		LFT_Dcb.State[fr].Timer = 0;
	}
		
}
void SDoor_Close(INT8U fr)								
{
	if(LFT_Dcb.State[fr].isShift){
		LFT_Dcb.State[fr].isShift = FALSE;
	}

	if(LFT_Fcb[LFT_Slctcb.FlrNo].breg.HAVECAR){
		closeState(fr);
	}
	if(LFT_Dcb.StaReg[fr] & DOOR_CL){
		DoorShift(fr, DOOR_CLOSED);	
		return;
	}

	if(LFT_Dcb.State[fr].Timer > ((INT16U)LFT_Dcb.Dcp * 10)){
		if(LFT_Fcb[LFT_Slctcb.FlrNo].breg.HAVECAR){
			DoorShift(fr, DOOR_OPEN);
	   	}
	   	else{
	   		DoorShift(fr, DOOR_INIT);
	   	}
	}
}

void SDoor_Opened(INT8U fr)
{
	if(LFT_Dcb.State[fr].isShift){
		LFT_Dcb.State[fr].isShift = FALSE;
		if(LFT_Dcb.Mode & ACL_KILL)
			LFT_Dcb.DWait[fr] = 0;
		else{
			if(fr = FRONT)
				LFT_Dcb.DWait[fr] =	
				Register_Get(EE_CDT + LFT_Fcb[LFT_Slctcb.FlrNo].breg.DFWait);
			else
				LFT_Dcb.DWait[fr] =	
				Register_Get(EE_CDT + LFT_Fcb[LFT_Slctcb.FlrNo].breg.DRWait);
		}
	}
	if(anyOPSw(fr) == FALSE && LFT_Carcb.Load != OVERLOAD)
		return;

	if(LFT_Dcb.DWait[fr] > 0){		// auto close
	 	if(LFT_Dcb.State[fr].Timer > LFT_Dcb.DWait[fr]){
	 		DoorShift(fr, DOOR_CLOSE);	
	 		return;
	 	}
	}
	if(anyCLSw(fr)){
		DoorShift(fr, DOOR_CLOSE);	
		return;			   
	}
}

void SDoor_Open(INT8U fr)								
{
	if(LFT_Dcb.State[fr].isShift){
		LFT_Dcb.State[fr].isShift = FALSE;
	}

	if((LFT_Dcb.Mode & AOP_REV) ||
		LFT_Dcb.State[fr].Timer > ((INT16U)LFT_Dcb.Dcp * 10)){
		if(anyOPSw(fr) == FALSE){
			DoorShift(fr, DOOR_CLOSE);	
			return;
		}
	}

	if(LFT_Dcb.StaReg[fr] & DOOR_OL){
		DoorShift(fr, DOOR_OPENED);	
		return;
	}
}

void Doors_Reg(void)
{
	INT8U FR;

	LFT_Dcb.PreReg |= Register_Get(EE_POE);

	LFT_Dcb.Dop = Register_Get(EE_DOP + FR);	// Open/Close Door Protective Time
	LFT_Dcb.Dcp = Register_Get(EE_DCP + FR);	// Open/Close Door Protective Time

	for(FR = 0; FR < 2; FR++){
		LFT_Dcb.Type[FR]   = Register_Get(EE_FDS + FR);			// Front/Rear Door Selecting (Type)
	}
}

void setup_Doors(void)
{
	static INT8U FR;

	LFT_Dcb.Mode = 0x00;
	LFT_Dcb.PreReg  = 0;
	Doors_Reg();
/*		
		if(LFT_Dcb.Type[FR] == DOOR_DISABLE){
			LFT_Dcb.State[FR].curState = DOOR_CLOSED;	// Front/Rear State number
			DoorShift(FR, DOOR_CLOSED);
		}
		else{
*/
	for(FR = 0; FR < 2; FR++){
		LFT_Dcb.State[FR].curState = DOOR_INIT;	// Front/Rear State number
		DoorShift(FR, DOOR_INIT);
	}
}
void task_RunDoor(void)
{
	static INT8U FR, event;
	static INT8U x, y;
	event = WaitSignal(DOOR_Signal, SIG_DOORTICK);  // 100 ms interval

	if(event & SIG_DOORTICK){
		FR = EEGRP_DOOR;
		y = FR >> 3; x = BitMapTbl[FR & 0x07];
		if(RAM_SSW[y] & x){
			Doors_Reg();
			RAM_SSW[y] &= ~x;
		}

		FR = 0;
		while(FR < 2){
			if(LFT_Dcb.State[FR].Timer < 0xFFFF)
				LFT_Dcb.State[FR].Timer++;

			if(LFT_Dcb.Type[FR] == DOOR_DISABLE){
				DoorShift(FR, DOOR_CLOSED);	
			}
			else{
				switch(LFT_Dcb.State[FR].curState){
				case DOOR_OPEN:
					SDoor_Open(FR); 
					break;
				case DOOR_OPENED:
					SDoor_Opened(FR);	
					break;
				case DOOR_CLOSE:
					SDoor_Close(FR);	
					break;
				case DOOR_CLOSED:
					SDoor_Closed(FR); 
					break;
				default:
					SDoor_Wait(FR);	break;
				}
			}
			FR++;
		}
	}
}

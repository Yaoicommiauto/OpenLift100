#define  FLR_GLOBALS
#include "includes.h"
//#pragma locate (LFT_Fcb		= 0x09F6)
extern INT8U const BitMapTbl[];


void Update_HaveCar(INT8U FlrNo)
{
	BOOLEAN event;
	
	event = FALSE;
	if(Auto_Phase() == 0)
		event = FALSE;
	else{
		if(FlrNo != LFT_Slctcb.FlrNo || LFT_Slctcb.DZone == FALSE)
			event = FALSE;
		else{
			if(!LFT_Fcb[FlrNo].breg.FLR_ENABLE  ||
   			   (!LFT_Fcb[FlrNo].breg.FDOOR_EN && !LFT_Fcb[FlrNo].breg.RDOOR_EN ))
				event = FALSE;
			else{
				if(LFT_Dcb.PreReg  == 0){
					if(DOGet(DO_BRC) && ifZVel())
						event = TRUE;
				}
				else{
					if(!ifOpVel())
						event = FALSE;
					else{
						if(LFT_Dcb.PreReg  & POPEN_EN){
							if(LFT_Dcb.PreReg & LOPEN_EN)
						   		event = PDoorPhase(1);
							else
						   		event = PDoorPhase(0);
						}
					}
				}
			}
		}
	}
	LFT_Fcb[FlrNo].breg.HAVECAR = (event == TRUE) ? 1 : 0;
}

void Update_UInter(INT8U FlrNo)
{
	BOOLEAN event;

	event = FALSE;
	if(FlrNo >= (DRegister_Get(FEE_FLR) - 1)){
		event = TRUE;
	}
	else{
		if(LFT_Fcb[FlrNo].breg.FCC_EN){
			if(CCB_JudgeOne(&(LFT_CCU[FRONT].LFT_Icb), FlrNo)){
				event = TRUE;
			}
			else if(CCB_JudgeOne(&(LFT_VCU[FRONT]), FlrNo)){
				event = TRUE;
			}
		}
		else if(LFT_Fcb[FlrNo].breg.RCC_EN){
			if(CCB_JudgeOne(&(LFT_CCU[REAR].LFT_Icb), FlrNo)){
				event = TRUE;
			}
			else if(CCB_JudgeOne(&(LFT_VCU[FRONT]), FlrNo)){
				event = TRUE;
			}
		}
		else if(LFT_Fcb[FlrNo].breg.FHU_EN){
			if(CCB_JudgeOne(&(LFT_HCU[FRONT].LFT_Hcb[UP]), FlrNo)){
				event = TRUE;
			}
		}
		else if(LFT_Fcb[FlrNo].breg.RHU_EN){
			if(CCB_JudgeOne(&(LFT_HCU[REAR].LFT_Hcb[UP]), FlrNo)){
				event = TRUE;
			}
		}
	}
	LFT_Fcb[FlrNo].breg.UINTER = event ? 1 : 0;
}

void Update_DInter(INT8U FlrNo)
{
	BOOLEAN event;

	event = FALSE;
	if(FlrNo == 0){
		event = TRUE;
	}
	else{
		if(LFT_Fcb[FlrNo].breg.FCC_EN){
			if(CCB_JudgeOne(&(LFT_CCU[FRONT].LFT_Icb), FlrNo)){
				event = TRUE;
			}
			else if(CCB_JudgeOne(&(LFT_VCU[FRONT]), FlrNo)){
				event = TRUE;
			}
		}
		else if(LFT_Fcb[FlrNo].breg.RCC_EN){
			if(CCB_JudgeOne(&(LFT_CCU[REAR].LFT_Icb), FlrNo)){
				event = TRUE;
			}
			else if(CCB_JudgeOne(&(LFT_VCU[FRONT]), FlrNo)){
				event = TRUE;
			}
		}
		else if(LFT_Fcb[FlrNo].breg.FHU_EN){
			if(CCB_JudgeOne(&(LFT_HCU[FRONT].LFT_Hcb[DN]), FlrNo)){
				event = TRUE;
			}
		}
		else if(LFT_Fcb[FlrNo].breg.RHU_EN){
			if(CCB_JudgeOne(&(LFT_HCU[REAR].LFT_Hcb[DN]), FlrNo)){
				event = TRUE;
			}
		}
	}
	LFT_Fcb[FlrNo].breg.DINTER = event ? 1 : 0;
}


void setup_Floor(void)
{
	INT8U i, Floors;
	INT16U fcw;
	INT16U Door[2];

	for(i = 0; i < 2; i++){
		Door[i] = Register_Get(EE_FDS + i);
	}

	Floors = (INT8U)DRegister_Get(FEE_FLR);
	for(i = 0; i < MAX_FLOORS; i++){
		fcw = Register_Get(EE_FCW_BASE + i) & 0x01FF;
		LFT_Fcb[i].wreg = fcw;
		if(LFT_Fcb[i].breg.FLR_ENABLE == 0){
			LFT_Fcb[i].wreg = 0;
		}
		else{
			if(Door[FRONT] == DOOR_DISABLE){
				LFT_Fcb[i].hbreg.F_EN = 0;
			}
			if(Door[REAR] == DOOR_DISABLE){
				LFT_Fcb[i].hbreg.R_EN = 0;
			}
		}
	}
}

void task_Floor(void)
{
	INT8U i, event;

	event = WaitSignal(FLR_Signal, SIG_FLRTICK);  	// 20ms interval	 
	if((event & SIG_FLRTICK) && Auto_Phase()){
		Update_HaveCar(LFT_Slctcb.FlrNo);

		if((LFT_Carcb.Direct & CUR_UP) == 0){			// No Up
			for(i = 0; i < LFT_Slctcb.FlrNo ; i++){
				Update_DInter(i);
			}
		}
		if((LFT_Carcb.Direct & CUR_DN) == 0){			// No Dn
			for(i = (LFT_Slctcb.FlrNo + 1); i < DRegister_Get(FEE_FLR); i++){
				Update_UInter(i);
			}
		}
	}
}

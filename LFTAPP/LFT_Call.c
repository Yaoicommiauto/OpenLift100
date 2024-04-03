#define  CALL_GLOBALS
#include "includes.h"
#pragma locate (LFT_CCU		= 0x0906)
#pragma locate (LFT_HCU		= 0x0924)
//#pragma locate (LFT_VCU		= 0x0958)


extern INT8U const BitMapTbl[];
extern INT8U const BitAboveTbl[];
extern INT8U const BitUnderTbl[];

static INT8U FlrX, FlrY, FlrBitX, FlrBitY, FlrAboveY, FlrAboveX, FlrUnderY, FlrUnderX;
void CCB_Clear(LFT_CCB *CallCB)
{
	INT8U i;
	CallCB->CallGrp = 0;
	for(i = 0; i < 4; i++){
		CallCB->LFT_CallBuf[i] = 0;
		//CallCB->LFT_LightBuf[i] = 0;
	}
	//CallCB->CCBCtr |= CALL_EVENT;
}

void Set_CallMode(LFT_CCB *CallCB, INT8U mode)
{
	CallCB->CCBMode = mode;
	if(mode > FROZEN){
		CCB_Clear(CallCB);
	}
}
void Set_CCallMode(INT8U mode)
{
	INT8U i;
	for(i = 0; i < 2; i++){
		if(Register_Get(EE_FDS + i) != DOOR_DISABLE){
			Set_CallMode(&(LFT_CCU[i].LFT_Icb), mode);
		}
		else{
			Set_CallMode(&(LFT_CCU[i].LFT_Icb), KILL);
		}
	}
}
void Set_VCallMode(INT8U mode)
{
	INT8U i;
	for(i = 0; i < 2; i++){
		if(Register_Get(EE_FDS + i) != DOOR_DISABLE){
			Set_CallMode(&(LFT_VCU[i]), mode);
		}
		else{
			Set_CallMode(&(LFT_VCU[i]), KILL);
		}
	}
}

void Set_HUCallMode(INT8U mode)
{
	INT8U i;
	for(i = 0; i < 2; i++){
		if(Register_Get(EE_FDS + i) != DOOR_DISABLE){
			Set_CallMode(&(LFT_HCU[i].LFT_Hcb[UP]), mode);
		}
		else{
			Set_CallMode(&(LFT_HCU[i].LFT_Hcb[UP]), KILL);
		}
	}
}

void Set_HDCallMode(INT8U mode)
{
	INT8U i;
	for(i = 0; i < 2; i++){
		if(Register_Get(EE_FDS + i) != DOOR_DISABLE){
			Set_CallMode(&(LFT_HCU[i].LFT_Hcb[DN]), mode);
		}
		else{
			Set_CallMode(&(LFT_HCU[i].LFT_Hcb[DN]), KILL);
		}
	}
}

void ClearCCall(void)
{	
	INT8U i;
	for(i = 0; i < 2; i++){
		CCB_Clear(&(LFT_CCU[i].LFT_Icb));
	}
}

void ClearVCall(void)
{	
	INT8U i;
	for(i = 0; i < 2; i++){
		CCB_Clear(&(LFT_VCU[i]));
	}
}

void ClearHCall(void)
{	
	INT8U i;
	for(i = 0; i < 2; i++){
		CCB_Clear(&(LFT_HCU[i].LFT_Hcb[UP]));
	}
}

void CCB_AddOne(LFT_CCB *CallCB, INT8U FlrNo)
{
	FlrY	 = FlrNo >> 3;
	FlrX	 = FlrNo & 0x07; 
	FlrBitX	 = BitMapTbl[FlrX];
	FlrBitY	 = BitMapTbl[FlrY];

	CallCB->CallGrp |= FlrBitY;
	CallCB->LFT_CallBuf[FlrY]  |= FlrBitX;
	//CallCB->LFT_LightBuf[FlrY] |= FlrBitX;
}

void CCB_ClearOne(LFT_CCB *CallCB, INT8U FlrNo)
{
//	INT8U FlrX, FlrY, FlrBitX, FlrBitY;

	FlrY	 = FlrNo >> 3;
	FlrX	 = FlrNo & 0x07; 
	FlrBitX	 = BitMapTbl[FlrX];
	FlrBitY	 = BitMapTbl[FlrY];

	//CallCB->LFT_LightBuf[FlrY]  &= ~FlrBitX;
	if((CallCB->LFT_CallBuf[FlrY]  &= ~FlrBitX) == 0){
		CallCB->CallGrp &= ~FlrBitY;
	}
}

BOOLEAN CCB_JudgeOne(LFT_CCB *CallCB, INT8U FlrNo)
{
//	INT8U FlrX, FlrY, FlrBitX, FlrBitY;

	FlrY	 = FlrNo >> 3;
	FlrX	 = FlrNo & 0x07; 
	FlrBitX	 = BitMapTbl[FlrX];
	FlrBitY	 = BitMapTbl[FlrY];

	if(CallCB->CCBMode == FROZEN) return 0;

	if((CallCB->LFT_CallBuf[FlrY] & FlrBitX) == 0){
		return FALSE;
	}
	else
		return TRUE;
}

void ClearCall_ThisFlr(INT8U fr)
{
	INT8U FlrNo, i;

	FlrNo = LFT_Slctcb.FlrNo;
	CCB_ClearOne(&(LFT_CCU[fr].LFT_Icb), FlrNo);
	CCB_ClearOne(&LFT_VCU[fr], FlrNo);

	if(LFT_Carcb.Direct != CUR_UP){
		CCB_ClearOne(&(LFT_HCU[fr].LFT_Hcb[DN]), FlrNo);
	}
	if(LFT_Carcb.Direct != CUR_DN){
		CCB_ClearOne(&(LFT_HCU[fr].LFT_Hcb[UP]), FlrNo);
	}
}

// Judge Call & Clear it
INT16U AnyCall_ThisFlr(INT8U fr)
{
	INT8U FlrNo;
	INT16U event;

	FlrNo = LFT_Slctcb.FlrNo;
	event = 0;

	if(CCB_JudgeOne(&(LFT_CCU[fr].LFT_Icb), FlrNo)){
		event |= CCALL;
	}
	
	if(CCB_JudgeOne(&(LFT_VCU[fr]), FlrNo)){
		event |= VCALL;
	}
	if(CCB_JudgeOne(&(LFT_HCU[fr].LFT_Hcb[UP]), FlrNo)){
			event |= HUCALL;
	}
	if(CCB_JudgeOne(&(LFT_HCU[fr].LFT_Hcb[DN]), FlrNo)){
			event |= HDCALL;
	}
	
	return event;
}

BOOLEAN CCB_JudgeAbove(LFT_CCB *CallCB, INT8U FlrNo)
{
//	INT8U FlrX, FlrY, FlrAboveY, FlrAboveX;

	FlrY	  = FlrNo >> 3;
	FlrX	  = FlrNo & 0x07; 
	FlrAboveY = BitAboveTbl[FlrY]; 
	FlrAboveX = BitAboveTbl[FlrX];
	
	if(CallCB->CCBMode == FROZEN) return 0;

	if(CallCB->CallGrp & FlrAboveY){
		return TRUE;
	}
	else if(CallCB->LFT_CallBuf[FlrY] & FlrAboveX){
		return TRUE;
	}
	else{
		return FALSE;
	}
}
BOOLEAN CCB_JudgeUnder(LFT_CCB *CallCB, INT8U FlrNo)
{
  //	INT8U FlrX, FlrY, FlrUnderY, FlrUnderX;

	FlrY	  = FlrNo >> 3;
	FlrX	  = FlrNo & 0x07; 
	FlrUnderY = BitUnderTbl[FlrY]; 
	FlrUnderX = BitUnderTbl[FlrX];

	if(CallCB->CCBMode == FROZEN) return 0;

	if(CallCB->CallGrp & FlrUnderY){
		return TRUE;
	}
	else if(CallCB->LFT_CallBuf[FlrY] & FlrUnderX){
		return TRUE;
	}
	else{
		return FALSE;
	}
}


INT16U AnyCall_Above(INT8U FlrNo)
{
	INT16U event;

	event = 0;
	if(CCB_JudgeAbove(&(LFT_CCU[FRONT].LFT_Icb), FlrNo)){
			event |= CCALL;
	}
	if(CCB_JudgeAbove(&(LFT_CCU[REAR].LFT_Icb), FlrNo)){
			event |= CCALL;
	}
	if(CCB_JudgeAbove(&(LFT_VCU[FRONT]), FlrNo)){
			event |= VCALL;
	}
	if(CCB_JudgeAbove(&(LFT_VCU[REAR]), FlrNo)){
			event |= VCALL;
	}

	if(CCB_JudgeAbove(&(LFT_HCU[FRONT].LFT_Hcb[0]), FlrNo)){
			event |=  HUCALL;
	}
	if(CCB_JudgeAbove(&(LFT_HCU[FRONT].LFT_Hcb[1]), FlrNo)){
			event |=  HDCALL;
	}
	if(CCB_JudgeAbove(&(LFT_HCU[REAR].LFT_Hcb[0]), FlrNo)){
			event |=  HUCALL;
	}
	if(CCB_JudgeAbove(&(LFT_HCU[REAR].LFT_Hcb[1]), FlrNo)){
			event |=  HDCALL;
	}
	return event;
}


INT16U AnyCall_Under(INT8U FlrNo)
{
	INT16U event;

	event = 0;
	if(CCB_JudgeUnder(&(LFT_CCU[FRONT].LFT_Icb), FlrNo)){
			event |= CCALL;
	}
	if(CCB_JudgeUnder(&(LFT_CCU[REAR].LFT_Icb), FlrNo)){
			event |= CCALL;
	}
	if(CCB_JudgeUnder(&(LFT_VCU[FRONT]), FlrNo)){
			event |= VCALL;
	}
	if(CCB_JudgeUnder(&(LFT_VCU[REAR]), FlrNo)){
			event |= VCALL;
	}
	if(CCB_JudgeUnder(&(LFT_HCU[FRONT].LFT_Hcb[0]), FlrNo)){
			event |=  HUCALL;
	}
	if(CCB_JudgeUnder(&(LFT_HCU[FRONT].LFT_Hcb[1]), FlrNo)){
			event |=  HDCALL;
	}
	if(CCB_JudgeUnder(&(LFT_HCU[REAR].LFT_Hcb[0]), FlrNo)){
			event |=  HUCALL;
	}
	if(CCB_JudgeUnder(&(LFT_HCU[REAR].LFT_Hcb[1]), FlrNo)){
			event |=  HDCALL;
	}
	return event;
}


BOOLEAN	HCU_DIGet(INT8U fr, INT8U n)
{
	INT8U val;

	val = LFT_HCU[fr].HCU_DI;
	if(val & BitMapTbl[n]){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

BOOLEAN	CCU_DIGet(INT8U FR, INT8U chn)
{
	INT8U DIX, DIY;
	if(chn < CCU_MAX_DI){
	 	DIY	 = chn >> 3;
		DIX	 = chn & 0x07; 
		if(LFT_CCU[FR].CCU_DI[DIY] & BitMapTbl[DIX]){
			return TRUE;
		}
		else{
			return FALSE;
		}
	}
	else
		return FALSE;
}

// from Hall or Single GC Call Event	
void Call_Event(LFT_CCB *CallCB, INT8U FlrNo, INT8U CallV)
{
	if(CallCB->CCBMode != KILL){
		if(CallV == 0x01){										// P shift
			if(CallCB->CCBMode == LASTONE || CallCB->CCBMode == CONHOLD){		
				CCB_Clear(CallCB);							// Clear all
			}
			else if(CallCB->CCBMode == PPAANG){
				if(CCB_JudgeOne(CallCB, FlrNo)){
					CCB_ClearOne(CallCB, FlrNo);
				}
				else{
					CCB_AddOne(CallCB, FlrNo);
				}
			}
			else{
				CCB_AddOne(CallCB, FlrNo);
			}
		}
		else if(CallV == 0x00){									// N shift
			if(CallCB->CCBMode == CONHOLD){
				CCB_Clear(CallCB);
			}
		}
	}
}

void setup_Call(void)
{
	ClearCCall(); 	ClearVCall();	ClearHCall();
}

void task_Call(void)
{
	INT8U i, event;

	event = WaitSignal(CALL_Signal, SIG_FLRTICK);  	// 100ms interval
	if(event & SIG_CALLTICK){
		if(CAR_M012 != M0){
			Set_CCallMode(KILL);	Set_VCallMode(KILL);
			if(Register_Get(EE_GLA) == 0){
				Set_HUCallMode(KILL);	Set_HDCallMode(KILL);
			}
		}
	}
	else{
		for(i = 0; i < 2; i++){
			if(LFT_Dcb.State[i].curState == DOOR_OPEN){
				if(LFT_Dcb.State[i].Timer >= 1){
					ClearCall_ThisFlr(i);
				}
			}
		}
	}
}		
			

#define  INSP_GLOBALS
#include "includes.h"
extern INT8U const BitMapTbl[];

void SFunc_I_Init(void)	
{
	INT16U event;
	if(LFT_Carcb.State.isShift){		
		LFT_Carcb.State.isShift = FALSE;
	}

	if(LFT_Carcb.StaReg & BitMapTbl[TEACHED]){		   
		StateShift(A_WAIT);
		return;
	}
	else{  
		if(DIGet(DI_DRELAY)){	  
			StateShift(I_CHECK);
		}
	}
}

/*
*********************************************************************************************************
*									   Inspection
*********************************************************************************************************
*/
void SFunc_I_Check(void)
{
	INT8U i;
	if(LFT_Carcb.State.isShift){	   
		LFT_Carcb.State.isShift = FALSE;
	}

	if(DIGet(DI_DRELAY)){				//2s ??
		if(LFT_Carcb.State.Timer > 10){
			if(!DIGet(DI_UL)){
				if(DIGet(DI_ICU) && !DIGet(DI_ICD)){
					StateShift(IU_START);
					return;
				}
			}
			if(!DIGet(DI_DL)){
				if((!DIGet(DI_ICU)) && DIGet(DI_ICD) ){
					StateShift(ID_START);
					return;
				}
			}
		}
	}
	else{
		StateShift(I_INIT);
		return;
	}
}
/*
*********************************************************************************************************
*									   Inspection urunning Phase
*********************************************************************************************************
*/
void SFunc_IU_Start(void) 				
{	
	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_BPT = Register_Get(EE_BPT);
	}
	if(DIGet(DI_ICU) && !DIGet(DI_UL)){  
		if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_BPT){
			StateShift(IU_RUN);	 
		}
	}
	else{
		StateShift(I_CHECK);
	}
}

void SFunc_IU_Run(void) 				
{	
	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
	}

	if(!DIGet(DI_ICU) || DIGet(DI_UL)){  
		StateShift(IU_STOP);
	}
}
void SFunc_IU_Stop(void) 				
{	
	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_MDT = Register_Get(EE_MDT);
	}
	if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_MDT){
		StateShift(I_CHECK); 
		return;
	}
}
void SFunc_ID_Start(void) 				
{	
	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_BDT = Register_Get(EE_BDT);
	}

	if(DIGet(DI_ICD) && !DIGet(DI_DL)){  
		if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_BDT){
			StateShift(ID_RUN);	 
		}
	}
	else{
		StateShift(I_CHECK);
	}
}

void SFunc_ID_Run(void) 				
{	
	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
	}

	if(!DIGet(DI_ICD) || DIGet(DI_DL)){  
		StateShift(ID_STOP);
	}
}
void SFunc_ID_Stop(void) 				
{	
	SFunc_IU_Stop();
}


INT8U Insp_Phase(void)
{
	if(LFT_Carcb.State.curState >= IU_START && LFT_Carcb.State.curState <= IU_STOP){
		return I_UP;
	}
	else if(LFT_Carcb.State.curState >= ID_START && LFT_Carcb.State.curState <= ID_STOP){
		return I_DN;
	}
	else {
		return 0;
	}
}

/*
*********************************************************************************************************
*										Run the sub-machine
*********************************************************************************************************
*/
void SInsp_Run(void)
{
  	if(LFT_Carcb.State.curState == I_INIT){
		SFunc_I_Init();
	}
	else if(LFT_Carcb.State.curState ==	I_CHECK){
		SFunc_I_Check();
	}
	else if(LFT_Carcb.State.curState ==	IU_START){
		SFunc_IU_Start();
	}
	else if(LFT_Carcb.State.curState ==	IU_RUN){
		SFunc_IU_Run();
	}
	else if(LFT_Carcb.State.curState ==	IU_STOP){
		SFunc_IU_Stop();
	}
	else if(LFT_Carcb.State.curState ==	ID_START){
		SFunc_ID_Start();
	}
	else if(LFT_Carcb.State.curState ==	ID_RUN){
		SFunc_ID_Run();
	}
	else if(LFT_Carcb.State.curState ==	ID_STOP){
		SFunc_ID_Stop();
	}
	else{
	}
}

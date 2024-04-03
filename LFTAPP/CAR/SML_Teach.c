#define  TEACH_GLOBALS
#include "includes.h"
extern INT8U const BitMapTbl[];

/*
*********************************************************************************************************
*									   Teach
*********************************************************************************************************
*/

void SFunc_T_Check(void)											// 
{
	INT8U status;
	if(LFT_Carcb.State.isShift){								// Hook?
		LFT_Carcb.State.isShift = FALSE;
   		if(status){
			//error report
			LFT_Faultcb.CtrReg |= EMG_ON;
		}
	}

	if(DRegister_Get(FEE_TSR) == TEACH_RESET){
		if(DIGet(DI_DL)){
				StateShift(TU_START);
		}
		if(DIGet(DI_UL)){
				StateShift(TU_START);
		}
		else{
			StateShift(TF_START);
		}
	}
}
/*
*********************************************************************************************************
*									   Teach Finding Phase
*********************************************************************************************************
*/
void SFunc_TF_Start(void) 				
{	
	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_BPT = Register_Get(EE_BPT);
	}

	if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_BPT){
		if(DIGet(DI_PLD)){
			StateShift(TF_RUNL);
			return;
		}
		else{
			StateShift(TF_RUNH);
			return;
		}
	}
}

void SFunc_TF_Runh(void) 				
{	
	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
	}
	if(DIGet(DI_PLD)){
		StateShift(TF_RUNL);
		return;
	}

}
void SFunc_TF_Runl(void) 				
{	

	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
	}
	if(!DIGet(DI_PLD)){
	// error
		return;
	}
	if(DIGet(DI_DL)){
		StateShift(TF_STOP);
		return;
	}
}

void SFunc_TF_Stop(void) 				
{	
	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_MDT = Register_Get(EE_MDT);
	}
	if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_MDT){
		StateShift(T_CHECK);
		return;
	}
}			
/*
*********************************************************************************************************
*									   Teach urunning Phase
*********************************************************************************************************
*/
void SFunc_TU_Start(void) 				
{	
	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
		setPosition(0);
		LFT_Drvcb.RAM_BPT = Register_Get(EE_BPT);
	}
	if(DIGet(DI_DLVL) && !DIGet(DI_ULVL)){  
		if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_BPT){
			StateShift(TU_RUNH );			// TU Running - h State
		}
	}
	else{
		//report error
	}
}

void SFunc_TU_Runh(void) 				
{	

	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
	}
	if(DIGet(DI_PLU)){
		StateShift(TU_RUNL );
	}
}

void SFunc_TU_Runl(void) 				
{	

	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
	}
	if(!DIGet(DI_PLU)){
		// error
		return;
	}

	if(DIGet(DI_UL)){
		StateShift(TU_STOP);						// TU Stop State
	}
}

void SFunc_TU_Stop(void) 				
{	
	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_MDT = Register_Get(EE_MDT);
	}
	if(LFT_Slctcb.FlrNo < 1){
		DRegister_Set(FEE_TSR, FLR_ERR0);
	}
	if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_MDT){
		DRegister_Set(FEE_FLR, LFT_Slctcb.FlrNo + 1);
		StateShift(T_CHECK);						// TD Start State
		return;
	}
}			

/*
*********************************************************************************************************
*									   Teach Drunning Phase
*********************************************************************************************************
*/
void SFunc_TD_Start(void) 				
{	
	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_BPT = Register_Get(EE_BPT);
	}

	if(!DIGet(DI_DLVL) && DIGet(DI_ULVL)){  
		if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_BPT){
			StateShift(TD_RUNH );
		}
	}
}

void SFunc_TD_Runh(void) 				
{	

	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
	}
	if(DIGet(DI_PLD)){
		StateShift(TD_RUNL);
	}
}

void SFunc_TD_Runl(void) 				
{	
	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
	}
	if(!DIGet(DI_PLD)){
		// error
		return;
	}
	if(DIGet(DI_DL)){
		StateShift(TD_STOP);
		return;
	}
}

void SFunc_TD_Stop(void) 				
{	
	INT8U i, Floors;
	long int p[2];

	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_MDT = Register_Get(EE_MDT);
	}
	// Verifying
	i = 1;
	Floors = DRegister_Get(FEE_FLR);

	while(i < Floors - 1){
		p[0] = DRegister_Get(FEE_DLVE_BASE + i * 2);
		p[0] += DRegister_Get(FEE_ULVE_BASE + i * 2);

		p[1] = DRegister_Get(FEE_DENT_BASE + i * 2);
		p[1] += DRegister_Get(FEE_UENT_BASE + i * 2);

		if(labs(p[0] - p[1]) > 30){					//?
		  //	error position
		  break;
		}
		else{
			DRegister_Set(i, (INT32U)(p[0] + p[1]) / 4); 
		}
	}	

	if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_MDT){
		if(DRegister_Get(FEE_TSR) == TEACH_RESET)
			DRegister_Set(FEE_TSR, TEACH_OK);
		
		StateShift(A_INIT);					// TD Start State
		return;
	}
}

INT8U Teach_Phase(void)
{
	if(LFT_Carcb.State.curState >= TF_START && LFT_Carcb.State.curState <= TF_STOP){
		return T_FIND;
	}
	else if(LFT_Carcb.State.curState >= TU_START && LFT_Carcb.State.curState <= TU_STOP){
		return T_UP;
	}
	else if(LFT_Carcb.State.curState >= TD_START && LFT_Carcb.State.curState <= TD_STOP){
		return T_DN;
	}
	else if(LFT_Carcb.State.curState == T_CHECK){
		return T_STOP;
	}
	else{
		return 0;
	}
}
/*
*********************************************************************************************************
*										Run the sub-machine
*********************************************************************************************************
*/
void STeach_Run(void)
{
 	if(WaitSignal(CAR_signal, SIG_TECH)){				// wait signal
		DRegister_Set(FEE_TSR, TEACH_ABORT);
		StateShift(T_CHECK);							// for shaft Teach In
		return;
	}

	if(LFT_Carcb.State.curState == T_CHECK){
		SFunc_T_Check();
	}
	else if(LFT_Carcb.State.curState == TF_START){
		SFunc_TF_Start();
	}
	else if(LFT_Carcb.State.curState == TF_RUNH){
		SFunc_TF_Runh();
	}
	else if(LFT_Carcb.State.curState == TF_RUNL){
		SFunc_TF_Runl();
	}
	else if(LFT_Carcb.State.curState == TF_STOP){
		SFunc_TF_Stop();
	}
	else if(LFT_Carcb.State.curState == TU_START){
		SFunc_TU_Start();
	}
	else if(LFT_Carcb.State.curState == TU_RUNH){
		SFunc_TU_Runh();
	}
	else if(LFT_Carcb.State.curState == TU_RUNL){
		SFunc_TU_Runl();
	}
	else if(LFT_Carcb.State.curState == TU_STOP){
		SFunc_TU_Stop();
	}
	else if(LFT_Carcb.State.curState == TD_START){
		SFunc_TD_Start();
	}
	else if(LFT_Carcb.State.curState == TD_RUNH){
		SFunc_TD_Runh();
	}
	else if(LFT_Carcb.State.curState == TD_RUNL){
		SFunc_TD_Runl();
	}
	else if(LFT_Carcb.State.curState == TD_STOP){
		SFunc_TD_Stop();
	}
	else{
		 // state error and LFT_Carcb.PreState  = ???
	}
}

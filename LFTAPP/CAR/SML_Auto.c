#define  AUTO_GLOBALS
#include "includes.h"
extern INT8U const BitMapTbl[];


void SFunc_A_Init(void)
{
	INT8U status; 

	if(LFT_Carcb.State.isShift){							// Hook?
		LFT_Carcb.State.isShift = FALSE;
	}

 	if(WaitSignal(CAR_signal, SIG_TECH)){				// wait signal
  		status = EEPROM_Format(EEPROM_RECORD_NUM); 
		if(status){
			// report error
		}
		else{
			status = DRegister_Set(FEE_TSR, TEACH_RESET);
			if(status){
				// report error
			}
			else{
				StateShift(T_CHECK);							// for shaft Teach In
			 	return;
			}
		}
	}

	if(LFT_Carcb.StaReg & TEACHED){							// TEACHED?
		if(LFT_Carcb.State.Timer > 10){
			StateShift(A_WAIT);	// 1s
			return;
		}
	}
}

/*
*********************************************************************************************************
*								 
*********************************************************************************************************
*/
void SFunc_A_Wait(void)											// 
{
	INT8U k; INT16U event;
	if(LFT_Carcb.State.isShift){								// Hook?
		LFT_Carcb.State.isShift = FALSE;
		LFT_Carcb.Direct == 0;
	}

	if(!isDoorClosed()){
		StateShift(A_DOOR);
		return;
	}

	if(!LFT_Slctcb.DZone){
		LFT_Carcb.StaReg |= RELEVING;
	 	if(LFT_Carcb.State.Timer > 10){					// > 1s
			if(LFT_Slctcb.Direct != SLCT_DN){
				StateShift(AD_DRIVE);	
				return;
			}
			else{
				StateShift(AU_DRIVE);								// AU Finding
				return;
			}
		}
	}
	else{
		if(LFT_Carcb.State.PreState == A_INIT){
			Call_Event(&(LFT_VCU[FRONT]), LFT_Slctcb.FlrNo , 1);
			Call_Event(&(LFT_VCU[REAR]), LFT_Slctcb.FlrNo, 1);
			return;
		}
	}
	if(LFT_Carcb.Direct == CUR_UP){
		event = VCALL | CCALL | HUCALL | HDCALL;
		if(AnyCall_Above(LFT_Slctcb.FlrNo) & event){
			StateShift(AU_DRIVE);
			return;
	  	}
	}
	if(LFT_Carcb.Direct == CUR_UP){
		event = VCALL | CCALL | HUCALL | HDCALL;
		if(AnyCall_Under(LFT_Slctcb.FlrNo) & event){
			StateShift(AD_DRIVE);
			return;				 
	  	}		
	}

	if(LFT_Carcb.Mode ==INSTALL){
		if(LFT_Carcb.State.Timer > 10){
			StateShift(A_INIT);
			return;
		}
	}
	if((LFT_Carcb.StaReg & TEACHED) == 0){			// TEACHED?
		StateShift(A_INIT);
		return;
	}
}

void SFunc_A_Door(void)											// 
{
	INT8U k; INT16U event;
	if(LFT_Carcb.State.isShift){								// Hook?
		LFT_Carcb.State.isShift = FALSE;
	}
	if(LFT_Carcb.State.Timer < Register_Get(EE_BDL)){								// 1.5 s
		return;
	}

	if(isDoorClosed()){
		if(!LFT_Slctcb.DZone){
			LFT_Carcb.StaReg |= RELEVING;
			if(LFT_Slctcb.Direct != SLCT_DN)
				StateShift(AD_DRIVE);	
			else
				StateShift(AU_DRIVE);					// AU Finding
			return;
		}
		if(LFT_Carcb.Direct == CUR_UP){
			event = VCALL | CCALL | HUCALL |HDCALL;
			if(AnyCall_Above(LFT_Slctcb.FlrNo)){
				StateShift(AU_DRIVE);
				return;
			}
		}
		else if(LFT_Carcb.Direct == CUR_DN){			// Valid call under car
			event = VCALL | CCALL | HUCALL |HDCALL;
			if(AnyCall_Under(LFT_Slctcb.FlrNo)){
				StateShift(AD_DRIVE);
				return;
			}
		}
		else{
			if(LFT_Dcb.State[FRONT].Timer > Register_Get(EE_ADA) &&
			   LFT_Dcb.State[FRONT].Timer > Register_Get(EE_ADA)){
				StateShift(A_WAIT);
				return;
			}
		}
	}
}

/*
*********************************************************************************************************
*									Auto Up-Running
*********************************************************************************************************
*/
void SFunc_AU_Drive(void) 				
{	
	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_BPT = Register_Get(EE_BPT);
	}
	if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_BPT){
		//if(DIGet(DI_MC2)){
		StateShift(AU_START);
		return;
		//}
	}
}
	
void SFunc_AU_Start(void)
{	
	if(LFT_Carcb.State.isShift){								// Hook?
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_BPH = Register_Get(EE_BPH);
		if(LFT_Carcb.StaReg & RELEVING){
			SetStartFlr(LFT_Slctcb.FlrNo);
			SetDestFlr(LFT_Slctcb.FlrNo);
		}
		else{
			SetStartFlr(LFT_Slctcb.FlrNo);
			SetDestFlr(LFT_Slctcb.FlrNo + 1);
		}
	}
	if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_BPH){
		if(LFT_Carcb.StaReg & RELEVING){
			StateShift(AU_FIND);
			return;
		}
		else{
		 	StateShift(AU_VACC);
		 	return;
		}
	}
}
void SFunc_AU_Vacc(void)
{	
	if(LFT_Carcb.State.isShift){								// Hook?
		LFT_Carcb.State.isShift = FALSE;
	}
	if(vstate.curState > VCON){
		StateShift(AU_VDEC);
		return;
	}
	if(curPReach > LFT_Carcb1.DestEnt){
		if((LFT_Fcb[LFT_Carcb1.DestFlr].breg.UINTER) == 0){
			if(AnyCall_Above(LFT_Carcb1.DestFlr)){
				SetDestFlr(LFT_Carcb1.DestFlr++);
			}
		}
	}
}


void SFunc_AU_Vdec(void)
{	
	if(LFT_Carcb.State.isShift){								// Hook?
		LFT_Carcb.State.isShift = FALSE;
	}

	if(ifZVel()){
		StateShift(AU_STOP);
		return;
	}
}

void SFunc_AU_Stop(void)
{	
	if(LFT_Carcb.State.isShift){								// Hook?
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_BDT = Register_Get(EE_BDT);
	}
	if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_BDT){
		StateShift(AU_PARK);
	}
}

/*
*********************************************************************************************************
*									Auto Down-Running
*********************************************************************************************************
*/

void SFunc_AD_Drive(void) 				
{	
	if(LFT_Carcb.State.isShift){ 
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_BPT = Register_Get(EE_BPT);
	}
	if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_BPT){
		StateShift(AD_START);
		return;
	}
}
	
void SFunc_AD_Start(void)
{	
	if(LFT_Carcb.State.isShift){								// Hook?
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_BPH = Register_Get(EE_BPH);

		if(LFT_Carcb.StaReg & RELEVING){
			SetStartFlr(LFT_Slctcb.FlrNo);
			SetDestFlr(LFT_Slctcb.FlrNo);
		}
		else{
			LFT_Carcb1.StartFlr = LFT_Slctcb.FlrNo;
			if(LFT_Slctcb.FlrNo > 0){
				SetDestFlr(LFT_Slctcb.FlrNo - 1);
			}
		}
	}
	if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_BPH){
		if(LFT_Carcb.StaReg & RELEVING){
			StateShift(AD_FIND);
			return;
		}
		else{
			StateShift(AD_VACC);
			return;
		}
	}
}
void SFunc_AD_Vacc(void)
{	
	if(LFT_Carcb.State.isShift){								// Hook?
		LFT_Carcb.State.isShift = FALSE;
	}
	if(vstate.curState > VCON){
		StateShift(AD_VDEC);
		return;
	}
	if(curPReach < LFT_Carcb1.DestEnt){
		if((LFT_Fcb[LFT_Carcb1.DestFlr].breg.DINTER) == 0){
			if(AnyCall_Under(LFT_Carcb1.DestFlr)){
				if(LFT_Carcb1.DestFlr > 0){
					SetDestFlr(LFT_Carcb1.DestFlr--);
				}
			}
		}
	}
}


void SFunc_AD_Vdec(void)
{	
	if(LFT_Carcb.State.isShift){								// Hook?
		LFT_Carcb.State.isShift = FALSE;
	}

	if(ifZVel()){
		StateShift(AD_STOP);
		return;
	}
}

void SFunc_AD_Stop(void)
{	
	if(LFT_Carcb.State.isShift){								// Hook?
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_BDT = Register_Get(EE_BDT);
	}
	if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_BDT){
		StateShift(AD_PARK);
	}
}


/*
*********************************************************************************************************
*										Find					
*********************************************************************************************************
*/
void SFunc_AU_Find(void)
{	
	if(LFT_Carcb.State.isShift){								// Hook?
		LFT_Carcb.State.isShift = FALSE;
	}

	if(ifZVel()){
		StateShift(AU_STOP);
		return;
	}
}

void SFunc_AD_Find(void)
{	
	if(LFT_Carcb.State.isShift){								// Hook?
		LFT_Carcb.State.isShift = FALSE;
	}
	if(ifZVel()){
		StateShift(AU_STOP);
		return;
	}
}

/*
*********************************************************************************************************
*										STOP & PARK					
*********************************************************************************************************
*/

void SFunc_AU_Park(void)
{	
	if(LFT_Carcb.State.isShift){								// Hook?
		LFT_Carcb.State.isShift = FALSE;
		LFT_Drvcb.RAM_MDT = Register_Get(EE_MDT);
	}
	if(LFT_Carcb.State.Timer > LFT_Drvcb.RAM_MDT){
		StateShift(A_DOOR);
		LFT_Carcb.StaReg  &= ~RELEVING;						//?????
	}
}
void SFunc_AD_Park(void)
{	
	SFunc_AU_Park();
}

INT8U Auto_Phase(void)
{
	if(LFT_Carcb.State.curState >= AU_DRIVE && LFT_Carcb.State.curState <= AU_PARK){
		return AU_UP;
	}
	else if(LFT_Carcb.State.curState >= AD_DRIVE && LFT_Carcb.State.curState <= AD_PARK){
		return AU_DN;
	}
	else if(LFT_Carcb.State.curState >= A_WAIT && LFT_Carcb.State.curState <= A_DOOR){
		return A_STOP;
	}
	else{
		return 0;
	}
}

BOOLEAN PDoorPhase(INT8U relevel)
{
	INT8U k;

	k = Auto_Phase();

	if(relevel == 0){
		if(LFT_Carcb.StaReg & RELEVING)
			return FALSE;
	}

	if(k == AU_UP){
		if(LFT_Carcb.State.curState >= AU_DRIVE && LFT_Carcb.State.curState <= AU_VACC)
			return FALSE;
		else
			return TRUE;
	}
	else if(k == AU_DN){
		if(LFT_Carcb.State.curState >= AD_DRIVE && LFT_Carcb.State.curState <= AD_VACC)
			return FALSE;
		else
			return TRUE;
	}
	else if(k == A_STOP){
		return TRUE;
	}
	else
		return FALSE;
}


/*
*********************************************************************************************************
*										Run the sub-machine
*********************************************************************************************************
*/
void SAuto_Run(void)
{	
  	if(LFT_Carcb.State.curState == A_INIT){
  		SFunc_A_Init();
	}
	else if(LFT_Carcb.State.curState == A_WAIT){
		SFunc_A_Wait();
	}
	else if(LFT_Carcb.State.curState == A_DOOR){
		SFunc_A_Door();
	} 
	else if(LFT_Carcb.State.curState == AU_DRIVE){
		SFunc_AU_Drive();
	}
	else if(LFT_Carcb.State.curState == AU_START){
		SFunc_AU_Start();
	}
	else if(LFT_Carcb.State.curState == AU_VACC){
		SFunc_AU_Vacc();
	}
	else if(LFT_Carcb.State.curState == AU_VDEC){
		SFunc_AU_Vdec();
	}
	else if(LFT_Carcb.State.curState == AU_STOP){
		SFunc_AU_Stop();
	}
	else if(LFT_Carcb.State.curState == AD_DRIVE){
		SFunc_AD_Drive();
	}
	else if(LFT_Carcb.State.curState == AD_START){
		SFunc_AD_Start();
	}
	else if(LFT_Carcb.State.curState == AD_VACC){
		SFunc_AD_Vacc();
	}
	else if(LFT_Carcb.State.curState == AD_VDEC){
		SFunc_AD_Vdec();
	}
	else if(LFT_Carcb.State.curState == AD_STOP){
		SFunc_AU_Stop();
	}
	else if(LFT_Carcb.State.curState == AU_FIND){
		SFunc_AU_Find();
	}
	else if(LFT_Carcb.State.curState == AD_FIND){
		SFunc_AD_Find();
	}
	else if(LFT_Carcb.State.curState == AU_PARK){
		SFunc_AU_Park();
	}
	else if(LFT_Carcb.State.curState == AD_PARK){
		SFunc_AD_Park();
	}
	else{
		// state error (auto and pre-state = ???
	}
}


void Car_Dir(void)
{
	INT16U event;
	
	if(Auto_Phase == 0){
		LFT_Carcb.Direct = 0;
	}
	if(LFT_Carcb.State.curState == A_DOOR){
		if(LFT_Slctcb.ZShaft = SHFT_PLD){					// Terminal Floor
			LFT_Carcb.Direct = CUR_UP;
		}
		else if(LFT_Slctcb.ZShaft = SHFT_PLU){				// Terminal Floor
			LFT_Carcb.Direct = CUR_DN;
		}
		else{
		  	if(LFT_Carcb.Direct == 0){
				event = HUCALL;
				if(AnyCall_ThisFlr( LFT_Slctcb.FlrNo) & event)
					LFT_Carcb.Direct == CUR_UP;
				event = HDCALL;
				if(AnyCall_ThisFlr( LFT_Slctcb.FlrNo) & event)
					LFT_Carcb.Direct == CUR_DN;
			}
		}
	}
	else if(LFT_Carcb.State.curState == A_WAIT){
	  	if(LFT_Carcb.Direct != CUR_DN){
			event = CCALL | VCALL | HUCALL;
			if(AnyCall_Above(LFT_Slctcb.FlrNo) & event)
				LFT_Carcb.Direct == CUR_UP;

			event = HUCALL;
			if(AnyCall_ThisFlr( LFT_Slctcb.FlrNo) & event)
				LFT_Carcb.Direct == CUR_UP;
		}
		else if(LFT_Carcb.Direct != CUR_UP){
			event = CCALL | VCALL | HDCALL;
			if(AnyCall_Under( LFT_Slctcb.FlrNo) & event)
				LFT_Carcb.Direct == CUR_DN;

			event = HDCALL;
			if(AnyCall_ThisFlr(LFT_Slctcb.FlrNo) & event)
				LFT_Carcb.Direct == CUR_DN;
		}
		else{
			LFT_Carcb.Direct == 0;
		}
	}
	else{		//running
	  	if(LFT_Carcb.Direct == CUR_UP){
			event = CCALL | VCALL | HUCALL;
			if((AnyCall_Above(LFT_Carcb1.DestFlr - 1) & event) == 0){
				event = CCALL | VCALL;
				if(AnyCall_Under(LFT_Carcb1.DestFlr) & event)
					LFT_Carcb.Direct == CUR_DN;								// d
			}
		}
	  	else if(LFT_Carcb.Direct == CUR_DN){
			event = CCALL | VCALL | HDCALL;
			if((AnyCall_Under(LFT_Carcb1.DestFlr - 1) & event) == 0){
				event = CCALL | VCALL;
				if(AnyCall_Above(LFT_Carcb1.DestFlr) & event)
					LFT_Carcb.Direct == CUR_UP;								// u
			}
		}
	}
}

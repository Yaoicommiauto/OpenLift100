#define  CAR_GLOBALS
#include "includes.h"
#pragma locate (LFT_Carcb	= 0x0100)

//#pragma locate (RAM_SSW	= 0x08C2)

extern INT8U const BitMapTbl[];

void Set_CarMode(INT8U mode)
{
	if(LFT_Carcb.Mode < mode){
		LFT_Carcb.Mode = mode;

		if(LFT_Carcb.Mode == HOMING){
			Set_DoorMode(NORMAL);

			Set_CCallMode(PPAANG);	Set_VCallMode(LASTONE);
			Set_HUCallMode(NORMAL);	Set_HDCallMode(NORMAL);

			Call_Event(&(LFT_VCU[FRONT]), LFT_Carset.LobbyFlr, 1);
			Call_Event(&(LFT_VCU[REAR]), LFT_Carset.LobbyFlr, 1);
		}
		else if(LFT_Carcb.Mode == ATTENDANT){
			Set_DoorMode(ACL_KILL);

			Set_CCallMode(PPAANG);	Set_VCallMode(LASTONE);
			Set_HUCallMode(NORMAL);	Set_HDCallMode(NORMAL);
		}
		else if(LFT_Carcb.Mode == FULL){
			Set_DoorMode(NORMAL);

			Set_CCallMode(PPAANG);	Set_VCallMode(LASTONE);
			Set_HUCallMode(FROZEN);	Set_HDCallMode(FROZEN);
		}
		else if(LFT_Carcb.Mode == VIP){
			Set_DoorMode(NORMAL);

			Set_CCallMode(LASTONE);	Set_VCallMode(LASTONE);
			Set_HUCallMode(KILL); 	Set_HDCallMode(KILL);
		}
		else if(LFT_Carcb.Mode == PARKING){
			Set_DoorMode(ACL_NOWAIT);

			Set_CCallMode(PPAANG);	Set_VCallMode(LASTONE);
			Set_HUCallMode(KILL);	Set_HDCallMode(KILL);

			Call_Event(&(LFT_VCU[FRONT]), LFT_Carset.HomeFlr, 1);
			Call_Event(&(LFT_VCU[REAR]), LFT_Carset.LobbyFlr, 1);
		}
		else if(LFT_Carcb.Mode == PARKED){
			Set_DoorMode(NORMAL);

			Set_CCallMode(KILL);	Set_VCallMode(KILL);
			Set_HUCallMode(KILL);	Set_HDCallMode(KILL);
		}
		else if(LFT_Carcb.Mode == FSTFIRE){
			Set_DoorMode(ACL_NOWAIT);

			Set_CCallMode(KILL);	Set_VCallMode(LASTONE);
			Set_HUCallMode(KILL);	Set_HDCallMode(KILL);

			Call_Event(&(LFT_VCU[FRONT]), LFT_Carset.FireFlr, 1);
			Call_Event(&(LFT_VCU[REAR]), LFT_Carset.FireFlr, 1);
		}
		else if(LFT_Carcb.Mode == FSTFIRED){
			Set_DoorMode(ACL_KILL);

			Set_CCallMode(KILL);	Set_VCallMode(KILL);
			Set_HUCallMode(KILL);	Set_HDCallMode(KILL); 
		}
		else if(LFT_Carcb.Mode == SNDFIRE){
			Set_DoorMode(ACL_KILL | AOP_REV | AOP_BTNONLY |ACL_KILL | ACL_REV);

			Set_CCallMode(LASTONE);	Set_VCallMode(KILL);
			Set_HUCallMode(KILL);	Set_HDCallMode(KILL);
		}
		else if(LFT_Carcb.Mode == EARTHQK){
			Set_DoorMode(ACL_NOWAIT);

			Set_CCallMode(KILL);	Set_VCallMode(LASTONE);
			Set_HUCallMode(KILL);	Set_HDCallMode(KILL);
			Call_Event(&(LFT_VCU[FRONT]), LFT_Carset.LobbyFlr, 1);
			Call_Event(&(LFT_VCU[REAR]), LFT_Carset.LobbyFlr, 1);
		}
		else if(LFT_Carcb.Mode == INSTALL){
			Set_DoorMode(AOP_KILL | ACL_KILL);

			Set_CCallMode(CONHOLD);	Set_VCallMode(LASTONE);
			Set_HUCallMode(KILL);	Set_HDCallMode(KILL);
		}
		else{
			Set_DoorMode(NORMAL);

			Set_CCallMode(PPAANG);	Set_VCallMode(LASTONE);	
			Set_HUCallMode(NORMAL);	Set_HDCallMode(NORMAL);
			
		}
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

void PostSignal(INT16U sno, INT16U event)
{
	signal[sno] |= event;
}

INT16U WaitSignal(INT16U sno, INT16U event)
{
	INT16U event1;

	event1 = (signal[sno] & event);
	signal[sno] &= ~event1;
	return event1;
}

INT16U InquirySignal(INT16U sno, INT16U event)
{
	INT16U event1;

	event1 = (signal[sno] & event);
	return event1;
}
void ClearSignal(INT16U sno, INT16U event)
{
	signal[sno] &= ~event;
}


// Signal Drive
void teach_Position(void)
{
	INT16U event, V_Record;
	INT32U tp[2];
	INT8U Floors;

	Floors = DRegister_Get(FEE_FLR);
	if(Teach_Phase() == T_UP){
		event = WaitSignal(Slct_Signal, SIG_TUENTER | SIG_TULEAVE);
		if(event == SIG_TUENTER){
			DRegister_Set(FEE_DLVE_BASE + LFT_Slctcb.FlrNo,  getPosition());
		}
		else if(event == SIG_TULEAVE){
			if(LFT_Slctcb.FlrNo == 0){
				DRegister_Set(FEE_POSI_BASE + 0, 10045L);

				V_Record = DRegister_Get(FEE_DLVE_BASE + 0);
				setPosition((getPosition() - V_Record) / 2 + 10045L); 

			}
			DRegister_Set(FEE_DENT_BASE + 2 * LFT_Slctcb.FlrNo, getPosition());
		}

		event = WaitSignal(Slct_Signal, SIG_PLDLEAVE | SIG_PLD2LEAVE);
		if(event == SIG_PLDLEAVE){
			DRegister_Set(FEE_PPLD_BASE, getPosition());
		}
		else if(event == SIG_PLD2LEAVE){
			DRegister_Set(FEE_PPLDD_BASE, getPosition());
		}
	}
	else if(Teach_Phase() == T_DN){

		event = WaitSignal(Slct_Signal, SIG_TDENTER | SIG_TDLEAVE);
		if(event == SIG_TDENTER){
			DRegister_Set(FEE_ULVE_BASE + 2 * LFT_Slctcb.FlrNo, getPosition());
		}
		else if(event == SIG_TDLEAVE){
			DRegister_Set(FEE_UENT_BASE + 2 * LFT_Slctcb.FlrNo, getPosition());

			if(LFT_Slctcb.FlrNo == (Floors - 1)){
				tp[0] = DRegister_Get(FEE_ULVE_BASE + 2 * LFT_Slctcb.FlrNo);
				tp[1] = DRegister_Get(FEE_DLVE_BASE + 2 * LFT_Slctcb.FlrNo);
				DRegister_Set(FEE_POSI_BASE + 2 * LFT_Slctcb.FlrNo, 
				              (tp[0] + tp[1]) / 2);
			}
		}
		event = WaitSignal(Slct_Signal, SIG_PLULEAVE | SIG_PLU2LEAVE);
		if(event == SIG_PLULEAVE){
			if(LFT_Slctcb.FlrNo < (INT8U)(DRegister_Get(FEE_FLR) - 1)){
				// error
				return;
			}
			else{
				DRegister_Set(FEE_PPLU_BASE, getPosition());
			}
		}
		else if(event == SIG_PLU2LEAVE){
			DRegister_Set(FEE_PPLUD_BASE, getPosition());
		}
	}
}



void running_Position(void)
{
	INT16U event;
	INT32U p[2];

	
	if(Auto_Phase() == AU_UP){

		event = WaitSignal(Slct_Signal, SIG_UENTER | SIG_ULEAVE);
		if(event == SIG_UENTER && LFT_Slctcb.FlrNo == LFT_Carcb1.DestFlr){
			p[0] = getPosition();
			p[1] = LFT_Carcb1.DestEnt;

			if(labs(p[0] - p[1]) < 100){							// error - p(level)
				setPosition(p[1]);
			}
			else{
						//error
			}
		}
		else if(event == SIG_ULEAVE && LFT_Slctcb.FlrNo == LFT_Carcb1.StartFlr){
			p[0] = getPosition();
			p[1] = LFT_Carcb1.StartLav;
			if(labs(p[0] - p[1]) < 100){							// error - p(level)
				 setPosition(p[1]);
			}
			else{
				//error
			}
		}
		event = WaitSignal(Slct_Signal, SIG_PLUENTER | SIG_PLU2ENTER);
		if(event == SIG_PLUENTER){
			// if e slow dn
		}
		else if(event == SIG_PLU2ENTER){
			// if e slow dn
		}
	}
	else if(Auto_Phase() == AU_DN){
		event = WaitSignal(Slct_Signal, SIG_DENTER | SIG_DLEAVE);
		if(event == SIG_DENTER && LFT_Slctcb.FlrNo == LFT_Carcb1.DestFlr){
			p[0] = getPosition();
			p[1] = LFT_Carcb1.DestEnt;
			if(labs(p[0] - p[1]) < 100){							// error - p(level)
				setPosition(p[1]);
			}
			else{
						//error
			}
		}
		else if(event == SIG_DLEAVE && LFT_Slctcb.FlrNo == LFT_Carcb1.StartFlr){
			p[0] = getPosition();
			p[1] = LFT_Carcb1.StartLav;
			if(labs(p[0] - p[1]) < 100){							// error - p(level)
				 setPosition(p[1]);
			}
			else{
				//error
			}
		}
		event = WaitSignal(Slct_Signal, SIG_PLDENTER | SIG_PLD2ENTER);
		if(event == SIG_PLDENTER){
			//if e slow down
		}
		else if(event == SIG_PLD2ENTER){
			//if e slow down
		}
	}
}


void SetStartFlr(INT8U FlrNo)
{
	LFT_Carcb1.StartFlr = FlrNo;
	//LFT_Carcb1.SPosi = DRegister_Get(FEE_POSI_BASE + 2 * FlrNo);

	if(Auto_Phase() == AU_UP){
		LFT_Carcb1.StartLav = DRegister_Get(FEE_ULVE_BASE + 2 * FlrNo);
	}
	else if(Auto_Phase() == AU_DN){
		LFT_Carcb1.StartLav = DRegister_Get(FEE_DLVE_BASE + 2 * FlrNo);
	}

}

void SetDestFlr(INT8U FlrNo)
{
	LFT_Carcb1.DestFlr = FlrNo;
	LFT_Carcb1.DestPos = DRegister_Get(FEE_POSI_BASE + 2 * FlrNo);

	if(Auto_Phase() == AU_UP){
		LFT_Carcb1.DestEnt = DRegister_Get(FEE_UENT_BASE + 2 * FlrNo);
	}
	else if(Auto_Phase() == AU_DN){
		LFT_Carcb1.DestEnt = DRegister_Get(FEE_DENT_BASE + 2 * FlrNo);
	}
}

INT8U Get_CState(void)
{
	return LFT_Carcb.State.curState;
}

void Car_Load(void)
{
	if(CCU_DIGet(CDI_OVLD, FRONT) || CCU_DIGet(CDI_OVLD, REAR)){
		LFT_Carcb.Load = OVERLOAD;		
	}
	else if(CCU_DIGet(CDI_FULL, FRONT) || CCU_DIGet(CDI_FULL, REAR)){
		LFT_Carcb.Load = FULLLOAD;		
	}
	else if(CCU_DIGet(CDI_LGTLD, FRONT) || CCU_DIGet(CDI_LGTLD, REAR)){
		LFT_Carcb.Load = LIGHTLOAD;		
	}
	else{
		LFT_Carcb.Load = NOLOAD;		
	}
}

void Car_Mode(void)
{
	if(CAR_M012 == M0){
		if(LFT_Carcb.StaReg & TEACHED){
			if(LFT_Carcb.StaReg & TOINSTALL){
				Set_CarMode(INSTALL);
			}
			else if(LFT_Carcb.Mode == EARTHQK){
			}
		}
	}
	else{
		if(LFT_Carcb.Mode != INSTALL && LFT_Carcb.Mode != SNDFIRE){
			Set_CarMode(NORMAL);
		}
	}
}


void Car_Position(void)
{
	if(Teach_Phase()){
		teach_Position();				// Position point learning and verifying
	}
	else if(Auto_Phase()){
		running_Position();				// Position Verifying and Preset or reset it 
	}
}

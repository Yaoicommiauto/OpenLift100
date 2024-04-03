#define  SLCT_GLOBALS
#include "includes.h"
//#pragma locate (LFT_Slctcb	= 0x0A4E)


extern INT8U const BitMapTbl[];

// (+) --- +
// ULVL auto drun. teach urun
void ULVL_Enter(void)
{
	if(LFT_Slctcb.Level == LVL_ABOVE){						// dn ruuning Enter
		if(LFT_Slctcb.Direct == SLCT_DN){
			if(LFT_Slctcb.FlrNo > 0){						// selector
				LFT_Slctcb.FlrNo--;
				LFT_Slctcb.LChar.ulint = DRegister_Get(EE_CHAR_BASE + LFT_Slctcb.FlrNo * 2);
			}						   
		}
		if(LFT_Slctcb.ZShaft = SHFT_PLD){
			LFT_Slctcb.FlrNo = 0;
		}
		if(Auto_Phase() == AU_DN){
			PostSignal(Slct_Signal, SIG_DENTER);
		}
	}
	else if(LFT_Slctcb.Level == LVL_LEVEL){						// up ruuning Enter
		if(Teach_Phase() == T_UP){
			PostSignal(Slct_Signal, SIG_TUENTER);
		}
	}
}

// (+) --- +
// ULVL auto drun. teach urun
void ULVL_Leave(void)
{
	if(LFT_Slctcb.Level == LVL_UNDER){							// auto dn 
		if(Auto_Phase() == AU_DN){
			PostSignal(Slct_Signal, SIG_DLEAVE);
		}
	}
	else if(LFT_Slctcb.Level == LVL_NONE){						// up ruuning 
		if(Teach_Phase() == T_DN){
			PostSignal(Slct_Signal, SIG_TULEAVE);
		}
		LFT_Slctcb.Direct = SLCT_UP;
	}
}
/* ----------------------------------------*/
// + --- (+)
// DLVL auto urun. teach drun
void DLVL_Enter(void)
{
	if(LFT_Slctcb.Level == LVL_UNDER){						// up ruuning Enter
		if(LFT_Slctcb.Direct == SLCT_UP){
			if(LFT_Slctcb.FlrNo < MAX_FLOORS){					// U- Enter
				LFT_Slctcb.FlrNo++;
				LFT_Slctcb.LChar.ulint = DRegister_Get(EE_CHAR_BASE + LFT_Slctcb.FlrNo * 2);
			}
		}
		if(LFT_Slctcb.ZShaft == SHFT_PLU){							// Top Terminal
			if(LFT_Carcb.StaReg & TEACHED){
				LFT_Slctcb.FlrNo = DRegister_Get(FEE_FLR);	// FloorNo Restore
			}
		}
		if(Auto_Phase() == AU_UP){
			PostSignal(Slct_Signal, SIG_UENTER);
		}
	}
	else if(LFT_Slctcb.Level == LVL_LEVEL){						// dn ruuning Enter
		if(Teach_Phase() == T_DN){
			PostSignal(Slct_Signal, SIG_TDENTER);
		}
	}
}

void DLVL_Leave(void)
{
	if(LFT_Slctcb.Level == LVL_ABOVE){							// auto up
		if(Auto_Phase() == AU_UP){
			PostSignal(Slct_Signal, SIG_ULEAVE);
		}
	}
	else if(LFT_Slctcb.Level == LVL_NONE){						// up ruuning 
		if(Teach_Phase() == T_DN){
			PostSignal(Slct_Signal, SIG_TDLEAVE);
		}
		LFT_Slctcb.Direct = SLCT_DN;
	}
}

void SLCT_SetLevel(void)
{
	INT8U k;

	k = 0;
	if(DIGet(DI_ULVL))	{k |= 0x01;}
	if(DIGet(DI_DLVL))	{k |= 0x02;}


	LFT_Slctcb.Level = k;

	if(k != 0){
		LFT_Slctcb.DZone = TRUE;
	}
	else{
		LFT_Slctcb.DZone = FALSE;
	}
	if(DIGetP(DI_ULVL))	{ULVL_Enter();}
	if(DIGetN(DI_ULVL))	{ULVL_Leave();}
	if(DIGetP(DI_ULVL))	{ULVL_Enter();}
	if(DIGetN(DI_ULVL))	{ULVL_Leave();}
}



void PLD_Enter(void)
{
	if(Auto_Phase() == AU_DN){
	 	PostSignal(Slct_Signal, SIG_PLDENTER);
	}
}
void PLD_Leave(void)
{
	if(Teach_Phase() == AU_UP){
	 	PostSignal(Slct_Signal, SIG_PLDLEAVE);
	}
}

void PLD2_Enter(void)
{
	if(Auto_Phase() == AU_DN){
	 	PostSignal(Slct_Signal, SIG_PLD2ENTER);
	}
}
void PLD2_Leave(void)
{
	if(Teach_Phase() == AU_UP){
	 	PostSignal(Slct_Signal, SIG_PLD2LEAVE);
	}
}

void PLU_Enter(void)
{
	if(Auto_Phase() == AU_DN){
	 	PostSignal(Slct_Signal, SIG_PLUENTER);
	}
}
void PLU_Leave(void)
{
	if(Teach_Phase() == AU_DN){
	 	PostSignal(Slct_Signal, SIG_PLULEAVE);
	}
}

void PLU2_Enter(void)
{
	if(Auto_Phase() == AU_UP){
	 	PostSignal(Slct_Signal, SIG_PLU2ENTER);
	}
}
void PLU2_Leave(void)
{
	if(Teach_Phase() == AU_DN){
	 	PostSignal(Slct_Signal, SIG_PLU2LEAVE);
	}
}
void SLCT_SetShaft(void)
{
	INT8U k;

	k = 0;
	if(DIGet(DI_DL))	{k |= 0x01;}
	if(DIGet(DI_PLD))	{k |= 0x02;}
	if(DIGet(DI_PLD2))	{k |= 0x04;}
	if(DIGet(DI_PLU2))	{k |= 0x08;}
	if(DIGet(DI_PLU))	{k |= 0x10;}
	if(DIGet(DI_UL))	{k |= 0x20;}
	LFT_Slctcb.ZShaft = k;
	if(DIGetP(DI_PLD))	{PLD_Enter();}
	if(DIGetN(DI_PLD))	{PLD_Leave();}
	if(DIGetP(DI_PLD2))	{PLD2_Enter();}
	if(DIGetN(DI_PLD2))	{PLD2_Leave();}

	if(DIGetP(DI_PLU))	{PLU_Enter();}
	if(DIGetN(DI_PLU))	{PLU_Leave();}
	if(DIGetP(DI_PLU2))	{PLU2_Enter();}
	if(DIGetN(DI_PLU2))	{PLU2_Leave();}
}

void setup_selector(void)
{
	LFT_Slctcb.LChar.ulint = DRegister_Get(EE_CHAR_BASE + LFT_Slctcb.FlrNo * 2);
}

void task_selector(void)
{
	INT8U ph;
	SLCT_SetLevel();
	SLCT_SetShaft();

	ph = Auto_Phase();
	if( ph == AU_UP){  
	}
	else if(ph == AU_DN){  
	}
}

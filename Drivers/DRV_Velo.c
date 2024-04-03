#define   VELO_GLOBALS
#include "includes.h"
extern INT8U const  BitMapTbl[8];
extern INT16U const ParMax[16][16];
extern INT16U const ParMin[16][16];

INT8U pdv;
// LiftRAM
#define	RAM_VRAT	0x00
#define RAM_PPR		0x01
#define RAM_RPM		0x02
#define RAM_INV		0x03
#define RAM_STV		0x05

#define RAM_ACR		0x06
#define RAM_DER		0x07
#define RAM_JSR		0x08
#define RAM_JTS		0x09
#define	RAM_JDR		0x0A
#define RAM_LTG		0x0B
#define RAM_FSR		0x0C
#define RAM_DTO		0x0D
#define RAM_DTD		0x0E
#define RAM_LVE		0x0F

// dVjerk[4];
#define dVJSR			0
#define dVJTS			1
#define dVJDR			2
#define dVLTG			3
// dSjerk[4];
#define pSJSR			0
#define nSJTS			1
#define nSJDR			2
#define pSLTG			3


INT32U  startP;

INT16U LiftRAM[16];
INT16U MotionRAM[16];

INT16U dVjerk[4];
INT16U dSjerk[4];
INT32U DPPMeter;

//INT32U Debugp;

rvel   Vrate;
rvmin  Vmin;

rvel   Vref;
int    Vfbk;

INT16U TopVelo;
INT16U TopDER;
INT16U TopACR;
INT16U dvltg;
INT16U ltg;


volatile INT32U  curPosition;
#pragma locate (LiftRAM 	= 0x0800)

#pragma locate (MotionRAM 	= 0x0860)

#pragma locate (dVjerk 		= 0x0880)
#pragma locate (dSjerk 		= 0x0888)
#pragma locate (DPPMeter	= 0x0890)
#pragma locate (Vmin		= 0x0894)
#pragma locate (Vref		= 0x08A6)
#pragma locate (curPosition	= 0x08B8)
#pragma locate (vstate		= 0x08BC)
#pragma locate (FbV	= 0x0902)

int PSacr;
int PSder;

const INT8U SQRT[100]= {
 0,  9,  14,  17,  19,  22,  24,  26,  28,  29, 
 31,  33,  34,  36,  37,  38,  39,  41,  42,  43, 
 44,  45,  46,  47,  48,  49,  50,  51,  52,  53, 
 54,  55,  56,  57,  58,  59,  59,  60,  61,  62, 
 63,  64,  64,  65,  66,  67,  67,  68,  69,  69, 
 70,  71,  72,  72,  73,  74,  74,  75,  76,  76, 
 77,  78,  78,  79,  79,  80,  81,  81,  82,  83, 
 83,  84,  84,  85,  86,  86,  87,  87,  88,  88, 
 89,  89,  90,  91,  91,  92,  92,  93,  93,  94, 
 94,  95,  95,  96,  96,  97,  97,  98,  98,  99 
 };

void setVelo(INT16U tv, INT16U ta,INT16U td)
{		
	TopVelo = tv;	
	TopACR = ta;
	TopDER = td;
}

void Load_EEGrpLift(void)
{
	INT8U i;
	INT32U k, l;

	INT32U  Vat;

	Load_EEGrp(4, LiftRAM);
	//for(i = 0; i < 16; i++)

	
	k = LiftRAM[RAM_PPR];
	if(k > 512){
		l = 512;
		pdv = k / l;
	}
	else{
		pdv = 1;
	}
	k = (INT32U)LiftRAM[RAM_PPR] * LiftRAM[RAM_RPM] /60;		
	DPPMeter = (100 * k) / LiftRAM[RAM_VRAT];						// DPP / 1000mm 
	DPPMeter /= pdv;													// 1024

	pdv <<= 2;

	//printf("PPM%d\n", DPPMeter);
	//LiftRAM[RAM_VRAT] = (INT16U)k;									// DPP / s (Rate)

	k = DPPMeter * (INT32U)LiftRAM[RAM_VRAT] / 100;
	LiftRAM[RAM_VRAT] =  (INT16U)k;

	k = (INT32U)LiftRAM[RAM_STV] *LiftRAM[RAM_VRAT] / 255;
	LiftRAM[RAM_STV] = k;


	for(i = 6; i <= 12; i++){
		k = DPPMeter * (INT32U)LiftRAM[i] / 100;
		LiftRAM[i] = (INT16U)k;									// DPP /s Jerks & ACR DER
	}

	LiftRAM[RAM_DTD] /= 10;										// DTD Ticks (10ms/tick)
	k = (INT32U)DPPMeter * LiftRAM[RAM_LVE] / 1000;
	LiftRAM[RAM_LVE] = (INT16U)k;								// LVE DPP / s 

	// Calculate the dVjerks (DPP/s)
	k = (INT32U)LiftRAM[RAM_ACR] * LiftRAM[RAM_ACR];
	l = (INT32U)LiftRAM[RAM_JSR] * 2;
	dVjerk[dVJSR] = k / l;
	l = (INT32U)LiftRAM[RAM_JTS] * 2;
	dVjerk[dVJTS] = k / l; 

	k = (INT32U)LiftRAM[RAM_DER] * LiftRAM[RAM_DER];
	l = (INT32U)LiftRAM[RAM_JDR] * 2;
	dVjerk[dVJDR] = k / l;
	l = (INT32U)LiftRAM[RAM_LTG] * 2 ;//+ LiftRAM[RAM_LVE];
	dVjerk[dVLTG] = k / l;
	
	k = (INT32U)dVjerk[dVJSR] * dVjerk[dVJSR];
	l = (INT32U)LiftRAM[RAM_ACR] * 6;
	dSjerk[pSJSR] = k / l;
	k = (INT32U)dVjerk[dVJTS] * dVjerk[dVJTS] * 2;
	l = (INT32U)LiftRAM[RAM_ACR] * 3;
	dSjerk[nSJTS] = k / l;
	k = (INT32U)dVjerk[dVJDR] * dVjerk[dVJDR] * 2;
	l = (INT32U)LiftRAM[RAM_DER]* 3;
	dSjerk[nSJDR] = k / l;
	k = (INT32U)dVjerk[dVLTG] * dVjerk[dVLTG];
	l = (INT32U)LiftRAM[RAM_DER] * 6; 
	dSjerk[pSLTG] = k / l;

	PSacr = (int)dSjerk[pSJSR] - dSjerk[nSJTS];
	PSder = (int)dSjerk[pSLTG]- dSjerk[nSJDR];// + LiftRAM[RAM_DTO];

	// Vmin
	Vmin.Vmacr =  dVjerk[dVJSR] + dVjerk[dVJTS];
	Vmin.Vmder =  dVjerk[dVJDR] + dVjerk[dVLTG];
	
	Vat = Vmin.Vmacr + dVjerk[dVJTS];
	k = Vat * Vat;
	l = (INT32U)LiftRAM[RAM_ACR] * 2;
	Vmin.Sacr = k / l + PSacr;
	
	Vat = Vmin.Vmder + dVjerk[dVJDR];
	k = Vat * Vat;
	l = (INT32U)LiftRAM[RAM_DER] * 2;
	Vmin.Sder = k / l + PSder;

}
/*******************************************************************
 *******************************************************************/

//INT16U MotionRAM[16];
#define RAM_VLD				0x00	// EE Motion Data
#define RAM_VIN				0x01
#define RAM_VTH				0x02
#define RAM_VFD				0x03
#define RAM_VZR				0x04

#define RAM_MUL				0x0C
#define RAM_MLL				0x0D
#define RAM_VEE				0x0E
#define RAM_PLE				0x0F

void Load_EEGrpMotion(void)
{
	INT8U i;
	INT32U k;

	Load_EEGrp(5, MotionRAM);

	k = (INT32U)LiftRAM[RAM_VRAT] * MotionRAM[RAM_VLD] / 100;  
	MotionRAM[RAM_VLD] = (INT16U)k;								// Velovity limited

	for(i = 1; i <=2; i++){
		k = DPPMeter * MotionRAM[i] / 100;						// inspection & Teach velo
		MotionRAM[i] = (INT16U)k;
	}
	for(i = 3; i <= 4; i++){									// Releveling & Zero velo
		k = DPPMeter * MotionRAM[i] / 1000;
		MotionRAM[i] = (INT16U)k;	 
	}
	for(i = 12; i < 14; i++){									// 
		k = DPPMeter * MotionRAM[i] / 100;
		MotionRAM[i] = (INT16U)k;
	}

	k = DPPMeter * MotionRAM[15] / 1000;						// 
	MotionRAM[15] = (INT16U)k;
}


void VStateShift(INT8U newState)
{	 
	//disable();
	vstate.Timer = 0;								//Timer restart
	vstate.PreState = vstate.curState;
	vstate.curState = newState;
	vstate.isShift = TRUE;
	//enable();
}


void vs_stop(void)
{
	if(vstate.isShift){
		vstate.isShift = 0;
		setVelo(0, LiftRAM[RAM_ACR], LiftRAM[RAM_DER]);
		Vref.curV = 0;	Vref.Vm   = 0;
		Vref.curA = 0;  
	}

	#ifdef DEBUG
		if(vstate.Timer > 100){
			setVelo(MotionRAM[RAM_VLD], LiftRAM[RAM_ACR], LiftRAM[RAM_DER]);
		}
	#endif

	if(DOGet(DO_BRC)){
		if(LFT_Carcb.State.curState == AU_VACC || LFT_Carcb.State.curState == AD_VACC)
			setVelo(MotionRAM[RAM_VLD], LiftRAM[RAM_ACR], LiftRAM[RAM_DER]);
		else if(LFT_Carcb.State.curState == AU_FIND || LFT_Carcb.State.curState == AD_FIND)
			setVelo(MotionRAM[RAM_VFD], LiftRAM[RAM_ACR], LiftRAM[RAM_DER]);
		else if(LFT_Carcb.State.curState == TU_RUNH || LFT_Carcb.State.curState == TD_RUNH)
			setVelo(MotionRAM[RAM_VFD], LiftRAM[RAM_ACR], LiftRAM[RAM_DER]);
		else if(LFT_Carcb.State.curState == TU_RUNH || 
				LFT_Carcb.State.curState == TD_RUNH ||
				LFT_Carcb.State.curState == TF_RUNH)
			setVelo(MotionRAM[RAM_VTH], LiftRAM[RAM_ACR], LiftRAM[RAM_DER]);
		else if(LFT_Carcb.State.curState == TU_RUNL || 
				LFT_Carcb.State.curState == TD_RUNL ||
				LFT_Carcb.State.curState == TF_RUNL)
			setVelo(LiftRAM[RAM_LVE], LiftRAM[RAM_ACR], LiftRAM[RAM_DER]);
		else if(LFT_Carcb.State.curState == IU_RUN || 
				LFT_Carcb.State.curState == ID_RUN)
			setVelo(MotionRAM[RAM_VIN], LiftRAM[RAM_ACR], LiftRAM[RAM_DER]);
		else
			setVelo(0, LiftRAM[RAM_ACR], LiftRAM[RAM_DER]);
	}
			
	if(TopVelo > 0){
		VStateShift(VJSR);
		return;
	}		
}

void vs_jsr(void)
{
	INT8U i;
	INT32U k, l;

	if(vstate.isShift){
		vstate.isShift = 0;	
		Vref.curV = LiftRAM[RAM_STV];
		return;
	}
	if(LFT_Slctcb.DPPcnt > LiftRAM[RAM_DTO]){			// point to FSR
		VStateShift(VFSR);
		return;
	}

	Vref.curA += LiftRAM[RAM_JSR] / 100;			// 10ms tick
	Vref.curV += Vref.curA / 100;	

  	if(Vref.curA >= TopACR){
  		VStateShift(VACR);
		return;
  	}

	k = (INT32U)Vref.curA * Vref.curA;
	k >>= 16;
	k *= Vmin.Vmacr;
	l = (INT32U)LiftRAM[RAM_ACR]* LiftRAM[RAM_ACR];
	l >>= 16;
	Vref.Vm = k / l;
	if(Vref.Vm >= TopVelo){
		VStateShift(VJTS);
		return;
	}

	i = Auto_Phase();
	if(i == AU_DN){
		if(curPReach <= LFT_Carcb1.DestPos)
			VStateShift(VJTS);
	}
	else if(i == AU_UP){
		if(curPReach >=  LFT_Carcb1.DestPos)
			VStateShift(VJTS);
	}
}

void vs_acr(void)
{
	INT8U i;
	
	if(vstate.isShift){
		vstate.isShift = 0;
	}
	if(LFT_Slctcb.DPPcnt > LiftRAM[RAM_DTO]){			// point to FSR
		VStateShift(VFSR);
		return;
	}
	if(LFT_Slctcb.DPPcnt > LiftRAM[RAM_DTO]){			// point to FSR
		VStateShift(VFSR);
		return;
	}

	Vref.curV += (INT16U)Vref.curA / 100;	
	Vref.Vm = Vref.curV + dVjerk[dVJTS];
	if(Vref.Vm >= TopVelo){
	 	VStateShift(VJTS);
		return;
	}

	i = Auto_Phase();
	if(i == AU_DN){
		if(curPReach <= LFT_Carcb1.DestPos)
			VStateShift(VJTS);
	}
	else if(i == AU_UP){
		if(curPReach >=  LFT_Carcb1.DestPos)
			VStateShift(VJTS);
	}
}


void vs_jts(void)
{
	int k;
	if(vstate.isShift){
		vstate.isShift = 0;
	}
	if(LFT_Slctcb.DPPcnt > LiftRAM[RAM_DTO]){			// point to FSR
		VStateShift(VFSR);
		return;
	}

	k =  (int)(Vref.curA - LiftRAM[RAM_JTS] / 100);
	if(k > 0){
		Vref.curA =	k;
		Vref.curV += Vref.curA / 100;	
	}
	else{
		Vref.curV += Vref.curA / 100;	
		if(Vref.curV >= TopVelo){
			Vref.curA = 0;
			VStateShift(VCON);
		}
		else{
			Vref.curV += LiftRAM[RAM_JTS] / 500;
		}
	}
}

void vs_con(void)
{
	INT8U i;
	INT32U  curPJerk;

	if(vstate.isShift){
		vstate.isShift = 0;
	}
	Vref.Vm = Vref.curV;
	if(LFT_Slctcb.DPPcnt > LiftRAM[RAM_DTO]){			// point to FSR
		VStateShift(VFSR);
		return;
	}

	#ifdef DEBUG
	if(vstate.Timer > 500){
		VStateShift(VJDR);
		return;
	}
	#endif

	if(Teach_Phase()){
		if(LFT_Carcb.State.curState == TU_RUNL || 
			LFT_Carcb.State.curState == TD_RUNL ||
			LFT_Carcb.State.curState == TF_RUNL){
			VStateShift(VJDR);
			return;
		}
	}
		
	i = Auto_Phase();
	if(i == AU_DN){
		if(curPReach <= LFT_Carcb1.DestPos)
			VStateShift(VJDR);
	}
	else if(i == AU_UP){
		if(curPReach >=  LFT_Carcb1.DestPos)
			VStateShift(VJDR);
	}
}

void vs_jdr(void)
{
	INT32U k, l;
	if(vstate.isShift){
		vstate.isShift = 0;

		if(Vref.curV < (dVjerk[dVJDR] + dVjerk[dVLTG])){
			l = (INT32U)dVjerk[dVJDR] + dVjerk[dVLTG];
		   	k = (INT32U)Vref.curV * 100 / l ;
			k = (INT32U)LiftRAM[RAM_DER] * SQRT[k] ;
			TopDER = k  / 100;
		}
		
		else{
			TopDER = LiftRAM[RAM_DER];
		}
	}
	Vref.curA +=  LiftRAM[RAM_JDR] / 100;
	Vref.curV -= Vref.curA / 100;	

	if(Vref.curA >= TopDER){
		if(Vref.curV > dVjerk[dVLTG]){
			VStateShift(VDER);
		}
		else{
			VStateShift(VLTG);
		}
	}
}

void vs_der(void)
{
	INT32U k, l;
	if(vstate.isShift){
		vstate.isShift = 0;
		k = (INT32U)Vref.curA * Vref.curA;	k >>= 16;
		k *= dVjerk[dVLTG];
		l = (INT32U)LiftRAM[RAM_DER] * LiftRAM[RAM_DER]; l >>= 16;
		dvltg = k / l;
	}
	if(LFT_Slctcb.DPPcnt > LiftRAM[RAM_DTO]){			// point to FSR
		VStateShift(VFSR);
		return;
	}

	Vref.curA = TopDER;

	if(Vref.curV >= Vref.curA / 100)	
		Vref.curV -= Vref.curA / 100;
	if(Vref.curV <= dvltg){
   //	if(Vref.curV <= dVjerk[dVLTG]){
		VStateShift(VLTG);
	}
}

void vs_ltg(void)
{
	INT32U k, l;
	if(vstate.isShift){
		vstate.isShift = 0;
		if(Vref.curV > LiftRAM[RAM_LVE]){
			k = (INT32U)Vref.curA * Vref.curA;
			l = (INT32U)Vref.curV - LiftRAM[RAM_LVE];
			l <<= 1;
			ltg = k / l;
		}else{
			VStateShift(VCRP);
			return;
		}
	}

	if(LFT_Slctcb.DPPcnt > LiftRAM[RAM_DTO]){			// point to FSR
		VStateShift(VFSR);
		return;
	}

	if(Vref.curA > (ltg / 100)){
		Vref.curA -= ltg / 100;
		
		if(Vref.curV > (Vref.curA / 100)){
			Vref.curV -= Vref.curA / 100;	
		}
	}
	else{
		if(Vref.curV > LiftRAM[RAM_LVE]){
			Vref.curV -= ltg / 500;
		}
		else{
			VStateShift(VCRP);
		}
	}
}


void vs_crp(void)
{
	if(vstate.isShift){
		vstate.isShift = 0;
	}

	if(LFT_Slctcb.DPPcnt > LiftRAM[RAM_DTO]){			// point to FSR
		VStateShift(VFSR);
		return;
	}
	#ifdef DEBUG
	if(vstate.Timer > 300){
		VStateShift(VFSR);
	}
	#endif
}

void vs_fsr(void)
{
	if(vstate.isShift){
		vstate.isShift = 0;
	}

	if(Vref.curA < LiftRAM[RAM_DER])
		Vref.curA +=  LiftRAM[RAM_FSR] / 100;	// 10ms tick
	
	if(Vref.curV > (Vref.curA / 100)){
		Vref.curV -= Vref.curA / 100;	
	}
	else{
		Vref.curV = 0;	
		VStateShift(VSTOP);
	}
}
// 10ms interrupt routine
void isr_VGenPtn(void)
{
	vstate.Timer++;					// tick ++

	PostSignal(VELO_Signal, SIG_VELOTICK);  		//	30ms

	if(vstate.curState == VSTOP){
	 	vs_stop();
	}
	else{
		#ifndef DEBUG
		if(!DOGet(DO_BRC)){
			VStateShift(VSTOP);
			return;
		}
		#endif

		if(vstate.curState == VJSR){
		 	vs_jsr();
		}
		else if(vstate.curState == VACR){
		 	vs_acr();
		}
		else if(vstate.curState == VJTS){
		 	vs_jts();
		}
		else if(vstate.curState == VCON){
		 	vs_con();
		}
		else if(vstate.curState == VJDR){
		 	vs_jdr();
		}
		else if(vstate.curState == VDER){
		 	vs_der();
		}
		else if(vstate.curState == VLTG){
		 	vs_ltg();
		}
		else if(vstate.curState == VCRP){
		 	vs_crp();
		}
		else if(vstate.curState == VFSR){
		 	vs_fsr();
		}
		else{
			VStateShift(VSTOP);
		}
	}
}


INT16U position;
void isrPoisition(void)
{									 
	int dcnt;
	//disable();
	//position.ULINT = getPosition();
	//enable();
	dcnt = (signed)(timer2 - position);
	position = timer2;
	curPosition += dcnt;
	curPosition;

	if(Auto_Phase() == AU_UP ||Auto_Phase() == AU_DN){
		if(LFT_Slctcb.FlrNo == LFT_Carcb1.DestFlr)
			LFT_Slctcb.DPPcnt += abs(dcnt);
		else
			LFT_Slctcb.DPPcnt = 0;
	}
	else
		LFT_Slctcb.DPPcnt = 0;
}

void setPosition(INT32U position)
{
	LCNT p;
	p.ULINT = position;
	disable();
	curPosition = p.ULINT;
	p.ULINT <<= 2;
	timer2 = p.WINT.lsw;
	enable();
}


INT32U getPosition(void)
{
//	Debugp = curPosition >> 3;
	return curPosition / pdv;		//
}

INT16U getVRef(void)
{
	return Vref.curV;
}

INT16U getVFbk(void)
{
	return (INT16U)((labs(P100ms[1] - P100ms[0])) * 5);
}

INT8U getFbk_Dir(void)
{
	long pgap;

	pgap = P100ms[1] - P100ms[0];
	if(pgap > 0)
		return CUR_UP;
	else if(pgap < 0)
		return CUR_DN;
	else
		return CUR_NDIR;
}

INT8U getVDegree(void)
{
	return (INT8U)((INT32U)Vref.curV * 225 / LiftRAM[RAM_VRAT]); //255
}

BOOLEAN ifOpVel(void)
{
	INT16U opv = 30;		//cm/s
	INT32U k;

	k = opv * DPPMeter;
	opv =  k / 100;

	if(Vref.curV <= opv && getVFbk() <=	opv)
		return TRUE;
	else
		return FALSE;

	return TRUE;
}
BOOLEAN ifZVel(void)
{
	if(Vref.curV <= MotionRAM[RAM_VZR] ||
	   getVFbk() <=	MotionRAM[RAM_VZR])
		return TRUE;
	else
		return FALSE;
}

void VPtn_Reg(void)
{
	Load_EEGrpLift();
	Load_EEGrpMotion();
}
void setup_VPtn(void)
{
	VPtn_Reg();
	vstate.curState = VSTOP;
	VStateShift(VSTOP);
}

INT8U tcnt;
void task_VPtn(void)
{	  
	INT16U event;
	INT32U Vat, Vatmin;
	INT32U k, l;
	
	getPosition();

	event = WaitSignal(VELO_Signal, SIG_VELOTICK);  	// 10ms interval from VPattern isr routine
	if(event & SIG_VELOTICK){
		Vfbk = (int)(P100ms[1] - P100ms[0]) * 10;			// feedback speed calcu.
		// Sa, Sd Algorithm
		if(vstate.curState == VSTOP){
			tcnt = 0;
			Vref.Sacr = 0;	Vref.Sder = 0;
			startP = curPReach = getPosition();
			 
			k =  EEGRP_LIFT; l = EEGRP_MOTION;
			if((RAM_SSW[k >> 3] & BitMapTbl[k & 0x07]) ||
			   (RAM_SSW[l >> 3] & BitMapTbl[l & 0x07])){
				VPtn_Reg();
	
				RAM_SSW[k >> 3] &= ~BitMapTbl[k & 0x07];
				RAM_SSW[l >> 3] &= ~BitMapTbl[l & 0x07];
			}
		}
		else{
			tcnt++;
			
			if(vstate.curState < VJTS && (tcnt & 0x03)){// acce
				if(Vref.Vm < Vmin.Vmacr){
					k = Vref.curA >> 6;
					k = k * k * k;
					l = LiftRAM[RAM_ACR] >> 6;
					l = l * l * l;
					k >>= 16; l >>= 16;
					Vref.Sacr = k * Vmin.Sacr / l;
				}
				else{
					Vat = (INT32U)Vref.Vm + dVjerk[dVJTS];
					k = Vat * Vat;
					l = (INT32U)LiftRAM[RAM_ACR] * 2;
					Vref.Sacr = k / l + PSacr;
				}
			}

			if((vstate.curState < VCON && (tcnt & 0x03)) ||
   			   (vstate.curState == VCON)){
				if(Vref.Vm < Vmin.Vmder){
					k = Vref.Vm >> 6;
					k = k * k * k;
					l = Vmin.Vmder >> 6;
					l = l * l * l;					
					k =  (INT32U)Vref.Vm * SQRT[(INT8U)((INT32U)k * 100 / l)];
					l =  (INT32U)Vmin.Vmder * 100;
					k >>= 8; l >>= 8;
					Vref.Sder = k * Vmin.Sder / l;
				}			
				else{
					Vat = (INT32U)Vref.Vm + dVjerk[dVJDR];
					k = Vat * Vat;
					l = (INT32U)LiftRAM[RAM_DER] * 2;
					Vref.Sder = k / l + PSder + LiftRAM[RAM_DTO];;
					//Vref.Sder += (INT32U)Vref.Vm * LiftRAM[RAM_DTD] / 1000;
				}
			}

			if(vstate.curState < VCON){
				if(DOGet(DO_FWD)){
					curPReach = startP + Vref.Sacr + Vref.Sder +
					                 (INT32U)Vref.Vm * (LiftRAM[RAM_DTD] + 5) / 100;
				}
				else if(DOGet(DO_REV)){
					curPReach = startP - (Vref.Sacr + Vref.Sder +
					(INT32U)Vref.Vm * (LiftRAM[RAM_DTD] + 5) / 100);
				}
			}
			else if(vstate.curState == VCON){
				startP = getPosition();
				if(DOGet(DO_FWD)){
					curPReach = startP + Vref.Sder +
					(INT32U)Vref.Vm * LiftRAM[RAM_DTD] / 100;
				}
				else if(DOGet(DO_REV)){
					curPReach = startP - (Vref.Sder +
					(INT32U)Vref.Vm * LiftRAM[RAM_DTD] / 100);
				}
			}
		}
	}
}





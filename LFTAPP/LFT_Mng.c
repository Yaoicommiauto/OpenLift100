#define  MNG_GLOBALS
#include "includes.h"


/*
*********************************************************************************************************
* 										Driver Control
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Driver control(General for YASKAWA & FUJI..):
* Drviver detect: Inverter ready/fault signals, Contactor Jam ect.
* Driver outputs:
* MC1 + 
* MC2  BRK  FWD/REV PWMVELO
* DO_BRC  DO_BRCx  DO_MC1  DO_MC2  DO_FWD  DO_REV 
*
*  DO_DRVEN 
*
*********************************************************************************************************
*/
void Fuji_Drive(void)
{
	INT8U k;
 	INT16U t;

	k   = LFT_Carcb.State.curState;
	t = LFT_Carcb.State.Timer;

	/******************************MC2*****************************/
	if(DOGet(DO_FWD) || DOGet(DO_REV))
		DOSet(DO_MC2, TRUE);
	else{
		if(t > 20){					// 20 * 25ms = 500ms MC2 Relase delay
			DOSet(DO_MC2, FALSE);
		}
	}

	if(CAR_M012 == M0 && k < T_CHECK){			//Auto
		/******************************MC1*****************************/
		DOSet(DO_MC1, TRUE);
		/*************************DO_FWD  DO_REV***********************/
		if(k >= AU_DRIVE && k <= AU_PARK)
			DOSet(DO_FWD, TRUE);
		else
			DOSet(DO_FWD, FALSE);
		if(k >= AD_DRIVE && k <= AD_PARK)
			DOSet(DO_REV, TRUE);
		else
			DOSet(DO_REV, FALSE);
		/******************************BRK*****************************/
		if((k >= AU_START && k <= AU_STOP) || (k >= AD_START && k <= AD_STOP))
			DOSet(DO_BRC, TRUE);
		else
			DOSet(DO_BRC, FALSE);
	}
	else if(CAR_M012 == M0 && k >= T_CHECK){			//Teach
		/******************************MC1*****************************/
		DOSet(DO_MC1, TRUE);
		/*************************DO_FWD  DO_REV***********************/
		if(k >= TU_START && k <= TU_STOP)
			DOSet(DO_FWD, TRUE);
		else
			DOSet(DO_FWD, FALSE);

		if((k >= TF_START && k <= TF_STOP)|| (k >= TD_START && k <= TD_STOP))
			DOSet(DO_REV, TRUE);
		else
			DOSet(DO_REV, FALSE);
		/******************************BRK*****************************/
		if((k >= TF_RUNH && k < TF_STOP) || 
		   (k >= TU_RUNH && k < TU_STOP) ||
		   (k >= TD_RUNH && k < TD_STOP))
			DOSet(DO_BRC, TRUE);
		else
			DOSet(DO_BRC, FALSE);
  	}
	else if(CAR_M012 == M1){			// inspection
		/******************************MC1*****************************/
		DOSet(DO_MC1, TRUE);
		/*************************DO_FWD  DO_REV***********************/
		if(k >= IU_START && k <= IU_STOP)
			DOSet(DO_FWD, TRUE);
		else
			DOSet(DO_FWD, FALSE);
		if(k >= ID_START && k <= ID_STOP)
			DOSet(DO_REV, TRUE);
		else
			DOSet(DO_REV, FALSE);
		/******************************BRK*****************************/
		if((k >= IU_RUN && k < IU_STOP) || (k >= ID_RUN && k < ID_STOP))
			DOSet(DO_BRC, TRUE);
		else
			DOSet(DO_BRC, FALSE);
	}
	else if(CAR_M012 == M2){
	}
	else{
		DOSet(DO_MC1, FALSE); 		DOSet(DO_MC2, FALSE);
		DOSet(DO_BRC, FALSE);
		DOSet(DO_FWD, FALSE); 	    DOSet(DO_REV, FALSE);
	}
}

/*
*********************************************************************************************************
* Driver control CPI inverter special:
* Drviver detect: Inverter ready/fault signals, Contactor Jam ect.
* Driver outputs:
* MC1 + 
* MC2  BRK  FWD/REV PWMVELO
* DO_BRC  DO_BRCx  DO_MC1  DO_MC2  DO_FWD  DO_REV 
*
*  DO_DRVEN 
*
*********************************************************************************************************
*/
void CPI_Drive(void)
{
}

void Inverter_Drive(void)
{
	if(LFT_Drvcb.Type == DRV_FUJI){
		Fuji_Drive();
	}
	else if(LFT_Drvcb.Type == DRV_CPI){
		CPI_Drive();
	}
	else{
		// Error?
	}
}
/*
*********************************************************************************************************
* 										Front/Rear Door Control
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* 	DIO Control door control
*	DO_FOP	DO_FCL
*	DO_ROP  DO_RCL
*********************************************************************************************************
*/
void DIO_Door(void)
{
	INT8U k, i;

	for(i = 0; i < 2; i++){
		k = LFT_Dcb.State[i].curState;
		if(LFT_Dcb.Type[i] == DOOR_DIO){
			if(k == DOOR_OPEN || k == DOOR_OPENED){
				DOSet(DO_FOP + i * 2, TRUE);
				DOSet(DO_FCL + i * 2, FALSE);
			}
			else if(k == DOOR_CLOSE || k == DOOR_CLOSED){
				DOSet(DO_FOP + i * 2, FALSE);
				DOSet(DO_FCL + i * 2, TRUE);
			}
			else{
				DOSet(DO_FOP + i * 2, FALSE);
				DOSet(DO_FCL + i * 2, FALSE);
			}
		}
		else{
			DOSet(DO_FOP + i * 2, FALSE);
			DOSet(DO_FCL + i * 2, FALSE);
		}
	}
}

/*
*********************************************************************************************************
* 	K100 door control
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* 										Front/Rear HCU Control
*********************************************************************************************************
*/
void HCU_Drive(void)
{
	INT8U k;
	letter *pletter;
	pletter = &(Folder[HCU_CFLD].Letter);

	k = PostOff[SHAFT_CAN].Txtmr;			// 30ms interval
	k &= 0x0F;								// 0 ~ 7 8* 60ms = 480ms
	
	if(k == 0x00){
		pletter->line[0] = GCOMM_FHCU;		// Front HCU group address
		pletter->line[1] = 0;				// Page 0 U-Call Lights
		Folder[HCU_CFLD].NewMsg = TRUE;
	}
	else if(k == 0x02){
		pletter->line[0] = GCOMM_RHCU;		// Rear HCU group address
		pletter->line[1] = 0;				// Page 0 U-Call Lights
		Folder[HCU_CFLD].NewMsg = TRUE;
	}
	else if(k == 0x04){
		pletter->line[0] = GCOMM_FHCU;		// Front HCU group address
		pletter->line[1] = 1;				// Page 1 D-Call Lights
		Folder[HCU_CFLD].NewMsg = TRUE;
	}
	else if(k == 0x06){
		pletter->line[0] = GCOMM_RHCU;		// Rear HCU group address
		pletter->line[1] = 1;				// Page 1 D-Call Lights
		Folder[HCU_CFLD].NewMsg = TRUE;
	}
	else if(k == 0x08){
		pletter->line[0] = GCOMM_AHCU;		// Rear HCU group address
		pletter->line[1] = 2;				// Page 2 HCU outputs control
		Folder[HCU_CFLD].NewMsg = TRUE;
	}
	else if(k == 0x0A){
		pletter->line[0] = GCOMM_AHCU;		// Rear HCU group address
		pletter->line[1] = 3;				// Page 2 HCU outputs control
		Folder[HCU_CFLD].NewMsg = TRUE;
	}
}
/*
*********************************************************************************************************
* 										Front/Rear CCU Control
*********************************************************************************************************
*/
void CCU_Drive(void)
{
	INT8U k;
	letter *pletter;
	pletter = &(Folder[CCU_CFLD].Letter);

	k = PostOff[CAR_CAN].Txtmr;
	k &= 0x0F;								// 30ms * 16 = 480 ms

	if(k == 0x00){
		pletter->line[0] = GCOMM_FCCU;		// Front CCU group address
		pletter->line[1] = 0;				// Page 0 Car-Call Lights
		Folder[CCU_CFLD].NewMsg = TRUE;
	}
	else if(k == 0x02){
		pletter->line[0] = GCOMM_RCCU;		// Rear CCU group address
		pletter->line[1] = 0;				// Page 0 U-Call Lights
		Folder[CCU_CFLD].NewMsg = TRUE;
	}
	else if(k == 0x04){
		pletter->line[0] = GCOMM_ACCU;		// Rear CCU group address
		pletter->line[1] = 2;				// Page 2 CCU outputs control
		Folder[CCU_CFLD].NewMsg = TRUE;
	}
	else if(k == 0x06){
		pletter->line[0] = GCOMM_ACCU;		// Rear CCU group address
		pletter->line[1] = 3;				// Page 2 CCU outputs control
		Folder[CCU_CFLD].NewMsg = TRUE;
	}
	else if(k == 0x08){
		pletter->line[0] = 0x08;			// Front DCU
		pletter->line[1] = 0x10;			// Page 0x10 DCU control
		Folder[CCU_CFLD].NewMsg = TRUE;
	}
}

/*
*********************************************************************************************************
* 										Miscellaneous output Control
*********************************************************************************************************
*/
void Misc_Outputs(void)
{
	INT8U k;
	k = (INT8U)(LFT_Carcb.State.Timer % 10);

	if(k < 4){ 					// 100ms *5 = 500ms
		if(CAR_M012 == M0){
			DOSet(DO_M0, TRUE);
		}
		else if(CAR_M012 == M1){
			DOSet(DO_M1, TRUE);
		}
		else if(CAR_M012 == M2){
			DOSet(DO_M2, TRUE);
		}
		else{
			DOSet(DO_M0, FALSE); DOSet(DO_M1, FALSE); DOSet(DO_M2, FALSE);
		}
	}
	else{
	 	DOSet(DO_M0, FALSE); DOSet(DO_M1, FALSE); DOSet(DO_M2, FALSE);
	}

}

void task_LFT_Mng(void)
{
	INT16U event;
	event = WaitSignal(MNG_Signal, SIG_MNGTICK);  // 25ms interval
	if(event & SIG_MNGTICK){
	 	Inverter_Drive();				// inverter

		DIO_Door(); //K100_Door();		// door
		Misc_Outputs();
	}
}
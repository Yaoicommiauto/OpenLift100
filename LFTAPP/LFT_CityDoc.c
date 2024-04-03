#define  DOC_GLOBALS
#include "includes.h"
//#pragma locate (Folder		= 0x0980)

extern INT8U const BitMapTbl[];
						/* Status  	Control*/
INT16U const DocBase[2]  = {0x0400, 0x0800};
						/*CCU HCU*/
INT16U const CityClass[2] = {3, 2};
////////////////////////////////////////////////////////////////////////////////
//  EnableFolder
//	Enables folder,
//
void SetPOff_mode(INT8U chn, INT8U mode)
{
	PostOff[chn].mode = mode;
	canSetBusOutputControl(chn, mode);
	canBusOn(chn);
}
void EnableFolder(INT8U n)
{
	if(n < NUMBER_FOLDERS)
	 	Folder[n].CK_FEN = 1;
}

void DisableFolder(INT8U n)
{
	if(n < NUMBER_FOLDERS)
		Folder[n].CK_FEN = 0;
}

void SetFolderInfo(INT8U n)
{
	if(n  < NUMBER_FOLDERS){
		Folder[n].CK_CHN = n / 2;
		Folder[n].CK_CMP = 1;
		Folder[n].CK_RTR = 0;
		Folder[n].CK_TXF = (n % 2);
		Folder[n].NewMsg = 0;
		Folder[n].CK_EXT = 1;
		Folder[n].Letter.msginfo  =  8;
		Folder[n].Letter.msginfo  |= PCAN_FF_EXT;
	}
}


/*
*********************************************************************************************************
*                                     HCU Status Pages from Shaft CANBUS
*										From HCU or GCU(Single Mode)
*********************************************************************************************************
*/
void HCU_StatusDoc(letter *pletter)
{
	INT8U FlrNo, FR;
	LFT_CCB *CallCB;

	FlrNo &= (pletter->cline[0]& 0x3F);						// Address
	if(LFT_Fcb[FlrNo].breg.FLR_ENABLE == 0)						// Floor number
		return;

	if(pletter->cline[0] < 64){
		FR = FRONT;
		if(pletter->cline[1] == 0){
			if(LFT_Fcb[FlrNo].breg.FHU_EN == 0)
				return;
		}
		if(pletter->cline[1] == 1){
			if(LFT_Fcb[FlrNo].breg.FHD_EN == 0)
				return;
		}
	}
	else if(pletter->cline[0] < 128){	
		FR = REAR;
		if(pletter->cline[1] == 0){
			if(LFT_Fcb[FlrNo].breg.RHU_EN == 0)
				return;
		}
		if(pletter->cline[1] == 1){
			if(LFT_Fcb[FlrNo].breg.RHD_EN == 0)
				return;
		}
	}
	else{
		return;					//Invalid address
	}

														// Page 0/1 HCU U/D- CALL event message
	if(pletter->cline[1] == 0 || pletter->cline[1] == 1){		
		if(FR == FRONT){
			PostOff[SHAFT_CAN].online |= WDT_FHCU;		// PostOffice WDT
		}
		else{
			PostOff[SHAFT_CAN].online |= WDT_RHCU;		// PostOffice WDT
		}
		CallCB = &(LFT_HCU[FR].LFT_Hcb[pletter->cline[1]]);
		if(pletter->line[0] & BitMapTbl[1]){			// Shifted
			Call_Event(CallCB, FlrNo, (pletter->line[0] & BitMapTbl[0])); 
		}
	}
	else if(pletter->cline[1] == 2){					// Page 2 Special Floor HCU DI Status
		if(FlrNo == LFT_Carset.ParkFlr){ 
			PostOff[SHAFT_CAN].online |= WDT_LOCK;		// PostOffice WDT
			if(pletter->line[0] & HCU_LOCK){
		  		LFT_HCU[FR].HCU_DI |= HCU_LOCK;
			}
			else{
		  		LFT_HCU[FR].HCU_DI &= ~HCU_LOCK;
			}			
		}
	}
}

/*
*********************************************************************************************************
*                                     CCU Status Pages from Car CANBUS
*********************************************************************************************************
*/
void CCU_StatusDoc(letter *pletter)
{
	INT8U i, FlrNo, FR;
	LFT_CCB *CallCB;
	
	FlrNo = pletter->line[2];
	if(LFT_Fcb[FlrNo].breg.FLR_ENABLE == 0)	//Floor number
		return;

	FR = (pletter->cline[0] < 0x10) ? FRONT : REAR;

	if(pletter->cline[1] == 0){					// HCU U/D- CALL event message
		CallCB = &(LFT_CCU[FR].LFT_Icb);
		if(pletter->line[0] & BitMapTbl[1]){		// Shifted
			Call_Event(CallCB, FlrNo, pletter->line[0] & BitMapTbl[0]); 
		}
	}
	else if(pletter->cline[1] == 1){							// Reserved
	}
	else if(pletter->cline[1] == 2){							// HCU DI Status
		for(i = 0; i <= (CCU_MAX_DI / 8); i++){
	  		LFT_CCU[FR].CCU_DI[i] = pletter->line[i];
		}
	}
	else if(pletter->cline[1] == 0x10){							// Door Status
		//pletter->line[0]???
		if(pletter->line[2] != 0){
			//report error
		}
		if(pletter->line[2] & DOOR_OL)
			LFT_Dcb.StaReg[FR] |= DOOR_OL;
		else
			LFT_Dcb.StaReg[FR] &= ~DOOR_OL;

		if(pletter->line[2] & DOOR_CL)
			LFT_Dcb.StaReg[FR] |= DOOR_CL;
		else
			LFT_Dcb.StaReg[FR] &= ~DOOR_CL;
	}
}

/*
*********************************************************************************************************
*                                     HCU Control Pages from Shaft CANBUS
*********************************************************************************************************
*/
INT8U HCU_ControlDoc(letter *pletter)
{
	INT8U Address, Pageno, FR, i;

	Address = pletter->line[0];
	Pageno  = pletter->line[1];

	pletter->cline[0] = Address;		
	pletter->cline[1] = Pageno;	

	FR = (Address == GCOMM_FHCU) ? FRONT : REAR;

	if(Pageno == 0 || Pageno == 1){			// HCU U/D-Call Lights Control Page
		for(i = 0; i <= (MAX_FLOORS / 8); i++){
			pletter->line[i] = LFT_HCU[FR].LFT_Hcb[Pageno].LFT_CallBuf[i];
		}
	}
	else if(Pageno == 2){					// HCU DO updating
		pletter->line[0] = LFT_HCU[FR].HCU_DO;
	}
	else if(Pageno == 3){							// Car Status Report to HCU 
		pletter->line[0] = LFT_Slctcb.FlrNo;
		pletter->line[1] = LFT_Carcb.Direct;
		pletter->line[2] = LFT_Carcb.Mode;
		pletter->line[3] = LFT_Carcb.State.curState;
		pletter->line[4] = LFT_Slctcb.LChar.uchar.b0;
		pletter->line[5] = LFT_Slctcb.LChar.uchar.b1;
		pletter->line[6] = LFT_Slctcb.LChar.uchar.b2;
		pletter->line[7] = LFT_Slctcb.LChar.uchar.b3;
	}
	else{
		return 2;
	}
	return 0;
}

void CCU_ControlDoc(letter *pletter)
{
	INT8U Address, Pageno, i, FR;

	Address = pletter->line[0];
	Pageno  = pletter->line[1];

	pletter->cline[0] = Address;		
	pletter->cline[1] = Pageno;

	if(Pageno == 0){								// CCU Call lights
		FR = (Address == GCOMM_FCCU) ? FRONT : REAR;
		for(i = 0; i <= (MAX_FLOORS / 8); i++){
			pletter->line[i] = LFT_CCU[FR].LFT_Icb.LFT_CallBuf[i];
		}
	}
	else if(Pageno == 1){							//reserved
	}
	else if(Pageno == 2){							// CCU DO Updating
		FR = (Address == GCOMM_FCCU) ? FRONT : REAR;
		for(i = 0; i <= (CCU_MAX_DO / 8); i++){
			pletter->line[i] = LFT_CCU[FR].CCU_DO[i];
		}
	}
	else if(Pageno == 3){
		FR = (Address == GCOMM_FCCU) ? FRONT : REAR;
		pletter->line[0] = LFT_Slctcb.FlrNo;
		pletter->line[1] = LFT_Carcb.Direct;
		pletter->line[2] = LFT_Carcb.Mode;
		pletter->line[3] = LFT_Carcb.State.curState;
		pletter->line[4] = LFT_Slctcb.LChar.uchar.b0;
		pletter->line[5] = LFT_Slctcb.LChar.uchar.b1;
		pletter->line[6] = LFT_Slctcb.LChar.uchar.b2;
		pletter->line[7] = LFT_Slctcb.LChar.uchar.b3;
	}
	else if(Pageno == 0x10){
		FR = (Address < 0x18) ? FRONT : REAR;
		pletter->line[0] = LFT_Dcb.State[FR].curState;
		pletter->line[1] = LFT_Dcb.Mode;
		pletter->line[2] = LFT_Dcb.Type[FR];
	}
}

/*
*********************************************************************************************************
*                                   STEP type std. door control message
*********************************************************************************************************
*/

void setup_PostOffice(void)
{
	INT8U i, k;
	INT32U acr, amr;
	canAllOpen();						// setup hardware interfaces

	for(i = 0; i < 4; i++){
		SetFolderInfo(i);
		EnableFolder(i);
	}

   								
	for(i = 0; i < 2; i++){
		for(k = 0; k < 2;  k++){
			Folder[i * 2 + k].Letter.ident = CityClass[i] + DocBase[k];
		}
	}

	for(i = 2; i < 4; i++){									// HCU / GCU Folders HCU Docs Base number
		Folder[i].Letter.ident += (Register_Get(EE_GLA) << 5);	// match the HCU or GCU
	}
	
/*
	acr = (long)(Folder[i].Letter.ident);
	acr <<= 16;	acr |= 0x80000000L;					// Extended B3 | = FFCAN_EXT
	amr = 0x0000FFFF;
	//canSetHwFilter(SHAFT_CAN, acr, amr);

	acr = 0L;
	amr = 0xFFFFFFFFL;
	//canSetHwFilter(SHAFT_CAN, acr, amr);

	*/
	for(i = 0; i < 2; i++){
	  	SetPOff_mode(i, COMMUNICATE);
	}
}

void POff_Timer(void)
{
	INT16U event, i;

	event = WaitSignal(CAN_Signal, SIG_CANTICK);  	// 30ms interval	 
													// from timming module, 5ms cnt
	if(event & SIG_CANTICK){
		PCANISR();
		for(i = 0; i < 2; i++){
			PostOff[i].Txtmr++;
			PostOff[i].Rxtmr++;						// 30 * 40 = 1.2s
		}			
		CCU_Drive();					// CCU
		HCU_Drive();					// HCU 
		if(PostOff[CAR_CAN].Rxtmr == 38){		// Timeout checking
			PostOff[CAR_CAN].Rxtmr = 0;
			PostOff[CAR_CAN].online = 0;
			if((PostOff[CAR_CAN].online & WDT_FCCU) == 0){
				// if(front door)
				// error
			}
			if((PostOff[CAR_CAN].online & WDT_RCCU) == 0){
				// if(rear door)
				// error
			}
			if((PostOff[CAR_CAN].online & WDT_FDCU) == 0){
				// if(front door serial type)
				// error
			}
			if((PostOff[CAR_CAN].online & WDT_FIRE) == 0){
				// if(rear door srial type)
				// error
			}
			PostOff[CAR_CAN].Rxtmr = 0;
			PostOff[CAR_CAN].online = 0;
		}

		if(PostOff[SHAFT_CAN].Rxtmr == 41){		// Timeout checking	25ms * 40 = 1000ms = 1s
			if((PostOff[SHAFT_CAN].online & WDT_FHCU) == 0){
				// if(front door)
				// warning
			}
			if((PostOff[SHAFT_CAN].online & WDT_RHCU) == 0){
				// if(rear door)
				// warning
			}
			if((PostOff[SHAFT_CAN].online & WDT_LOCK) == 0){
				// warning and Lock floor V_CALL
			}
			if((PostOff[SHAFT_CAN].online & WDT_FIRE) == 0){
				// NOT USED
			}
			PostOff[SHAFT_CAN].Rxtmr = 0;
			PostOff[SHAFT_CAN].online = 0;
		}
	}
}

void task_CAN_ProcMail(void)
{
	INT8U i, k;
    letter *pletter;                  /* Function to execute when timer times out           */

	POff_Timer();
 	i = 0;
	while(i < NUMBER_FOLDERS) {
		WDTClear();
		if(PostOff[Folder[i].CK_CHN].mode == COMMUNICATE){
			if(Folder[i].CK_FEN && Folder[i].NewMsg == 1){	// Folrder Enable && Newmsg
				if(Folder[i].CK_TXF){					// Transmit Message					
					pletter = &(Folder[i].Letter);
					if(PCAN_TXAvailable(Folder[i].CK_CHN)){
						if(i == HCU_CFLD){
							HCU_ControlDoc(pletter);
						}
						else if(i == CCU_CFLD){
							CCU_ControlDoc(pletter);
						}
					   	if(canWrite(Folder[i].CK_CHN, pletter) == 0){
							Folder[i].NewMsg = 0;
					   	}
					}
				}
				else{												// Received Message
					if(i == HCU_SFLD){
						HCU_StatusDoc(pletter);
					}
					else if(i == CCU_SFLD){
						CCU_StatusDoc(pletter);
					}
					else{
					}
					Folder[i].NewMsg = FALSE;
				}
			}
		}
		else{
			Folder[i].NewMsg = FALSE;
		}
		i++;
	}
}

 
#define  DIO_GLOBALS
#include "includes.h"

extern INT8U const BitMapTbl[];

#pragma locate (DITbl		= 0x08C2)
//#pragma locate (DOTbl		= 0x08E2)
 // next 0x08F2

/*
*********************************************************************************************************
*                                   CONFIGURE DISCRETE INPUT MODE
*                     DI_MODE_LOW               input is forced LOW
*                     DI_MODE_HIGH              input is forced HIGH
*                     DI_MODE_DIRECT            input is based on state of physical sensor (default)
*                     DI_MODE_INV               input is based on the complement of physical sensor
*********************************************************************************************************
*/

void  DICfgMode (INT8U n, INT8U mode)
{
    if (n < DIO_MAX_DI){
		DITbl[n].DIModeSel = mode;
    }
}

INT8U  DIGet(INT8U n)
{
    if (n < DIO_MAX_DI) {
        return(DITbl[n].DIVal); 
    } 
    else {
        return (0);                                        /* Return negated for invalid channel       */
    }
}

INT8U  DIGetP(INT8U n)
{
    if (n < DIO_MAX_DI) {
		if((DITbl[n].DIVal) && !(DITbl[n].DIPrev))
			return 1; 
	    else 
    	    return (0);                                        /* Return negated for invalid channel       */
    }
	else 
		return 0;
}

INT8U  DIGetN(INT8U n)
{
    if (n < DIO_MAX_DI) {
		if(!(DITbl[n].DIVal) && (DITbl[n].DIPrev))
			return 1; 
	    else 
    	    return (0);                                        /* Return negated for invalid channel       */
    }
	else 
		return 0;
}

static  void  DIUpdate (void)
{
    INT8U  i, mode;
    for (i = 0; i < DIO_MAX_DI; i++) {
		mode = 	DITbl[i].DIModeSel;

		if(DITbl[i].DIVal){
			DITbl[i].DIPrev = 1;
		}
		else{
			DITbl[i].DIPrev = 0;
		}

    	if(mode == DI_MODE_LOW){
				DITbl[i].DIVal = 0;
		}
		else if(mode == DI_MODE_HIGH){
			DITbl[i].DIVal = 1;
		}
		else if(mode == DI_MODE_INV){
			if(DITbl[i].DIIn)
				DITbl[i].DIVal = 0;
			else
				DITbl[i].DIVal = 1;
		}
		else{									//mode = DI_MODE_DIRECT
			if(DITbl[i].DIIn)
				DITbl[i].DIVal = 1;
			else
				DITbl[i].DIVal = 0;
		}
	}		
}



BOOLEAN  DOGet (INT8U n)
{
    if (n < DIO_MAX_DO) {
        return (DOTbl[n].DOOut);
    } else {
        return (FALSE);
    }
}


void  DOSet (INT8U n, BOOLEAN state)
{
    if (n < DIO_MAX_DO){
		if(state)
        	DOTbl[n].DOCtrl = 1;
		else
        	DOTbl[n].DOCtrl = 0;
    }
}

void  DOUpdate(void)
{
    INT8U     i, mode;
    for (i = 0; i < DIO_MAX_DO; i++) {           /* Process all discrete output channels               */
		if(DOTbl[i].DOCtrl)
		  	DOTbl[i].DOOut = 1;
		else
        	DOTbl[i].DOOut = 0;
	}
}



/*
*********************************************************************************************************
*                                         read PHYSICAL INPUTS
*
* Description : This function is called to read and map all of the physical inputs used for discrete
*               inputs and map these inputs to their appropriate discrete input data structure.
* Arguments   : None.
* Returns     : None.
*********************************************************************************************************
*/

void  DIRd(void)
{
    INT8U    i, j, k;
    unsigned char   msk;

    for(j = 0; j < 4; j++){
	    msk = 0x01;                                       /* Set mask to extract bit 0                */
    	for (i = 0; i < 8; i++) {                         /* Map all 8 bits to first 8 DI channels    */
	    	in[0]  = INPORT0;                                     /* Read the physical port (8 bits)          */
	    	in[1]  = INPORT1;                                     /* Read the physical port (8 bits)          */
	    	in[2]  = INPORT2;                                     /* Read the physical port (8 bits)          */
	    	in[3]  = ioport0;                                     /* Read the physical port (8 bits)          */
			DITbl[j * 8 +i].DIIn   = ((in[j] & msk) > 0) ? 1 : 0;
	            msk <<= 1;
		}
    }
}
void  DOWr(void)
{
    INT8U    i, j;
    INT8U    msk;

    for(j = 0; j < 2; j++) {
		msk = 0x01;                             /* First DO will be mapped to bit 0                        */
		out[j] = 0x00;                          /* Local 8 bit port image                                  */
		for (i = 0; i < 8; i++) {               /* Map first 8 DO to 8 bit port image                      */
	    	if (DOTbl[j * 8 + i].DOOut){
	        	out[j] |= msk;
	    	}
	    	msk <<= 1;
		}
	}

    OUTPORT0 = (INT8U)out[0];
    OUTPORT1 = (INT8U)out[1];
}

void  DIOInit (void)
{
    INT8U i, j, mode;
    INT16U V_Record;

	for(j = 0; j < 2; j++){
		V_Record = Register_Get(EE_MDI_BASE + j);
	    for (i = 0; i < 16; i++) {
			if(V_Record & ((INT16U)0x01 << i)){
				DICfgMode(j * 16 + i, DI_MODE_INV);
			}
			else{			
				DICfgMode(j * 16 + i, DI_MODE_DIRECT);
			}
		}
	}

    for (i = 0; i < DIO_MAX_DI; i++) {
		mode = 	DITbl[i].DIModeSel;
   		if(mode == DI_MODE_DIRECT)
			DITbl[i].DIVal = 0;
		else if(mode == DI_MODE_INV)
			DITbl[i].DIVal = 1;
	}
    for (i = 0; i < DIO_MAX_DO; i++){
        	//DOTbl[i] = 0;
	}
}

void  DIOTask (void)
{
	INT16U event;

	event = WaitSignal(DIO_Signal, SIG_DIOTICK);  	  /* 10ms interval	 */
	if(event & SIG_DIOTICK){
		DIRd();                                       /* Read physical inputs and map to DI channels   */
		DIUpdate();                                   /* Update all DI channels                        */
		DOUpdate();                                   /* Update all DO channels                        */
		DOWr();                                       /* Map DO channels to physical outputs           */
	}
}

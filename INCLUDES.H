
#define DEBUG 
#include <cpu_sfrs.h>
#include <cpu_funcs.h>

#include <CPLD.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>


//#endif
/*
*********************************************************************************************************
*                                           FILES TO INCLUDE
*********************************************************************************************************
*/
#define FRONT 	0
#define REAR  	1
#define UP 	  	0		// UP Led/Button 
#define DN  	1		

#define CCALL	0x01
#define HUCALL	0x04
#define HDCALL	0x10
#define VCALL	0x40

#define NOCALL 	0x00


#include  "EEDEFINE.H"
#include  "drivers/DRV_BSP.H"

typedef struct{
	INT16U 		Timer;

	INT16U 		curState  : 8;
	INT16U 		PreState : 8;

	INT16U	 	isShift  : 8;
	INT16U		reserved  : 8;
}LFT_STATE;


#include  "drivers/DRV_CANKing.h"
#include  "drivers/DRV_CANP1000.h"
#include  "drivers/DRV_DIO.H"
#include  "drivers/DRV_pwm.H"
#include  "drivers/DRV_feerom.h"
#include  "drivers/DRV_timing.h"
#include  "drivers/DRV_COMMBGND.H"
#include  "drivers/DRV_Velo.H"

#include  "LFTAPP/LFT_ModBus.h"
#include  "LFTAPP/LFT_SLCT.h"
#include  "LFTAPP/LFT_Floor.h"
#include  "LFTAPP/LFT_Call.h"
#include  "LFTAPP/LFT_Door.h"

#include  "LFTAPP/LFT_Car.h"

#include  "LFTAPP/CAR/SML_M012.h"
#include  "LFTAPP/CAR/SML_Auto.h"
#include  "LFTAPP/CAR/SML_Teach.h"
#include  "LFTAPP/CAR/SML_Insp.h"
#include  "LFTAPP/CAR/SML_Emg.h"

#include  "LFTAPP/LFT_citydoc.h"
#include  "LFTAPP/LFT_city.h"
#include  "LFTAPP/LFT_MNG.H"

#include  "LFTAPP/LFT_PAR.H"

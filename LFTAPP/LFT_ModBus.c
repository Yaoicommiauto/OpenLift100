#define  RTU_GLOBALS
#include "includes.h"
#pragma locate (RTU_POff	= 0x0A36)
#pragma locate (RTUBuf		= 0x0A3A)

#define MAX_MODFUNC		7
// MODBUS Operation Functions
#define 	FUN_RDDO 			0
#define 	FUN_RDDI 			1
#define 	FUN_RDMUL 			2
#define 	FUN_WRONE 			3
#define 	FUN_WRMUL 			4
#define 	FUN_RDID 			5
#define		FUN_DIAG			6
// diagnostic subfunction
#define 	SUB_QUERY			0

// Exception
#define 	FUNC_INVAID 		1	// The function requested is not recognized by the slave
#define 	ADDR_INVAID 		2 	// The bit or word addresses indicated in the request do not exist in the slave
#define 	NUM_INVAID 	    	3 	// The bit or word values indicated in the request are not permissible in the slave
#define 	EXCU_INVAID			4 	// The slave has started to execute the request but cannot continue to process it completely
BOOLEAN RTUStarted;
INT8U CRCHi ; /* high byte of CRC initialized */
INT8U CRCLo ; /* low byte of CRC initialized */
INT8U const MODFUNC[MAX_MODFUNC] = {1, 2, 3, 6, 16, 43, 8};

void transdat(INT8U *buf,INT8U len)
{
	INT8U i;
	DI_TI;
	CommFlush();
	for(i = 0; i < len; i++){
		putch(RTUBuf[i]);
	}
	EN_TI;
	CommPtr = 0;
	RTUStarted = FALSE;
	SetCommDir(DIR_IN);
}


INT8U CheckRTUFunc(void)
{
	INT8U i, fc;

	i = 0; fc = 0xFF;
	while(i < MAX_MODFUNC){
		if(RTUBuf[1] == MODFUNC[i]){
			fc = i;
			break;
		}
		else{
			i++;
		}
	}
	return fc;
}


/* CRC16 Table High byte */
INT8U const CRC16Hi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* CRC16 Table Low byte */
INT8U const CRC16Lo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2,
	0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
	0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
	0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
	0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
	0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6,
	0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
	0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
	0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE,
	0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
	0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA,
	0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
	0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
	0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62,
	0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
	0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE,
	0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
	0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
	0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76,
	0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
	0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
	0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
	0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
	0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A,
	0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86,
	0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

INT8U CRCHi ; /* high byte of CRC initialized */
INT8U CRCLo ; /* low byte of CRC initialized */
/*
  SendRtuCmdToModBus will add the CRC16
*/
void GetCRC16(INT8U *puchMsg, int DataLen)
{
	INT16U Index ; /* will index into CRC16 lookup table */

	CRCHi = 0xFF ; /* high byte of CRC16 initialized */
  	CRCLo = 0xFF ; /* low byte of CRC16 initialized */
  	while (DataLen--){
    	Index = CRCHi ^ *puchMsg++ ; /* calculate the CRC16 */
    	CRCHi = CRCLo ^ CRC16Hi[Index] ;
    	CRCLo = CRC16Lo[Index];
  	}
}
/*
FUNC_INVAID 		1
ADDR_INVAID 		2
NUM_INVAID 	    	3
EXCU_INVAID			4
*/
INT8U Mod_NACK(INT8U ncode)
{
	RTUBuf[0] = RTUBuf[0];
	RTUBuf[1] = RTUBuf[1] + 0x80;
	RTUBuf[2] = ncode;
	return 3;
	//GetCRC16(RTUBuf, 3);
}

INT8U Mod_ACK_Diag(void)
{
	INT8U subcode, len, i;

	subcode = RTUBuf[2] * 256 + RTUBuf[3];
	if(subcode == SUB_QUERY){
		for(i = 0; i < 6; i++){
			RTUBuf[i] = RTUBuf[i];
		}
		len = 6;
	}
	else{
		return Mod_NACK(ADDR_INVAID);
	}
	return len;
}
void test(void)
{
	RTUBuf[0] = 0x01;
	RTUBuf[1] = 0x08;
	RTUBuf[2] = 0x00;
	RTUBuf[3] = 0x00;
	RTUBuf[4] = 0xA5;
	RTUBuf[5] = 0x37;
}

/*
*********************************************************************************************************
* 									01 Read Coil Status
* Description
*  Reads the ON/OFF status of discrete outputs (0X references, coils) in the slave. 
*  Broadcast is not supported.
*********************************************************************************************************
*/
INT8U Mod_ACK_DO(void)					//
{
	INT8U i, j, k;
	INT16U address, len, msk;

	address = (INT16U)RTUBuf[2]* 256 + RTUBuf[3];
	if(address < 0x0100){
		RTUBuf[3] =  out[0];
		RTUBuf[4] =  out[1];
		len = (DIO_MAX_DO / 8);
	}
	else if(address >= 0x0100 && address <= 0x0101){		// CCU DO
		len = 2;
		RTUBuf[3] = LFT_CCU[FRONT].CCU_DO[0];
		RTUBuf[4] = LFT_CCU[REAR].CCU_DO[0];
	}
	else if(address >= 0x1000){	// HCU DO
		len = 2;
		address -= 0x1000;
		address %= MAX_FLOORS;
		
		RTUBuf[3] = LFT_HCU[FRONT].HCU_DO;
		RTUBuf[4] = LFT_HCU[REAR].HCU_DO;
	}
	else{
		return Mod_NACK(ADDR_INVAID);
	}
	RTUBuf[2] = len;
	return len + 3;
}

/*
*********************************************************************************************************
*											02 Read Input Status
* Description
* Reads the ON/OFF status of discrete inputs (1X references) in the slave.
* Broadcast is not supported.
*********************************************************************************************************
*/

INT8U Mod_ACK_DI(void)
{
	INT8U i, j, k;
	INT16U address, len;

	address = (INT16U)RTUBuf[2]* 256 + RTUBuf[3];
	RTUBuf[0] = RTUBuf[0];
	RTUBuf[1] = RTUBuf[1];

	if(address < 0x001F){
		len = (DIO_MAX_DI / 8);
		for(j = 0; j < len; j++){
			RTUBuf[3 + j] = in[j];
		}
	}
	else if(address >= 0x0100 && address <= 0x0101){
		len = 4;
		RTUBuf[3] = LFT_CCU[FRONT].CCU_DI[0];
		RTUBuf[4] = LFT_CCU[FRONT].CCU_DI[1];
		RTUBuf[5] = LFT_CCU[REAR].CCU_DI[0];
		RTUBuf[6] = LFT_CCU[REAR].CCU_DI[1];
	}
	else if(address >= 0x1000){
		len = 2;
		address -= 0x1000;
		address %= MAX_FLOORS;
		
		RTUBuf[3] = LFT_HCU[FRONT].HCU_DI;
		RTUBuf[4] = LFT_HCU[REAR].HCU_DI;
	}
	else{
		return Mod_NACK(ADDR_INVAID);
	}
	RTUBuf[2] = len;
	return len + 3;
}

/*
*********************************************************************************************************
*									03 Read Holding Registers
* Description
* Reads the binary contents of holding registers (4X references) in the slave.
* Broadcast is not supported.
*********************************************************************************************************
*/
INT8U Mod_ACK_RdRegs(void)
{
	INT8U buf[4]  = {0xFF, 0xFF, 0xFF, 0xFF};

	INT8U i,  *base;
	INT16U address, len;

	address = RTUBuf[2] * 256 + RTUBuf[3];
	len = RTUBuf[4] * 256 + RTUBuf[5];

	if(len > 6){
		len = 6;
	}

	if(address <= SPIEE_END){								// SPI EEPROM
		i = 0;
		while(1){
			if((address + i) > SPIEE_END){
				len = i;
				break;
			}
			Read_SPIEE(address + i, buf);
			RTUBuf[3 + 2 * i] = buf[1];
			RTUBuf[4 + 2 * i] = buf[0];
			i++;
			if(i >= len) break;
		}
	}
	else if(address <= FEE_END){								// Flash EEPROM
		if(address & 0x01)	return Mod_NACK(ADDR_INVAID);
		address -= FEE_START;
		address >>= 1;
		i = 0;
		while(1){
			if((i + address) >= EEPROM_RECORD_NUM){
				len = i;
				break;
			}

			Read_Record(address + i, buf);
			RTUBuf[3 + 2 * i] = buf[1];
			RTUBuf[4 + 2 * i] = buf[0];
			RTUBuf[5 + 2 * i] = buf[3];
			RTUBuf[6 + 2 * i] = buf[2];
			i += 2;
			if(i >= len) break;
		}
	}
	else if(address >= RAM_START && address <= RAM_END){
		for(i = 0; i < len; i++){
			base = (INT8U *)address;
			RTUBuf[3 + 2 * i] = (INT8U) *(base + 1);
			RTUBuf[4 + 2 * i] = (INT8U) *(base + 0);
			address += 2;
		}
	}
	else if(address >= 0x1000 && address < 0x1FF0){		//100h
		address >>= 4;
		for(i = 0; i < len; i++){
			base = (INT8U *)address;
			RTUBuf[3 + 2 * i] = (INT8U) *(base + 1);
			RTUBuf[4 + 2 * i] = (INT8U) *(base + 0);
			address += 2;
		}
	}
	else{
		return Mod_NACK(ADDR_INVAID);
	}

	len *= 2;
	RTUBuf[2] = len;
	return len + 3;
}

						
/*
*********************************************************************************************************
*	16 (10 Hex) Preset Multiple Registers
* Description
* Presets values into a sequence of holding registers (4X references). When
* broadcast, the function presets the same register references in all attached slaves.
*********************************************************************************************************
*/

INT8U Mod_ACK_WrOne(void)
{
	INT8U *base;
	INT8U buf[4]  = {0xFF, 0xFF, 0xFF, 0xFF};
	INT16U address;

	address = RTUBuf[2] * 256 + RTUBuf[3];

	if(address <= SPIEE_END){		// SPI EEPROM
		buf[1] = RTUBuf[4];
		buf[0] = RTUBuf[5];
		Update_SPIEE(address, buf);
	}
	else if(address >= RAM_START && address <= RAM_END){
		base = (INT8U *)address;
		*(base + 1) = RTUBuf[4];
		*(base + 0) = RTUBuf[5];
	}
	else if(address >= 0x1000 && address < 0x1FF0){		//100h
		address >>= 4;
		base = (INT8U *)address;
		*(base + 1) = RTUBuf[4];
		*(base + 0) = RTUBuf[5];
	}
	else{
		return Mod_NACK(ADDR_INVAID);
	}
	return 6;	//len
}

INT8U Mod_ACK_WrRegs(void)
{
/*
	INT8U i, len;
	INT8U buf[4]  = {0xFF, 0xFF, 0xFF, 0xFF};
	INT16U address;

	address = RTUBuf[2] * 256 + RTUBuf[3];
	len = RTUBuf[4] * 256 + RTUBuf[5];
	if(len > 6){
		len = 6;
	}
	if(address <= SPIEE_END){		// SPI EEPROM
		i = 0;
		while(1){
			if((i + address) > SPIEE_END){
				len = i;
				RTUBuf[4] = 0;
				RTUBuf[5] = i;
				break;
			}
			buf[1] = RTUBuf[7 + 2 * i];
			buf[0] = RTUBuf[8 + 2 * i];
			Update_SPIEE(address + i, buf);
			i++;
			if(i >= len) break;
		}
	}
	else if(address <= FEE_END){		// Flash EEPROM
		if((address & 0x01) || (len & 0x01)){
			return Mod_NACK(ADDR_INVAID);
		}
		else{
			address -= FEE_START;
			address >>= 1;
			i = 0;
			while(1){
				if((i + address) > EEPROM_RECORD_NUM){
					RTUBuf[4] = 0;
					RTUBuf[5] = i;
					break;
				}
				buf[1] = RTUBuf[7  + 2 * i];    //M
				buf[0] = RTUBuf[8  + 2 * i];    // L
				buf[3] = RTUBuf[9  + 2 * i];	// M
				buf[2] = RTUBuf[10 + 2 * i];	// L
				Update_Record(address + i, buf);
				i += 2;
				if(i >= len) break;
			}
		}
	}
	else{
		return Mod_NACK(ADDR_INVAID);
	}
	return 6;	//len
	*/
		return Mod_NACK(ADDR_INVAID);
}


// Running
INT8U Mod_ACK_RSet(void)
{
	return 1;
}


// check receive
BOOLEAN CheckModBusRtu(void) 
{ 
	INT8U c;
	BOOLEAN event;
//	if(RTU_POff.address	== 0 )
   	RTU_POff.address = 1;
	if(CommDir == DIR_OUT || CommBuf.RingBufTxCtr > 0){
		event = FALSE;
	}
	else{
		while(!CommIsEmpty()){
			c = CommGetChar();
			if(!RTUStarted){
				if(c == RTU_POff.address){
					CommPtr = 0;
					RTUStarted = TRUE;
				}
			}
			RTUBuf[CommPtr] = c;
			CommPtr++;
			CommPtr %= RTUBuf_SIZE;

			if(CommPtr >= 4 && RTUStarted){
				GetCRC16(RTUBuf, CommPtr - 2);
				if(CRCHi == RTUBuf[CommPtr - 2] && CRCLo == RTUBuf[CommPtr - 1]){
				 	RTU_POff.NewMsg = 1;
				 	CommPtr = 0;
					SetCommDir(DIR_OUT);
				 	break;
				}
				else{
					if(CommPtr > 18)
						RTUStarted = FALSE;
				}
			}
		}
	}
	return 1;
}

void setup_RTUPoffice(void)
{
	//Init_Comm(BAUD_192);

	RTU_POff.address = Register_Get(EE_CLA);
	if(RTU_POff.address > 240) RTU_POff.address = 1;
	RTU_POff.mode = LISTEN_ONLY;
	RTU_POff.NewMsg = FALSE;
	RTU_POff.timer = 0;
}

// 40ms Interval and NewMsg Driven
void task_RTUProcMail(void)
{
	INT8U Func, len;  INT16U event;

//	if(CommPtr > 0){
 //	}
	if(RTU_POff.NewMsg == 0)
		CheckModBusRtu();
	
	event = WaitSignal(RTU_Signal, SIG_RTUTICK);  	// 30ms interval	 
	if(event & SIG_RTUTICK){
		if(RTU_POff.NewMsg == 0)
			return;
		Func = CheckRTUFunc();
		if(Func == 0xff){
			len = Mod_NACK(FUNC_INVAID);	// NAK
		}
		else{
			if(Func == FUN_RDDO){
				len = Mod_ACK_DO();
			}
			else if(Func == FUN_RDDI){
				len = Mod_ACK_DI();
			}
			else if(Func == FUN_RDMUL){
				len = Mod_ACK_RdRegs();		// Read EEPROM or RAM registers
			}
			else if(Func == FUN_WRMUL){
				len = Mod_ACK_WrRegs();
			}
			else if(Func == FUN_WRONE){
				len = Mod_ACK_WrOne();
			}
			else if(Func == FUN_DIAG){
				len = Mod_ACK_Diag();
			}
			else{
				len = Mod_NACK(EXCU_INVAID);
			}

			GetCRC16(RTUBuf, len);
			RTUBuf[len] 	= CRCHi;
			RTUBuf[len + 1] = CRCLo;
			
			if(len > 0){
				transdat(RTUBuf, len + 2);
			}

			RTU_POff.NewMsg = 0;
			RTU_POff.timer	= 0;
			RTU_POff.mode = COMMUNICATE;
	 	}
		
		if(RTU_POff.mode == COMMUNICATE){
			if(RTU_POff.timer < 250){
				RTU_POff.timer++;
			}
			if(RTU_POff.timer > Register_Get(EE_CLT)){				// Timeout checking
				RTU_POff.mode = LISTEN_ONLY;
			}
		}
	}
}


#define  SYS_GLOBALS
#include "includes.h"

INT8U const  BitMapTbl[8]   = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
INT8U const  BitAboveTbl[8] = {0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00};
INT8U const  BitUnderTbl[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F};
INT8U const   BitUnMapTbl[8] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};

extern INT16U const ParMax[16][16];
extern INT16U const ParMin[16][16];


void bitset(INT8U var, INT8U bitno)
{
	INT8U x;
	x = BitMapTbl[bitno];
	var |= x;
}
void bitclr(INT8U var, INT8U bitno)
{
	INT8U x;
	x = BitMapTbl[bitno];
	var &= ~x;
}
INT8U bittest(INT8U num, INT8U bitno)
{
	INT8U x;
	x = BitMapTbl[bitno];
	if(num & x)
		return TRUE;
	else
		return FALSE;
}


void time(unsigned char cc)
{
    unsigned char i;
    for (i = 0; i < cc; i++){
        asm{
	    NOP;
	    NOP;
	    NOP;
		}
    }
}

void InitIOPORT(void)
{
/* 
* INITIALIZE THE QUASI-BIDIRECTIONAL PORT PINS.  TO USE THESE
* PINS AS INPUTS THEY MUST BE WRITTEN WITH A ONE.
*/
    OUTPORT0 	= 0;		OUTPORT1  = 0;

    int_mask 	= 0;    	int_mask1 = 0;
    int_pending = 0;		int_pend1 = 0;				       
    
  	ioport1 = 0x0FF;		ioport2 = 0x0FF;

  	wsr  = 0;
  	ioc0 = 0x01; ioc1 = 0x65; ioc2 = 0x43; 
	wsr  = 1;
	ioc3 = 0x06; //0000 0100 .2 PWM1 .3 PWM2 Enable
	wsr  =  0;

	int_mask  = 0x0A1;	// 1010 0001
	int_mask1 = 0x23;	//0000 0011
}


void InitPSD(void)
{
	unsigned char i;
    CPLD_PA_CON = 0x00; /* MCU I/O MODE */
    CPLD_PB_CON = 0x00;

    CPLD_PA_DRV = 0x00; /* CMOS OUTPUT ,LOWER SLEW RATE */
    CPLD_PB_DRV = 0000;
    CPLD_PC_DRV = 0x00;
    CPLD_PD_DRV = 0x00;

    CPLD_PA_OUT = 0x0F7; /* OUTPUT VALUE */
    CPLD_PB_OUT = 0x0FF;
    CPLD_PC_OUT = 0x98;
    CPLD_PD_OUT = 4;

    CPLD_PA_DIR = 0x08;
    CPLD_PB_DIR = 0x00; 
    CPLD_PC_DIR = 0x98;  
    CPLD_PD_DIR = 0x04;
}

void delay(unsigned char time)
{
	unsigned char i,j;

	for (i = 0; i < time; i++)
		{
			for (j = 0; j < 45; j++);
		}
}
/* This function sets the SPI to work with MCP2510 and 25LCxx Memory */
void write(INT8U cmd)
{
	INT8U i;

	for(i = 0; i < 8; i++){
		if(bittest(cmd, 7 - i))
			SDIN_1;
       	else 
       		SDIN_0;
       SCLK_1; SCLK_1; SCLK_1;
       SCLK_0; SCLK_0;
	}
}
INT8U read(void)
{
	INT8U result, i;    
	
	result = 0;
    SDOUT_1;
	for(i = 0; i < 8; i++){
		SCLK_1;	SCLK_1;
       	SDOUT_1;
		result <<= 1;
       	if(bittest(ioport1, 7))
       		result++;
       	SCLK_0;
	}
	return result;
}

INT8U read_status(void)
{
	INT8U result;
	
    SCLK_0; CSMEM_0;
	write(CMD_RDSR);
	result = read();
    CSMEM_1;
	return result;
}


void init_eerom(void)
{
	INT8U status;
	if((read_status() & 0x3C) != 0x10){
		SCLK_0; CSMEM_0;
		write(CMD_WREN);
		CSMEM_1;
		
        SCLK_0; CSMEM_0;
		write(CMD_WRSR);
		write(0x10); 	// watchdog Timer_out ~ 0.5s,PPR block protect
		delay(1);
        CSMEM_1; CSMEM_1;
		
        SCLK_0; CSMEM_0;
		write(CMD_WRDI);
        CSMEM_1; CSMEM_1;
	}
	else{
		// report error
	}
	WDTClear();
}

INT8U read_eerom(INT16U addr)
{
	INT8U result;	
//	disable();	
    SCLK_0; SCLK_0;
    CSMEM_0; CSMEM_0;
    if(addr > 0xff)
    	write(CMD_READ + 8);
    else  
    	write(CMD_READ);
     
	delay(1);
   	write((INT8U)(addr & 0xff));
	result = read();
    CSMEM_1;    CSMEM_1; CSMEM_1;

 //	enable();
	return  result;
}

void write_eerom(INT16U addr, INT8U dat)
{
	INT8U i;
//	disable();	
    SCLK_0; CSMEM_0;
    write(CMD_WREN);
    CSMEM_1; CSMEM_0;
    if(addr > 0xff)
    	write(CMD_WRITE + 8);
    else  
    	write(CMD_WRITE);
	delay(1);
	write((INT8U)(addr & 0xff));
	write(dat);
    CSMEM_1; CSMEM_0;
	write(CMD_WRDI);
    CSMEM_1;
	
	i = 0;
	while(((read_status() & 1) != 0) && (i < 50)){
	   WDTClear();
		i++;
	}
  //	enable();
}

void WDTClear(void)
{
   	CSMEM_0;   	CSMEM_0;   	CSMEM_0;
   	CSMEM_1;
}

INT8U Update_SPIEE(INT16U addr, INT8U *buf)
{
	INT8U EEGrp;
	LREG tmpv;
	if(((addr + 1) * 2) > EEPROM_SIZE)
		return 1;

	EEGrp = addr >> 4;

	RAM_SSW[EEGrp >> 3] |= BitMapTbl[EEGrp & 0x07];
	tmpv.uchar.b1 = buf[1];
	tmpv.uchar.b0 = buf[0];

	if(tmpv.wint.lsw < ParMin[EEGrp][addr & 0x0F])
		tmpv.wint.lsw = ParMin[EEGrp][addr & 0x0F];
	else if(tmpv.wint.lsw > ParMax[EEGrp][addr & 0x0F])
		tmpv.wint.lsw = ParMax[EEGrp][addr & 0x0F];

	write_eerom(addr * 2 + 0, tmpv.uchar.b0);
	write_eerom(addr * 2 + 1, tmpv.uchar.b1);
	return 0;
}

INT8U Read_SPIEE(INT16U addr, INT8U *buf)
{
	if(((addr + 1) * 2) > EEPROM_SIZE)
		return 1;
	buf[0] = read_eerom(addr * 2);
	buf[1] = read_eerom(addr * 2 + 1);
	// // // //printf("reading ee %d = %d ", (INT8U)addr, read_eerom(addr * 2));
	return 0;
}




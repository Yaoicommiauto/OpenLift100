/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/
#ifdef 	SYS_GLOBALS
#define SYS_EXT
#else
#define SYS_EXT	extern
#endif

#define BOOLEAN unsigned char  
#define INT8U   unsigned char                     /* Unsigned  8 bit quantity                           */
#define INT8S   signed   char                     /* Signed    8 bit quantity                           */
#define INT16U  unsigned int                    /* Unsigned 16 bit quantity                           */
#define INT16S  signed   int                    /* Signed   16 bit quantity                           */
#define INT32U  unsigned long                     /* Unsigned 32 bit quantity                           */
#define INT32S  signed   long                     /* Signed   32 bit quantity                           */
#define FP32    float                             /* Single precision floating point                    */
#define FP64    double                            /* Double precision floating point                    */

#define TRUE    1
#define FALSE   0



typedef union {
    unsigned long ulint;
    struct {
        unsigned int lsw;
        unsigned int msw; 
     } wint;

     struct {
        unsigned int  sfilter;
        unsigned int  hfilter;
     } filter;

     struct {
        unsigned char  cline1;
        unsigned char  cline0;
	    unsigned int   envelope;
     } msgid;

     struct{ 
        unsigned char  b0;
        unsigned char  b1;
        unsigned char  b2;
        unsigned char  b3;
    } uchar;
}LREG;


typedef union {
    unsigned long ULINT;
    struct {
        INT16U lsw;
        INT16U msw; 
     } WINT;
    struct { 
        INT8U b0;
        INT8U b1;
        INT8U b2;
        INT8U b3;
    } UCHAR;
} LCNT;


typedef struct {
    unsigned char mode;			//0
    unsigned char command;
    unsigned char status;
    unsigned char intr;
    unsigned char intrEnable;
    unsigned char _reserved0;
    unsigned char btr0;
    unsigned char btr1;
    unsigned char outputCtrl;
    unsigned char test;

    unsigned char _reserved1;
    unsigned char arbLost;
    unsigned char errorCode;
    unsigned char errorWarningLimit;
    unsigned char rxError;
    unsigned char txError;
    unsigned char frameFormat;		   //16
    union {				   
        struct {
            unsigned char id0;             //17
            unsigned char id1;		   //18
            unsigned char id2;		   //29
            unsigned char id3;		   //20
            unsigned char data[8];         //21, 22, 23 ,24, 25, 26, 27,28
        } ext;
        struct {
            unsigned char id0;
            unsigned char id1;
            unsigned char rev0;
            unsigned char rev1;
            unsigned char data[8];
        } std;
    } msg;				   
    unsigned char reserved3;		   //29
    unsigned char rxBufStart;		   //30
    unsigned char clockDivider;		   //31
}sja1000;


#define CMD_WREN				6
#define CMD_RDSR                5
#define CMD_WRDI                4
#define CMD_READ                3
#define CMD_WRITE               2
#define CMD_WRSR                1

//    ; Bit defines within status register
#define   WIP   0                 //write in progress 
#define   WEL   1                 //write enable latch 
#define   BP0   2                 //block protection bit 
#define   BP1   3                 //block protection bit 

#define MEMBLOCK1   0x01
#define MEMBLOCK2   0x02
#define MEMBLOCK3   0x04
#define MEMBLOCK4   0x08

#define EEPROM_SIZE	512
#define EE_GROUPS 	16


#define  OS_ENTER_CRITICAL()  disable();
#define  OS_EXIT_CRITICAL()   enable();

#define CSMEM_1                 ioport1 |= BitMapTbl[1]
#define CSMEM_0                 ioport1 &= ~BitMapTbl[1]

#define SCLK_1                  ioport1 |= BitMapTbl[5]
#define SCLK_0                  ioport1 &= ~BitMapTbl[5]

#define SDIN_1                  ioport1 |= BitMapTbl[6]
#define SDIN_0                  ioport1 &= ~BitMapTbl[6]

#define SDOUT_1                 ioport1 |= BitMapTbl[7]

#define CANBLK0 		(*(volatile sja1000*)( 0x1300))
#define CANBLK1 		(*(volatile sja1000*)( 0x1400))

#define OUTPORT0 	 	(*(volatile unsigned char*)( 0x1203))
#define OUTPORT1 	 	(*(volatile unsigned char*)( 0x1204))
  
#define INPORT0 	 	(*(volatile unsigned char*)( 0x1205))
#define INPORT1 	 	(*(volatile unsigned char*)( 0x1206))
#define INPORT2 	 	(*(volatile unsigned char*)( 0x1207))

#define CSIOP 		 (unsigned int) 0x1700
#define CPLD_PA_IN 	 (*(volatile unsigned char*)(CSIOP + 0x00))
#define CPLD_PB_IN 	 (*(volatile unsigned char*)(CSIOP + 0x01))
#define CPLD_PC_IN 	 (*(volatile unsigned char*)(CSIOP + 0x10))
#define CPLD_PD_IN 	 (*(volatile unsigned char*)(CSIOP + 0x11))

#define CPLD_PA_OUT 	 (*(volatile unsigned char*)(CSIOP + 0x04))
#define CPLD_PB_OUT 	 (*(volatile unsigned char*)(CSIOP + 0x05))
#define CPLD_PC_OUT 	 (*(volatile unsigned char*)(CSIOP + 0x12))
#define CPLD_PD_OUT 	 (*(volatile unsigned char*)(CSIOP + 0x13))

#define CPLD_PA_DIR 	 (*(volatile unsigned char*)(CSIOP + 0x06))
#define CPLD_PB_DIR 	 (*(volatile unsigned char*)(CSIOP + 0x07))
#define CPLD_PC_DIR 	 (*(volatile unsigned char*)(CSIOP + 0x14))
#define CPLD_PD_DIR 	 (*(volatile unsigned char*)(CSIOP + 0x15))

#define CPLD_PA_DRV 	 (*(volatile unsigned char*)(CSIOP + 0x08))
#define CPLD_PB_DRV 	 (*(volatile unsigned char*)(CSIOP + 0x09))
#define CPLD_PC_DRV 	 (*(volatile unsigned char*)(CSIOP + 0x16))
#define CPLD_PD_DRV 	 (*(volatile unsigned char*)(CSIOP + 0x17))

#define CPLD_PA_OUTENABLE (*(volatile unsigned char*)(CSIOP + 0x0C))
#define CPLD_PB_OUTENABLE (*(volatile unsigned char*)(CSIOP + 0x0D))
#define CPLD_PC_OUTENABLE (*(volatile unsigned char*)(CSIOP + 0x1A))
#define CPLD_PD_OUTENABLE (*(volatile unsigned char*)(CSIOP + 0x1B))

#define CPLD_PA_CON 	 (*(volatile unsigned char*)(CSIOP + 0x02))
#define CPLD_PB_CON 	 (*(volatile unsigned char*)(CSIOP + 0x03))

#define CPLD_IMC_A 	 (*(volatile unsigned char*)(CSIOP + 0x0A))
#define CPLD_IMC_B 	 (*(volatile unsigned char*)(CSIOP + 0x0B))
#define CPLD_IMC_C 	 (*(volatile unsigned char*)(CSIOP + 0x18))

#define CPLD_OMC_AB 	 (*(volatile unsigned char*)(CSIOP + 0x20))
#define CPLD_OMC_BC 	 (*(volatile unsigned char*)(CSIOP + 0x21))

#define CPLD_OMCMASK_AB 	 (*(volatile unsigned char*)(CSIOP + 0x22))
#define CPLD_OMCMASK_BC	 (*(volatile unsigned char*)(CSIOP + 0x23))

#define CPLD_MAINPROTECT	 (*(volatile unsigned char*)(CSIOP + 0x0C0))
#define CPLD_ALTPROTECT 	 (*(volatile unsigned char*)(CSIOP + 0x0C2))
#define CPLD_JTAG	 (*(volatile unsigned char*)(CSIOP + 0x0C7))

#define CPLD_PMMR0 	 (*(volatile unsigned char*)(CSIOP + 0x0B0))
#define CPLD_PMMR1 	 (*(volatile unsigned char*)(CSIOP + 0x0B2))
#define CPLD_PMMR2 	 (*(volatile unsigned char*)(CSIOP + 0x0B4))

#define CPLD_PAGE 	 (*(volatile unsigned char*)(CSIOP + 0x0E0))
#define CPLD_VM 		 (*(unsigned char*)(CSIOP + 0x0E2))



void time(unsigned char cc);
void InitIOPORT(void);
void InitPSD(void);


void bitset(INT8U var, INT8U bitno);
void bitclr(INT8U var, INT8U bitno);
INT8U bittest(INT8U num, INT8U bitno);

/*
 * SPI memory Operation
 */
void init_eerom(void);
void WDTClear(void);

INT8U  read_eerom(INT16U addr);
void   write_eerom(INT16U addr, INT8U dat);

INT8U Update_SPIEE(INT16U addr, INT8U *buf);
INT8U Read_SPIEE(INT16U addr, INT8U *buf);

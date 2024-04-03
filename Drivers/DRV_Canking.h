
#define CANKING_MAJOR_VERSION 1
#define CANKING_MINOR_VERSION 1

#define NIL (void*) 0 
#define NAN -1
#define TRUE 1
#define FALSE 0
#define OK 1
#define FAIL -1
#define ulong unsigned long
#define uword unsigned short int
#define ubyte INT8U

// Kings page 0 defines

// Action Mode
#define CURRENT_MODE 	0
#define RUN 			1
#define FREEZE 			2
#define RESET 			3
#define TOIAP 			4

// City Mode
#define INSPECT			1
#define ISERROR 		2
#define ISAUTO			3

// Folder information
#define ENABLE 				1
#define DISABLE 			0
#define INVALID_ENVELOPE 	-1
#define INVALID_FOLDER  	-1
#define INVALID_MSG_OBJ 	-1

// Time information

#define CLOSED_WINDOW 0
#define OPENED_WINDOW 1
#define READY_2_CLOSE 2

// Default infromation

#define NUMBER_FOLDERS 		4
#define DEFAULT_ENVELOPE 2031		//0x7ef
// Folder Information satored in flash eeprom

/*
#define	CK_FEN		0x10
#define	CK_EXT		0x20
#define	CK_CMP		0x40
#define	CK_TXF		0x80
#define	CK_RTR		0x08
#define	CK_CHN		0x07
*/


// Envelope
// TXType	Compressed	EXT		Enable 	RTR	CAN#
#define KING			0
#define M_MAYOR			1
#define M_CTRL			2
#define M_STAT			3
#define C_CTRL			4
#define C_STAT			5
#define H_CTRL			6
#define H_STAT			7

// Setup Phase Lift Control Pages
#define KP_FLRSET		32
#define MP_FLRSET		32

#define KP_CHSET		33
#define MP_CHSET		33

#define KP_POSITIONSET	34
#define MP_POSITIONSET	34

#define KP_DOORSET		35
#define MP_DOORSET		35

//#define EEROM_


// Running Phase Lift Control Pages
#define KP_CALLRUN		128
#define MP_CALLRUN		128

#define KP_FLRRUN		129
#define MP_FLRRUN		129

#define KP_DOORRUN		130
#define MP_DOORRUN		130

// Default for this uninitalized city
#define BASE_NUMBER 		0x1000
#define CITY_NUMBER 		255




////////////////////////////////////////////////////////////////////////////////
//
//	Error codes
//

#define ERROR_UNSUPPORTED_PAGE  2
#define ERROR_INVALID_ENVELOPE  3
#define ERROR_INVALID_FOLDER    4
#define ERROR_INVALID_DOCUMENT  5
#define ERROR_INVALID_FORM      6
#define ERROR_INVALID_PARAMETER 7

//------------------------------------------------------------------------------
// structure for SET_OUTPUT_MODE
#define OUTPUT_MODE_SILENT 0
#define OUTPUT_MODE_NORMAL 1


//for sja1000
#define PCAN_FF_EXT     	 0x80
#define PCAN_FF_RTR		     0x40
#define PCAN_FF_DLC       	 0x0F
typedef struct {
	INT16U	msginfo;
	INT16U	ident;
	INT8U 	cline[2];
	INT8U	line[8];
}letter;


typedef struct
{
    INT16U 	CK_TXF	: 1;
	INT16U	CK_CMP  : 1;
	INT16U	CK_EXT	: 1;
	INT16U	CK_FEN	: 1;
	INT16U	CK_RTR	: 1;
	INT16U  CK_CHN	: 10;
	INT16U  NewMsg	: 1;

    letter 	Letter; 
}folder;

//post office mode defines
// Communiction Mode
#define DISABLE			0
#define SILENT 			1
#define LISTEN_ONLY 	2
#define COMMUNICATE 	3

// online CCU
// RDCU FDCU RCCU FCCU 
#define	WDT_FCCU		0x01
#define	WDT_RCCU		0x02
#define	WDT_FDCU		0x04
#define	WDT_RDCU		0x08
// online HCU
// FIRE LOCK RHCU FHCU 
#define	WDT_FHCU		0x01
#define	WDT_RHCU		0x02
#define	WDT_LOCK		0x04
#define	WDT_FIRE		0x08
typedef struct
{
    INT8U 	mode;
	INT8U	online;

	INT8U	Txtmr;		// 25ms tick
	INT8U	Rxtmr;		// 25ms tick  0 Clear the online  and 39 check the online (1 sec)
}postoff;




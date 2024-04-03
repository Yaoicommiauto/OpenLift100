#ifdef   RTU_GLOBALS
   #define  RTU_EXT
#else
   #define  RTU_EXT  extern
#endif

#define 	RTUBUF_SIZE		20
// Register Address
#define		SPIEE_START			0x0000
#define		SPIEE_END			0x00FF

#define		FEE_START			0x0100
#define		FEE_END				0x04FF

#define		RAM_START			0x0800
#define		RAM_END				0x0FFF

/*
				0
SILENT 			1
LISTEN_ONLY 	2
COMMUNICATE 	3
*/
typedef struct {
	INT8U 	mode;
	INT8U 	address;
	INT8U	timer;
	INT8U	NewMsg;	
}LFT_POSTOFF;

#define RTUBuf_SIZE	20

RTU_EXT LFT_POSTOFF RTU_POff;
RTU_EXT	INT8U		RTUBuf[20];
RTU_EXT	INT8U		CommPtr;

void setup_RTUPoffice(void);
void task_RTUProcMail(void);


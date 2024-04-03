#ifdef   DOC_GLOBALS
   #define  DOC_EXT
#else
   #define  DOC_EXT  extern
#endif

#define CCU_SFLD		0		// Car CAN channel
#define CCU_CFLD		1
#define HCU_SFLD		2		// HCU CAN channel
#define HCU_CFLD		3

#define GCOMM_AHCU		0xF0
#define GCOMM_FHCU		0xF1
#define GCOMM_RHCU		0xF2

#define GCOMM_ACCU		0xF3
#define GCOMM_FCCU		0xF4
#define GCOMM_RCCU		0xF5

#define COMM_FDCU		0x08
#define COMM_RDCU		0x18

#define CAR_CAN			0
#define SHAFT_CAN		1

DOC_EXT postoff PostOff[2];
DOC_EXT folder Folder[NUMBER_FOLDERS];

void	HCU_StatusDoc(letter *pletter);
INT8U 	HCU_ControlDoc(letter *pletter);

void 	CCU_StatusDoc(letter *pletter);


void 	EnableFolder(INT8U n);
void 	DisableFolder(INT8U n);
void 	SetFolderInfo(INT8U n);

void 	setup_PostOffice(void);
void 	task_CAN_ProcMail(void);

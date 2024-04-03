#ifdef   FROM_GLOBALS
   #define  FROM_EXT
#else
   #define  FROM_EXT  extern
#endif

#define EEPROM_RECORD_SIZE 	4			// size in bytes of each record
#define SECTOR_SIZE			0x2000		// number of bytes per sector

#define EEPROM_RECORD_NUM 	512


// Addresses to use to invoke embedded algorithm in FLASH memory. Modify for your memory map.	
#define	BOOT_FLASH_SECTOR_0_XAAA (volatile unsigned char  *) 0xCAAA
#define	BOOT_FLASH_SECTOR_1_XAAA (volatile unsigned char  *) 0xEAAA 

#define	BOOT_FLASH_SECTOR_0_X555 (volatile unsigned char  *) 0xC555 
#define	BOOT_FLASH_SECTOR_1_X555 (volatile unsigned char  *) 0xE555 

// Sector base addresses. Modify for your memory map.
#define SECTOR_0_BASE_ADDRESS 0xC000
#define SECTOR_1_BASE_ADDRESS 0xE000


// Sector status definitions
#define ERASED				0xFF
#define RECEIVE_DATA 		0xFE
#define	VALID__SECTOR		0xFC
#define TRANSFER_COMPLETE	0xF8

// Record status definitions
#define UNINITIALIZED		0xFF
#define VALID_DATA			0xFE
#define UPDATE_DATA			0xFC
#define SUPERSEDED			0xF8

// Secondary flash sector definitions
#define SECTOR_0			0x00
#define SECTOR_1			0x01

// EEPROM error codes
#define SECTOR_FULL				0x80
#define FORMAT_FAILED			0x81
#define ILLEGAL_RECORD_NUMBER	0x82
#define INVALID_COMMAND			0x83
#define INVALID_STATUS			0x84
#define SECTOR_ID_ERROR			0x85
#define EEPROM_INIT_FAILED		0x86
#define SECTOR_ERASE_ERROR		0x87
#define FLASH_WRITE_ERROR		0x88
#define ADDRESS_ERROR			0x89
#define INVALID_SECTOR_STATE	0x8A
#define FLASH_ACCESS_ERROR		0x8B

// Flash parameters
#define NVM_DATA_POLL     0x80		// flash status "data poll" bit at DQ7   
#define NVM_DATA_TOGGLE   0x40		// flash status "toggle poll" bit at DQ6
#define NVM_ERROR         0x20 		// flash status "error" bit at DQ5   

// Direction indicators for sector swap
#define F_READ	0x00
#define F_WRITE  0x01

// Data Structures for EEPROM Emulation

struct record_entry
{
	INT8U status;
	INT8U record_data[EEPROM_RECORD_SIZE];	// data record byte
	struct record_entry *last_record_update;	// Pointer to next update. If 0xFFFF, this is the valid record.
};

// The parameter table holds the data record information for the active sector.
// Only one sector can be active at a time.
struct sector_header
{
	INT16U sector_status;		// see sector status definitions for usage of this byte
							// this must be the first byte in the sector to facilitate erase checking
	INT16U sector;			// sector number
	INT16U sector_checksum;	// complement of sector - used to verify block is not corrupted
	INT16U max_records;		// 2/set by application programmer when calling Init.
	INT16U rec_length;		// 2number of data bytes in each record (set to EEPROM_RECORD_SIZE in Init)
};


// ******** User API Function Prototypes ********//

/***** EEPROM_Format *****/
// Formats sectors 0 and 1 to accept record data
// Accepts maximum number of records allowed.
// Returns 0 on success. If error, returns 1.
// ********** WARNING ********** // 
// This function erases any existing data in both sectors
// ********** WARNING ********** // 
INT8U EEPROM_Format(INT16U max_records);

/***** Eeprom_Init *****/
// Polls secondary flash for valid sector status byte and 
// Initializes boot flash sector containing data records after power up.
// Restores any incomplete operations to last known good status.
// Returns 0 on success. If error, returns 1.
INT8U Eeprom_Init(void);

/***** Update_Record *****/
// Update record in EEPROM.
// Accepts record id and new record data.
// Returns 0 on success. If error, returns 1.
// Will swap parameters to new sector when full.
INT8U Update_Record(INT16U id, INT8U  *buf);

/***** Read_Record_Data *****/
// Reads record data into a buffer.
// Accepts record id number and buffer pointer.
// Returns 0 on success. If error, returns 1.
INT8U Read_Record(INT16U id_number, INT8U* buffer);



// ******** Low Level Function Prototypes ********//


// Flash write routines

/***** Write_Record *****/
// Update record in EEPROM.
// Accepts record id and new record data.
// Returns 0 on success. If error, returns 1.
INT8U Write_Record(INT16U id, INT8U  *buffer);

/***** Boot_Flash_Write *****/
// Writes data to secondary flash.
// Accepts address in flash to write, data byte.
// Returns 0 for successful write. If error, returns 1. 
INT8U Boot_Flash_Write(INT16U address, INT8U data_byte);

//Flash read routines

/***** Read_Record_Data_Structure *****/
// Reads an entire record structure from memory.
// Accepts record id number.
// Returns 0 on valid read. If error, returns 1.
INT16U Read_Record_Data_Structure(INT16U id_number, INT8U* buffer);

/***** Boot_Flash_Read *****/
// Reads data from secondary flash.
// Accepts flash address to read from.
// Returns data. 
INT8U Boot_Flash_Read(INT16U address);

// Flash erase routines

/***** Eeprom_Sector_Erase *****/
// Erases one boot flash sector.
// Accepts sector number (0-3).
// Returns 0 for successful erasure. If error, returns 1. 
INT8U Eeprom_Sector_Erase(INT8U sector);

/***** Eeprom_Sector_Erase_Start*****/
// Start Erases one boot flash sector.
// Accepts sector number (0-3).
// Returns 0 for successful erasure. If error, returns 1. 
INT8U Eeprom_Sector_Erase_Start(INT8U sector);

/***** Eeprom_Sector_Erase_Suspend*****/
// Susupend Erases one boot flash sector.
// Accepts sector number (0-3).
// Returns 0 for success. If error, returns 1. 
INT8U Eeprom_Sector_Erase_Suspend(INT8U sector);

/***** Eeprom_Sector_Erase Resume*****/
// Resume Erases one boot flash sector.
// Accepts sector number (0-3).
// Returns 0 for successful erasure. If error, returns 1. 
INT8U Eeprom_Sector_Erase_Resume(INT8U sector);

/***** Eeprom_Sector_Erase_Status *****/
// Gets status of sector erase.
// Accepts sector number (0-3).
// Returns 1 for erase in progress. If OK, returns 0. 
INT8U Eeprom_Sector_Erase_Status(INT8U sector);

// Database management routines

/***** Find_Next_Address *****/
// Finds next available address for data record.
// Returns address. If error, returns error code. 
INT16U Find_Next_Address(void);

/***** Find_Active_Sector *****/
// Finds active sector.
// Returns sector number. 
// If error, returns error code. 
INT8U Find_Active_Sector(INT8U io);

/***** Eeprom_Sector_Swap *****/
// Transfers data from full sector to empty one. 
INT8U Eeprom_Sector_Swap(INT16U inn, INT8U  *buf);

/***** ERASE and FORMAT SECTOR *****/
// Update record in EEPROM.
// Accepts sector id, command, and max record size.
// If error, returns error code.
INT8U E_andF_Sector(INT8U sector, INT16U max_rec);

/***** Get_Sector_Status *****/
// Returns sector status.
// If error, returns INVALID_STATUS. 
INT8U Get_Sector_Status(INT8U sector);


INT16U Register_Get(INT16U rid);
INT8U  Register_Set(INT16U rid, INT16U rv);

INT32U DRegister_Get(INT16U rid);
INT8U  DRegister_Set(INT16U rid, INT32U rv);

void   Load_EEGrp(INT8U grp, INT16U *buf);

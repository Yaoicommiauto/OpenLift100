#define  FROM_GLOBALS
#include "includes.h"
extern INT8U const BitMapTbl[];

//INT8U rrom[0x3fff];
//#pragma locate (rrom = 0xC000)


INT8U EEPROM_Format(INT16U max_records)
{
	struct record_entry  record;

	// Verify data will fit into half of sector
	if  (((sizeof(record) * max_records))  > (SECTOR_SIZE)){
		return ILLEGAL_RECORD_NUMBER;
	}

	// Format sector 0
	if ( E_andF_Sector(SECTOR_0, max_records) ) return FORMAT_FAILED;
	// Erase sector 1
	if ( Eeprom_Sector_Erase(SECTOR_1) ) return SECTOR_ERASE_ERROR;

	return 0;
}

/***** Eeprom_Init *****/
// Verifies database integrity after power loss.
// Attempts to recover data corruption after power loss.
// Re-formats database if corrupted. 
INT8U Eeprom_Init(void)
{
	struct sector_header  sector_header_0;
	struct sector_header  sector_header_1;
	struct record_entry  record;
	INT16U i, j;
	INT8U *ptr;
	INT16U  max_rec;
	INT16U  last_address;
	INT16U  base_address;
	INT16U  new_address;
	INT8U  valid_sector;

	// Get both sector headers
	ptr = (INT8U*) (&sector_header_0);
	base_address = SECTOR_0_BASE_ADDRESS;
	for ( i = 0; i < (sizeof(sector_header_0)); i++ )
	{
		ptr[i] = Boot_Flash_Read(base_address++ );
	}
	ptr = (INT8U*) (&sector_header_1);
	base_address = SECTOR_1_BASE_ADDRESS;
	for ( i = 0; i < (sizeof(sector_header_1)); i++ )
	{
		ptr[i] = Boot_Flash_Read( base_address++ );
	}

	// Check for corrupted sectors
	// This would occur if a sector erase was interrupted by a power loss
	// In this case, the sector must be re-erased
	if ( ~(sector_header_0.sector ^ sector_header_0.sector_checksum) )
	{
		if ( Eeprom_Sector_Erase(SECTOR_0) ) return SECTOR_ERASE_ERROR;
		sector_header_0.sector_status = ERASED;
	}
	if ( ~(sector_header_1.sector ^ sector_header_1.sector_checksum) )
	{
		if ( Eeprom_Sector_Erase(SECTOR_1) ) return SECTOR_ERASE_ERROR;
		sector_header_1.sector_status = ERASED;
	}

	// Get maximum number of records from header
	// If unable, return error
	if ( sector_header_0.max_records != 0xFFFF )
		max_rec = sector_header_0.max_records;
	else if ( sector_header_1.max_records != 0xFFFF )
		max_rec = sector_header_1.max_records;
	else
		return ILLEGAL_RECORD_NUMBER;

	// Check for invalid header states and repair
			DOSet(sector_header_0.sector_status / 16, 1);

	switch(sector_header_0.sector_status)
	{
    case ERASED:
		if( sector_header_1.sector_status == VALID__SECTOR )  // sector 1 is valid
		{		    
          	if ( Eeprom_Sector_Erase(SECTOR_0) ) return SECTOR_ERASE_ERROR;			
		}     
		else  // invalid state re-format database
		{
          	if ( Eeprom_Sector_Erase(SECTOR_1) ) return SECTOR_ERASE_ERROR;			
			if ( E_andF_Sector(SECTOR_0, max_rec) ) return FORMAT_FAILED;
			return INVALID_SECTOR_STATE;
     	}
		break;
	case RECEIVE_DATA:	
       	if ( sector_header_1.sector_status == VALID__SECTOR ) // use sector 1
       	{
          	if ( Eeprom_Sector_Erase(SECTOR_0) ) return SECTOR_ERASE_ERROR;			
      	}
		else if(sector_header_1.sector_status == TRANSFER_COMPLETE) //use sector 0
		{
          	if ( Eeprom_Sector_Erase(SECTOR_1) ) return SECTOR_ERASE_ERROR;	
			// update sector 0 header to valid data 
			sector_header_0.sector_status = VALID__SECTOR;
			ptr = (INT8U*) (&sector_header_0);
			base_address = SECTOR_0_BASE_ADDRESS;
			for ( i=0; i <( sizeof(sector_header_0)); i++ )
			{
				if(Boot_Flash_Write(base_address++, ptr[i] ) ) return FLASH_WRITE_ERROR;
			}
		}
		else  // invalid state erase both sectors
		{
          	if ( Eeprom_Sector_Erase(SECTOR_1) ) return SECTOR_ERASE_ERROR;			
			if ( E_andF_Sector(SECTOR_0, max_rec) ) return FORMAT_FAILED;
			return INVALID_SECTOR_STATE;
     	}
		break;
	case VALID__SECTOR:	
		if ( sector_header_1.sector_status == VALID__SECTOR ) // invalid state erase both sectors
		{		    
          	if ( Eeprom_Sector_Erase(SECTOR_1) ) return SECTOR_ERASE_ERROR;			
			if ( E_andF_Sector(SECTOR_0, max_rec) ) return FORMAT_FAILED;
			return INVALID_SECTOR_STATE;
       	}
		else // sector 0 is valid sector, erase sector 1
       	{
          	if ( Eeprom_Sector_Erase(SECTOR_1) ) return SECTOR_ERASE_ERROR;			
		}
		break;
   	case TRANSFER_COMPLETE:
		if ( sector_header_1.sector_status == VALID__SECTOR ) // erase sector 0
		{		    
          	if ( Eeprom_Sector_Erase(SECTOR_0) ) return SECTOR_ERASE_ERROR;			
		}				
		else if (sector_header_1.sector_status == RECEIVE_DATA) // erase sector 0, use sector 1
		{
          	if ( Eeprom_Sector_Erase(SECTOR_0) ) return SECTOR_ERASE_ERROR;	
			// mark sector 1 as valid sector 
			sector_header_1.sector_status = VALID__SECTOR;
			ptr = (INT8U*) (&sector_header_1);
			base_address = SECTOR_1_BASE_ADDRESS;
			for ( i=0; i <( sizeof(sector_header_1)); i++ )
			{
				if ( Boot_Flash_Write( base_address++, ptr[i] ) ) return FLASH_WRITE_ERROR;
			}
		}
		else // invalid state, format both sectors
		{
          	if ( Eeprom_Sector_Erase(SECTOR_1) ) return SECTOR_ERASE_ERROR;			
			if ( E_andF_Sector(SECTOR_0, max_rec) ) return FORMAT_FAILED;
			return INVALID_SECTOR_STATE;
 		}
		break;
	default:  // any other state, erase both sectors
 	
          	if ( Eeprom_Sector_Erase(SECTOR_1) ) return SECTOR_ERASE_ERROR;			
			if ( E_andF_Sector(SECTOR_0, max_rec) ) return FORMAT_FAILED;
			return INVALID_SECTOR_STATE;
		break;
	}

	// Check for corrupted data 
	// This would happen if a data write/update was interrupted by a power loss
	for ( i=0; i< max_rec; i++ )
	{
		WDTClear();
		// get address of last entry of each record
		last_address = Read_Record_Data_Structure( i, (INT8U*) (&record) );
		// repair record entry if necessary
		if ( record.status == UPDATE_DATA )
  		{
			// get sector	
			valid_sector = Find_Active_Sector(F_WRITE);
			if ( valid_sector == SECTOR_ID_ERROR ) return SECTOR_ID_ERROR;
			// get base address of sector
			base_address = 	SECTOR_0_BASE_ADDRESS + ((INT16U)valid_sector * SECTOR_SIZE);
 	  		// get address for repaired entry
	   		new_address = Find_Next_Address();
			if( new_address == SECTOR_FULL ) return SECTOR_FULL; // abort if sector is full
			// set status and pointer to next data
			record.status = VALID_DATA;
			record.last_record_update = (struct record_entry *)0x0FFFF;
			// write new record with old data
			ptr = (INT8U*) (&record);
			for ( j=0; j<(sizeof(record)); j++ )
			{
				 if ( Boot_Flash_Write(new_address++, ptr[j]) ) return FLASH_WRITE_ERROR;
			}
			// fix status and pointer of old record
			record.status = SUPERSEDED;
			record.last_record_update =(struct record_entry *)(new_address - sizeof(record));
			for ( j=0; j < (sizeof(record)); j++ )
			{
				if ( Boot_Flash_Write( last_address++, ptr[j] ) ) return FLASH_WRITE_ERROR;
			}
		}
	}

	return 0;
}


/***** Eeprom_Sector_Swap *****/
// Transfers data from full sector to empty one.
// Returns new sector number. 
INT8U Eeprom_Sector_Swap(INT16U inn, INT8U  *buf)
{
	struct sector_header  sector_header_old;
	struct sector_header  sector_header_new; 
	struct record_entry  buffer; 		   	
	INT8U *ptr; 
	INT16U  id;			// INT8U id 
	INT8U *ptr_old;
	INT8U *ptr_new;
	INT16U address;
	INT8U  status;
	INT8U i;
	INT8U  valid_sector;
	INT16U  new_sector; // place holder for the sector we are moving to
	INT16U  old_sector; // place holder for the sector we are moving from
        
	// get active sector  (This is the sector we are moving from)
	valid_sector = Find_Active_Sector(F_READ);
		if ( valid_sector == SECTOR_ID_ERROR ) return SECTOR_ID_ERROR;

	if(valid_sector == SECTOR_1)
	{
		new_sector = SECTOR_0_BASE_ADDRESS;  // move data to this sector
		old_sector = SECTOR_1_BASE_ADDRESS;  // move data from this sector
		sector_header_new.sector = SECTOR_0;
	}
	else
	{
		new_sector = SECTOR_1_BASE_ADDRESS; // move data to this sector
		old_sector = SECTOR_0_BASE_ADDRESS; // move data from this sector
		sector_header_new.sector = SECTOR_1;
	}
		
	// Get old sector header
	ptr_old = (INT8U*) (&sector_header_old);
	address = old_sector;
	for ( i = 0; i < sizeof(sector_header_old); i++ )
	{		
        ptr_old[i] = Boot_Flash_Read(address++);
	}

	// Verify new sector is erased
	if ( Eeprom_Sector_Erase(sector_header_new.sector)) return SECTOR_ERASE_ERROR;

	sector_header_new.sector_checksum = ~(sector_header_new.sector);
	sector_header_new.max_records = sector_header_old.max_records;
	sector_header_new.rec_length = (INT16U)EEPROM_RECORD_SIZE;
	sector_header_new.sector_status = RECEIVE_DATA;

	// mark receiving sector (new sector)      
	// mark new sector to receive 
	address = new_sector; 
	ptr_new = (INT8U*) (&sector_header_new);
	for ( i=0; i < sizeof(sector_header_new); i++ )
	{  
		if ( Boot_Flash_Write( address++, ptr_new[i] ) ) return FLASH_WRITE_ERROR;      
	}
                    
	for (id=0; id < sector_header_old.max_records; id++)
	{                       
		if(id == inn) 
		{ 
			ptr = buf; // add the new record as the data is swapped into the new sector 
			status = 0;
		}
		else
		{
			ptr = (INT8U*) (&buffer);
			status = Read_Record(id, ptr);
		}
		if( status != UNINITIALIZED ) Write_Record(id, ptr );			            
	}
      
	// mark good and bad sectors

	// mark old sector TRANSFER_COMPLETE BEFORE marking new sector ACTIVE 
	sector_header_old.sector_status =  TRANSFER_COMPLETE;                      
	address = old_sector; 
	for ( i=0; i < sizeof(sector_header_old); i++ )
	{  
		if ( Boot_Flash_Write( address++, ptr_old[i])) return FLASH_WRITE_ERROR;      
	}

	// mark new sector ACTIVE AFTER marking old TRANSFER_COMPLETE   
	sector_header_new.sector_status =   VALID__SECTOR;      
	address = new_sector; 
	for ( i=0; i < sizeof(sector_header_new); i++ )
	{  
		if ( Boot_Flash_Write( address++, ptr_new[i] ) ) return FLASH_WRITE_ERROR;      
	}
	
	// Start erase of old sector
	if ( Eeprom_Sector_Erase_Start(valid_sector) ) return SECTOR_ERASE_ERROR;
			
	return 0;
}

/***** Update_Record *****/
// Update record in EEPROM.
// Accepts record id and new record data.
// Swaps to empty sector if current sector is full
INT8U Update_Record(INT16U id, INT8U  *buf)
{
    INT8U i;
    INT8U  status;
	INT8U  bufover[EEPROM_RECORD_SIZE];
	status = Write_Record(id, buf);
   
	// when the sector is full, save the new record to ram and transfer to a blank sector	
	if( status == SECTOR_FULL ) 
	{
		// store new data in temporary buffer
//		for ( i=0; i < (sizeof(bufover)); i++ )
  //		{
			memcpy(bufover, buf, EEPROM_RECORD_SIZE);
	//	}
		// perform sector swap
		status = Eeprom_Sector_Swap(id, (INT8U *)(bufover));
	}
	return status;	
}
/***** Read_Record *****/
// Reads a data element from EEPROM.
// Accepts record id number.
// Returns record data byte. If error, returns error code.
INT8U Read_Record(INT16U id_number, INT8U* buffer)
{
	INT8U  valid_sector = 0xFF;
	  struct sector_header  header;
	  struct record_entry  record;
	INT16U i;
	INT8U *ptr;
	INT8U  *data_buf;
	INT16U  address;
	INT16U  base_address;
	INT16U  last_address;

	// get active sector
	valid_sector = Find_Active_Sector(F_READ);
	if ( valid_sector == SECTOR_ID_ERROR ){
		 return SECTOR_ID_ERROR;
	}
	// get pointer to data
	data_buf = buffer;

	// calculate base address of data
	base_address = 	SECTOR_0_BASE_ADDRESS + ((INT16U)valid_sector * SECTOR_SIZE) + 
					(INT16U)(sizeof(header)) + ( id_number * (INT16U)(sizeof(record)));

	// get base record
	ptr = (INT8U*) (&record);
	address = base_address;
	for ( i = 0; i<(sizeof(record)); i++ )
	{
		ptr[i] = Boot_Flash_Read( address++ );
	}

	// get last record
	if ( record.last_record_update != (struct record_entry *)0xFFFF )
	{
		address = base_address;
		do	
		{
			ptr = (INT8U*) (&record);
			last_address = address;
			for (i = 0; i < (sizeof(record)); i++ )
			{
				ptr[i] = Boot_Flash_Read( address++ );
			}
			address = (INT16U)record.last_record_update;
		} while ((INT16U)record.last_record_update != 0xFFFF );
	 }
	 else
	 {
	 	last_address = base_address;
	 }

	 if( record.status == UNINITIALIZED ) return UNINITIALIZED;

	// Set data buffer
	for ( i=0; i<EEPROM_RECORD_SIZE; i++ )
	{
		data_buf[i] = record.record_data[i];
	}

	return 0;
}

/***** Write_Record *****/
// Write or update record in EEPROM.
// Accepts record id and new record data.
// If error, returns error code.
INT8U Write_Record(INT16U id, INT8U  *buffer)
{
    struct sector_header  header;
    struct record_entry  record;
	INT16U i;
	INT8U  valid_sector;
	INT16U  last_address;
	INT16U  base_address;
	INT16U  new_address;
	INT16U address;
	INT8U *ptr;
	INT8U  *data_buf;

	// get active sector
	valid_sector = Find_Active_Sector(F_WRITE);

	if ( valid_sector == SECTOR_ID_ERROR ) return SECTOR_ID_ERROR;

	// get pointer to data
	data_buf = buffer;

	// calculate base address of data
	base_address = 	SECTOR_0_BASE_ADDRESS + ((INT16U)valid_sector * SECTOR_SIZE) + 
					(INT16U)(sizeof(header)) + ( id * (INT16U)(sizeof(record)));

	// get base record
	ptr = (INT8U*) (&record);
	address = base_address;
	for ( i=0; i<(sizeof(record)); i++ )
	{
		ptr[i] = Boot_Flash_Read( address++ );
	}

	// write data if record not initialized
	if ( record.status == UNINITIALIZED )
	{
		record.status = VALID_DATA;
		record.last_record_update =(struct record_entry *)0xFFFF;
//		for ( i=0; i < EEPROM_RECORD_SIZE; i++ )
 //		{
		memcpy(record.record_data, data_buf, EEPROM_RECORD_SIZE);	// set data byte
   //		}

		// write record to flash
		ptr = (INT8U*) (&record);
		address = base_address;
		for ( i=0; i < (sizeof(record)); i++ )
		{
			if ( Boot_Flash_Write( address++, ptr[i] ) ) return FLASH_WRITE_ERROR;
		}

		return 0;
	}
	
	// find last entry if record initialized	
	address = base_address;
	do
	{
		WDTClear();
		ptr = (INT8U*) (&record);
		for ( i=0; i<(sizeof(record)); i++ )
		{
			ptr[i] = Boot_Flash_Read( address++ );
		}
		if ( record.status == SUPERSEDED )
		{
			address = (INT16U)record.last_record_update;
		}
		
	} while ( record.status == SUPERSEDED);

	last_address = address - (INT16U)(sizeof(record));

	// get next available address
	new_address = Find_Next_Address();
	if( new_address == SECTOR_FULL ) return SECTOR_FULL; // abort if sector is full

	// set old record to update in progress
	address = last_address;
	for ( i=0; i<(sizeof(record)); i++ )
	{
		ptr[i] = Boot_Flash_Read( address++ );
	}
	record.status = UPDATE_DATA;
	address = last_address;
	for ( i=0; i < (sizeof(record)); i++ )
	{
		if ( Boot_Flash_Write( address++, ptr[i] ) ) return FLASH_WRITE_ERROR;
	}

	// write data to new address
	address = new_address;
	record.status = VALID_DATA;
	record.last_record_update = (struct record_entry *)0xFFFF;
	for ( i=0; i < EEPROM_RECORD_SIZE; i++ )
	{
		record.record_data[i] = data_buf[i];	// set data byte
	}
	address = new_address;
	for ( i=0; i <( sizeof(record)); i++ )
	{
		if ( Boot_Flash_Write( address++, ptr[i] ) ) return FLASH_WRITE_ERROR;
	}

	// update old record to superseded
	address = last_address;
	for ( i=0; i<(sizeof(record)); i++ )
	{
		ptr[i] = Boot_Flash_Read( address++ );
	}
	record.status = SUPERSEDED;
	record.last_record_update = (struct record_entry *)new_address;
	address = last_address;
	for ( i=0; i < (sizeof(record)); i++ )
	{
		if ( Boot_Flash_Write( address++, ptr[i] ) ) return FLASH_WRITE_ERROR;
	}

	return 0;
}

/***** Boot_Flash_Write *****/
// Writes data byte to secondary flash.
// Accepts address in flash; data (byte).
// Returns 0 for successful write. If error, returns 1. 
INT8U Boot_Flash_Write(INT16U address, INT8U data_byte)
{
	INT8U  readback;
	INT8U  done;
	INT8U  error;
	INT8U  err;
	INT8U  poll;
	INT8U  erase_flag_0 = 0;
	INT8U  erase_flag_1 = 0;
	static INT8U reentry_flag;

	// check for re-entrant call to this function
	if ( reentry_flag ) return FLASH_ACCESS_ERROR;

	reentry_flag = 1;

	// un-comment for re-entrancy test
	//EA = 1;		/* enable interrupts */

	done = FALSE;
   	err = FALSE;

	// suspend any erase in progress		
	if ( Eeprom_Sector_Erase_Status(SECTOR_0) )
    {
		if ( Eeprom_Sector_Erase_Suspend(SECTOR_0) ) return SECTOR_ERASE_ERROR;
		erase_flag_0 = 1;
	}
	if ( Eeprom_Sector_Erase_Status(SECTOR_1) )
    {
		if ( Eeprom_Sector_Erase_Suspend(SECTOR_1) ) return SECTOR_ERASE_ERROR;
		erase_flag_1 = 1;
	}

	// disable interrupts during flash write sequence
  	disable();//EA = 0;

	// write data byte
	*((char  *) (SECTOR_0_BASE_ADDRESS + 0x0555)) = 0xAA;
	*((char  *) (SECTOR_0_BASE_ADDRESS + 0x0AAA)) = 0x55;
	*((char  *) (SECTOR_0_BASE_ADDRESS + 0x0555)) = 0xA0;
	*((char  *) address) = data_byte;

	// enable interrupts following write
   	enable();//EA = 1;

	// now use dat polling method to verify successful write
	do
    {  
   		poll = *((char  *) address);	// Read the location that was just programmed
		error = poll & NVM_ERROR;		// save timeout error bit at DQ5
		poll = poll & NVM_DATA_POLL;	// get DQ7 of poll byte read from flash  

		if ((data_byte & NVM_DATA_POLL) == poll)	// compare DQ7 
			done = TRUE;		// dat byte programmed into flash OK, indicate successful exit criteria
		else if (error == NVM_ERROR)	// check for timeout error   
			err = TRUE;					// indicate timeout error occurred

    } while((done == FALSE) && (err == FALSE));

	// make sure timeout error and dat poll didn't occur simultaneously
	if (err == TRUE)                
	{
   		poll = *((char  *) address);	// Read location in flash again
		poll = poll & NVM_DATA_POLL;   				// get DQ7 of poll byte read from flash  

		if ((data_byte & NVM_DATA_POLL) == poll)	// compare DQ7 
			done = TRUE;	// dat byte programmed into flash OK at the same time timout 
							//error occured, indicate successful exit criteria

		*((char  *) (SECTOR_0_BASE_ADDRESS + 0x0555)) = 0xF0;
		//*((char  *) 0x0555) = 0xF0;	// reset the flash array (short reset instruction) 
	}	        							// now delay 3 msec per dat sheet

	// verify successful write by reading back data and comparing with original	
	readback = *((char  *) address);	// readback data from flash

	// resume any suspended erase
	if ( erase_flag_0 )
	{
		if ( Eeprom_Sector_Erase_Resume(SECTOR_0) ) return SECTOR_ERASE_ERROR;
	}
	if ( erase_flag_1 )
	{
		if ( Eeprom_Sector_Erase_Resume(SECTOR_1) ) return SECTOR_ERASE_ERROR;
	}

	reentry_flag = 0;

	return !(readback == data_byte);	// if valid return success
}

/***** Read_Record_Data_Structure *****/
// Reads an entire record structure from memory.
// Accepts record id number.
// Returns address of last record. If error, returns error message.
INT16U Read_Record_Data_Structure(INT16U id_number, INT8U* buffer)
{
	INT8U  valid_sector = 0xFF;
	  struct sector_header  header;
	  struct record_entry  record;
	INT8U i;
	INT8U *ptr;
	INT8U  *data_buf;
	INT16U address;
	INT16U  base_address;
	INT16U  last_address;

	// get active sector
	valid_sector = Find_Active_Sector(F_READ);
	if ( valid_sector == SECTOR_ID_ERROR ) return SECTOR_ID_ERROR;

	// get pointer to data
	data_buf = buffer;

	// calculate base address of data
	base_address = 	SECTOR_0_BASE_ADDRESS + ((INT16U)valid_sector * SECTOR_SIZE) + 
					(INT16U)(sizeof(header)) + ( id_number * sizeof(record));

	// get base record
	ptr = (INT8U*) (&record);
	address = base_address;
	for ( i=0; i < sizeof(record); i++ )
	{
		ptr[i] = Boot_Flash_Read( address++ );
	}

	// get last record
	if ( (INT16U)record.last_record_update != 0xFFFF )
	{
		address = base_address;
		do	
		{
			ptr = (INT8U*) (&record);
			last_address = address;
			for ( i=0; i<(sizeof(record)); i++ )
			{
				ptr[i] = Boot_Flash_Read(address++ );
			}
			address = (INT16U)record.last_record_update;
		} while ( (INT16U)record.last_record_update != 0xFFFF );
	 }
	 else
	 {
	 	last_address = base_address;
	 }

	// Get last record
	for (i=0; i<(sizeof(record)); i++)
	{
		data_buf[i] = Boot_Flash_Read(last_address++);
	}

	// return address of last record
	return last_address - sizeof(record)*2;
}

/***** Boot_Flash_Read *****/
// Reads data from secondary flash.
// Accepts address.
// Returns data at address. 
INT8U Boot_Flash_Read(INT16U address)
{	
	INT8U  erase_flag_0 = 0;
	INT8U  erase_flag_1 = 0;
	INT8U data_byte;
	static INT8U  reentry_flag;

	// check for re-entrant call to this function
	if ( reentry_flag ) return FLASH_ACCESS_ERROR;

	reentry_flag = 1;

	// un-comment for re-entrancy test
	//EA = 1;		/* enable interrupts */

	// suspend any erase in progress		
	if ( Eeprom_Sector_Erase_Status(SECTOR_0) )
    {
		if ( Eeprom_Sector_Erase_Suspend(SECTOR_0) ) return SECTOR_ERASE_ERROR;
		erase_flag_0 = 1;
	}
	if ( Eeprom_Sector_Erase_Status(SECTOR_1) )
    {
		if ( Eeprom_Sector_Erase_Suspend(SECTOR_1) ) return SECTOR_ERASE_ERROR;
		erase_flag_1 = 1;
	}

	// read data byte from flash
	data_byte = *((char  *) address);

	// resume any suspended erase
	if ( erase_flag_0 )
	{
		if ( Eeprom_Sector_Erase_Resume(SECTOR_0) ) return SECTOR_ERASE_ERROR;
	}
	if ( erase_flag_1 )
	{
		if ( Eeprom_Sector_Erase_Resume(SECTOR_1) ) return SECTOR_ERASE_ERROR;
	}

	reentry_flag = 0;

	return data_byte;
}

/***** Eeprom_Sector_Erase *****/
// Erases one boot flash sector.
// Accepts sector number (0-3).
// Returns 0 for successful erasure. If error, returns 1. 
INT8U Eeprom_Sector_Erase(INT8U sector)
{
	INT8U  done;
 	INT8U  poll;
 	INT8U  error;
 	INT8U  err;

	done = FALSE;
   	err = FALSE;

	switch (sector)
	{
	case SECTOR_0:
		*(BOOT_FLASH_SECTOR_0_X555) = 0xAA;		// unlock main flash, write 0xAA to addess 0xX555
		*(BOOT_FLASH_SECTOR_0_XAAA) = 0x55;		// unlock main flash, write 0x55 to addess 0xXAAA
		*(BOOT_FLASH_SECTOR_0_X555) = 0x80;		// write 0x80 command to erase entire chip
		*(BOOT_FLASH_SECTOR_0_X555) = 0xAA;     // continue unlock sequence
		*(BOOT_FLASH_SECTOR_0_XAAA) = 0x55;		// continue unlock sequence
		*((volatile unsigned char  *)SECTOR_0_BASE_ADDRESS) = 0x30;// erase sector
		do		  			// now use dat polling method to verify successful erase
    	{  
			poll = *((volatile unsigned char  *)SECTOR_0_BASE_ADDRESS); 	// read flash status from any address
			error = poll & NVM_ERROR;	// save timeout error bit at DQ5
			poll = poll & NVM_DATA_POLL;	// look at D7   
			if ( poll == NVM_DATA_POLL )	// compare DQ7 
				done = TRUE;		// bulk erase OK,
			else if (error == NVM_ERROR)	// check for timeout error   
				err = TRUE;		// indicate timeout error occurred
    	} while( (done == FALSE) && (err == FALSE) ); 
		if ( err == TRUE )			// make sure timeout error and dat poll didn't occur simultaneously
    	{
			poll = *((volatile unsigned char  *)SECTOR_0_BASE_ADDRESS); 	// Read flash status again
			poll = poll & NVM_DATA_POLL;	// get DQ7 of poll byte read from flash  
			if (poll == NVM_DATA_POLL)		// compare DQ7 
				done = TRUE;				// the flash erased OK at the same time timout error occured
			*(BOOT_FLASH_SECTOR_0_X555) = 0xF0;  // reset the flash array (short reset instruction) 
    	}
		break;
	case SECTOR_1:
		*(BOOT_FLASH_SECTOR_1_X555) = 0xAA;		// unlock main flash, write 0xAA to addess 0xX555
		*(BOOT_FLASH_SECTOR_1_XAAA) = 0x55;		// unlock main flash, write 0x55 to addess 0xXAAA
		*(BOOT_FLASH_SECTOR_1_X555) = 0x80;		// write 0x80 command to erase entire chip
		*(BOOT_FLASH_SECTOR_1_X555) = 0xAA;     // continue unlock sequence
		*(BOOT_FLASH_SECTOR_1_XAAA) = 0x55;		// continue unlock sequence
		*((volatile unsigned char  *)SECTOR_1_BASE_ADDRESS) = 0x30;// erase sector
		do		  			// now use dat polling method to verify successful erase
    	{  
			poll = *((volatile unsigned char  *)SECTOR_1_BASE_ADDRESS); 	// read flash status from any address
			error = poll & NVM_ERROR;	// save timeout error bit at DQ5
			poll = poll & NVM_DATA_POLL;	// look at D7   
			if ( poll == NVM_DATA_POLL )	// compare DQ7 
				done = TRUE;		// bulk erase OK,
			else if (error == NVM_ERROR)	// check for timeout error   
				err = TRUE;		// indicate timeout error occurred
    	} while( (done == FALSE) && (err == FALSE) ); 
		if ( err == TRUE )			// make sure timeout error and dat poll didn't occur simultaneously
    	{
			poll = *((volatile unsigned char  *)SECTOR_1_BASE_ADDRESS); 	// Read flash status again
			poll = poll & NVM_DATA_POLL;	// get DQ7 of poll byte read from flash  
			if (poll == NVM_DATA_POLL)		// compare DQ7 
				done = TRUE;				// the flash erased OK at the same time timout error occured
			*(BOOT_FLASH_SECTOR_0_X555) = 0xF0;  // reset the flash array (short reset instruction) 
    	}
		break;
	default:
		return 1;
	}

   	return !(done);	
}

/***** Eeprom_Sector_Erase_Start*****/
// Start Erases one boot flash sector.
// Accepts sector number (0-3).
// Returns 0 for successful erasure. If error, returns 1. 
INT8U Eeprom_Sector_Erase_Start(INT8U sector)
{
	switch (sector)
	{
	case SECTOR_0:
		*(BOOT_FLASH_SECTOR_0_X555) = 0xAA;		// unlock main flash, write 0xAA to addess 0xX555
		*(BOOT_FLASH_SECTOR_0_XAAA) = 0x55;		// unlock main flash, write 0x55 to addess 0xXAAA
		*(BOOT_FLASH_SECTOR_0_X555) = 0x80;		// write 0x80 command to erase entire chip
		*(BOOT_FLASH_SECTOR_0_X555) = 0xAA;     // continue unlock sequence
		*(BOOT_FLASH_SECTOR_0_XAAA) = 0x55;		// continue unlock sequence
		*((volatile unsigned char  *)SECTOR_0_BASE_ADDRESS) = 0x30;// erase sector
		break;
	case SECTOR_1:
		*(BOOT_FLASH_SECTOR_1_X555) = 0xAA;		// unlock main flash, write 0xAA to addess 0xX555
		*(BOOT_FLASH_SECTOR_1_XAAA) = 0x55;		// unlock main flash, write 0x55 to addess 0xXAAA
		*(BOOT_FLASH_SECTOR_1_X555) = 0x80;		// write 0x80 command to erase entire chip
		*(BOOT_FLASH_SECTOR_1_X555) = 0xAA;     // continue unlock sequence
		*(BOOT_FLASH_SECTOR_1_XAAA) = 0x55;		// continue unlock sequence
		*((volatile unsigned char  *)SECTOR_1_BASE_ADDRESS) = 0x30;// erase sector
		break;
	default:
		return 1;

	}
   	return 0;
}

/***** Eeprom_Sector_Erase_Suspend*****/
// Susupend Erases one boot flash sector.
// Accepts sector number (0-3).
// Returns 0 for successful erasure. If error, returns 1. 
INT8U Eeprom_Sector_Erase_Suspend(INT8U sector)
{
	switch (sector)
	{
	case SECTOR_0:
		*((volatile unsigned char  *)SECTOR_0_BASE_ADDRESS) = 0xB0;// Suspend erase sector
		break;
	case SECTOR_1:
		*((volatile unsigned char  *)SECTOR_1_BASE_ADDRESS) = 0xB0;// Suspend erase sector
		break;
	default:
		return 1;
	}


   	return 0;
}

/***** Eeprom_Sector_Erase_Resume*****/
// Resume Erases one boot flash sector.
// Accepts sector number (0-3).
// Returns 0 for successful erasure. If error, returns 1. 
INT8U Eeprom_Sector_Erase_Resume(INT8U sector)
{
	switch (sector)
	{
	case SECTOR_0:
		*((volatile unsigned char  *)SECTOR_0_BASE_ADDRESS) = 0x30;// Resume erase sector
		break;
	case SECTOR_1:
		*((volatile unsigned char  *)SECTOR_1_BASE_ADDRESS) = 0x30;// Resume erase sector
		break;
	default:
		return 1;
	}

   	return 0;
}

/***** Eeprom_Sector_Erase_Status *****/
// Gets status of sector erase.
// Accepts sector number (0-3).
// Returns 0 if erase cycle finished. Returns 1 if erase in progress. 
INT8U Eeprom_Sector_Erase_Status(INT8U sector)
{
	INT8U poll;

	switch (sector)
	{
	case SECTOR_0:
		poll = *((volatile unsigned char  *)SECTOR_0_BASE_ADDRESS);
		return !( poll & 0x80); 
	case SECTOR_1:
		poll = *((volatile unsigned char  *)SECTOR_1_BASE_ADDRESS);
		return !( poll & 0x80); 
	default:
		return 1;
	}
   	return 0;
}

/***** Find_Next_Address *****/
// Finds next available address for data record.
// Returns next available address. 
INT16U Find_Next_Address(void)
{
	INT8U  valid_sector;
	  struct sector_header  header;
	  struct record_entry  record;
	INT16U address;
	INT8U *ptr;
	INT16U i;

	// Find valid sector
	valid_sector = Find_Active_Sector(F_WRITE);
	if ( valid_sector == SECTOR_ID_ERROR ) return SECTOR_ID_ERROR;

	// Get sector header
	ptr = (INT8U*) (&header);
	address = SECTOR_0_BASE_ADDRESS + ((INT16U)valid_sector * SECTOR_SIZE);	
	for ( i=0; i<(INT16U)(sizeof(header)); i++ )
	{
		ptr[i] = Boot_Flash_Read( address++ );
	}

	// Calculate address
	address = SECTOR_0_BASE_ADDRESS + ((INT16U)valid_sector * SECTOR_SIZE) + 	// sector base address
	(INT16U)(sizeof(header)) + 													// sector header
	(header.max_records * (INT16U)(sizeof(record)));							// first instance of each record
	ptr = (INT8U*) (&record);
	for ( i=0; i < SECTOR_SIZE; i += sizeof(record) )
	{
		if ( Boot_Flash_Read( address ) == UNINITIALIZED ) return address;
		address += sizeof(record);
		if ( address >= (SECTOR_0_BASE_ADDRESS + ((INT16U)valid_sector * SECTOR_SIZE) 
			+ SECTOR_SIZE - (INT16U)(sizeof(record))) ) return SECTOR_FULL;
	}
	return ADDRESS_ERROR;
}


/***** Find_Active_Sector *****/
// Finds active sector.
// Returns sector number. 
// If error, returns error code. 
INT8U Find_Active_Sector(INT8U io)
{
	  struct sector_header  header0;
	  struct sector_header  header1;
	INT16U address;
	INT8U *ptr;
	INT8U i;

	// Check sector 0 for status	
	ptr = (INT8U*) (&header0);
	address = SECTOR_0_BASE_ADDRESS;
	for ( i=0; i < (sizeof(header0)); i++ )
	{
		ptr[i] = Boot_Flash_Read( address++ );
	}
	// Check sector 1 for status	
	ptr = (INT8U*) (&header1);
	address = SECTOR_1_BASE_ADDRESS;
	for ( i=0; i < (sizeof(header1)); i++ )
	{
		ptr[i] = Boot_Flash_Read( address++ ); 
	}	

	switch (io)
	{
	case F_WRITE: // write
		if( header0.sector_status == VALID__SECTOR )
		{
			if( header1.sector_status == RECEIVE_DATA )
			{
				return (SECTOR_1);
			} 
			else
			{
				return (SECTOR_0); 
			}
		}
		else if( header1.sector_status == VALID__SECTOR )			 
		{
			if( header0.sector_status == RECEIVE_DATA )
			{
				return (SECTOR_0);
			}
			else
			{
				return (SECTOR_1); 
			}
		}
	case F_READ:  // read  
		if( header0.sector_status == VALID__SECTOR )
		{	    
			return (SECTOR_0); 
		}
		else if( header1.sector_status == VALID__SECTOR )           
		{
			return (SECTOR_1); 
		}			 
	default: 
	        return (SECTOR_0); 		
	}
	return 1;
}



/***** ERASE and FORMAT SECTOR *****/
// Erases and formats a sector.
// Accepts sector number, type of operation and max records allowed.
// Legal sectors are:
// 	SECTOR_0 for address 0x8000
//	SECTOR_1 for address 0xA000
// If error, returns error code.
INT8U E_andF_Sector(INT8U sector, INT16U max_rec)
{                                          
	struct sector_header  header;           
	INT8U i;
	INT16U address;
	INT8U *ptr;

	header.sector_status = VALID__SECTOR;
	header.sector = sector;
	header.sector_checksum = ~(sector);
	header.max_records = max_rec;
	header.rec_length = (INT16U)EEPROM_RECORD_SIZE;

	if(sector == SECTOR_0) address = SECTOR_0_BASE_ADDRESS;
	if(sector == SECTOR_1) address = SECTOR_1_BASE_ADDRESS;

	if ( Eeprom_Sector_Erase(sector) ) return SECTOR_ERASE_ERROR;

	ptr = (INT8U*) (&header);

	// write sector header		
	for ( i=0; i < sizeof( header ); i++ ) 
	{
		if ( Boot_Flash_Write( address++, ptr[i] ) ) return FLASH_WRITE_ERROR;
	}

	return 0;
}

/***** Get_Sector_Status *****/
// Returns sector status.
// If error, returns INVALID_STATUS. 
INT8U Get_Sector_Status(INT8U sector)
{
	struct sector_header  header;
	INT16U address;
	INT8U *ptr;
	INT8U i;

	// Get sector header	
	ptr = (INT8U*) (&header);
	address = SECTOR_0_BASE_ADDRESS + (sector * SECTOR_SIZE);
	for ( i=0; i < sizeof(header); i++ )
	{
		ptr[i] = Boot_Flash_Read( address++ );
	}
	
	switch (header.sector_status)
	{
		case ERASED:
			return ERASED;
		case RECEIVE_DATA:
			return RECEIVE_DATA;
		case VALID__SECTOR:
			return VALID__SECTOR;
		case TRANSFER_COMPLETE:
			return TRANSFER_COMPLETE;
		default:
			return INVALID_STATUS;			
	}
}


INT16U Register_Get(INT16U rid)
{
	INT16U address;
	INT8U buf[4]  = {0xFF, 0xFF, 0xFF, 0xFF};
	LREG tmpv;
	
	if(rid <= SPIEE_END){
		Read_SPIEE(rid, buf);
		tmpv.uchar.b0 = buf[0];
		tmpv.uchar.b1 = buf[1];
	}
	else if(rid <= FEE_END){
		address = rid - FEE_START;
		if(address & 0x01){
			address >>= 1;
			Read_Record(address, buf);
			tmpv.uchar.b0 = buf[2];
			tmpv.uchar.b1 = buf[3];
		}
		else{
			address >>= 1;
			Read_Record(address, buf);
			tmpv.uchar.b0 = buf[0];
			tmpv.uchar.b1 = buf[1];
		}
	}
	else{
		tmpv.wint.lsw = 0;
	}

	return tmpv.wint.lsw;
}


INT8U Register_Set(INT16U rid, INT16U rv)
{
	INT8U x, y;
	INT8U buf[4]  = {0xFF, 0xFF, 0xFF, 0xFF};
	INT8U EEGrp;
	LREG tmpv;
	tmpv.ulint = rv;
	buf[0] = tmpv.uchar.b0;
	buf[1] = tmpv.uchar.b1;
	buf[2] = tmpv.uchar.b2;
	buf[3] = tmpv.uchar.b3;

	EEGrp = (INT8U)(rid >> 4);
	y = EEGrp >> 3; x = BitMapTbl[EEGrp & 0x07];
	RAM_SSW[y] |= x;

	if(rid <= SPIEE_END){
		Update_SPIEE(rid,  buf);
	}
	else{
		return 0x55;		// Invalid record No
	}
	return 0;
}

INT32U DRegister_Get(INT16U rid)
{
	LREG tmpv;
	INT8U buf[4]  = {0xFF, 0xFF, 0xFF, 0xFF};
	INT16U address;

	if(rid <= SPIEE_END){
		tmpv.wint.lsw = Register_Get(rid);
		tmpv.wint.msw = Register_Get(rid + 1);
	}
	else if(rid <= FEE_END){
		if(rid & 0x01) return 0;

		address = rid - FEE_START;
		address >>= 1;
		Read_Record(address , buf);
		tmpv.uchar.b0 = buf[0];
		tmpv.uchar.b1 = buf[1];
		tmpv.uchar.b2 = buf[2];
		tmpv.uchar.b3 = buf[3];
	}
	else{
		return 0;
	}
	return tmpv.ulint;
}

INT8U  DRegister_Set(INT16U rid, INT32U rv)
{
	INT16U address;
	LREG tmpv;
	INT8U fbuf[4]  = {0xFF, 0xFF, 0xFF, 0xFF};
	INT8U status;

	tmpv.ulint = rv;
	if(rid <= SPIEE_END){
		status = Register_Set(rid, tmpv.wint.lsw);
		status = Register_Set(rid + 1, tmpv.wint.msw);
	}
	else if(rid <= FEE_END){
		address = rid - FEE_START;
		address >>= 1;
		fbuf[0] = tmpv.uchar.b0;
		fbuf[1] = tmpv.uchar.b1;
		fbuf[2] = tmpv.uchar.b2;
		fbuf[3] = tmpv.uchar.b3;
		status = Update_Record(address, fbuf);
	}
	else{
		return 1;
	}
	return status;
}

void Load_EEGrp(INT8U grp, INT16U *fbuf)
{
	INT16U i, x, y;
	LREG tmpv;
	for(i = 0; i < 16; i += 2){

		tmpv.ulint = DRegister_Get(grp * 16 + i);
		fbuf[0 + i] = tmpv.wint.lsw;
		fbuf[1 + i] = tmpv.wint.msw;
	}
	y = grp >> 4; x = 0x01 << (grp & 0x0F);//BitMapTbl[grp & 0x0F];
	RAM_SSW[y] &= ~x;
}

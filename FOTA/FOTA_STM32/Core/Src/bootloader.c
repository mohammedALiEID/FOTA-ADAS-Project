
/* ----------------- Includes -----------------*/
#include "bootloader.h"

/* ----------------- Static Functions Decleration -----------------*/
static void Bootloader_Get_Version(uint8_t *Host_Buffer);
static void Bootloader_Get_Help(uint8_t *Host_Buffer);
static void Bootloader_Get_Chip_Identification_Number(uint8_t *Host_Buffer);
static void Bootloader_Read_Protection_Level(uint8_t *Host_Buffer);
static void Bootloader_Erase_Flash(uint8_t *Host_Buffer);
static void Bootloader_Memory_Write(uint8_t *Host_Buffer);
static void Bootloader_Change_Read_Protection_Level(uint8_t *Host_Buffer);
static void Bootloader_Jump_To_User_App(uint8_t *Host_Buffer) ;

static uint8_t Bootloader_CRC_Verify(uint8_t *pData, uint32_t Data_Len, uint32_t Host_CRC);
static void Bootloader_Send_ACK(uint8_t Replay_Len);
static void Bootloader_Send_NACK(void);
static void Bootloader_Send_Data_To_Host(uint8_t *Host_Buffer, uint32_t Data_Len);
static uint8_t Host_Address_Verification(uint32_t Jump_Address);
static uint8_t Perform_Flash_Erase(uint32_t Page_address, uint8_t Number_Of_Pages) ;

/* ----------------- Global Variables Definitions -----------------*/
static uint8_t BL_Host_Buffer[BL_HOST_BUFFER_RX_LENGTH];
static uint8_t APPJUMP = 1;
static uint8_t Bootloader_Supported_CMDs[12] = {
		CBL_GET_VER_CMD,
		CBL_GET_HELP_CMD,
		CBL_GET_CID_CMD,
		CBL_GET_RDP_STATUS_CMD,
		CBL_JUMP_TO_APP_CMD ,
		CBL_FLASH_ERASE_CMD,
		CBL_MEM_WRITE_CMD,
		CBL_ED_W_PROTECT_CMD,
		CBL_MEM_READ_CMD,
		CBL_READ_SECTOR_STATUS_CMD,
		CBL_OTP_READ_CMD,
		CBL_CHANGE_ROP_Level_CMD
};

/* -----------------  Software Interfaces Definitions -----------------*/

BL_Status BL_UART_Fetch_Host_Command(void){
	BL_Status Status = BL_NACK;
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint8_t Data_Length = 0;

	memset(BL_Host_Buffer, 0, BL_HOST_BUFFER_RX_LENGTH);

	/* Read the length of the command packet received from the HOST */
	HAL_Status = HAL_UART_Receive(BL_HOST_COMMUNICATION_UART, BL_Host_Buffer, 1, HAL_MAX_DELAY);
	if(HAL_Status != HAL_OK){
		Status = BL_NACK;
	}
	else{
		Data_Length = BL_Host_Buffer[0];
		/* Read the command packet received from the HOST */
		HAL_Status = HAL_UART_Receive(BL_HOST_COMMUNICATION_UART, &BL_Host_Buffer[1], Data_Length, HAL_MAX_DELAY);
		if(HAL_Status != HAL_OK){
			Status = BL_NACK;
		}
		else{
			switch(BL_Host_Buffer[1]){
			case CBL_GET_VER_CMD:
				Bootloader_Get_Version(BL_Host_Buffer);

				Status = BL_OK;
				break;
			case CBL_GET_HELP_CMD:
				Bootloader_Get_Help(BL_Host_Buffer);
				Status = BL_OK;
				break;
			case CBL_GET_CID_CMD:
				Bootloader_Get_Chip_Identification_Number(BL_Host_Buffer);
				Status = BL_OK;
				break;
			case CBL_GET_RDP_STATUS_CMD:
				Bootloader_Read_Protection_Level(BL_Host_Buffer);
				Status = BL_OK;
				break;
			case CBL_JUMP_TO_APP_CMD:
				Bootloader_Jump_To_User_App(BL_Host_Buffer) ;
				Status = BL_OK;
				break;
			case CBL_FLASH_ERASE_CMD:
				Bootloader_Erase_Flash(BL_Host_Buffer);
				Status = BL_OK;
				break;
			case CBL_MEM_WRITE_CMD:
				Bootloader_Memory_Write(BL_Host_Buffer);
				Status = BL_OK;
				break;

			case CBL_CHANGE_ROP_Level_CMD:
				Bootloader_Change_Read_Protection_Level(BL_Host_Buffer);
				Status = BL_OK;
				break;
			default:
				break;
			}
		}
	}

	return Status;
}









/* ----------------- Static Functions Definitions -----------------*/

static void Bootloader_Jump_To_User_App(uint8_t *Host_Buffer)
{
	uint16_t Host_CMD_Packet_Len = 0;
	uint32_t Host_CRC32 = 0;

	/* Extract the CRC32 and packet length sent by the HOST */
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t *)((Host_Buffer + Host_CMD_Packet_Len) - CRC_TYPE_SIZE_BYTE));
	/* CRC Verification */
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32))
	{
		if(0xFFFFFFFF != *((volatile uint32_t *)FLASH_Page_BASE_ADDRESS))
		{
			APPJUMP = 1 ;
			//Bootloader_Send_ACK(1);
			Bootloader_Send_Data_To_Host((uint8_t *)&APPJUMP, 1);
			/* Value of the main stack pointer of our main application */
			uint32_t MSP_Value = *((volatile uint32_t *)FLASH_Page_BASE_ADDRESS);

			/* Reset Handler definition function of our main application */
			uint32_t MainAppAddr = *((volatile uint32_t *)(FLASH_Page_BASE_ADDRESS + 4));

			/* Fetch the reset handler address of the user application */
			pMainApp ResetHandler_Address = (pMainApp)MainAppAddr;

			/* Set Main Stack Pointer */
			__set_MSP(MSP_Value);



			/* Jump to Application Reset Handler */
			ResetHandler_Address();
		}
		else
		{
			APPJUMP = 0 ;

			Bootloader_Send_Data_To_Host((uint8_t *)&APPJUMP, 1);
		}
	}
	else{
		Bootloader_Send_NACK();
	}
}





static uint8_t Bootloader_CRC_Verify(uint8_t *pData, uint32_t Data_Len, uint32_t Host_CRC){
	uint8_t CRC_Status = CRC_VERIFICATION_FAILED;
	uint32_t MCU_CRC_Calculated = 0;
	uint8_t Data_Counter = 0;
	uint32_t Data_Buffer = 0;
	/* Calculate CRC32 */
	for(Data_Counter = 0; Data_Counter < Data_Len; Data_Counter++){
		Data_Buffer = (uint32_t)pData[Data_Counter];
		MCU_CRC_Calculated = HAL_CRC_Accumulate(CRC_ENGINE_OBJ, &Data_Buffer, 1);
	}
	/* Reset the CRC Calculation Unit */
	__HAL_CRC_DR_RESET(CRC_ENGINE_OBJ);
	/* Compare the Host CRC and Calculated CRC */
	if(MCU_CRC_Calculated == Host_CRC){
		CRC_Status = CRC_VERIFICATION_PASSED;
	}
	else{
		CRC_Status = CRC_VERIFICATION_FAILED;
	}

	return CRC_Status;
}






static void Bootloader_Send_ACK(uint8_t Replay_Len){
	uint8_t Ack_Value[2] = {0};
	Ack_Value[0] = CBL_SEND_ACK;
	Ack_Value[1] = Replay_Len;
	HAL_UART_Transmit(BL_HOST_COMMUNICATION_UART, (uint8_t *)Ack_Value,2, HAL_MAX_DELAY);///////////////////////
}




static void Bootloader_Send_NACK(void){
	uint8_t Ack_Value = CBL_SEND_NACK;
	HAL_UART_Transmit(BL_HOST_COMMUNICATION_UART, &Ack_Value, 1, HAL_MAX_DELAY);
}




static void Bootloader_Send_Data_To_Host(uint8_t *Host_Buffer, uint32_t Data_Len){
	HAL_UART_Transmit(BL_HOST_COMMUNICATION_UART, Host_Buffer, Data_Len, HAL_MAX_DELAY);
}




static void Bootloader_Get_Version(uint8_t *Host_Buffer){
	uint8_t BL_Version[4] = { CBL_VENDOR_ID, CBL_SW_MAJOR_VERSION, CBL_SW_MINOR_VERSION, CBL_SW_PATCH_VERSION };
	uint16_t Host_CMD_Packet_Len = 0;
	uint32_t Host_CRC32 = 0;


	/* Extract the CRC32 and packet length sent by the HOST */
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t *)((Host_Buffer + Host_CMD_Packet_Len) - CRC_TYPE_SIZE_BYTE));
	/* CRC Verification */
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){


		//Bootloader_Send_ACK(4);
		Bootloader_Send_Data_To_Host((uint8_t *)(&BL_Version[0]), 4);


	}
	else{

		Bootloader_Send_NACK();
	}
}





static void Bootloader_Get_Help(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len = 0;
	uint32_t Host_CRC32 = 0;

	/* Extract the CRC32 and packet length sent by the HOST */
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t *)((Host_Buffer + Host_CMD_Packet_Len) - CRC_TYPE_SIZE_BYTE));
	/* CRC Verification */
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){

		//Bootloader_Send_ACK(12);
		Bootloader_Send_Data_To_Host((uint8_t *)(&Bootloader_Supported_CMDs[0]), 12);
	}
	else{

		Bootloader_Send_NACK();
	}
}





static void Bootloader_Get_Chip_Identification_Number(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len = 0;
	uint32_t Host_CRC32 = 0;
	uint16_t MCU_Identification_Number = 0;

	/* Extract the CRC32 and packet length sent by the HOST */
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t *)((Host_Buffer + Host_CMD_Packet_Len) - CRC_TYPE_SIZE_BYTE));
	/* CRC Verification */
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){


		/* Get the MCU chip identification number */
		MCU_Identification_Number = (uint16_t)((DBGMCU->IDCODE) & 0x00000FFF);
		/* Report chip identification number to HOST */
		//	Bootloader_Send_ACK(2);/////////////////////////
		Bootloader_Send_Data_To_Host((uint8_t *)&MCU_Identification_Number, 2);

	}
	else{

		Bootloader_Send_NACK();
	}
}




static uint8_t Host_Address_Verification(uint32_t Jump_Address){
	uint8_t Address_Verification = ADDRESS_IS_INVALID;
	if((Jump_Address >= SRAM_BASE) && (Jump_Address <= STM32F103_SRAM_END)){
		Address_Verification = ADDRESS_IS_VALID;
	}

	else if((Jump_Address >= FLASH_BASE) && (Jump_Address <= STM32F103_FLASH_END)){
		Address_Verification = ADDRESS_IS_VALID;
	}
	else{
		Address_Verification = ADDRESS_IS_INVALID;
	}
	return Address_Verification;
}









static uint8_t CBL_STM32F103_Get_RDP_Level(){


	FLASH_OBProgramInitTypeDef FLASH_OBProgram;
	/* Get the Option byte configuration */
	HAL_FLASHEx_OBGetConfig(&FLASH_OBProgram);

	return ((uint8_t)(FLASH_OBProgram.RDPLevel));
}






static void Bootloader_Read_Protection_Level(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len = 0;
	uint32_t Host_CRC32 = 0;
	uint8_t RDP_Level = 0;

	/* Extract the CRC32 and packet length sent by the HOST */
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t *)((Host_Buffer + Host_CMD_Packet_Len) - CRC_TYPE_SIZE_BYTE));
	/* CRC Verification */
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){


		/* Read Protection Level */
		RDP_Level = CBL_STM32F103_Get_RDP_Level();
		/* Report Valid Protection Level */
		//	Bootloader_Send_ACK(1);
		Bootloader_Send_Data_To_Host((uint8_t *)&RDP_Level, 1);
	}
	else{

		Bootloader_Send_NACK();
	}
}





static uint8_t Perform_Flash_Erase(uint32_t Page_address, uint8_t Number_Of_Pages){
	uint8_t Page_Validity_Status = INVALID_PAGE_NUMBER;
	FLASH_EraseInitTypeDef pEraseInit;
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint32_t PagError = 0;

	if(Number_Of_Pages > CBL_FLASH_MAX_Page_NUMBER){
		/* Number Of sectors is out of range */
		Page_Validity_Status = INVALID_PAGE_NUMBER;
	}
	else{
		if((Number_Of_Pages <= (CBL_FLASH_MAX_Page_NUMBER )) || (CBL_FLASH_MASS_ERASE == Page_address)){
			/* Check if user needs Mass erase */
			if(CBL_FLASH_MASS_ERASE == Page_address){
				pEraseInit.TypeErase = FLASH_TYPEERASE_MASSERASE; /* Flash Mass erase activation */

			}
			else{
				/* User needs Sector erase */


				pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES; /* Sectors erase only */
				pEraseInit.NbPages = Number_Of_Pages;        /* Initial FLASH sector to erase when Mass erase is disabled */
				pEraseInit.PageAddress =  Page_address  ; //0x8008000 ; /* Number of sectors to be erased. */
			}

			pEraseInit.Banks = FLASH_BANK_1; /* Bank 1  */


			/* Unlock the FLASH control register access */
			HAL_Status = HAL_FLASH_Unlock();
			/* Perform a mass erase or erase the specified FLASH memory sectors */
			HAL_Status = HAL_FLASHEx_Erase(&pEraseInit, &PagError);
			if(HAL_SUCCESSFUL_ERASE == PagError){
				Page_Validity_Status = SUCCESSFUL_ERASE;
			}
			else{
				Page_Validity_Status = UNSUCCESSFUL_ERASE;
			}
			/* Locks the FLASH control register access */
			HAL_Status = HAL_FLASH_Lock();
		}
		else{
			Page_Validity_Status = UNSUCCESSFUL_ERASE;
		}
	}
	return Page_Validity_Status;
}





static void Bootloader_Erase_Flash(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len = 0;
	uint32_t Host_CRC32 = 0;
	uint8_t Erase_Status = 0;


	/* Extract the CRC32 and packet length sent by the HOST */
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t *)((Host_Buffer + Host_CMD_Packet_Len) - CRC_TYPE_SIZE_BYTE));
	/* CRC Verification */
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){

		/* Send acknowledgement to the HOST */

		/* Perform Mass erase or page erase of the user flash */
		Erase_Status = Perform_Flash_Erase(FLASH_Page_BASE_ADDRESS,64);/////////////////////////////////////////////////
		if(SUCCESSFUL_ERASE == Erase_Status){
			/* Report erase Passed */

			//Bootloader_Send_ACK(1); ////////////////////////
			Bootloader_Send_Data_To_Host((uint8_t *)&Erase_Status, 1);

		}
		else{
			/* Report erase failed */
			//Bootloader_Send_ACK(1); /////////////////////////
			Bootloader_Send_Data_To_Host((uint8_t *)&Erase_Status, 1);

		}
	}
	else{

		/* Send Not acknowledge to the HOST */
		Bootloader_Send_NACK();
	}
}






static uint8_t Flash_Memory_Write_Payload(uint16_t *Host_Payload, uint32_t Payload_Start_Address, uint8_t Payload_Len){
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint8_t Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
	uint16_t Payload_Counter = 0;

	uint16_t addres_counter = 0 ;

	/* Unlock the FLASH control register access */
	HAL_Status = HAL_FLASH_Unlock();

	if(HAL_Status != HAL_OK){
		Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
	}
	else{
		for(Payload_Counter = 0 ,addres_counter=0; Payload_Counter < (Payload_Len/2); Payload_Counter++ ,addres_counter+=2){
			/* Program a byte at a specified address */
			HAL_Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Payload_Start_Address +addres_counter, Host_Payload[Payload_Counter]);
			if(HAL_Status != HAL_OK){
				Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
				break;
			}
			else{
				Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_PASSED;
			}
		}
	}

	if((FLASH_PAYLOAD_WRITE_PASSED == Flash_Payload_Write_Status) && (HAL_OK == HAL_Status)){
		/* Locks the FLASH control register access */
		HAL_Status = HAL_FLASH_Lock();
		if(HAL_Status != HAL_OK){
			Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
		}
		else{
			Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_PASSED;
		}
	}
	else{
		Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;
	}

	return Flash_Payload_Write_Status;
}








static void Bootloader_Memory_Write(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len = 0;
	uint32_t Host_CRC32 = 0;
	uint32_t HOST_Address = 0;
	uint8_t Payload_Len = 0;
	uint8_t Address_Verification = ADDRESS_IS_INVALID;
	uint8_t Flash_Payload_Write_Status = FLASH_PAYLOAD_WRITE_FAILED;


	/* Extract the CRC32 and packet length sent by the HOST */
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t *)((Host_Buffer + Host_CMD_Packet_Len) - CRC_TYPE_SIZE_BYTE));
	/* CRC Verification */
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){

		/* Send acknowledgement to the HOST */

		/* Extract the start address from the Host packet */
		HOST_Address = *((uint32_t *)(&Host_Buffer[2]));

		/* Extract the payload length from the Host packet */
		Payload_Len = Host_Buffer[6];
		/* Verify the Extracted address to be valid address */
		Address_Verification = Host_Address_Verification(HOST_Address);
		if(ADDRESS_IS_VALID == Address_Verification){
			/* Write the payload to the Flash memory */
			Flash_Payload_Write_Status = Flash_Memory_Write_Payload((uint16_t *)&Host_Buffer[7], HOST_Address, Payload_Len);
			if(FLASH_PAYLOAD_WRITE_PASSED == Flash_Payload_Write_Status){
				/* Report payload write passed */

				//Bootloader_Send_ACK(1);///////////////////////////
				Bootloader_Send_Data_To_Host((uint8_t *)&Flash_Payload_Write_Status, 1);

			}
			else{

				//Bootloader_Send_ACK(1);//////////////////////////////
				/* Report payload write failed */
				Bootloader_Send_Data_To_Host((uint8_t *)&Flash_Payload_Write_Status, 1);
			}
		}
		else{
			/* Report address verification failed */
			Address_Verification = ADDRESS_IS_INVALID;
			//Bootloader_Send_ACK(1);//////////////////////////////////////
			Bootloader_Send_Data_To_Host((uint8_t *)&Address_Verification, 1);
		}
	}
	else{

		/* Send Not acknowledge to the HOST */
		Bootloader_Send_NACK();
	}
}







static uint8_t Change_ROP_Level(uint32_t ROP_Level){
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	FLASH_OBProgramInitTypeDef FLASH_OBProgramInit;
	uint8_t ROP_Level_Status = ROP_LEVEL_CHANGE_INVALID;

	/* Unlock the FLASH Option Control Registers access */
	HAL_Status = HAL_FLASH_OB_Unlock();
	if(HAL_Status != HAL_OK){
		ROP_Level_Status = ROP_LEVEL_CHANGE_INVALID;

	}
	else{

		FLASH_OBProgramInit.OptionType = OPTIONBYTE_RDP; /* RDP option byte configuration */
		FLASH_OBProgramInit.Banks = FLASH_BANK_1;
		FLASH_OBProgramInit.RDPLevel = ROP_Level;
		/* Program option bytes */
		HAL_Status = HAL_FLASHEx_OBProgram(&FLASH_OBProgramInit);
		if(HAL_Status != HAL_OK){

			HAL_Status = HAL_FLASH_OB_Lock();
			ROP_Level_Status = ROP_LEVEL_CHANGE_INVALID;
		}
		else{

			/* Launch the option byte loading */
			HAL_FLASH_OB_Launch();


			/* Lock the FLASH Option Control Registers access */
			HAL_Status = HAL_FLASH_OB_Lock();
			if(HAL_Status != HAL_OK){
				ROP_Level_Status = ROP_LEVEL_CHANGE_INVALID;
			}
			else{
				ROP_Level_Status = ROP_LEVEL_CHANGE_VALID;

			}
		}

	}
	return ROP_Level_Status;
}






static void Bootloader_Change_Read_Protection_Level(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len = 0;
	uint32_t Host_CRC32 = 0;
	uint8_t ROP_Level_Status = ROP_LEVEL_CHANGE_INVALID;
	uint8_t Host_ROP_Level = 0;


	/* Extract the CRC32 and packet length sent by the HOST */
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t *)((Host_Buffer + Host_CMD_Packet_Len) - CRC_TYPE_SIZE_BYTE));
	/* CRC Verification */
	if(CRC_VERIFICATION_PASSED == Bootloader_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32)){

		Bootloader_Send_ACK(1);
		/* Request change the Read Out Protection Level */
		Host_ROP_Level = Host_Buffer[2];
		/* Warning: When enabling read protection level 2, it s no more possible to go back to level 1 or 0 */
		if(CBL_ROP_LEVEL_2 == Host_ROP_Level) {
			ROP_Level_Status = ROP_LEVEL_CHANGE_INVALID;
		}
		else{
			if(CBL_ROP_LEVEL_0 == Host_ROP_Level){
				Host_ROP_Level = 0xAA;
			}
			else if(CBL_ROP_LEVEL_1 == Host_ROP_Level){
				Host_ROP_Level = 0x55;
			}
			ROP_Level_Status = Change_ROP_Level(Host_ROP_Level);
		}
		Bootloader_Send_Data_To_Host((uint8_t *)&ROP_Level_Status, 1);
	}
	else{

		Bootloader_Send_NACK();
	}
}

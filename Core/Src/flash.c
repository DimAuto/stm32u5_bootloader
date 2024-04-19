/*
 * flash_memory.c
 *
 *  Created on: Aug 1, 2023
 *      Author: dkalaitzakis
 */


#include "flash.h"
#include "uart.h"
//#include "Fusion/Fusion.h"


void float2Bytes(uint32_t * ftoa_bytes_temp, float float_variable);
float Bytes2float(uint8_t * ftoa_bytes_temp);
void double2Bytes(uint8_t * ftoa_bytes_temp, float float_variable);
float Bytes2double(uint8_t * ftoa_bytes_temp);

CRC_HandleTypeDef hcrc;


/**
  * @brief FLASH Initialization Function
  * @param None
  * @retval None
  */
void MX_FLASH_Init(void)
{

  /* USER CODE BEGIN FLASH_Init 0 */

  /* USER CODE END FLASH_Init 0 */

  FLASH_OBProgramInitTypeDef pOBInit = {0};

  /* USER CODE BEGIN FLASH_Init 1 */

  /* USER CODE END FLASH_Init 1 */
  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    Error_Handler();
  }

  /* Option Bytes settings */

  if (HAL_FLASH_OB_Unlock() != HAL_OK)
  {
    Error_Handler();
  }
  pOBInit.OptionType = OPTIONBYTE_USER;
  pOBInit.USERType = OB_USER_BOR_LEV;
  pOBInit.USERConfig = OB_BOR_LEVEL_4;

  pOBInit.OptionType = OPTIONBYTE_USER;
  pOBInit.USERType = OB_USER_NSWBOOT0;
  pOBInit.USERConfig = OB_BOOT0_FROM_PIN;			// IMPORTAND SET SOFTWARE BOOT SELECT TO PIN
  if (HAL_FLASHEx_OBProgram(&pOBInit) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_FLASH_OB_Lock() != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_FLASH_Lock() != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;

  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }

  return page;
}


void float2Bytes(uint32_t * ftoa_bytes_temp, float float_variable)
{
    union {
      float a;
      uint8_t bytes[4];
    } thing;

    thing.a = float_variable;

    for (uint8_t i = 0; i < 4; i++) {
      ftoa_bytes_temp[i] = thing.bytes[i];
    }
    for (uint8_t i = 4; i < 8; i++) {
    	ftoa_bytes_temp[i] = 0xFF;
    }

}

float Bytes2float(uint8_t * ftoa_bytes_temp)
{
    union {
      float a;
      uint8_t bytes[4];
    } thing;

    for (uint8_t i = 0; i < 4; i++) {
    	thing.bytes[i] = ftoa_bytes_temp[i];
    }

   float float_variable =  thing.a;
   return float_variable;
}

void double2Bytes(uint8_t * ftoa_bytes_temp,float float_variable)
{
    union {
      float a;
      uint8_t bytes[8];
    } thing;

    thing.a = float_variable;

    for (uint8_t i = 0; i < 8; i++) {
      ftoa_bytes_temp[i] = thing.bytes[i];
    }

}

float Bytes2double(uint8_t * ftoa_bytes_temp)
{
    union {
      float a;
      uint8_t bytes[8];
    } thing;

    for (uint8_t i = 0; i < 8; i++) {
    	thing.bytes[i] = ftoa_bytes_temp[i];
    }

   float float_variable =  thing.a;
   return float_variable;
}


void FlashReadData(uint32_t StartPageAddress, uint32_t *RxBuf, uint32_t numberofbytes)
{	
	uint32_t temp;
	while (1)
	{

		*RxBuf = *(__IO uint32_t *)StartPageAddress;
		StartPageAddress += 4;
		RxBuf++;
		if (!(numberofbytes--)){
			break;
		}
	}
}

uint32_t FlashErase(uint32_t StartPageAddress, uint32_t numberofpages){
	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError;

  	uint32_t StartPage = GetPage(StartPageAddress);

   /* Fill EraseInit structure*/
   EraseInitStruct.Banks = FLASH_BANK_1;
   EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
   EraseInitStruct.Page = StartPage;
   EraseInitStruct.NbPages = numberofpages;

   HAL_FLASH_Unlock();
   if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
   {
	 /*Error occurred while page erase.*/
	   uart_write_debug("Failed to erase flash\r\n",UART_THERMIS);
	  return HAL_FLASH_GetError ();

   }
   HAL_FLASH_Lock();
   return 0;
}


uint32_t FlashWriteData(uint32_t StartPageAddress, uint32_t *pData, uint32_t DataLength)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError;
	uint32_t index;
	uint32_t length;
	uint32_t remainder;
	uint8_t remainder_data[16] = {0x0U};

   /* Erase the user Flash area*/

	uint32_t StartPage = GetPage(StartPageAddress);
	uint32_t EndPageAdress = (DataLength % FLASH_PAGE_SIZE) ? StartPageAddress + (DataLength / FLASH_PAGE_SIZE) + 1:
								StartPageAddress + (DataLength / FLASH_PAGE_SIZE);
	uint32_t EndPage = GetPage(EndPageAdress);

	/* Check the remaining of quad-word */
	length = DataLength;
	remainder = length & 0xFU;

	if (remainder != 0U)
	{
		length = (length & 0xFFFFFFF0U);

		/* Copy the remaining bytes */
		for (index = 0U; index < remainder; index++)
		{
		remainder_data[index] = pData[length + index];
		}

		/* Fill the upper bytes with 0xFF */
		for (index = remainder; index < 16U; index++)
		{
		remainder_data[index] = 0xFFU;
		}
	}

	/* Fill EraseInit structure*/
	EraseInitStruct.Banks = FLASH_BANK_1;
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Page = StartPage;
	EraseInitStruct.NbPages = (EndPage - StartPage) + 1;

	HAL_FLASH_Unlock();
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
	 /*Error occurred while page erase.*/
	   uart_write_debug("Failed to erase flash\r\n",UART_THERMIS);
	   HAL_FLASH_Lock();
	   return HAL_FLASH_GetError ();

	}

	/* Program the user Flash area word by word*/

	for (index = 0U; index < length; (index += 16U))
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, (StartPageAddress + index), (uint32_t)(&pData[index])) != HAL_OK){
			uart_write_debug("Failed to write flash\r\n",UART_THERMIS);
		 	HAL_FLASH_Lock();
		 	return HAL_FLASH_GetError ();
		}
	}
	/* Lock the Flash to disable the flash control register access (recommended
	  to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();
	return 0;
}

void Flash_Write_NUM(uint32_t StartSectorAddress, float Num)
{
	uint32_t bytes_temp[4] = {0};
	float2Bytes(bytes_temp, Num);

	FlashWriteData(StartSectorAddress, (uint32_t *)bytes_temp, 1);
}


float Flash_Read_NUM(uint32_t StartSectorAddress)
{
	uint8_t buffer[16];
	float value;

	FlashReadData(StartSectorAddress, (uint32_t *)buffer, 1);
	value = Bytes2float(buffer);
	return value;
}


void Flash_Write_Double(uint32_t StartSectorAddress, double Num)
{
	uint8_t bytes_temp[8] = {0};
	float2Bytes(bytes_temp, Num);

	FlashWriteData(StartSectorAddress, (uint32_t *)bytes_temp, 1);
}


double Flash_Read_Double(uint32_t StartSectorAddress)
{
	uint8_t buffer[16];
	double value;

	FlashReadData(StartSectorAddress, (uint32_t *)buffer, 1);
	value = Bytes2float(buffer);
	return value;
}

uint32_t Flash_isWritten(uint32_t StartSectorAddress)
{
	uint8_t buffer[8] = {0};

	FlashReadData(StartSectorAddress, (uint32_t *)buffer, 1);
	if (buffer[0] != 0xFF){
		return 0;
	}
	uart_write_debug("Flash addr is empty\r\n",UART_THERMIS);
	return 1;
}


uint32_t Flash_Write_Vector(uint32_t StartSectorAddress, FusionVector *data)
{
	uint32_t res;
	float temp[3] = {0.0f};
	temp[0] = data->array[0];
	temp[1] = data->array[1];
	temp[2] = data->array[2];
	uint8_t bytes_temp[16] = {0};
	union {
	  float a;
	  uint8_t bytes[4];
	} thing;

	uint8_t j,v=0;
	for (uint8_t i = 0; i < 3; i++){
		thing.a = temp[i];

		for (j = 0; j < 4; j++) {
		  bytes_temp[v+j] = thing.bytes[j];
		}
		v+=4;
	}
	for (uint8_t i = 12; i < 16; i++){
		bytes_temp[i] = 0xFF;
	}
	res = FlashWriteData(StartSectorAddress, (uint32_t *)bytes_temp, 2);
	return res;
}

uint32_t Flash_Read_Vector(uint32_t StartSectorAddress, FusionVector *data)
{
	uint8_t buffer[20] = {0};
	float temp[3] = {0.0f};

	FlashReadData(StartSectorAddress, (uint32_t *)buffer, 2);

	if ((buffer[0] == 255) && (buffer[1] == 255)){
		return 1;
	}
	union {
	  float a;
	  uint8_t bytes[4];
	} thing;

	uint8_t v=0;
	for (uint8_t j = 0; j < 3; j++){
		for (uint8_t i = 0; i < 4; i++) {
			thing.bytes[i] = buffer[v+i];
		}
		v+=4;
		temp[j] =  thing.a;
	}
	data->array[0] = temp[0];
	data->array[1] = temp[1];
	data->array[2] = temp[2];

	if (temp[0] == 0.0f){
		uart_write_debug("Failed to read flash\r\n",UART_THERMIS);
	}
	return 0;
}

uint32_t Flash_Write_Matrix(uint32_t StartSectorAddress, FusionMatrix *data)
{
	uint32_t res;
	float temp[9] = {0.0f};
	for (uint8_t i=0; i<3; i++){
		temp[i * 3 + 0] = data->array[i][0];
		temp[i * 3 + 1] = data->array[i][1];
		temp[i * 3 + 2] = data->array[i][2];
	}
	uint8_t bytes_temp[40] = {0};
	union {
	  float a;
	  uint8_t bytes[4];
	} thing;

	uint8_t j,v=0;
	for (uint8_t i = 0; i < 9; i++){
		thing.a = temp[i];

		for (j = 0; j < 4; j++) {
		  bytes_temp[v+j] = thing.bytes[j];
		}
		v+=4;
	}
	for (uint8_t i = 36; i < 40; i++){
		bytes_temp[i] = 0xFF;
	}
	res = FlashWriteData(StartSectorAddress, (uint32_t *)bytes_temp, 5);
	return res;
}

uint32_t Flash_Read_Matrix(uint32_t StartSectorAddress, FusionMatrix *data)
{
	uint8_t buffer[42] = {0};
	float temp[9] = {0.0f};

	FlashReadData(StartSectorAddress, (uint32_t *)buffer, 5);

	if ((buffer[0] == 255) && (buffer[1] == 255)){
		return 1;
	}
	union {
	  float a;
	  uint8_t bytes[4];
	} thing;

	uint8_t v=0;
	for (uint8_t j = 0; j < 9; j++){
		for (uint8_t i = 0; i < 4; i++) {
			thing.bytes[i] = buffer[v+i];
		}
		v+=4;
		temp[j] =  thing.a;
	}
	for (uint8_t i=0; i<3; i++){
		data->array[i][0] = temp[i * 3 + 0];
		data->array[i][1] = temp[i * 3 + 1];
		data->array[i][2] = temp[i * 3 + 2];
	}

	if (temp[0] == 0.0f){
		uart_write_debug("Failed to read flash\r\n",UART_THERMIS);
	}
	return 0;
}


void Convert_To_Str(uint32_t *Data, char *Buf)
{
	int numberofbytes = ((strlen((char *)Data)/4) + ((strlen((char *)Data) % 4) != 0)) *4;

	for (int i=0; i<numberofbytes; i++)
	{
		Buf[i] = Data[i/4]>>(8*(i%4));
	}
}

/**
  * @brief  Return the FLASH Read Protection level.
  * @retval The return value can be one of the following values:
  *         @arg OB_RDP_LEVEL_0: No protection
  *         @arg OB_RDP_LEVEL_1: Read protection of the memory
  *         @arg OB_RDP_LEVEL_2: Full chip protection
  */
uint32_t FLASH_GetReadOutProtectionLevel(void)
{
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Get the Option bytes configuration */
  HAL_FLASHEx_OBGetConfig(&flash_ob);

  return flash_ob.RDPLevel;
}

uint8_t bootloader_verify_crc (uint8_t *pData, uint32_t len, uint32_t crc_host)
{
    uint32_t uwCRCValue=0xff;

    for (uint32_t i=0 ; i < len ; i++)
	{
        uint32_t i_data = pData[i];
        uwCRCValue = HAL_CRC_Accumulate(&hcrc, &i_data, 1);
	}

	 /* Reset CRC Calculation Unit */
  __HAL_CRC_DR_RESET(&hcrc);

	if( uwCRCValue == crc_host)
	{
		return 0;
	}

	return 1;
}


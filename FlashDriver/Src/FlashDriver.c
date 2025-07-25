/**
******************************************************************************
* @file      FlashDriver.c
* @author    @OnWert
* @version   
* @brief     This file provides common functions for the flash driver   
*/       
/* ------------------------Includes ----------------------------------*/
#include "FlashDriver.h"
#include "cmsis_os.h"
/* ------------------------External variables -------------------------*/

/* ------------------------Global variables----------------------------*/

/* ------------------------Locale variables----------------------------*/

/* ------------------------Functions-----------------------------------*/


void Flash_Read_Data (uint32_t StartSectorAddress, uint32_t *RxBuf, uint16_t numberofwords)
{
	while (1)
	{

		*RxBuf = *(__IO uint32_t *)StartSectorAddress;
		StartSectorAddress += 4;
		RxBuf++;
		if (!(numberofwords--)) break;
	}
}

uint32_t Flash_Write_Data (uint32_t StartSectorAddress, uint32_t *Data, uint16_t numberofwords)
{
  static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SECTORError;
	int sofar=0;


	 /* Unlock the Flash to enable the flash control register access *************/
	  HAL_FLASH_Unlock();

	  /* Erase the user Flash area */

	  /* Get the number of sector to erase from 1st sector */

	  uint32_t StartSector = getSector(StartSectorAddress);
	  uint32_t EndSectorAddress = StartSectorAddress + numberofwords * 4;
	  uint32_t EndSector = getSector(EndSectorAddress);

	  /* Fill EraseInit structure*/
	  EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	  EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	  EraseInitStruct.Sector        = StartSector;
	  EraseInitStruct.NbSectors     =  1;

	  /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
	     you have to make sure that these data are rewritten before they are accessed during code
	     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	     DCRST and ICRST bits in the FLASH_CR register. */
	  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
	  {
		  return HAL_FLASH_GetError ();
	  }

	  /* Program the user Flash area word by word
	    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

	   while (sofar<numberofwords)
	   {
	     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartSectorAddress, Data[sofar]) == HAL_OK)
	     {
	    	 StartSectorAddress += 4;  // use StartPageAddress += 2 for half word and 8 for double word
	    	 sofar++;
	     }
	     else
	     {
	       /* Error occurred while writing data in Flash memory*/
	    	 return HAL_FLASH_GetError ();
	     }
	   }

	  /* Lock the Flash to disable the flash control register access (recommended
	     to protect the FLASH memory against possible unwanted operation) *********/
	  HAL_FLASH_Lock();

	   return 0;
}

uint8_t getSector(uint32_t flashAddr)
{
  if(flashAddr == (uint32_t)FLASH_ADDR_SECTOR_0)
	 {
	   return FLASH_SECTOR_0 ;
	 }
	 else if (flashAddr == (uint32_t)FLASH_ADDR_SECTOR_1)
	 {
	  return FLASH_SECTOR_1 ;
	 }	 
	 else if(flashAddr == (uint32_t)FLASH_ADDR_SECTOR_2)
	 {
	  return FLASH_SECTOR_2 ;
	 }
	 else if(flashAddr == (uint32_t)FLASH_ADDR_SECTOR_3)
	 {
		 return FLASH_SECTOR_3 ;
	 } 
 	 else if(flashAddr == (uint32_t)FLASH_ADDR_SECTOR_4)
	 {
	  return FLASH_SECTOR_4 ;
	 }	
	  
	 else if (flashAddr == (uint32_t)FLASH_ADDR_SECTOR_5)
	 {
	  return FLASH_SECTOR_5 ;
	 } 
	 else if(flashAddr == (uint32_t)FLASH_ADDR_SECTOR_6)
	 {
	  return FLASH_SECTOR_6 ;
	 }
	 else if(flashAddr == (uint32_t)FLASH_ADDR_SECTOR_7)
	 {
	  return FLASH_SECTOR_7;
	 }	 
	 else if(flashAddr == (uint32_t)FLASH_ADDR_SECTOR_8)
	 {
	  return FLASH_SECTOR_8;
	 }	 
	 else if(flashAddr == (uint32_t)FLASH_ADDR_SECTOR_9)
	 {
	  return FLASH_SECTOR_9;
	 }
	 else if(flashAddr == (uint32_t)FLASH_ADDR_SECTOR_10)
	 {
	  return FLASH_SECTOR_10;
	 }
	 else if(flashAddr == (uint32_t)FLASH_ADDR_SECTOR_11)
	 {
	  return FLASH_SECTOR_11;
	 }
	 
	 return FLASH_SECTOR_0;
}

/************************ (C) COPYRIGHT @OnWert *****END OF FILE****/




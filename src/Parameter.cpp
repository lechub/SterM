/*
 * Params.cpp
 *
 *  Created on: 07.09.2016
 *      Author: lechu
 */

//#include "eeprom.h"
#include "Parameter.h"
//#include "stm32f4xx.h"
//#include "settings.h"

#define MAX_NR_OF_EEPROM_PARAMS 3
#define FLASH_COMPLETE 2
#define FLASH_VARIABLE_NOT_FOUND 1


void RTC_WriteBackupRegister(uint8_t location, uint32_t value) {
  /* Write data to backup register */
  *(uint32_t *)((&RTC->BKP0R) + (4 * location)) = value;
}

uint32_t RTC_ReadBackupRegister(uint8_t location) {
  /* Read data from backup register */
  return *(uint32_t *)((&RTC->BKP0R) + (4 * location));
}


uint16_t Parameter::WriteVariable(Parameter::Nazwa virtAdr,  uint16_t &value){
  if (virtAdr >= Nazwa::LAST_PARAMETER) return FLASH_VARIABLE_NOT_FOUND;
  RTC_WriteBackupRegister((uint8_t)virtAdr, value);
//  switch(virtAdr){
//  case 0:
//    //&RTC->BKP0R = value;
//    break;
//  case 1:
//    //&RTC->BKP1R = value;
//    break;
//  case 2:
//    //&RTC->BKP2R = value;
//    break;
//  case 3:
//    //&RTC->BKP3R = value;
//    break;
//  default:
//    return FLASH_VARIABLE_NOT_FOUND;
//    break;
//  }
  return FLASH_COMPLETE;
}

uint16_t Parameter::ReadVariable(Parameter::Nazwa virtAdr,  uint16_t &value){
  if (virtAdr >= Nazwa::LAST_PARAMETER) return FLASH_VARIABLE_NOT_FOUND;
  value = (uint16_t)RTC_ReadBackupRegister(uint8_t(virtAdr));
//  switch(virtAdr){
//  case 0:
//    value = (uint16_t)RTC->BKP0R;
//    break;
//  case 1:
//    value = (uint16_t)RTC->BKP1R;
//    break;
//  case 2:
//    value = (uint16_t)RTC->BKP2R;
//    break;
//  case 3:
//    value = (uint16_t)RTC->BKP3R;
//    break;
//  default:
//    return FLASH_VARIABLE_NOT_FOUND;
//    break;
//  }
  return FLASH_COMPLETE;
}


//Parameter::Parameter() {
//	// TODO Auto-generated constructor stub
//
//}

const Parameter::ParamConstDefs Parameter::paramDefsTable[LAST_PARAMETER+1] = {
		{ LICZNIK, 0, 0, 0xffff},
    { NAPED,    0,    1,  0xffff},
		{ FLAGS1, 	0,		0, 		0xffff},
		{ LAST_PARAMETER, 		0,		0,		1},
};

//bool FlashUnlock(void);
//bool FlashLock(void);

//void clearFlashRegs();


//constexpr uint8_t NB_OF_VAR = 30;

uint16_t Parameter::nrOfParams = MAX_NR_OF_EEPROM_PARAMS;
uint16_t VirtAddVarTab[MAX_NR_OF_EEPROM_PARAMS];

uint16_t fastCopyofData[Parameter::Nazwa::LAST_PARAMETER + 1];

bool Parameter::fillFastDataTab(){
	bool result = true;
	for (uint16_t i = 0; i < Parameter::Nazwa::LAST_PARAMETER; i++){
		Nazwa nazwa = Nazwa(i);
		uint16_t val = 0;
		uint16_t stat = ReadVariable(nazwa, val);
		if (stat == FLASH_VARIABLE_NOT_FOUND){ // nie ma takiego jeszcze?
			val = getDefaultValue(nazwa);			// zaladowac wartosc domyslna
			stat = WriteVariable(nazwa, val);
			if (stat != FLASH_COMPLETE){
				result =  false;
			}
		}else if (stat != FLASH_COMPLETE){
			result =  false;
		}
		fastCopyofData[i] = val;
	}
	return result;
}

///**
// * @brief  Unlock the FLASH control register access
// * @retval HAL Status
// */
//bool FlashUnlock(void){
//	if((FLASH->CR & FLASH_CR_LOCK) != RESET)
//	{
//		/* Authorize the FLASH Registers access */
//		FLASH->KEYR = FLASH_KEY1;
//		FLASH->KEYR = FLASH_KEY2;
//	} else{
//		return false;
//	}
//	return true;
//}
//
///**
// * @brief  Locks the FLASH control register access
// * @retval HAL Status
// */
//bool FlashLock(void){
//	/* Set the LOCK Bit to lock the FLASH Registers access */
//	FLASH->CR |= FLASH_CR_LOCK;
//	return true;
//}
//


bool Parameter::initEepromMemory(){
	bool result = true;

	// Use Backup registers to count reset events
	  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	  PWR->CR |= PWR_CR_DBP;

//	  count = RTC->BKP0R;
//	  my_printf("Reset #%d\r\n", count);
//	  count++;
//	  RTC->BKP0R = count;

	  // Clear reset flags for next reset
	  RCC->CSR |= RCC_CSR_RMVF;


//	FlashUnlock();
//	//clearFlashRegs();
//	result = (EE_Init() == FLASH_COMPLETE);
//	//if (!result) result = (EE_Init() == FLASH_COMPLETE);
	fillFastDataTab();
//	FlashLock();
	return result;
}


bool Parameter::setEEpromValue(Nazwa virtAdr, uint16_t value){
	fastCopyofData[virtAdr] = value;
//	FlashUnlock();
	uint16_t result = WriteVariable(virtAdr,  value);
//	FlashLock();
	return (result == FLASH_COMPLETE);
}

int32_t Parameter::getEEpromValue(Nazwa virtAdr){
	if (virtAdr >= Nazwa::LAST_PARAMETER) return -1;
	return fastCopyofData[virtAdr];

//		uint16_t val = 0;
//		uint16_t ret = ReadVariable(virtAdr, val);
//		if (ret == 1){
//			if (setEEpromValue(virtAdr, 0)) return 0;
//			else return -1;
//		}
//		return (ret == FLASH_COMPLETE) ? val : -1;

}





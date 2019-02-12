/**
  ******************************************************************************
  * @file    STM32F0xx_EEPROM_Emulation/inc/eeprom.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    29-May-2012
  * @brief   This file contains all the functions prototypes for the EEPROM 
  *          emulation firmware library.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM_H
#define __EEPROM_H

/* Includes ------------------------------------------------------------------*/
//#include "stm32f0xx.h"
#include "stm32f0xx_flash.h"
#include "systemDefs.h"

/* Exported constants --------------------------------------------------------*/
/* Define the size of the sectors to be used */
#define PAGE_SIZE             ((uint32_t)0x0400)  /* Page size = 1KByte */
/* Variables' number */
#define NB_OF_VAR             ((uint8_t)10) // tu zmieniac max ilosc komorek

/* EEPROM start address in Flash */
/*EEPROM emulation start address:
 * dla procka 64kB		0x0800F800
 * dla procka 32kB		0x08007800
 * dla procka 16kB		0x08003800
*/
//#define FLASH_BASE            ((uint32_t)0x08000000U)              /*!< FLASH base address in the alias region */
//#define FLASH_BANK1_END       ((uint32_t)0x08007FFFU) /*!< FLASH END address of bank1 */

//#define EEPROM_START_ADDRESS  ((uint32_t)0x0800F800) //0x08002000)
//#define EEPROM_START_ADDRESS  ((uint32_t)((FLASH_BANK1_END +1) - (2 * PAGE_SIZE))) //0x08007800)
#define EEPROM_START_ADDRESS  ((uint32_t)0x0800F800)

/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS + 0x0000))
#define PAGE0_END_ADDRESS     ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))

#define PAGE1_BASE_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS + PAGE_SIZE))
#define PAGE1_END_ADDRESS     ((uint32_t)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE - 1)))

/* Used Flash pages for EEPROM emulation */
#define PAGE0                 ((uint16_t)0x0000)
#define PAGE1                 ((uint16_t)0x0001)

/* No valid page define */
#define NO_VALID_PAGE         ((uint16_t)0x00AB)

/* Page status definitions */
#define ERASED                ((uint16_t)0xFFFF)     /* Page is empty */
#define RECEIVE_DATA          ((uint16_t)0xEEEE)     /* Page is marked to receive data */
#define VALID_PAGE            ((uint16_t)0x0000)     /* Page containing valid data */

/* Valid pages in read and write defines */
#define READ_FROM_VALID_PAGE  ((uint8_t)0x00)
#define WRITE_IN_VALID_PAGE   ((uint8_t)0x01)

/* Page full define */
#define PAGE_FULL             ((uint8_t)0x80)


/* Exported types ------------------------------------------------------------*/

///**
//  * @brief  FLASH Status
//  */
//typedef enum
//{
//  FLASH_BUSY = 1,
//  FLASH_ERROR_WRP,
//  FLASH_ERROR_PROGRAM,
//  FLASH_COMPLETE,
//  FLASH_TIMEOUT
//}FLASH_Status;


/* Exported macro ------------------------------------------------------------*/


//----------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------

/* Exported functions ------------------------------------------------------- */

uint16_t EE_Init(void);
uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);

//----------------------------------------------------
#ifdef __cplusplus
}
#endif
//----------------------------------------------------

#endif /* __EEPROM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

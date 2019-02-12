/*
 * Eprom.h
 *
 *  Created on: 18 sty 2019
 *      Author: lechu
 */

#ifndef VEPROM_H_
#define VEPROM_H_

#include "systemDefs.h"
#include "eeprom.h"

class VEprom {

private:

public:
  typedef enum{
    Checksum = 0,
    SoftVersion,
    LICZNIK,
    NAPED,
    FLAGS1,
    Adres,
    COUNT,
  }VirtAdres;



  static bool init(){
    FLASH_Unlock();
    return (EE_Init() == FLASH_Status::FLASH_COMPLETE);
    FLASH_Lock();
  }

  static bool writeWord(VirtAdres virtAdres, uint16_t val){
    if (virtAdres > NB_OF_VAR) return false;
    FLASH_Unlock();
    uint16_t result = EE_WriteVariable((uint16_t)virtAdres, val);
    FLASH_Lock();
    return (result == FLASH_Status::FLASH_COMPLETE);
  }

  static bool readWord(VirtAdres virtAdres, uint16_t * result){
    if (virtAdres > NB_OF_VAR) return false;
    uint16_t res = EE_ReadVariable((uint16_t)virtAdres, result);
    if (res == 0){    // read ok
      return true;
    }else if (res == 1){  // variable not found
      *result = 0;
      return writeWord(virtAdres, *result);
    }
    return false;
  }

  static uint32_t readWord(VirtAdres virtAdres){
    uint16_t result;
    if (readWord(virtAdres, &result)){
      return (uint32_t)result;
    }else{
      return 0xffffffff;
    }
  }

  static bool addToValue(VirtAdres virtAdres, uint16_t valueToAdd){
    uint16_t val;
    if (!readWord(virtAdres, &val)) return false;
    if (!writeWord(virtAdres, (uint16_t)(val + valueToAdd))) return false;
    return true;
  }

  static bool subtractFromValue(VirtAdres virtAdres, uint16_t valueToSub){
    uint16_t val;
    if (!readWord(virtAdres, &val)) return false;
    if (!writeWord(virtAdres, (uint16_t)(val - valueToSub))) return false;
    return true;
  }


};

#endif /* VEPROM_H_ */

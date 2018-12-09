/*
 * Params.h
 *
 *  Created on: 07.09.2016
 *      Author: lechu
 */

#ifndef INC_PARAMETER_H_
#define INC_PARAMETER_H_

#include "stdint.h"
#include "stm32f0xx.h"
//#include "eeprom.h"




//extern constexpr uint8_t NB_OF_VAR = 30;


class Parameter {
public:

	typedef enum{
		LICZNIK = 0,
    NAPED = 1,
    FLAGS1 = 2,
		LAST_PARAMETER,
	}Nazwa;

private:
	static uint16_t nrOfParams;// = 0;

	uint16_t nr = 0; 		// indeks parametru w eepromie
	uint16_t actualValue = 0;
	const char * name = nullptr;

	static bool setEEpromValue(Nazwa virtAdr, uint16_t value);
	static int32_t getEEpromValue(Nazwa virtAdr);
	static bool fillFastDataTab();

  static uint16_t WriteVariable(Parameter::Nazwa virtAdr,  uint16_t &value);
  static uint16_t ReadVariable(Parameter::Nazwa virtAdr,  uint16_t &value);


private:
	typedef struct{
		Nazwa		param;
		uint16_t	minValue;
		uint16_t	defValue;
		uint16_t	maxValue;
	}ParamConstDefs;

	static const ParamConstDefs paramDefsTable[LAST_PARAMETER+1];

	static const ParamConstDefs * getConstDefs(Nazwa nazwa){
		for (uint8_t i = 0; i< LAST_PARAMETER; i++){
			if (paramDefsTable[i].param == nazwa) return &paramDefsTable[i];
		}
		return nullptr;
	}

	uint32_t * ptr = nullptr;

public:



	static int32_t getValue(Nazwa nazwa){
		return getEEpromValue(nazwa);
	}

	static bool setValue(Nazwa nazwa, uint16_t newValue){
		newValue = adjustValueToFit(nazwa, newValue);
		return setEEpromValue(nazwa, newValue);
	}

	  static uint16_t getDefaultValue(Nazwa nazwa){
	    const ParamConstDefs * par = getConstDefs(nazwa);
	    if (par == nullptr) return 10;
	    return par->defValue;
	  }



	  static uint16_t adjustValueToFit(Nazwa nazwa, uint32_t value){
	    const ParamConstDefs * par = getConstDefs(nazwa);
	    if (par == nullptr) return 99;
	    if (value > par->maxValue) value = par->maxValue;
	    if (value < par->minValue) value = par->minValue;
	    return uint16_t(value);
	  }


	  static bool saveDefaultValues(){
	    bool result = true;
	    for (uint16_t i = 1; i < uint16_t(Nazwa::LAST_PARAMETER); i++){
	      Nazwa param = Nazwa(i);
	      if (!setValue(param, getDefaultValue(param) )) result = false;
	    }
	    return result;
	  }
	static bool initEepromMemory();



};


//
//class Parameter {
//private:
//  static uint16_t nrOfParams;// = 0;
//
//  uint16_t nr;    // indeks parametru w eepromie
//  uint16_t actualValue;
//  const char * name;
//
//  static bool setEEpromValue(uint16_t virtAdr, uint16_t value);
//  static int32_t getEEpromValue(uint16_t virtAdr);
//  static bool fillFastDataTab();
//
//public:
//
//  typedef enum{
//    NONE = 0,
//    MODE_AUTO_MANUAL = 1,
//    TEMPERATURA_CO,
//
//    LAST_PARAMETER
//  }Nazwa;
//
//private:
//  typedef struct{
//    Nazwa   param;
//    uint16_t  minValue;
//    uint16_t  defValue;
//    uint16_t  maxValue;
//  }ParamConstDefs;
//
//  static const ParamConstDefs paramDefsTable[LAST_PARAMETER+1];
//
//  static const ParamConstDefs * getConstDefs(Nazwa nazwa){
//    for (uint8_t i = 0; i< LAST_PARAMETER; i++){
//      if (paramDefsTable[i].param == nazwa) return &paramDefsTable[i];
//    }
//    return nullptr;
//  }
//
//public:
//
//
//  static int32_t getValue(Nazwa nazwa){
//    return getEEpromValue(uint16_t(nazwa));
//  }
//
//  static bool setValue(Nazwa nazwa, uint16_t newValue){
//    newValue = adjustValueToFit(nazwa, newValue);
//    return setEEpromValue(nazwa, newValue);
//  }
//
//  static const char * getParamPattern(Nazwa nazwa){
//    switch(nazwa){
//    case Parameter::Nazwa::PODAJNIK_PRACA:
//    case Parameter::Nazwa::PODAJNIK_OPOZNIENIE:
//    case Parameter::Nazwa::PODTRZYMANIE_PRACA:
//    case Parameter::Nazwa::OPOZNIENIE_DMUCHAWY:
//      //------>1234567890123456<
//      return  "   00.0 sek.    ";
//
//    case Parameter::Nazwa::TEMPERATURA_CO:
//      //------>1234567890123456<
//      return  "T. ustaw:00.0 oC";
//
//    case Parameter::Nazwa::PODAJNIK_PRZERWA:
//    case Parameter::Nazwa::PODTRZYMANIE_PRZERWA:
//      //------>1234567890123456<
//      return  " 00 min. 60 sek.";
//
//    case Parameter::Nazwa::HISTEREZA_CO_TEMP:
//    case Parameter::Nazwa::HISTEREZA_CWU_TEMP:
//      //------>1234567890123456<
//      return  "temp: 00.0 st.C ";
//
//    case Parameter::Nazwa::POMPA_CO_TEMP_ZALACZ:
//    case Parameter::Nazwa::POMPA_CWU_TEMP_ZALACZ:
//    case Parameter::Nazwa::ALARM_TEMP_PODAJNIKA:
//      //------>1234567890123456<
//      return  "wlacz >00.0 oC  ";
//
//    case Parameter::Nazwa::DMUCHAWA_MOC:
//      //------>1234567890123456<
//      return  "     = 00 =     ";
//
//    case Parameter::Nazwa::ALARM_SPADKU_TEMP:
//      //------>1234567890123456<
//      return  "mniej od 00.0 oC";
//
//    case Parameter::Nazwa::TRYB_LETNI:
//    case Parameter::Nazwa::AKTYWNA_CWU:
//    case Parameter::Nazwa::USTAWIENIA_FABRYCZNE:
//    case Parameter::Nazwa::MODE_AUTO_MANUAL:
//    case Parameter::Nazwa::NONE:
//    case Parameter::Nazwa::LAST_PARAMETER:
//    default:
//      return "??";
//    }
//  }
//
//  static bool initEepromMemory();
//
//
//  static uint16_t getDefaultValue(Nazwa nazwa){
//    const ParamConstDefs * par = getConstDefs(nazwa);
//    if (par == nullptr) return 10;
//    return par->defValue;
//  }
//
//
//
//  static uint16_t adjustValueToFit(Nazwa nazwa, uint32_t value){
//    const ParamConstDefs * par = getConstDefs(nazwa);
//    if (par == nullptr) return 99;
//    if (value > par->maxValue) value = par->maxValue;
//    if (value < par->minValue) value = par->minValue;
//    return uint16_t(value);
//  }
//
//
//  static bool saveDefaultValues(){
//    bool result = true;
//    for (uint16_t i = 1; i < uint16_t(Nazwa::LAST_PARAMETER); i++){
//      Nazwa param = Nazwa(i);
//      if (!setValue(param, getDefaultValue(param) )) result = false;
//    }
//    return result;
//  }
//
//
//};



#endif /* INC_PARAMETER_H_ */

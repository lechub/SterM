/*
 * Pomiar.h
 *
 *  Created on: 30.08.2016
 *      Author: lechu
 */

#ifndef INC_POMIAR_H_
#define INC_POMIAR_H_

#include "systemDefs.h"


class Pomiar{

public:

  typedef enum{
    UCurrent = 0,
    UNap1,
    UNap2,
    UNap3,
    UNap4,
    IntTemp,
    UBattery,
    ADC_COUNT,  // counter for determine table size
  }AdcEnum;



  static const uint16_t * TS_CAL1_30oC;
  static const uint16_t * TS_CAL2_110oC;

  static constexpr int32_t UREF = 3300;
  static constexpr int32_t MAX_ADC = 4095;

  static constexpr int32_t OutDivider = 11;


  static constexpr int32_t CurrShuntRmOhm = 5;  // 5 mOhm
  static constexpr int32_t CurrMultiplier = 5 * 11;

private:

  typedef struct{
    GPIO_TypeDef * gpio;
    uint8_t      pinNr;
    uint8_t      adcChannel;
  }AdcDef;

  //static const AdcDef adcDefs[];
  static const AdcDef adcDefs[];

  volatile static uint16_t adcValues[ADC_COUNT + 1];

  static uint16_t getPomiarRaw(AdcEnum nr);

  static void initAdcGpios();

  static bool initADC();

  static bool initDma();

  static int32_t getBattery_mV(uint16_t rawValue);
  static int32_t getCurrent_mA(uint16_t rawValue);
  static int32_t getOutput_mV(uint16_t rawValue);

  static void enableVBat(bool newstate){
    newstate ? ADC->CCR |= ADC_CCR_VBATEN : ADC1->CR &= ~ADC_CCR_VBATEN;
  }

  static void enableIntTemp(bool newstate){
    newstate ? ADC->CCR |= ADC_CCR_TSEN : ADC1->CR &= ~ADC_CCR_TSEN;
  }

  static void enableVrefInt(bool newstate){
    newstate ? ADC->CCR |= ADC_CCR_VREFEN : ADC1->CR &= ~ADC_CCR_VREFEN;
  }

public:

  static bool setup(AdcDef *defs);

  //static uint8_t getIloscWejsc(){ return count; };

  /** Zwraca pomiar w miliwoltach, miliamperach, miliCelsjuszach itd.
   *
   */
  static int32_t getPomiar(AdcEnum nrAdc){
    uint16_t adcVal = getPomiarRaw(nrAdc);
    switch(nrAdc){
    case UCurrent:
      return getCurrent_mA(adcVal);
    case UNap1:
      return getOutput_mV(adcVal);
    case UBattery:
      return getBattery_mV(adcVal);
    case IntTemp:
    default: break;
    }
    return adcVal;
  }

  //static volatile uint16_t * getDataTablePtr();
};


#endif /* INC_POMIAR_H_ */

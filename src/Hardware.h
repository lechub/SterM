/*
 * Hardware.h
 *
 *  Created on: 28.06.2017
 *      Author: lechu
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

//#include "stm32f4xx.h"
#include "I2C.h"

#include "Fifo.h"

class Hardware {

public:

  static constexpr uint32_t nvicPriority = 0x03U;
  static constexpr uint32_t SYSTICK_FREQUENCY_HZ = 1000;

  typedef enum{
    OK, Incident, Failure, DMA_FAIL, ADC_FAIL,
  }ErrorCode;


  Hardware(){	}

  static void init();

  static uint32_t getTickMs();
  static void delayMsDirty(uint32_t milis);
  static void delayMsWithBackground(uint32_t milis);

  static void WDOG_Init();
  static void WDOG_Reload();
  static void NVIC_init();
  static bool adcDmaError;


  static void errorDispatch(ErrorCode errCode){
    switch(errCode){
    case Failure:
      while (true){;}// ToDo mruganie dioda
      break;
    case Incident: break;
    case DMA_FAIL: adcDmaError = true; break;
    case ADC_FAIL: adcDmaError = true; break;
    case OK:
      // Todo zasygnalizowanie
      break;
    }
  }

  inline static uint32_t getSYSCLK(){
    return SystemCoreClock;
  }

  // Freq on AHB bus
  static uint32_t getHCLK(){
    uint32_t prescaler = (RCC->CFGR & RCC_CFGR_HPRE) >> 4;
    if ((prescaler & 0b1000) != 0){
      prescaler &= 0b0111;
      prescaler++;
      if (prescaler > 0b011){    // presc /2 /4 /8 /16
        prescaler++;             // presc /64 /128 /256 /512
      }
      return getSYSCLK() >> prescaler;
    }
    return getSYSCLK();
  }

  // Freq on APB bus
  static uint32_t getPCLK(){
    uint32_t prescaler = (RCC->CFGR & RCC_CFGR_PPRE) >> 8;
    if ((prescaler & 0b100) != 0){
      prescaler &= 0b011;
      prescaler++;
      return getHCLK() >> prescaler;
    }
    return getHCLK();
  }

private:
  static void gpioInit();
  static void adcInit();




};

#ifdef __cplusplus
extern "C" {
#endif

void DMA1_Stream0_IRQHandler(void) __attribute__ ((interrupt));
void DMA2_Stream0_IRQHandler(void) __attribute__ ((interrupt));
void ADC_IRQHandler(void) __attribute__ ((interrupt));

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_H_ */

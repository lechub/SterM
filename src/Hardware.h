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

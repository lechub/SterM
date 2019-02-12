/*
 * Pomiar.cpp
 *
 *  Created on: 6 lut 2019
 *      Author: lechu
 */
#include "Pomiar.h"
#include "Gpio.h"

const uint16_t * Pomiar::TS_CAL1_30oC = ((uint16_t*) ((uint32_t) 0x1FFFF7B8));
const uint16_t * Pomiar::TS_CAL2_110oC = ((uint16_t*) ((uint32_t) 0x1FFFF7C2));


const Pomiar::AdcDef Pomiar::adcDefs[]={
    // GPIOx, pinNr, ADC_Channel
    {GPIOA, 3, 3},   // Pomiar::AdcEnum::UCurrent},
    {GPIOC, 0, 10},   // Pomiar::AdcEnum::UNap1},   ADC_IN10
    {GPIOC, 1, 11},   // Pomiar::AdcEnum::UNap2},   // ADC_IN10
    {GPIOC, 2, 12},   // Pomiar::AdcEnum::UNap3},   // ADC_IN10
    {GPIOC, 3, 13},   // Pomiar::AdcEnum::UNap4},   // ADC_IN10
    {nullptr, 0, 16},  // Temperature sensor
    //    {nullptr, 0, 17},  // Internal power Vrefint
    {nullptr, 0, 18},  // VBattery
    {nullptr, 0, 0},
};



int32_t Pomiar::getBattery_mV(uint16_t rawValue){
  int32_t result = rawValue;
  result *= UREF * 2; // internal battery divider = 1/2
  result /= MAX_ADC;
  return result;
}

int32_t Pomiar::getCurrent_mA(uint16_t rawValue){
  int32_t result = rawValue;
  result *= CurrShuntRmOhm * CurrMultiplier * UREF;
  result /= MAX_ADC;
  return result;

}

int32_t Pomiar::getOutput_mV(uint16_t rawValue){
  int32_t result = rawValue;
  result *= OutDivider * UREF;
  result /= MAX_ADC;
  return result;
}



bool Pomiar::setup(AdcDef *defs){
  //  adcDefs = defs;

  initADC();
  initDma();

  //  ADC1->CR |= ADC_CR_ADSTART; /* start the ADC conversions */
  return true;
}


void Pomiar::initAdcGpios(){
  for (uint8_t i = 0; i < ADC_COUNT; i++){
    if (adcDefs[i].gpio == nullptr) continue;
    Gpio::setup(adcDefs[i].gpio, adcDefs[i].pinNr, Gpio::GpioMode::ANALOG, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
  }
}



bool Pomiar::initADC(){

  // ADC Clock selection
  // This code selects the HSI14 as clock source.
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // (1) Enable the peripheral clock of the ADC
  RCC->CR2 |= RCC_CR2_HSI14ON; // (2) Start HSI14 RC oscillator
  while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0){; } // (3)  Wait HSI14 is ready // For robust implementation, add here time-out management
  ADC1->CFGR2 &= (~ADC_CFGR2_CKMODE); // (4) Select HSI14 by writing 00 in CKMODE (reset value)

  // ADC sampling time
  ADC1->SMPR = (ADC1->SMPR & (~ADC_SMPR_SMP)) | ADC_SMPR_SMP_2; // 0b100 - 41.5 ADC clock cycle

  // ADC result mode
  ADC1->CFGR1 = (ADC1->CFGR1 & (~ADC_CFGR1_RES)) // 0b00 ->12 bit
                  & (~ADC_CFGR1_ALIGN) ;                    // 0b0  ->right alignment

  // channel selection
  ADC1->CFGR1 &= (~ADC_CFGR1_SCANDIR);    // scan order - up
  uint32_t selected = 0;
  for (uint8_t i = 0; i < ADC_COUNT; i++){
    selected |= 1 << adcDefs[i].adcChannel;
  }
  ADC1->CHSELR = selected;

  // calibration ADC
  if ((ADC1->CR & ADC_CR_ADEN) != 0){ // (1)  Ensure that ADEN = 0
    ADC1->CR |= ADC_CR_ADDIS; // (2) Clear ADEN by setting ADDIS
  }
  while ((ADC1->CR & ADC_CR_ADEN) != 0){;} // For robust implementation, add here time-out management
  ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN; // (3) Clear DMAEN*/
  ADC1->CR |= ADC_CR_ADCAL; // (4) Launch the calibration by setting ADCAL
  while ((ADC1->CR & ADC_CR_ADCAL) != 0){;} // (5) Wait until ADCAL=0 // For robust implementation, add here time-out management

  // ADC enable
  if ((ADC1->ISR & ADC_ISR_ADRDY) != 0){ // (1) Ensure that ADRDY = 0
    ADC1->ISR |= ADC_ISR_ADRDY; // (2) Clear ADRDY
  }
  ADC1->CR |= ADC_CR_ADEN; // (3) Enable the ADC
  while ((ADC1->ISR & ADC_ISR_ADRDY) == 0){;} // (4) Wait until ADC ready // For robust implementation, add here time-out management */

  return true;
}

bool Pomiar::initDma(){
  RCC->AHBENR |= RCC_AHBENR_DMA1EN; // (1) Enable the peripheral clock on DMA
  ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG; // (2) Enable DMA transfer on ADC and circular mode
  DMA1_Channel1->CPAR = (uint32_t) (&(ADC1->DR)); // (3)  Configure the peripheral data register address
  DMA1_Channel1->CMAR = (uint32_t)(adcValues); // (4) Configure the memory address
  DMA1_Channel1->CNDTR = AdcEnum::ADC_COUNT; // (5)  Configure the number of DMA tranfer to be performs on DMA channel 1
  DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0
      | DMA_CCR_TEIE | DMA_CCR_CIRC; // (6) Configure increment, size, interrupts and circular mode
  DMA1_Channel1->CCR |= DMA_CCR_EN; // (7) Enable DMA Channel 1;
  return true;
}



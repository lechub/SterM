

#include "Sterownik.h"

void Sterownik::initOVC(){
  // init PORTA15 EXTI irq - line EXTI15
  wewy->gpioOVCdetect.setup(Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
  wewy->gpioOVCdetect.setAlternateFunc(3);
  SYSCFG->EXTICR[3] = SYSCFG_EXTICR4_EXTI15_PA;     // same zera, wiec niepotrzebne

  EXTI->IMR = 1<<15; // linia 15 odmaskowana
  //EXTI->RTSR = 0x0001; /* (4) */
  EXTI->FTSR = 1<<15; // falling edge interrupt
  NVIC_EnableIRQ(EXTI4_15_IRQn);
  NVIC_SetPriority(EXTI4_15_IRQn,0);
}


void EXTI4_15_IRQHandler(void){
  sterM->onOVC();
  EXTI->PR = 1<<15; // clear pending flag
}

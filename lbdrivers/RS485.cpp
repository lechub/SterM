/*
 * RS485.cpp
 *
 *  Created on: 21 lut 2014
 *      Author: lechu
 */

#include "RS485.h"
#include "Hardware.h"
#include "systemDefs.h"

//USART_TypeDef * usartReg;

RS485 * RS485::rs485OnUSART1 = nullptr;
RS485 * RS485::rs485OnUSART2 = nullptr;

void setupToUsart(GPIO_TypeDef * gpio, uint8_t pin, uint8_t alternateFunc){
  Gpio::setup(gpio, pin, Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::MaximumSpeed);
  Gpio::setAlternateFunc(gpio, pin, alternateFunc);
}

void RS485::setup(USART_TypeDef * usartRegisters, uint32_t baudRate){
  USART_TypeDef * usart = usartRegisters;
  if(usart == USART1){
    rs485OnUSART1 = this;
    setupToUsart(GPIOB, 6, 0x00);
    setupToUsart(GPIOB, 7, 0x00);
  }else if (usart == USART2){
    rs485OnUSART2 = this;
    setupToUsart(GPIOD, 6, 0x00);
    setupToUsart(GPIOD, 5, 0x00);
  }else{
    while(true){;} // error - niewlasciwy USART
  }

  getHDUsart()->setup(usart);
  getHDUsart()->setMode(MODE_WAITING);


  /*
00: PCLK selected as USART1 clock source (default)
01: System clock (SYSCLK) selected as USART1 clock
10: LSE clock selected as USART1 clock
11: HSI clock selected as USART1 clock
   * */

  RCC_TypeDef * rcc = RCC;

  if(usart == USART1){
    rcc->CFGR3 =  (RCC->CFGR3 & ~RCC_CFGR3_USART1SW) | RCC_CFGR3_USART1SW_0; // 0b01 - SYSCLK
    rcc->APB2ENR |= RCC_APB2ENR_USART1EN;
    NVIC_SetPriority (USART1_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
    NVIC_EnableIRQ(USART1_IRQn);
  }else if(usart == USART1){
    rcc->CFGR3 =  (RCC->CFGR3 & ~RCC_CFGR3_USART2SW) | RCC_CFGR3_USART2SW_0; // 0b01 - SYSCLK
    rcc->APB1ENR |= RCC_APB1ENR_USART2EN;
    NVIC_SetPriority (USART2_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
    NVIC_EnableIRQ(USART2_IRQn);
  }


  CLEAR_BIT(usart->CR1, USART_CR1_UE); // disable USART
  usart->CR1 = 0
      //      |USART_CR1_TXEIE
      //      |USART_CR1_TCIE
      |USART_CR1_RXNEIE
      |USART_CR1_TE
      |USART_CR1_RE
      ;
  usart->BRR = uint16_t(SystemCoreClock / baudRate);

  usart->CR2 = 0;
  usart->CR3 = 0
      | USART_CR3_OVRDIS
      ;

  SET_BIT(usart->CR1, USART_CR1_UE); // enable USART
  SET_BIT(usart->CR1, USART_CR1_RE); // enable READ

}




inline void usartHandler(RS485::HalfDuplexUsart * hdUsart){

  USART_TypeDef * usart = hdUsart->getRegs();

  uint32_t isr = usart->ISR;
  uint32_t cr1 = usart->CR1;

  if (isr & USART_ISR_NE){    // noise??
    SET_BIT(usart->ICR, USART_ICR_NCF);
  }
  if (isr & USART_ISR_FE){    // framing error
    SET_BIT(usart->ICR, USART_ICR_FECF);
  }

  if (isr & USART_ISR_RXNE){      //odbior
    uint8_t data = (uint8_t)(usart->RDR);
    if (hdUsart->getMode() != RS485::Mode::MODE_TX){ // odbierac tylko gdy jest w trybie odbioru
      hdUsart->getFifo()->put(data);
    }
  }

  //nadawanie
  if (((isr & USART_ISR_TXE)&&(cr1 & USART_CR1_TXEIE))||
      ((isr & USART_ISR_TC)&&(cr1 & USART_CR1_TCIE))){
    if((hdUsart->getMode() == RS485::Mode::MODE_TX)  // wysylac tylko gdy tryb nadawania
        &&(hdUsart->getFifo()->isNotEmpty())){  // i cos jest w buforze
      CLEAR_BIT(usart->CR1, USART_CR1_TCIE);    // transmit complete nie teraz
      SET_BIT(usart->CR1, USART_CR1_TXEIE);     // potrzebna informacja o pustym buforze
      uint16_t data = hdUsart->getFifo()->get();
      usart->TDR = data;
    }else{                      // buffer empty
      CLEAR_BIT(usart->CR1, USART_CR1_TXEIE);   // żeby nie waliło przerwaniami, gdy pusty bufor
      if (isr & USART_ISR_TC){                  // to koniec transmisji?
        CLEAR_BIT(usart->CR1, USART_CR1_TCIE);  // zablokowanie przerwań od transmisji
        hdUsart->setMode(RS485::Mode::MODE_WAITING);  // wajcha na odbiór
      }else{
        SET_BIT(usart->CR1, USART_CR1_TCIE);    // będzie tylko przerwanie od końca transmisji
      }
    }
  }

}


void USART1_IRQHandler(void){
  RS485 * hdu = RS485::rs485OnUSART1;
  usartHandler(hdu->getHDUsart());
}

void USART2_IRQHandler(void){
  RS485 * hdu = RS485::rs485OnUSART2;
  usartHandler(hdu->getHDUsart());
}

bool RS485::canPut(void){
  if (getHDUsart()->getMode() != RS485::Mode::MODE_TX ){
    return getFifo()->isEmpty();
  }
  return !getFifo()->isFull();
}

bool RS485::canPut(uint32_t nrOfBytes){
  if (getHDUsart()->getMode() != RS485::Mode::MODE_TX ){
    return getFifo()->isEmpty();
  }
  return (nrOfBytes <= getFifo()->countFree());
}

//zwraca false jesli bufor jest pelny i sie nie da
bool RS485::put(uint8_t znak){
  if (canPut()){
    getHDUsart()->setMode(MODE_TX);
    getFifo()->put( znak);
    SET_BIT(getHDUsart()->getRegs()->CR1, USART_CR1_TXEIE);	// odblokowac przerwania dla nadawania,
    return true;
  }
  return false;
}

//zwraca false jesli bufor jest pelny i sie nie da
bool RS485::putAll(Fifo *dane){
  bool val = getFifo()->putAll( dane);
  getHDUsart()->setMode(MODE_TX);
  SET_BIT(getHDUsart()->getRegs()->CR1, USART_CR1_TXEIE); // odblokowac przerwania dla nadawania,
  return val;
}


// zwraca znak z bufora. Stan bufora nalezy sprawdzic przez usartCanGet() - czy nie jest pusty.
uint16_t RS485::get(void){
  if (canGet()){
    return getFifo()->get();
  }
  return Fifo::BUFFER_EMPTY_FLAG_U16;
}

bool RS485::canGet(void){
  if (getHDUsart()->getMode() != RS485::Mode::MODE_TX ){
    return getFifo()->isNotEmpty();
  }
  return false;
}


bool RS485::putString(const char* tekst){
  bool val = true;
  getFifo()->putString(tekst);
  if (getFifo()->isNotEmpty()){
    getHDUsart()->setMode(MODE_TX);
    SET_BIT(getHDUsart()->getRegs()->CR1, USART_CR1_TXEIE);	// odblokowac przerwania dla nadawania, bo cos jest w buforze
  }
  return val;
}

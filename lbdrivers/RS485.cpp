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


void RS485::setup(InitStruct * iStr){
  if(iStr->usart == USART1){
    halfDuplexUsart.setup(iStr->usart, iStr->rs485Dir);
    rs485OnUSART1 = this;
  }else{
    while(true){;} // error - niewlasciwy USART
  }

  iStr->rs485Rx->setup(Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::MaximumSpeed);
  iStr->rs485Rx->setAlternateFunc(0x01);  //
  iStr->rs485Tx->setup(Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::MaximumSpeed);
  iStr->rs485Tx->setAlternateFunc(0x01);  //
  iStr->rs485Dir->setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
  //getHDUsart()->rs485Dir = iStr->rs485Dir;

  getHDUsart()->setMode(MODE_WAITING);


  /*
00: PCLK selected as USART1 clock source (default)
01: System clock (SYSCLK) selected as USART1 clock
10: LSE clock selected as USART1 clock
11: HSI clock selected as USART1 clock
   * */

  RCC_TypeDef * rcc = RCC;
  rcc->CFGR3 =  (RCC->CFGR3 & ~RCC_CFGR3_USART1SW) | RCC_CFGR3_USART1SW_0; // 0b01 - SYSCLK
  rcc->APB2ENR |= RCC_APB2ENR_USART1EN;
  NVIC_SetPriority (USART1_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
  NVIC_EnableIRQ(USART1_IRQn);

  USART_TypeDef * usart = getHDUsart()->getRegs();

  CLEAR_BIT(usart->CR1, USART_CR1_UE); // disable USART
  usart->CR1 = 0
//      |USART_CR1_TXEIE
//      |USART_CR1_TCIE
      |USART_CR1_RXNEIE
      |USART_CR1_TE
      |USART_CR1_RE
      ;
  usart->BRR = SystemCoreClock / iStr->baudRate;

  usart->CR2 = 0;
  usart->CR3 = 0
      | USART_CR3_OVRDIS
      ;

  SET_BIT(usart->CR1, USART_CR1_UE); // enable USART
  SET_BIT(usart->CR1, USART_CR1_RE); // enable READ


}




inline void usartHandler(RS485::HalfDuplexUsart * hdUsart){
  //  RS485::DataStruct ds = data;
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

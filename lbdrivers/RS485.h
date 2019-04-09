/*
 * RS485.h
 *
 *  Created on: 21 lut 2014
 *      Author: lechu
 */

#ifndef RS485_H_
#define RS485_H_

#include "Fifo.h"
#include "Gpio.h"

#include "systemDefs.h"

//#ifdef USE_USART1
//
//#endif


class RS485{
public:
  static constexpr uint32_t USART_ZB40_BAUD_RATE  = 57600;
  static constexpr uint32_t USART_DEFAULT_BAUD_RATE  = 115200;
  static constexpr uint32_t USART_DEFAULT_BUFFER_SIZE  = 200;

  typedef enum{
    MODE_WAITING,
    MODE_RX,
    MODE_TX
  }Mode;


public:
  class HalfDuplexUsart{
  private:
    volatile Mode  mode = Mode::MODE_WAITING;
  protected:
    USART_TypeDef * usart = nullptr;
    Gpio rs485Dir = Gpio(GPIOB, 12);
    uint8_t bufRxTx[USART_DEFAULT_BUFFER_SIZE];
    Fifo  fifoRxTx = Fifo(bufRxTx, USART_DEFAULT_BUFFER_SIZE);
  public:

    HalfDuplexUsart()noexcept {}

    Fifo* getFifo()  {
      return &fifoRxTx;
    }

    void setup(USART_TypeDef * registers){
      usart = registers;
      if (registers == USART1){
        rs485Dir.mutate(GPIOA, 12);
      }else if (registers == USART2){
        rs485Dir.mutate(GPIOD, 4);
      }else{
        while(true){;} //error!
      }
      rs485Dir.setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
    }

    Mode getMode() const {
      return mode;
    }

    void setMode(const Mode newMode){
      mode = newMode;
      rs485Dir.setOutput(mode == MODE_TX);
    }

    USART_TypeDef * getRegs()const{ return usart; }
  };

private:

  HalfDuplexUsart halfDuplexUsart = HalfDuplexUsart();

  Fifo * getFifo(){
    return getHDUsart()->getFifo();
  }

public:

  static RS485 * rs485OnUSART1;
  static RS485 * rs485OnUSART2;

  void setup(USART_TypeDef * usartRegisters, uint32_t baudRate);

  bool txCompleted();

  //zwraca false jesli bufor jest pelny i sie nie da
  bool put(uint8_t znak);
  bool putAll(Fifo *dane);
  bool putString(const char* tekst);

  bool canPut(void);
  bool canPut(uint32_t nrOfBytes);

  bool canGet(void);

  inline void flush(){
    getHDUsart()->getFifo()->flush();
  }

  inline bool isEmpty(){
    return getHDUsart()->getFifo()->isEmpty();
  }


  // zwraca BUFFER_EMPTY_FLAG_U16 jesli bufor jest pusty lub znak na 1 bajcie
  uint16_t get(void);

  HalfDuplexUsart * getHDUsart() {
    return &halfDuplexUsart;
  }

};


//----------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------

void USART1_IRQHandler(void);
void USART2_IRQHandler(void);

//----------------------------------------------------
#ifdef __cplusplus
}
#endif
//----------------------------------------------------


#endif /* RS485_H_ */

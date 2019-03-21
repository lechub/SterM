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

class RS485{
public:
  static constexpr uint32_t USART_DEFAULT_BAUD_RATE  = 38400;
  static constexpr uint32_t USART_DEFAULT_BUFFER_SIZE  = 200;

  typedef enum{
    MODE_WAITING,
    MODE_RX,
    MODE_TX
  }Mode;


  typedef struct{
    USART_TypeDef * usart;
    Gpio *rs485Rx;
    Gpio *rs485Tx;
    Gpio *rs485Dir;
    uint32_t baudRate;
  }InitStruct;

public:
  class HalfDuplexUsart{
  public:
    Fifo* getFifo()  {
      return &fifoRxTx;
    }
  private:
    volatile Mode  mode = Mode::MODE_WAITING;
  protected:
    USART_TypeDef * usart = nullptr;
    Gpio *rs485Dir = nullptr;
    uint8_t bufRxTx[USART_DEFAULT_BUFFER_SIZE];
    Fifo  fifoRxTx = Fifo(bufRxTx, USART_DEFAULT_BUFFER_SIZE);
  public:
    void setup(USART_TypeDef * registers, Gpio *gpioDir){
      usart = registers;
      rs485Dir = gpioDir;
    }

    Mode getMode() const {
      return mode;
    }

    void setMode(const Mode newMode){
      mode = newMode;
      rs485Dir->setOutput(mode == MODE_TX);
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


//  RS485(USART_TypeDef * regs){
//    getHDUsart()->setRegisters(regs);
//  }

  void setup(InitStruct * iStr);

  bool txCompleted();

  //zwraca false jesli bufor jest pelny i sie nie da
  bool put(uint8_t znak);
  bool putAll(Fifo *dane);
  bool putString(const char* tekst);

  bool canPut(void);
  bool canPut(uint32_t nrOfBytes);

  bool canGet(void);

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

//----------------------------------------------------
#ifdef __cplusplus
}
#endif
//----------------------------------------------------


#endif /* RS485_H_ */

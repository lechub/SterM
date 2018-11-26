/*
 * I2C.h
 *
 *  Created on: 13 wrz 2018
 *      Author: lechu
 */

#ifndef I2C_H_
#define I2C_H_




//namespace STM32F0xx {

//#include "systemDefs.h"
#include <stdint.h>
#include "Gpio.h"
#include "Fifo.h"

class I2C {

public:
  I2C() {
    // TODO Auto-generated constructor stub

  }
  //	virtual ~I2C();


public:
  static constexpr uint32_t TIMEOUT_MS = 1000;
  static constexpr uint32_t NVIC_I2C_PRIORITY = 0x03U;
  static constexpr uint32_t COMMAND_MIN_DELAY_MS = 8;
  static constexpr uint32_t MAX_DATA_SIZE = 100;

  typedef struct{
    I2C_TypeDef * base = nullptr;
    Gpio * sda = nullptr;
    Gpio * scl = nullptr;
    uint32_t i2cFreqkHz = 100;
  }DeviceDefs;

private:
  static constexpr uint8_t I2C_BLOCK_START = 0xfe;

  typedef enum{
    IDLE, START, ADR, DATA, STOP, FAIL
  }State;

  DeviceDefs * i2c = nullptr;
  Fifo * data = nullptr;
  volatile uint16_t slaveAdr = 0;
  volatile State state = State::IDLE;

  void setState(State newstate){
    if (newstate == State::IDLE){
      state = State::IDLE;
    }else{
      state = newstate;
    }
  }
  State getState(){ return state; }

  bool init();	// zainicjowanie pinow, i2c itp.
  void cyclicJob();
  void sendSTART();

public:
  static I2C * getInstance();

  bool init(DeviceDefs * initDefsPtr);

  void setSlaveAdres(uint16_t slaveAdres) {slaveAdr = slaveAdres; }

  //bool masterTransmit(Fifo * frame);
  //bool masterTransmit(uint8_t * buffer, uint8_t amount);
//  bool masterTransmit(uint16_t slaveAdres, uint8_t * buffer, uint8_t amount){
//    setSlaveAdres(slaveAdres);
//    return masterTransmit(buffer, amount);
//  }

 inline bool masterTransmit(Fifo * frame){
   return masterWriteBlock(frame);
 }

  bool masterWriteBlock(Fifo * toWrite);
  bool masterReadStart(uint32_t size);
  bool masterReadComleted();
  bool masterReadGet(Fifo *toRead);

  bool isBusy(){ return state != State::IDLE; }

  void irqEvent();

  void irqError();

};


#ifdef __cplusplus
extern "C" {
#endif

#ifdef STM32F072
void I2C1_IRQHandler(void) __attribute__ ((interrupt));
#endif
#ifdef STM32F4xx
void I2C1_EV_IRQHandler(void) __attribute__ ((interrupt));
void I2C1_ER_IRQHandler(void) __attribute__ ((interrupt));
#endif

#ifdef __cplusplus
}
#endif

//} /* namespace STM32F4xx */

#endif /* I2C_H_ */

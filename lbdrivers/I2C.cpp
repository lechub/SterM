/*
 * I2C.cpp
 *
 *  Created on: 13 wrz 2018
 *      Author: lechu
 */

#include "I2C.h"
#include "Fifo.h"

//using namespace STM32F0xx;

//namespace STM32F0xx {

//I2C::~I2C() {
//	// TODO Auto-generated destructor stub
//}





I2C i2cInstance = I2C();

I2C * I2C::getInstance(){ return &i2cInstance; }

///**
// * Aktywny delay - ale musi byc uruchomiony Systick z QuickTask
// */
//void I2C::dirtyDelayMs(uint32_t miliseconds){
//	volatile uint32_t tmp = QuickTask::getCounter();
//	while(QuickTask::getCounter() - tmp < miliseconds ){
//		cyclicJob();
//	}
//}
// ****************************************************************

uint8_t buffer[I2C::MAX_DATA_SIZE];
Fifo dataFrame = Fifo(buffer, I2C::MAX_DATA_SIZE);


//uint8_t frameBuffer1[50];
//Fifo frame1Fifo = Fifo(frameBuffer1, 50);
//
//uint8_t frameBuffer2[50];
//Fifo frame2Fifo = Fifo(frameBuffer1, 50);
//
//uint8_t dataFifo[250];
//Fifo dataStreamFifo = Fifo(dataFifo, 250);

// ****************************************************************
bool I2C::init(DeviceDefs * initDefsPtr){
  i2c = initDefsPtr;
  if (i2c->base == nullptr) return false;
  if (initDefsPtr->sda == nullptr) return false;
  if (initDefsPtr->scl == nullptr) return false;
  if (i2c->i2cFreqkHz == 0) i2c->i2cFreqkHz = 100;
  return init();
}

bool I2C::init(){
  data = &dataFrame;
  setState(State::IDLE);

  // RCC
  uint32_t RCC_BITS = RCC_APB1ENR_I2C1EN;
  if (i2c->base == I2C2){
    RCC_BITS = RCC_APB1ENR_I2C2EN;
  }
  do {
    SET_BIT(RCC->APB1ENR, RCC_BITS);
    /* Delay after an RCC peripheral clock enabling */
    __IO uint32_t tmpreg = READ_BIT(RCC->APB1ENR, RCC_BITS);
    (void)(tmpreg);
  } while(0U);

  CLEAR_BIT(i2c->base->CR1, I2C_CR1_PE); // musi byc wylaczony zeby zainicjowac filtry
  while(i2c->base->CR1 & I2C_CR1_PE){;} // odczekanie na koniec resetu I2C

  /**I2C1 GPIO Configuration
PB10     ------> I2C2_SCL
PB11     ------> I2C2_SDA
   */
  { // GPIO setup
    i2c->scl->setup(Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::OpenDrain, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::MaximumSpeed);
    i2c->sda->setup(Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::OpenDrain, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::MaximumSpeed);
    i2c->scl->setAlternateFunc(0x01); //AF01 - );
    i2c->sda->setAlternateFunc(0x01); //AF01 - );
  }

  // set analog and digital noise filters
  SET_BIT(i2c->base->CR1, I2C_CR1_ANFOFF);  // analog filter wlaczony
  MODIFY_REG(i2c->base->CR1, I2C_CR1_DFN, 0x01<< 8);  //digital filter na 1 I2Cclock

  //i2c->base->CR1 &= ~I2C_CR1_PE;
  //while(i2c->base->CR1 & I2C_CR1_PE){;}	// odczekanie na koniec resetu I2C

  if (i2c->base == I2C1){
    NVIC_SetPriority(IRQn_Type::I2C1_IRQn, I2C::NVIC_I2C_PRIORITY);
    NVIC_EnableIRQ(IRQn_Type::I2C1_IRQn);
  }else if (i2c->base == I2C2){
    NVIC_SetPriority(IRQn_Type::I2C2_IRQn, I2C::NVIC_I2C_PRIORITY);
    NVIC_EnableIRQ(IRQn_Type::I2C2_IRQn);
  }else{
    return false;
  }


  CLEAR_BIT(i2c->base->CR1, I2C_CR1_NOSTRETCH);   //

  WRITE_REG(i2c->base->TIMINGR, 0x060101A27); //dla APBClk==48MHz: PRESC=0x6, SCLDEL=0x1, SDADEL=0x0, SCLH=0x1A, SCLL=0x27

  //	WRITE_REG(i2c->base->TIMINGR, 0x00707BBA); // dla APBClk==32MHz: PRESC=0x0, SCLDEL=0x7, SDADEL=0x0, SCLH=0x7B, SCLL=0xBA

  //  i2c->base->OAR1 = 0x00 + (1UL<<14);


  SET_BIT(i2c->base->CR1, I2C_CR1_PE);
  while (READ_BIT(i2c->base->CR1, I2C_CR1_PE) == 0){;}  // odczekanie na koniec resetu I2C
  //	while(!(i2c->base->CR1 & I2C_CR1_PE)){;}	// odczekanie na koniec resetu I2C

  return true;
}

void I2C::irqEvent(){
  //makeStamp();	// i2c cos wlasnie robi (przerwanie)
  I2C_TypeDef * base = i2c->base;
  Fifo * frame = data;

  if (READ_BIT(base->ISR, I2C_ISR_NACKF)){
    setState(I2C::State::FAIL);
    SET_BIT(base->ICR, I2C_ICR_NACKCF);
  }

  if (READ_BIT(base->ISR, I2C_ISR_RXNE)){ // nastepny bajt do odebrania
    frame->put(uint8_t(i2c->base->RXDR));
  }

  if (READ_BIT(base->ISR, I2C_ISR_TXIS)){ // nastepny bajt do wyslania
    i2c->base->TXDR = uint8_t(frame->get());
  }

  //  if (READ_BIT(base->ISR, I2C_ISR_TXE)){ // nastepny bajt do wyslania
  //    i2c->base->TXDR = uint8_t(frame->get());
  //  }

  if (READ_BIT(base->ISR, I2C_ISR_STOPF)){
    SET_BIT(base->ICR, I2C_ICR_STOPCF);
    setState(State::IDLE);
    callCompletedCallback();
  }

}


void I2C::irqError(){
  static uint32_t err = 0;
  err++;
  uint32_t isr = i2c->base->ISR;

  setState(State::IDLE);
}



bool I2C::masterWriteBlock(Fifo * toWrite){
  if (isBusy()) return false; // sprawdzic czy mozna
  setState(State::START);             // znacznik poczatku
  data->flush();
  data->putAll(toWrite);
  CLEAR_BIT(i2c->base->CR2, I2C_CR2_RD_WRN);    //Transfer direction: RD_WRN

  CLEAR_BIT(i2c->base->CR2, I2C_CR2_ADD10);           // Addressing mode (7-bit or 10-bit): ADD10
  MODIFY_REG(i2c->base->CR2, I2C_CR2_SADD, slaveAdr); //  Slave address to be sent: SADD[9:0]

  CLEAR_BIT(i2c->base->CR2, I2C_CR2_HEAD10R);   //In case of 10-bit address read: HEAD10R bit. HEAD10R must be configure to indicate if the complete address sequence must be sent, or only the header in case of a direction change.

  SET_BIT(i2c->base->CR2, I2C_CR2_AUTOEND); // Stop pojdzie z automatu

  uint32_t size = data->get_count();
  if (size > 0x0ff) size = 0x0ff;
  MODIFY_REG(i2c->base->CR2, I2C_CR2_NBYTES, size<<16); // The number of bytes to be transferred: NBYTES[7:0]. If the number of bytes is equal to or greater than 255 bytes, NBYTES[7:0] must initially be filled with 0xFF.

  i2c->base->CR1 |= I2C_CR1_TXIE
      //| I2C_CR1_TCIE
      //| I2C_CR1_RXIE
      //| I2C_CR1_NACKIE
      | I2C_CR1_STOPIE
      //| I2C_CR1_ERRIE
      ;

  i2c->base->CR2 |= I2C_CR2_START;        // start transmisji i2c
  return true;
}

bool I2C::masterReadStart(uint32_t size){
  if (isBusy()) return false; // sprawdzic czy mozna
  setState(State::START);             // znacznik poczatku
  data->flush();
  SET_BIT(i2c->base->CR2, I2C_CR2_RD_WRN);

  CLEAR_BIT(i2c->base->CR2, I2C_CR2_ADD10);           // Addressing mode (7-bit or 10-bit): ADD10
  MODIFY_REG(i2c->base->CR2, I2C_CR2_SADD, slaveAdr); //  Slave address to be sent: SADD[9:0]

  CLEAR_BIT(i2c->base->CR2, I2C_CR2_HEAD10R);   //In case of 10-bit address read: HEAD10R bit. HEAD10R must be configure to indicate if the complete address sequence must be sent, or only the header in case of a direction change.

  // uint32_t size = data->get_count();
  //if (size > 0x0fff) size = 0x0fff;
  MODIFY_REG(i2c->base->CR2, I2C_CR2_NBYTES, size<<16); // The number of bytes to be transferred: NBYTES[7:0]. If the number of bytes is equal to or greater than 255 bytes, NBYTES[7:0] must initially be filled with 0xFF.

  i2c->base->CR1 |= I2C_CR1_TCIE
      | I2C_CR1_TXIE
      //| I2C_CR1_RXIE
      //| I2C_CR1_NACKIE
      | I2C_CR1_STOPIE
      //| I2C_CR1_ERRIE
      ;

  i2c->base->CR2 |= I2C_CR2_START;        // start transmisji i2c
  return true;
}



//bool I2C::masterTransmit(Fifo * frame){
//
//  if (isBusy()) return false;	// sprawdzic czy mozna
//  data->putAll(frame);
//  //sendSTART(i2c->base);
//  setState(State::START);							// znacznik poczatku
//  SET_BIT(i2c->base->CR2, I2C_CR2_START);				// start transmisji i2c
//  return true;
//}
//
//
//bool I2C::masterTransmit(uint8_t * dataBuffer, uint8_t amount){
//
//  if (isBusy()) return false;	// sprawdzic czy mozna
//  for (uint32_t i = 0; i < amount; i++){			// zaladowac do bufora
//    data->put(dataBuffer[i]);
//  }
//  setState(State::START);							// znacznik poczatku
//  i2c->base->CR1 |= I2C_CR1_START;				// start transmisji i2c
//  return true;
//}



void I2C1_IRQHandler(void){
  i2cInstance.irqEvent();
}

void I2C2_IRQHandler(void){
  i2cInstance.irqEvent();
}

//void I2C2_IRQHandler(void){
//  i2cInstance.irqEvent();
//}

/*
Arbitration Loss (ARLO)
Bus Error detection (BERR)
Overrun/Underrun (OVR)
Timeout detection (TIMEOUT)
PEC error detection (PECERR)
Alert pin event detection (ALERT)
 */


//} /* namespace STM32F4 */

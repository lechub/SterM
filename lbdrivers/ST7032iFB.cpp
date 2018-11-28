/*
 * st7032ifb.cpp
 *
 *  Created on: 12 wrz 2018
 *      Author: lechu
 */

#include "ST7032iFB.h"
#include "Hardware.h"
#include "QuickTask.h"

void dummyCallback();

void dummyCallback(){

  ST7032iFB::callb();
}

void ST7032iFB::callb(){
//  (pollFunction)();
  callbackObject->process();
}

ST7032iFB * ST7032iFB::callbackObject = nullptr;


// periodycznie wykonywana funkcja monitor() opakowana w aku_callback()
QuickTask st7032Task(QuickTask::QT_PERIODIC, dummyCallback, ST7032iFB::LCD_REFRESH_INTERVAL_MILISECOND);

//void (* ST7032iFB::pollFunction)() = nullptr;
//void setPollFunction(void (ST7032iFB::*pollF)()){
//  ;
//}

uint8_t dataTab1[ST7032iFB::LCD_CHARS_AMOUNT +5];
FrameBuffer frame_buffer = FrameBuffer(ST7032iFB::LCD_COL, ST7032iFB::LCD_ROW, dataTab1);

uint8_t dataTab2[ST7032iFB::LCD_CHARS_AMOUNT +5];
Fifo dataFifo = Fifo(dataTab2, ST7032iFB::LCD_CHARS_AMOUNT +5);


//void (ST7032iFB::* pollFunction)() = nullptr;

ST7032iFB::~ST7032iFB() {
  // TODO Auto-generated destructor stub
}

// Functions for ST7032I

void ST7032iFB::delayMs(uint32_t milis){
  Hardware::delayMsWithBackground(milis);
}

void ST7032iFB::delayMsDirty(uint32_t milis){
  Hardware::delayMsDirty(milis);
}

//void ST7032iFB::setPollFunction(void (ST7032iFB::*pollF)()){
//  st7032Task.setCallback(pollF);
//}

//void ST7032iFB::setPollFunction(){
//  st7032Task.setCallback((void(*)())(&ST7032iFB::process));
//}

// LCD initialization procedure
void ST7032iFB::init(I2C * i2cPort, Gpio * backLightPin, Gpio * resetLCDPin){
  //keybQtsk.setCallback(&process);
  callbackObject = this;
  gpioBackLight = backLightPin;
  gpioResetLCD = resetLCDPin;
  gpioBackLight->setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
  gpioResetLCD->setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::OpenDrain, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
  frameBuffer = &frame_buffer;
  fifo = &dataFifo;
  lcdStage = LcdStage::START;
  i2c = i2cPort;
  i2c->setSlaveAdres(ST7032I_ADDRESS<<1); // czemu do k.... nędzy adres jest przesuniety???


  setBackLight(true);
  setResetPin(false);
  delayMsDirty(50);
  setResetPin(true);
  delayMsDirty(100);

  sendCommand(0x38);
  delayMsDirty(300);
  sendCommand(0x39);
  delayMsDirty(300);

  sendCommand(0x14);
  delayMsDirty(100);
#ifdef LCD5V
  sendCommand(0x79);
  delayMsDirty(100);
  sendCommand(0x50);
  delayMsDirty(100);
  sendCommand(0x6c);
  delayMsDirty(100);
#else
  sendCommand(0x74);
  delayMsDirty(30);
  sendCommand(0x54);
  delayMsDirty(30);
  sendCommand(0x6f);
  delayMsDirty(30);
#endif
  sendCommand(0x0c);
  delayMsDirty(100);
  sendCommand(0x01);
  delayMsDirty(100);

  sendCommand(0x38);	//cmmd table 0
  delayMsDirty(100);
//  frameBuffer->printXY(0,0,"Chyba dziala..");
//  sendLine(0);  //cmmd table 0
}



bool ST7032iFB::sendCmdOrData(bool isCMD, uint8_t byteValue){
  fifo->flush();
  fifo->put(isCMD ? CMD : DATA);
  fifo->put(byteValue);
  //return i2c->masterTransmit(fifo);
  return i2c->masterWriteBlock(fifo);
}

bool ST7032iFB::sendCommand(uint8_t cmd){
  return sendCmdOrData(true, cmd);
}

bool ST7032iFB::sendData(uint8_t data){
  return sendCmdOrData(false, data);
}


bool ST7032iFB::sendLine(uint32_t lineNr){
  fifo->flush();
  fifo->put(DATA);
  if (!frameBuffer->getLine(lineNr, fifo)) return false;
  return i2c->masterTransmit(fifo);
}


void ST7032iFB::process(){
  LcdStage stage = lcdStage;
  switch(stage){
  case LcdStage::START:
  case LcdStage::WAIT_GOTO_L1:
    lcdStage = LcdStage::GOTO_LINE1;
    break;
  case LcdStage::GOTO_LINE1:
    if (!isBusy()){
      sendCommand(LcdCommand::LCD_HOME);
      lcdStage = LcdStage::WAIT_LINE1;
    }
    break;
  case LcdStage::WAIT_LINE1:
    lcdStage = LcdStage::SEND_LINE1;
    break;
  case LcdStage::SEND_LINE1:
    if (!isBusy()){
      sendLine(0);
      lcdStage = LcdStage::WAIT_GOTO_L2;
    }
    break;
  case LcdStage::WAIT_GOTO_L2:
    lcdStage = LcdStage::GOTO_LINE2;
    break;
  case LcdStage::GOTO_LINE2:
    if (!isBusy()){
      gotoXY(0,1);
      lcdStage = LcdStage::WAIT_LINE2;
    }
    break;
  case LcdStage::WAIT_LINE2:
    lcdStage = LcdStage::SEND_LINE2;
       break;
  case LcdStage::SEND_LINE2:
    if (!isBusy()){
      sendLine(1);
      lcdStage = LcdStage::START;
    }
    break;
  default:
    lcdStage = LcdStage::START;
    break;
  }
}






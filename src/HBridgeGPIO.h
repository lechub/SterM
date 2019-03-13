/*
 * HBridge.h
 *
 *  Created on: 14 lis 2018
 *      Author: lechu
 */

#ifndef HBRIDGEGPIO_H_
#define HBRIDGEGPIO_H_

#include "Gpio.h"
#include "HBridge.h"

class HBridgeGPIO : public HBridge {

protected:
  typedef enum{
    S1_LOW_OFF,
    S2_HI_OFF,
    S3_HI_ON,
    S4_LOW_ON,
  }Stage;

  Gpio * gpioLeftH;
  Gpio * gpioLeftL;
  Gpio * gpioRightH;
  Gpio * gpioRightL;

  POWER pState  = POWER::HOLD_DOWN;
  Stage stage = S1_LOW_OFF;

  uint32_t stageTimeMs = 0;

  void setStage(Stage st){
    stage = st;
    switch(stage){
    case Stage::S3_HI_ON:{
      switch(pState){
      case POWER::HOLD_UP:
        if (!gpioLeftL->getOutput()) gpioLeftH->setOutputUp();
        if (!gpioRightL->getOutput()) gpioRightH->setOutputUp();
        break;
      case POWER::LeftToRight:
        if (!gpioLeftL->getOutput()) gpioLeftH->setOutputUp();
        break;
      case POWER::RightToLeft:
        if (!gpioRightL->getOutput()) gpioRightH->setOutputUp();
        break;
      default: break;
      }
    }
    break;
    case Stage::S4_LOW_ON: {
      switch(pState){
      case POWER::HOLD_DOWN:
        if (!gpioLeftH->getOutput()) gpioLeftL->setOutputUp();
        if (!gpioRightH->getOutput()) gpioRightL->setOutputUp();
        break;
      case POWER::LeftToRight:
        if (!gpioRightH->getOutput()) gpioRightL->setOutputUp();
        break;
      case POWER::RightToLeft:
        if (!gpioLeftH->getOutput()) gpioLeftL->setOutputUp();
        break;
      default: break;
      }
    }
    break;
    case Stage::S2_HI_OFF:
      gpioLeftH->setOutputDown();
      gpioRightH->setOutputDown();
      break;
    case Stage::S1_LOW_OFF:
    default:
      gpioLeftL->setOutputDown();
      gpioRightL->setOutputDown();
      break;
    }
  }


public:
  HBridgeGPIO(Gpio * gpioLeftHigh, Gpio * gpioLeftLow, Gpio * gpioRightHigh, Gpio * gpioRightLow){
    gpioLeftH = gpioLeftHigh;
    gpioLeftL = gpioLeftLow;
    gpioRightH = gpioRightHigh;
    gpioRightL = gpioRightLow;
  }

  virtual void init(){
    gpioLeftH->setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
    gpioLeftH->setOutputDown();
    gpioLeftL->setupFromClone(gpioLeftH);
    gpioLeftL->setOutputDown();
    gpioRightH->setupFromClone(gpioLeftH);
    gpioRightH->setOutputDown();
    gpioRightL->setupFromClone(gpioLeftH);
    gpioRightL->setOutputDown();

    setPower(POWER::HOLD_FLOAT);
    setStage(Stage::S1_LOW_OFF);
  }

  virtual void setPower(POWER powerMode){
    if (pState == powerMode) return;
    pState = powerMode;
    setStage(Stage::S1_LOW_OFF);
  }

  virtual POWER getPowerMode(){ return pState; }

  virtual void poll(){
    //
    stageTimeMs += TIME_POLL_PERIOD_MS;
    if (stageTimeMs < TIME_SWITCH_DELAY_MS) return;
    stageTimeMs = 0;

    Stage st = stage;
    switch(st){
    case Stage::S1_LOW_OFF: setStage(S2_HI_OFF); break;
    case Stage::S2_HI_OFF: setStage(S3_HI_ON); break;
    case Stage::S3_HI_ON: setStage(S4_LOW_ON); break;
    case Stage::S4_LOW_ON: break;
    default:
      setPower(POWER::HOLD_FLOAT);
      break;
    }
  }


};




#endif /* HBRIDGEGPIO_H_ */

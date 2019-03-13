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

public:

  Gpio * gpioLeftH;
  Gpio * gpioLeftL;
  Gpio * gpioRightH;
  Gpio * gpioRightL;
  PULL pLeft = PULL::LOW;
  PULL pRight = PULL::LOW;
  PULL pLeftSet = PULL::FLOATING;
  PULL pRightSet = PULL::FLOATING;
  POWER pState  = POWER::HOLD_DOWN;

  void setPullLeft(PULL ouputPosition){
    pLeft = ouputPosition;
    switch(ouputPosition){
    case PULL::HIGH:    //
      gpioLeftL->setOutputDown(); // najpierw wylaczyc dolny
      gpioLeftH->setOutputUp();   // a potem wlaczyc gorny
      break;
    case PULL::LOW:
      gpioLeftH->setOutputDown();   // najpierw wylaczyc gorny
      gpioLeftL->setOutputUp();     //  a potem wlaczyc dolny
      break;
    case PULL::FLOATING: // cokolwiek innego - wszystko wylaczyc w diably
    default:
      gpioLeftH->setOutputDown();
      gpioLeftL->setOutputDown();
      break;
    }
  }

  void setPullRight(PULL ouputPosition){
    pRight = ouputPosition;
    switch(ouputPosition){
    case PULL::HIGH:    //
      gpioRightL->setOutputDown(); // najpierw wylaczyc dolny
      gpioRightH->setOutputUp();   // a potem wlaczyc gorny
      break;
    case PULL::LOW:
      gpioRightH->setOutputDown();   // najpierw wylaczyc gorny
      gpioRightL->setOutputUp();     //  a potem wlaczyc dolny
      break;
    case PULL::FLOATING: // cokolwiek innego - wszystko wylaczyc w diably
    default:
      gpioRightH->setOutputDown();
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

    setPullRight(PULL::FLOATING);
    setPullLeft(PULL::FLOATING);
    setPower(POWER::HOLD_FLOAT);
  }

  virtual void setPower(POWER powerMode){
    pState = powerMode;
    switch(pState){
    case POWER::LeftToRight:
      pRightSet = PULL::LOW;
      pLeftSet = PULL::HIGH;
      break;
    case POWER::RightToLeft:   // naped idzie w dol
      pLeftSet = PULL::LOW;
      pRightSet = PULL::HIGH;
      break;
    case POWER::HOLD_UP:  // oba bieguny na plusie
      pLeftSet = PULL::HIGH;
      pRightSet = PULL::HIGH;
      break;
    case POWER::HOLD_DOWN:  // oba bieguny na minusie
      pLeftSet = PULL::LOW;
      pRightSet = PULL::LOW;
      break;
    case POWER::HOLD_FLOAT:  // naped bez zasilania
    default:
      pLeftSet = PULL::FLOATING;
      pRightSet = PULL::FLOATING;
    }
  }

  virtual POWER getPowerMode(){ return pState; }


  virtual void poll(){
    if ((pRight != pRightSet) || (pLeft != pLeftSet)){
      if ((pRight == PULL::FLOATING) && (pLeft == PULL::FLOATING)){
        setPullRight(pRightSet);
        setPullLeft(pLeftSet);
      }else{
        setPullRight(PULL::FLOATING);
        setPullLeft(PULL::FLOATING);
      }
    }
  }

};




#endif /* HBRIDGEGPIO_H_ */

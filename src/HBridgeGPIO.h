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

  Gpio * gpioPH;
  Gpio * gpioPL;
  Gpio * gpioMH;
  Gpio * gpioML;
  PULL pPlus = PULL::LOW;
  PULL pMinus = PULL::LOW;
  PULL pPlusSet = PULL::FLOATING;
  PULL pMinusSet = PULL::FLOATING;
  POWER pState  = POWER::HOLD_DOWN;

  void setPullPlus(PULL ouputPosition){
    pPlus = ouputPosition;
    switch(ouputPosition){
    case PULL::HIGH:    //
      gpioPL->setOutputDown(); // najpierw wylaczyc dolny
      gpioPH->setOutputUp();   // a potem wlaczyc gorny
      break;
    case PULL::LOW:
      gpioPH->setOutputDown();   // najpierw wylaczyc gorny
      gpioPL->setOutputUp();     //  a potem wlaczyc dolny
      break;
    case PULL::FLOATING: // cokolwiek innego - wszystko wylaczyc w diably
    default:
      gpioPH->setOutputDown();
      gpioPL->setOutputDown();
      break;
    }
  }

  void setPullMinus(PULL ouputPosition){
    pMinus = ouputPosition;
    switch(ouputPosition){
    case PULL::HIGH:    //
      gpioML->setOutputDown(); // najpierw wylaczyc dolny
      gpioMH->setOutputUp();   // a potem wlaczyc gorny
      break;
    case PULL::LOW:
      gpioMH->setOutputDown();   // najpierw wylaczyc gorny
      gpioML->setOutputUp();     //  a potem wlaczyc dolny
      break;
    case PULL::FLOATING: // cokolwiek innego - wszystko wylaczyc w diably
    default:
      gpioMH->setOutputDown();
      gpioML->setOutputDown();
      break;
    }
  }

public:
  HBridgeGPIO(Gpio * gpioPlusH, Gpio * gpioPlusL, Gpio * gpioMinusH, Gpio * gpioMinusL){
    gpioPH = gpioPlusH;
    gpioPL = gpioPlusL;
    gpioMH = gpioMinusH;
    gpioML = gpioMinusL;
  }

  virtual void init(){
    gpioPH->setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
    gpioPL->setupFromClone(gpioPH);
    gpioMH->setupFromClone(gpioPH);
    gpioML->setupFromClone(gpioPH);

    setPullMinus(PULL::FLOATING);
    setPullPlus(PULL::FLOATING);
    setPower(POWER::FLOAT);
  }

  virtual void setPower(POWER powerMode){
    pState = powerMode;
    switch(pState){
    case POWER::STRAIGHT:
      pMinusSet = PULL::LOW;
      pPlusSet = PULL::HIGH;
      break;
    case POWER::INVERT:   // naped idzie w dol
      pPlusSet = PULL::LOW;
      pMinusSet = PULL::HIGH;
      break;
    case POWER::HOLD_UP:  // oba bieguny na plusie
      pPlusSet = PULL::HIGH;
      pMinusSet = PULL::HIGH;
      break;
    case POWER::HOLD_DOWN:  // oba bieguny na minusie
      pPlusSet = PULL::LOW;
      pMinusSet = PULL::LOW;
      break;
    case POWER::FLOAT:  // naped bez zasilania
    default:
      pPlusSet = PULL::FLOATING;
      pMinusSet = PULL::FLOATING;
    }
  }

  virtual POWER getPowerMode(){ return pState; }


  virtual void poll(){
    if ((pMinus != pMinusSet) || (pPlus != pPlusSet)){
      if ((pMinus == PULL::FLOATING) && (pPlus == PULL::FLOATING)){
        setPullMinus(pMinusSet);
        setPullPlus(pPlusSet);
      }else{
        setPullMinus(PULL::FLOATING);
        setPullPlus(PULL::FLOATING);
      }
    }
  }

};




#endif /* HBRIDGEGPIO_H_ */

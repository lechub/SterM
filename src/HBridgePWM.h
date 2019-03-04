/*
 * HBridge.h
 *
 *  Created on: 14 lis 2018
 *      Author: lechu
 */

#ifndef HBRIDGEPWM_H_
#define HBRIDGEPWM_H_

#include "Gpio.h"
#include "HBridge.h"
#include "SoftStart.h"

class HBridgePWM : public HBridge {

public:

protected:

  SoftStart sStart = SoftStart();
  PULL pLeft = PULL::LOW;
  PULL pRight = PULL::LOW;
  PULL pLeftSet = PULL::FLOATING;
  PULL pRightSet = PULL::FLOATING;
  uint16_t  pwmLeft = 0;
  uint16_t  pwmRight = 0;

  POWER pState  = POWER::HOLD_DOWN;

  void setPullLeft(PULL ouputPosition, uint16_t pwm){
    pLeft = ouputPosition;
    switch(ouputPosition){
    case PULL::HIGH:    //
      sStart.setLeftPWM(pwm);   //  gpioPL->setOutputDown(); // najpierw wylaczyc dolny
      sStart.setLeftH(true);  //  gpioPH->setOutputUp();   // a potem wlaczyc gorny
      break;
    case PULL::LOW:
      sStart.setLeftH(false);   //  gpioPH->setOutputDown();   // najpierw wylaczyc gorny
      sStart.setLeftPWM(pwm);  //  gpioPL->setOutputUp();     //  a potem wlaczyc dolny
      break;
    case PULL::FLOATING: // cokolwiek innego - wszystko wylaczyc w diably
    default:
      sStart.setLeftH(false);   //  gpioPH->setOutputDown();
      sStart.setLeftPWM(pwm);  //   gpioPL->setOutputDown();
      break;
    }
  }

  void setPullRight(PULL ouputPosition, uint16_t pwm){
    pRight = ouputPosition;
    switch(ouputPosition){
    case PULL::HIGH:    //
      sStart.setRightPWM(pwm); //  gpioML->setOutputDown(); // najpierw wylaczyc dolny
      sStart.setRightH(true); //  gpioMH->setOutputUp();   // a potem wlaczyc gorny
      break;
    case PULL::LOW:
      sStart.setRightH(false); //  gpioMH->setOutputDown();   // najpierw wylaczyc gorny
      sStart.setRightPWM(pwm); //  gpioML->setOutputUp();     //  a potem wlaczyc dolny
      break;
    case PULL::FLOATING: // cokolwiek innego - wszystko wylaczyc w diably
    default:
      sStart.setRightH(false); //  gpioMH->setOutputDown();
      sStart.setRightPWM(pwm); //  gpioML->setOutputDown();
      break;
    }
  }

public:
  HBridgePWM(){  }


  virtual void init(){
    sStart.init();
    setPullRight(PULL::FLOATING, 0);
    setPullLeft(PULL::FLOATING, 0);
    setPower(POWER::FLOAT);
  }



  void setPower(POWER powerMode){
    pState = powerMode;
    switch(pState){
    case POWER::STRAIGHT:
      pRightSet = PULL::LOW;
      pLeftSet = PULL::HIGH;
      break;
    case POWER::INVERT:   // naped idzie w dol
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
    case POWER::FLOAT:  // naped bez zasilania
    default:
      pLeftSet = PULL::FLOATING;
      pRightSet = PULL::FLOATING;
    }

  }

  POWER getPowerMode(){ return pState; }

  void poll(){
    if ((pRight != pRightSet) || (pLeft != pLeftSet)){
      if ((pRight == PULL::FLOATING) && (pLeft == PULL::FLOATING)){
        setPullRight(pRightSet, 0);
        setPullLeft(pLeftSet, 0);
      }else{
        setPullRight(PULL::FLOATING, 0);
        setPullLeft(PULL::FLOATING, 0);
      }
    }
  }

};




#endif /* HBRIDGEPWM_H_ */

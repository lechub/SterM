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

  static constexpr uint32_t SOFTSTART_MIN_TIME_mS = 2000;
  static constexpr uint32_t SOFTSTART_MAX_TIME_mS = 4000;

public:

protected:

  SoftStart sStart = SoftStart();
  Gpio ovc = Gpio(GPIOA, 15);

  //  uint16_t  pwmLeft = 0;
  uint16_t  pwm = 0;
  uint32_t stepPwm = 0;
  uint32_t timePwmMs = 0;

  POWER pState  = POWER::HOLD_FLOAT;


public:
  HBridgePWM(){  }


  virtual void init(){
    ovc.setup(Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
    sStart.init();
    setPower(POWER::HOLD_FLOAT);
  }


  bool isOVCDetected(){
    return !ovc.getInput();
  }

  void setPower(POWER powerMode){
    if (powerMode == pState) return;
    pState = powerMode;

    sStart.setLeftL(false);
    sStart.setRightL(false);
    sStart.setLeftH(false);
    sStart.setRightH(false);
    pwm = 0;
    timePwmMs = 0;

    switch(pState){
    case POWER::LeftToRight:
      sStart.setRightPWM(50);
      sStart.setLeftH(true);
      break;
    case POWER::RightToLeft:   // naped idzie w dol
      sStart.setLeftPWM(50);
      sStart.setRightH(true);
      break;
    case POWER::HOLD_UP:  // oba bieguny na plusie
      sStart.setLeftH(true);
      sStart.setRightH(true);
      break;
    case POWER::HOLD_DOWN:  // oba bieguny na minusie
      sStart.setLeftL(true);
      sStart.setRightL(true);
      break;
    case POWER::HOLD_FLOAT:  // naped bez zasilania
    default:
      break;
    }
  }



  POWER getPowerMode(){ return pState; }

  void poll(){
    if ((pState != POWER::LeftToRight)&& (pState != POWER::RightToLeft)) return;
    if (pwm >= SoftStart::PWM_RESOLUTION) return;
    stepPwm += TIME_POLL_PERIOD_MS;
    timePwmMs += TIME_POLL_PERIOD_MS;
    if (timePwmMs > SOFTSTART_MAX_TIME_mS){   // przekroczenie czasu dla PWM?
      setPower(POWER::HOLD_DOWN);
      return;
    }
    uint16_t newPWM = pwm;
    constexpr uint32_t STEP = SOFTSTART_MIN_TIME_mS/(SoftStart::PWM_RESOLUTION);
    if (isOVCDetected()){
      newPWM = (newPWM >= 2 )? uint16_t(newPWM - 2) : 0;
    }else {
      if (stepPwm >= STEP){
        stepPwm -= STEP;
        newPWM++;
      }
    }
    if (pwm != newPWM){
      pwm = newPWM;
      switch(pState){
      case POWER::LeftToRight: sStart.setRightPWM(pwm); break;
      case POWER::RightToLeft: sStart.setLeftPWM(pwm);  break;
      default: break;
      }
    }

  }

};


/*
 *
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
    setPower(POWER::HOLD_FLOAT);
  }



  void setPower(POWER powerMode){
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
 * */

#endif /* HBRIDGEPWM_H_ */

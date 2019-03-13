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
#include "Sterownik.h"

class HBridgePWM : public HBridge {

  static constexpr uint32_t SOFTSTART_MIN_TIME_mS = 1000;
  static constexpr uint32_t SOFTSTART_MAX_TIME_mS = 4000;

public:

protected:

  typedef enum{
    S1_LOW_OFF,
    S2_HI_OFF,
    S3_HI_ON,
    S4_LOW_ON,
  }Stage;

  SoftStart sStart = SoftStart();
  Gpio ovc = Gpio(GPIOA, 15);

  //  uint16_t  pwmLeft = 0;
  uint16_t  pwm = 0;
  uint32_t stepPwm = 0;
  uint32_t stageTimeMs = 0;

  POWER pState  = POWER::HOLD_FLOAT;
  Stage stage = S1_LOW_OFF;

  uint32_t switchDelay = 0;

  void setStage(Stage st){
    stage = st;
    switch(stage){
    case Stage::S3_HI_ON:{
      switch(pState){
      case POWER::HOLD_UP:
        if (sStart.getLeftPWM() == 0) sStart.setLeftH(true);
        if (sStart.getRightPWM() == 0) sStart.setRightH(true);
        break;
      case POWER::LeftToRight:
        if (sStart.getLeftPWM() == 0) sStart.setLeftH(true);
        break;
      case POWER::RightToLeft:
        if (sStart.getRightPWM() == 0) sStart.setRightH(true);
        break;
      default: break;
      }
    }
    break;
    case Stage::S4_LOW_ON: {
      switch(pState){
      case POWER::HOLD_DOWN:
        if (!sStart.getLeftH()) sStart.setLeftL(true);
        if (!sStart.getRightH()) sStart.setRightL(true);
        break;
      case POWER::LeftToRight:  // wlaczane przez PWM-a
      case POWER::RightToLeft:
      default: break;
      }
    }
    break;
    case Stage::S2_HI_OFF:
      sStart.setLeftH(false);
      sStart.setRightH(false);
      break;
    case Stage::S1_LOW_OFF:
    default:
      sStart.setLeftL(false);
      sStart.setRightL(false);
      break;
    }
  }


public:
  HBridgePWM(){  }


  virtual void init(){
    ovc.setup(Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
    sStart.init();
    setPower(POWER::HOLD_FLOAT);
    setStage(Stage::S1_LOW_OFF);
  }


  bool isOVCDetected(){
    return !ovc.getInput();
  }

  virtual void setPower(POWER powerMode){
    if (pState == powerMode) return;
    pState = powerMode;
    setStage(Stage::S1_LOW_OFF);
    stageTimeMs = 0;
  }


  POWER getPowerMode(){ return pState; }

  void poll();

};



#endif /* HBRIDGEPWM_H_ */

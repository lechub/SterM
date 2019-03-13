/*
 * HBridgePWM.cpp
 *
 *  Created on: 12 mar 2019
 *      Author: lechu
 */

#include "HBridgePWM.h"
#include "Sterownik.h"

 void HBridgePWM::poll(){
    stageTimeMs += TIME_POLL_PERIOD_MS;
    Stage st = stage;
    switch(st){
    case Stage::S1_LOW_OFF:
      if (stageTimeMs >= TIME_SWITCH_DELAY_MS){
        setStage(S2_HI_OFF);
        stageTimeMs = 0;
      }
      break;
    case Stage::S2_HI_OFF:
      if (stageTimeMs >= TIME_SWITCH_DELAY_MS){
        setStage(S3_HI_ON);
        stageTimeMs = 0;
      }
      break;
    case Stage::S3_HI_ON:
      if (stageTimeMs >= TIME_SWITCH_DELAY_MS){
        setStage(S4_LOW_ON);
        stageTimeMs = 0;
        pwm = 0;
        stepPwm = 0;
      }
      break;
    case Stage::S4_LOW_ON:{
      switch(pState){
      case POWER::HOLD_UP:
      case POWER::HOLD_FLOAT:
        break;
      case POWER::HOLD_DOWN:
        sStart.setLeftPWM(SoftStart::PWM_MAX);
        sStart.setRightPWM(SoftStart::PWM_MAX);
        break;
      case POWER::LeftToRight:
      case POWER::RightToLeft:{
        if (sterM->isSoftStartUse()){
          if (pwm >= SoftStart::PWM_RESOLUTION) break;
          stepPwm += TIME_POLL_PERIOD_MS;
          if (stageTimeMs > SOFTSTART_MAX_TIME_mS){   // przekroczenie czasu dla PWM?
            setPower(POWER::HOLD_FLOAT);
            break;
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
        }else{  // jednak nie stosowac PWM-a
          switch(pState){
          case POWER::LeftToRight: sStart.setRightPWM(SoftStart::PWM_MAX); break;
          case POWER::RightToLeft: sStart.setLeftPWM(SoftStart::PWM_MAX);  break;
          default: break;
          }
        }
      }
      break;
      default: break;
      }
      break;
    }
    break;
    default:
      setPower(POWER::HOLD_FLOAT);
      break;
    }
  }




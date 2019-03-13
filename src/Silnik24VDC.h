/*
 * Naped42V.h
 *
 *  Created on: 14 lis 2018
 *      Author: lechu
 */

#ifndef SILNIK24VDC_H_
#define SILNIK24VDC_H_

#include "HBridgePWM.h"
#include "HBridgeGPIO.h"
#include "SilnikNapedu.h"

class Silnik24VDC : public SilnikNapedu{


private:

  HBridge * hBridge12;
  HBridge * hBridge34;

  bool type1234 = false;

public:
  Silnik24VDC(HBridge * hbridge12, HBridge * hbridge34){
    hBridge12 = hbridge12;
    hBridge34 = hbridge34;
  }


  void init(){
    hBridge12->init();
    hBridge34->init();
  }


  void setType1234(bool enable){
    type1234 = enable;
  }

  void setMove(MOVE newMove){
    if (safePosition) newMove = MOVE::FLOAT;
    move = newMove;
    if (type1234){
      switch (move){
      case MOVE::UP:
        hBridge12->setPower(HBridge::POWER::LeftToRight);
        hBridge34->setPower(HBridge::POWER::RightToLeft);
        break;
      case MOVE::DOWN:   // naped idzie w dol
        hBridge12->setPower(HBridge::POWER::RightToLeft);
        hBridge34->setPower(HBridge::POWER::RightToLeft);
        break;
      case MOVE::HOLD_UP:  // oba bieguny na plusie
      case MOVE::HOLD_DOWN:  // oba bieguny na minusie
      case MOVE::FLOAT:  // naped bez zasilania
      default:
        hBridge12->setPower(HBridge::POWER::HOLD_DOWN);
        hBridge34->setPower(HBridge::POWER::HOLD_DOWN);
      }
    }else{
      switch (move){
      case MOVE::UP:
        hBridge34->setPower(HBridge::POWER::LeftToRight);
        break;
      case MOVE::DOWN:   // naped idzie w dol
        hBridge34->setPower(HBridge::POWER::RightToLeft);
        break;
      case MOVE::HOLD_UP:  // oba bieguny na plusie
        hBridge34->setPower(HBridge::POWER::HOLD_UP);
        break;
      case MOVE::HOLD_DOWN:  // oba bieguny na minusie
        hBridge34->setPower(HBridge::POWER::HOLD_DOWN);
        break;
      case MOVE::FLOAT:  // naped bez zasilania
      default:
        hBridge34->setPower(HBridge::POWER::HOLD_FLOAT);
      }
    }
  }



};


#endif /* SILNIK24VDC_H_ */

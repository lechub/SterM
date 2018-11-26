/*
 * Naped42V.h
 *
 *  Created on: 14 lis 2018
 *      Author: lechu
 */

#ifndef SILNIK24VDC_H_
#define SILNIK24VDC_H_

#include "HBridge.h"
#include "SilnikNapedu.h"

class Silnik24VDC : public SilnikNapedu{


private:

  HBridge * hBridge;

  MOVE move = MOVE::FLOAT;

public:
  Silnik24VDC(HBridge * hbridge){
    hBridge = hbridge;
  }


  void init(){
    hBridge->init();
  }

  void setMove(MOVE newMove){
    move = newMove;
    switch (move){
    case MOVE::UP:
      hBridge->setPower(HBridge::POWER::STRAIGHT);
      break;
    case MOVE::DOWN:   // naped idzie w dol
      hBridge->setPower(HBridge::POWER::INVERT);
      break;
    case MOVE::HOLD_UP:  // oba bieguny na plusie
      hBridge->setPower(HBridge::POWER::HOLD_UP);
      break;
    case MOVE::HOLD_DOWN:  // oba bieguny na minusie
      hBridge->setPower(HBridge::POWER::HOLD_DOWN);
      break;
    case MOVE::FLOAT:  // naped bez zasilania
    default:
      hBridge->setPower(HBridge::POWER::FLOAT);
    }
  }

  MOVE getMove(){
    return move;
  }

};


#endif /* SILNIK24VDC_H_ */

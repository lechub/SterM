/*
 * Hamulec.h
 *
 *  Created on: 14 lis 2018
 *      Author: lechu
 */

#ifndef HAMULEC_H_
#define HAMULEC_H_

#include "HBridge.h"

class Hamulec {

public:
  typedef enum{
    NORMALNIE,
    PRZECIWNIE,
    OFF,
  }MODE;

private:

  HBridge * hBridge;

  bool hamowanie = false;
  bool safePosition = true;
  MODE mode = OFF;

public:
  Hamulec(HBridge * hbridge){
    hBridge = hbridge;
  }

  bool gotoSafePosition(bool enable){
    safePosition = enable;
    if (safePosition)setMode(OFF);
     return safePosition;
   }

  void init(){
    hBridge->init();
    gotoSafePosition(true);
  }

  /**
   * Ustawia tryb hamowania i odlacza napiecie hamulca
   * @param newMode
   */
  void setMode(MODE newMode){
    //if (safePosition) newMode = MODE::OFF; // wymuszenie safePosition
    mode = newMode;
    if (mode == OFF)    // jesli off to nie ruszac tego bridge-a
      hBridge->setPower(HBridge::POWER::HOLD_FLOAT);
  }

  void setMode(MODE newMode, bool hamowac){
    setMode(newMode);
    hamuj(hamowac);
  }


  void hamuj(bool state){
    hamowanie = state;
    MODE tmpMode = mode;
    if (safePosition) tmpMode = OFF;
    switch(tmpMode){
    case MODE::NORMALNIE:
      if (hamowanie){
        hBridge->setPower(HBridge::POWER::LeftToRight);
      }else{
        hBridge->setPower(HBridge::POWER::HOLD_DOWN);
      }
      break;
    case MODE::PRZECIWNIE:
      if (hamowanie){
             hBridge->setPower(HBridge::POWER::HOLD_DOWN);
           }else{
             hBridge->setPower(HBridge::POWER::LeftToRight);
           }
      break;
    case MODE::OFF: break;  // wylaczone, nie rusza hbridge'a
    default:
      //hBridge->setPower(HBridge::POWER::FLOAT);
      break;
    }
  }

  MODE getMode(){ return mode; }
  bool isHamuje(){ return hamowanie; }
  bool isInSafePosition(){ return safePosition; }

};

#endif /* HAMULEC_H_ */

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
  MODE mode = OFF;

public:
  Hamulec(HBridge * hbridge){
    hBridge = hbridge;
  }

  void init(){
    hBridge->init();
  }

  /**
   * Ustawia tryb hamowania i odlacza napiecie hamulca
   * @param newMode
   */
  void setMode(MODE newMode){
    mode = newMode;
    if (mode != OFF)    // jesli off to nie ruszac tego bridge-a
      hBridge->setPower(HBridge::POWER::FLOAT);
  }

  void setMode(MODE newMode, bool hamowac){
    setMode(newMode);
    hamuj(hamowac);
  }


  void hamuj(bool state){
    hamowanie = state;
    switch(mode){
    case MODE::NORMALNIE:
      if (hamowanie){
        hBridge->setPower(HBridge::POWER::STRAIGHT);
      }else{
        hBridge->setPower(HBridge::POWER::FLOAT);
      }
      break;
    case MODE::PRZECIWNIE:
      if (hamowanie){
             hBridge->setPower(HBridge::POWER::FLOAT);
           }else{
             hBridge->setPower(HBridge::POWER::STRAIGHT);
           }
      break;
    case MODE::OFF:
    default:
      //hBridge->setPower(HBridge::POWER::FLOAT);
      break;
    }
  }

  MODE getMode(){ return mode; }
  bool isHamuje(){ return hamowanie; }

};

#endif /* HAMULEC_H_ */

/*
 * HBridge.h
 *
 *  Created on: 14 lis 2018
 *      Author: lechu
 */

#ifndef HBRIDGE_H_
#define HBRIDGE_H_

#include "Gpio.h"

class HBridge {


public:
  typedef enum{
     STRAIGHT,     // plus na +24V a minus na masie
     INVERT,   // plus ma masie a minus na +24V
     FLOAT,  // naped bez zasilania
     HOLD_UP,  // oba bieguny na plusie
     HOLD_DOWN,  // oba bieguny na minusie
   }POWER;

 protected:
  typedef enum{
    HIGH, LOW, FOATING
  }POSITION;

  typedef enum{
    PLUS, MINUS
  }LINE;

  Gpio * gpioPH;
  Gpio * gpioPL;
  Gpio * gpioMH;
  Gpio * gpioML;
  POSITION posPlus = POSITION::FOATING;
  POSITION posMinus = POSITION::FOATING;
  POWER power = POWER::FLOAT;

  void setOutputLine(LINE line, POSITION ouputPosition){
    Gpio * high;
    Gpio * low;
    switch(line){
    case LINE::PLUS:
      posPlus = ouputPosition;
      high = gpioPH;
      low = gpioPL;
      break;
    case LINE::MINUS:
    default:
      posMinus = ouputPosition;
      high = gpioMH;
      low = gpioML;
      break;
    }

    switch(ouputPosition){
    case POSITION::HIGH:    //
      low->setOutputDown(); // najpierw wylaczyc dolny
      high->setOutputUp();   // a potem wlaczyc gorny
      break;
    case POSITION::LOW:
      high->setOutputDown();   // najpierw wylaczyc gorny
      low->setOutputUp();     //  a potem wlaczyc dolny
      break;
    case POSITION::FOATING: // cokolwiek innego - wszystko wylaczyc w diably
    default:
      high->setOutputDown();
      low->setOutputDown();
      break;
    }
  }


public:
  HBridge(Gpio * gpioPlusH, Gpio * gpioPlusL, Gpio * gpioMinusH, Gpio * gpioMinusL){
    gpioPH = gpioPlusH;
    gpioPL = gpioPlusL;
    gpioMH = gpioMinusH;
    gpioML = gpioMinusL;
  }

  void setPlus(POSITION ouputPosition){
    setOutputLine(LINE::PLUS, ouputPosition);
  }

  void setMinus(POSITION ouputPosition){
    setOutputLine(LINE::MINUS, ouputPosition);
  }

  void init(){
    setPlus(POSITION::FOATING);
    setMinus(POSITION::FOATING);
  }

  void setPower(POWER powerMode){
    power = powerMode;
    switch (power){
    case POWER::STRAIGHT:
      setPlus(POSITION::FOATING);
      setMinus(POSITION::LOW);
      setPlus(POSITION::HIGH);
      break;
    case POWER::INVERT:   // naped idzie w dol
      setMinus(POSITION::FOATING);
      setPlus(POSITION::LOW);
      setMinus(POSITION::HIGH);
      break;
    case POWER::HOLD_UP:  // oba bieguny na plusie
      setMinus(POSITION::FOATING);
      setPlus(POSITION::FOATING);
      setMinus(POSITION::HIGH);
      setPlus(POSITION::HIGH);
      break;
    case POWER::HOLD_DOWN:  // oba bieguny na minusie
      setMinus(POSITION::FOATING);
      setPlus(POSITION::FOATING);
      setMinus(POSITION::LOW);
      setPlus(POSITION::LOW);
      break;
    case POWER::FLOAT:  // naped bez zasilania
    default:
      setMinus(POSITION::FOATING);
      setPlus(POSITION::FOATING);
    }
  }

  POWER getPowerMode(){ return power; }
  POSITION getPositionPlus(){ return posPlus; }
  POSITION getPositionMinus() { return posMinus; }



};

#endif /* HBRIDGE_H_ */

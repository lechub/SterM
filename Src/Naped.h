/*
 * Naped.h
 *
 *  Created on: 14 lis 2018
 *      Author: lechu
 */

#ifndef NAPED_H_
#define NAPED_H_

#include "Gpio.h"

class Naped {
protected:
  Gpio * gpioKrancUp;
  Gpio * gpioKrancDown;

public:
  typedef enum{
    UNKNOWN,
    ROTATE_UP,     // naped idzie do gory
    ROTATE_DOWN,   // naped idzie w dol
    FULL_UP,  // naped bez zasilania
    FULL_DOWN,  // oba bieguny na plusie
    FAILURE,  // oba bieguny na minusie
  }State;


  Naped(Gpio * krancowkaGora, Gpio * krancowkaDol){
    gpioKrancUp = krancowkaGora;
    gpioKrancDown = krancowkaDol;
  }

//
//  void rotateUP() = 0;
//  void rotateDOWN() = 0;

  bool isUp(){
    return !gpioKrancUp->getInput();
  }
  bool isDown(){
    return !gpioKrancDown->getInput();
  }

};

#endif /* NAPED_H_ */

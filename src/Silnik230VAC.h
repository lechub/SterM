/*
 * Naped230VAC.h
 *
 *  Created on: 14 lis 2018
 *      Author: lechu
 */

#ifndef SILNIK230VAC_H_
#define SILNIK230VAC_H_

#include "Gpio.h"
#include "SilnikNapedu.h"

class Silnik230VAC : public SilnikNapedu {


private:

  typedef enum{
    HIGH, LOW, FOATING
  }POSITION;

  typedef enum{
    PLUS, MINUS
  }LINE;

  Gpio * gpioZamknij;
  Gpio * gpioOtworz;


public:
  Silnik230VAC(Gpio * gpioZamk, Gpio * gpioOtw) {
    gpioZamknij = gpioZamk;
    gpioOtworz = gpioOtw;
  }


  void init(){
    //    gpio->setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MediumSpeed);
    gpioZamknij->setOutputDown();
    gpioOtworz->setOutputDown();
  }

  void setMove(MOVE newMove){
    if (safePosition) newMove = MOVE::FLOAT;
    move = newMove;
    switch (move){
    case MOVE::UP:
      gpioZamknij->setOutputDown();
      gpioOtworz->setOutputUp();
      break;
    case MOVE::DOWN:   // naped idzie w dol
      gpioOtworz->setOutputDown();
      gpioZamknij->setOutputUp();
      break;
    case MOVE::HOLD_UP:  // oba bieguny na plusie
    case MOVE::HOLD_DOWN:  // oba bieguny na minusie
    case MOVE::FLOAT:  // naped bez zasilania
    default:
      gpioZamknij->setOutputDown();
      gpioOtworz->setOutputDown();
    }
  }





};

#endif /* SILNIK230VAC_H_ */

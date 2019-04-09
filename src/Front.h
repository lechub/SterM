/*
 * Front.h
 *
 *  Created on: 28 lut 2019
 *      Author: lechu
 */

#ifndef FRONT_H_
#define FRONT_H_

#include "Gpio.h"

class Front {

public:
  typedef enum{
    DISABLED, UP, DOWN, STOP, NONE,
  }State;

private:



  Gpio * bUp;
  Gpio * bStop;
  Gpio * bDown;
  Gpio * stacyjka;

  State state;

  State getkeyState(){
    if (!isKeyActive()) return State::DISABLED;
    if (!bUp->getInput()) return State::UP;
    if (!bDown->getInput()) return State::DOWN;
    if (!bStop->getInput()) return State::STOP;
    return State::NONE;
  }


public:
  Front(Gpio * btnUp, Gpio * btnStop, Gpio * btnDown, Gpio * btnKey):
    bUp(btnUp), bStop(btnStop), bDown(btnDown), stacyjka(btnKey) {
    state = NONE;
  }


  void init(){
    bUp->setup(Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::LowSpeed);
    bStop->setupFromClone(bUp);
    bDown->setupFromClone(bUp);
    stacyjka->setupFromClone(bUp);
  }

  inline bool isKeyActive()const {
    return !stacyjka->getInput();
  }


  void poll();


  inline State getState()const {
    return state;
  }

  inline void pushStop(){
    state = State::STOP;
  }

};

#endif /* FRONT_H_ */

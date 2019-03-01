/*
 * Front.cpp
 *
 *  Created on: 28 lut 2019
 *      Author: lechu
 */

#include <Front.h>
#include "Sterownik.h"


void Front::poll(){
  if (sterM->isPozarOrAlarmAkust()){
    state = State::DISABLED;
    return;
  }
  State keyState = getkeyState();
  switch(keyState){
  case State::UP:
    state = (sterM->isOtwarte()) ? State::STOP : keyState;
    break;
  case State::DOWN:
    state = (sterM->isZamkniete()) ? State::STOP : keyState;
    break;
  case State::DISABLED:
  case State::STOP:  state = keyState; break;
  case State::NONE:{
    switch(state){
    case State::UP:
        if (sterM->isOtwarte()) state = State::STOP;
        break;
    case State::DOWN:
        if (sterM->isZamkniete()) state = State::STOP;
        break;
    case State::DISABLED: state = State::STOP; break;
    default: break;
    }
    break;
  }
  default: break;
  }
}

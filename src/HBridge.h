/*
 * HBridge.h
 *
 *  Created on: 14 lis 2018
 *      Author: lechu
 */

#ifndef HBRIDGE_H_
#define HBRIDGE_H_

#include "Gpio.h"
#include "OVCdetector.h"

class HBridge {

public:
  static constexpr uint32_t TIME_POLL_PERIOD_MS = 1;
  static constexpr uint32_t TIME_SWITCH_DELAY_MS = 5;

  typedef enum{
    LeftToRight,     // plus z lewej a minus z prawej
    RightToLeft,   // plus z prawej a minus z lewej
    HOLD_FLOAT,  // oba bieguny bez zasilania
    HOLD_UP,  // oba bieguny na plusie
    HOLD_DOWN,  // oba bieguny na minusie
  }POWER;

protected:
  typedef enum{
    HIGH, LOW, FLOATING
  }PULL;


public:
  virtual ~HBridge(){}
  virtual void setPower(POWER powerMode) = 0;
  virtual POWER getPowerMode() = 0;
  virtual void poll() = 0;
  virtual void init() = 0;

};




#endif /* HBRIDGE_H_ */

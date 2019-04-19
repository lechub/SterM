/*
 * OVCdetector.h
 *
 *  Created on: 11 kwi 2019
 *      Author: lechu
 */

#ifndef OVCDETECTOR_H_
#define OVCDETECTOR_H_

#include "Settings.h"
#include "QuickTask.h"

class OVCdetector {

protected:
  volatile uint32_t OVCDelay = 0;
  volatile uint32_t OVCTimeStamp = 0;
public:
  // wylaczenie obciazenia dla usuniecia OVC
  virtual void ovcHold(bool state) = 0;

  // dla zainicjowania innych akcji przy OVC
  virtual void ovcStart() = 0;

  // metoda wywolywana przez przerwanie od OVC,
  virtual void onOVC(){
    ovcStart();
    ovcHold(true);
    OVCDelay = OVC_SILENCE_MS;
    OVCTimeStamp = QuickTask::getCounter();
  }

  // wywolywac cyklicznie, dla sterowania OVC
  virtual void ovcCheck(){
    uint32_t newStamp = QuickTask::getCounter();
    if (OVCDelay > 0){
      uint32_t interval = newStamp - OVCTimeStamp;
      OVCDelay = (interval >= OVCDelay) ? 0 : OVCDelay - interval;
    }
    OVCTimeStamp = newStamp;
    ovcHold(OVCDelay != 0);
  }

  virtual void ovcIrqEnable(bool state) = 0;

  virtual bool inline isInOVC(){
    return OVCDelay != 0;
  }

  virtual ~OVCdetector(){};
};

#endif /* OVCDETECTOR_H_ */

/*
 * Events.h
 *
 *  Created on: 8 kwi 2019
 *      Author: lechu
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#include "Event.h"

class Events {

public:

  typedef enum{
    //NoEvents = 0,
    BladKrancowek = 1,
    Brak230VAC,
    UszkodzenieAkumulatora,
    NiskiStanAkumulatora,
    WysokaRezystancjaAku,
    UszkodzenieCzujnikaTemp,
    UszkodzenieBezpiecznika,
    InnyBladZB40,
    BladAutoTestu,
    PrzeciazenieNapedu,
    LastNumber,
  }Numer;

  typedef enum{
    DISABLED = 0,
    ENABLED = 1,
  }EnableValue;

  static Event * getEvent(Numer eventNr);


private:
  Events();

  static constexpr uint8_t eventTabLen = LastNumber - BladKrancowek;
  static Event eventTab[eventTabLen];

public:
  static bool setEvent(Numer eventNr, uint32_t newValue){
    Event * ev = getEvent(eventNr);
    if (ev == nullptr) return false;
    if (ev->value == newValue) return true;
    {
      ev->value = newValue;
      // ToDo wygenerować zdarzenie, jeśli trzeba
    }
    return true;
  }

  static inline bool setEvent(Numer eventNr, EnableValue enableValue){
    return setEvent(eventNr, (uint32_t)enableValue);
  }

};


#endif /* EVENTS_H_ */

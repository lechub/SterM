/*
 * Event.h
 *
 *  Created on: 8 kwi 2019
 *      Author: lechu
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "Settings.h"

class Event {

public:

  typedef enum{
      Informacja = 0,        // Zdarzenie normalne - interesujące tylko w toku testów systemu
      WaznaInformacja,   // Zdarzenie ważne, kluczowe dla działania, ale nie nosi znamion problemów
      Ostrzezenie,       // System działa normalnie, ale są przesłanki do przyszłych komplikacji
      Usterka,          // System jest w stanie awaryjnym, ale może samoczynnie z niego wyjść po ustapieniu awarii
      Alarm,            // Stan alarmu. System wykonuje czynności alarmowe do czasu jego zakończenia
      Awaria,           // System wykrył usterkę wymagającą interwencji obsługi
      AwariaKrytyczna,  // Awaria nieusuwalana przez obsługę. Wymaga autoryzowanego serwisu producenta
   }Priority;

//#pragma GCC diagnostic ignored "-Wreorder"
  const char * name;
  volatile uint32_t   value;
  uint8_t    enumNr;
  Priority    priority;

  Event(const uint8_t enumValNr, const char * eventName, Priority pri)
    : name(eventName), value(0), enumNr(enumValNr), priority(pri) {
  }


  inline bool isActive()const {
    return value != 0;
  }

  inline void setActive(bool active) {
    value = active;
  }

  inline uint32_t getValue()const {
    return value;
  }

  inline void setValue(uint32_t newValue) {
    value = newValue;
  }


};

#endif /* EVENT_H_ */

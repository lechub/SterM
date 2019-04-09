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
      WaznaInformacja = 3,   // Zdarzenie ważne, kluczowe dla działania, ale nie nosi znamion problemów
      Ostrzezenie = 6,       // System działa normalnie, ale są przesłanki do przyszłych komplikacji
      Usterka = 10,          // System jest w stanie awaryjnym, ale może samoczynnie z niego wyjść po ustapieniu awarii
      Alarm = 15,            // Stan alarmu. System wykonuje czynności alarmowe do czasu jego zakończenia
      Awaria = 20,           // System wykrył usterkę wymagającą interwencji obsługi
      AwariaKrytyczna = 30,  // Awaria nieusuwalana przez obsługę. Wymaga autoryzowanego serwisu producenta
   }Priority;

//#pragma GCC diagnostic ignored "-Wreorder"
  const char * name;
  uint32_t   value;
  uint8_t    enumNr;
  Priority    priority;

  Event(const uint8_t enumValNr, const char * eventName, Priority pri)
    : name(eventName), value(0), enumNr(enumValNr), priority(pri) {
  }

};

#endif /* EVENT_H_ */

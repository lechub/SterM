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
    BrakPolaczeniaZB40,
    BladAutoTestu,
    PrzeciazenieNapedu,
    AlarmPA,
    LastNumber,
  }Numer;

  typedef enum{
    DISABLED = 0,
    ENABLED = 1,
  }EnableValue;

  typedef enum{
    BrakAlarmu = 0,
    AlarmPozarowy = 1,
    AlarmAkustyczny = 2,
  }Alarm;

  static Event * getEvent(Numer eventNr);

  typedef uint8_t Iterator;

  static Iterator getIterator(){ return 0; }

  static Event * getNextEvent(Iterator & iterator) {
    if (iterator >= eventTabLen) return nullptr;
    return &eventTab[iterator++];
  }

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

  static inline bool setEvent(Numer eventNr, bool enableV){
    return setEvent(eventNr, enableV ? EnableValue::ENABLED : EnableValue::DISABLED);
  }

  static Alarm getAlarm(){
    return (Alarm)getEvent(Numer::AlarmPA)->value;
  }

  static bool isAwaria(Event::Priority minPriority){
    Iterator it = getIterator();
//    Event * najwyzszy = nullptr;
    while (true){
      Event * ev = getNextEvent(it);
      if (ev == nullptr) break;                     // przeszukano wszystkie
      if (ev->priority < minPriority) continue;     // olewac mniejsze priorytety
      if (ev->enumNr == Numer::AlarmPA) continue;   // nie liczyc alarmow pozarowych
      if(ev->value > 0) return true;
    }
    return false;
  }


};


#endif /* EVENTS_H_ */

/*
 * Events.cpp
 *
 *  Created on: 8 kwi 2019
 *      Author: lechu
 */

#include <Events.h>

//NoEvents = 0,
//    BladKrancowek = 1,
//    Brak230VAC,
//    BrakAkumulatora,
//    NiskiStanAkumulatora,
//    WysokaRezystancjaAku,
//    UszkodzenieCzujnikaTemp,
//    UszkodzenieBezpiecznika,
//    InnyBladZB40,
//

Event Events::eventTab[eventTabLen]={
    Event(BladKrancowek, "Blad kranc.", Event::Priority::Awaria),
    Event(Brak230VAC, "Brak 230VAC",  Event::Priority::Usterka),
    Event(UszkodzenieAkumulatora, "Uszk. akum.",  Event::Priority::Awaria),
    Event(NiskiStanAkumulatora, "Niski stan akum.",  Event::Priority::Usterka),
    Event(WysokaRezystancjaAku, "Rezyst. akum.",  Event::Priority::Awaria),
    Event(UszkodzenieCzujnikaTemp, "Czujnik temp.",  Event::Priority::Awaria),
    Event(UszkodzenieBezpiecznika, "Uszk. bezpiecz.",  Event::Priority::Awaria),
    Event(InnyBladZB40, "Inny blad",  Event::Priority::Awaria),
    Event(BladAutoTestu, "Blad SelfTest",  Event::Priority::Awaria),
    Event(PrzeciazenieNapedu, "Przeciazenie",  Event::Priority::Usterka),
};

Event * Events::getEvent(Numer enumNr){
  for (uint8_t i = 0; i < eventTabLen; i++ ){
    Event * ev = &eventTab[i];
    if (ev->enumNr == enumNr) return ev;
  }
  return nullptr;
}




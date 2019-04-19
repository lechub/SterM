/*
 * Events.cpp
 *
 *  Created on: 8 kwi 2019
 *      Author: lechu
 */

#include <Events.h>

// NoEvents = 0,
//    BladKrancowek = 1,
//    Brak230VAC,
//    UszkodzenieAkumulatora,
//    NiskiStanAkumulatora,
//    WysokaRezystancjaAku,
//    UszkodzenieCzujnikaTemp,
//    UszkodzenieBezpiecznika,
//    InnyBladZB40,
//    BrakPolaczeniaZB40,
//    BladAutoTestu,
//    PrzeciazenieNapedu,
//    AlarmPA,
//    LastNumber,


Event Events::eventTab[eventTabLen]={
    Event(NoEvents, "Inf._test1", Event::Priority::WaznaInformacja),
    Event(BladKrancowek, "Obie_kranc.", Event::Priority::Awaria),
    Event(Brak230VAC, "230VAC",  Event::Priority::Usterka),
    Event(UszkodzenieAkumulatora, "Uszk._akum.",  Event::Priority::Awaria),
    Event(NiskiStanAkumulatora, "Niski akum.",  Event::Priority::Usterka),
    Event(WysokaRezystancjaAku, "Rezyst. akum.",  Event::Priority::Awaria),
    Event(UszkodzenieCzujnikaTemp, "Czujnik t.",  Event::Priority::Awaria),
    Event(UszkodzenieBezpiecznika, "Bezpiecz.",  Event::Priority::Awaria),
    Event(InnyBladZB40, "Inny_ZB40",  Event::Priority::Awaria),
    Event(BrakPolaczeniaZB40, "Polacz._ZB40",  Event::Priority::Awaria),
    Event(BladAutoTestu, "Blad SelfTest",  Event::Priority::Awaria),
    Event(PrzeciazenieNapedu, "Przeciazenie",  Event::Priority::Usterka),
    Event(AlarmPozarowy, "Alarm_Pozarowy",  Event::Priority::Alarm),
    Event(AlarmAkustyczny, "Alarm_Akust.",  Event::Priority::Alarm),//    Event(AlarmPA, "Alarm",  Event::Priority::Alarm),
    Event(LastNumber, "AwariaK_test2",  Event::Priority::AwariaKrytyczna),
};

//Event * Events::getEvent(Name enumNr){
//  for (uint8_t i = 0; i < eventTabLen; i++ ){
//    Event * ev = &eventTab[i];
//    if (ev->enumNr == enumNr) return ev;
//  }
//  return nullptr;
//}




/*
 * Sterownik.h
 *
 *  Created on: 13.09.2016
 *      Author: lechu
 */

#ifndef STEROWNIK_H_
#define STEROWNIK_H_

//#include "Led.h"
#include "Silnik230VAC.h"

#include "Silnik24VDC.h"
#include "Hamulec.h"
#include "Pinout.h"

class Sterownik {

private:

  //	Praca praca = Praca();

public:

  typedef enum {
    ZATRZYMANA,   // silnik stoi, krańcówki niewłączone
    OTWARTA,  // górna krańcówka włączona
    ZAMKNIETA,   // dolna krańcówka włączona
    OTWIERA_SIE,   // silnik idzie do góry, krańcówki niewłączone
    ZAMYKA_SIE,   // silnik idzie w dół, krańcówki niewłączone
    USZKODZONA,   // stan niewłaściwy, np. obie krańcówki włączone
  }StanBramy;

  typedef enum{
    NIEOKRESLONY,
    VIC_012x, // 230VAC z hamulcem o odwrotnej logice
    VIC_042x, // 230VAC z hamulcem o zwykłej logice
    VIC_010x, // Becker 24V: 1 i 2 - wybor kierunku, 3 i 4 zasilanie
    VIC_040x, // Simu 24V: 3 i 4 podaje napięcie wprost lub na odwrót
    VIC_0701, // Simu 24V: 3 i 4 podaje napięcie wprost lub na odwrót, 1 i 2 -hamulec o zwykłej logice
  }NAPED;

private:
  Silnik24VDC * silnik24VDC;
  Silnik230VAC * silnik230VAC;
  Hamulec * hamulec;
  Pinout *wewy = nullptr;

  NAPED typNapedu = NIEOKRESLONY;
  StanBramy stanBramy = ZATRZYMANA;



  SilnikNapedu * getSilnik() const{
    SilnikNapedu * sn;
    if (isTyp230VAC()){
      sn = silnik230VAC;
    }else{
      sn = silnik24VDC;
    }
    return sn;
//    return reinterpret_cast<SilnikNapedu>( ?  : );
  }



public:
  Sterownik(Silnik24VDC * silnik24, Silnik230VAC * silnik230, Hamulec * hamulecSilnika){
    silnik24VDC = silnik24;
    silnik230VAC = silnik230;
    hamulec = hamulecSilnika;
  }

  bool init(){
    wewy = &pins;
    silnik24VDC->init();
    silnik230VAC->init();
    hamulec->init();
    return true;
  }

  inline bool isTyp230VAC() const{
    switch(getTypNapedu()){
        case VIC_012x:
        case VIC_042x: return true;
        case VIC_010x:
        case VIC_040x:
        case VIC_0701:
        case NIEOKRESLONY:
        default:    return false;
        }
  }

  inline NAPED getTypNapedu() const{ return typNapedu; }

  inline bool isOtwarte()const{ return wewy->gpioInKrancOtwarte.getInput(); }
  inline bool isZamkniete()const{ return wewy->gpioInKrancZamkniete.getInput(); }
  inline bool isZakazOtwierania()const{ return !(wewy->gpioInZakazOtwierania.getInput()); }
  inline bool isZakazZamykania()const{ return !(wewy->gpioInZakazZamykania.getInput()); }
  inline bool isPozar()const{ return !(wewy->gpioInPozar.getInput()); }

  StanBramy podnies(){
    bool mozna = (      // mozna podniesc jesli
        (!isOtwarte())    // nie jest otwarte
        && (!isZakazOtwierania()) // nie ma zakazu otwierania
        && (!isPozar())   // nie ma pozaru
    );

    switch(stanBramy){
    case  OTWARTA:
      getSilnik()->setMove(SilnikNapedu::MOVE::HOLD_DOWN);
      break;
    case  ZAMKNIETA:
    case  OTWIERA_SIE:
    case  ZATRZYMANA:
      getSilnik()->setMove(SilnikNapedu::MOVE::UP);
      stanBramy = (mozna) ? OTWIERA_SIE : OTWARTA;
      break;
    case  ZAMYKA_SIE:
      getSilnik()->setMove(SilnikNapedu::MOVE::FLOAT); // zatrzymaj a potem podnos
      hamulec->hamuj(false);
      stanBramy = ZATRZYMANA;
      break;
    case  USZKODZONA:
      getSilnik()->setMove(SilnikNapedu::MOVE::HOLD_DOWN);
      break;
    default:
      stanBramy = ZATRZYMANA;
      break;
    }
    return stanBramy;
  }

  StanBramy opusc(){
    bool mozna = (      // mozna podniesc jesli
        (!isZamkniete())    // nie jest zamkniete
        && (!isZakazZamykania()) // nie ma zakazu zamykania
        && (!isPozar())   // nie ma pozaru
    );

    switch(stanBramy){
    case  ZAMKNIETA:
      hamulec->hamuj(false);
      getSilnik()->setMove(SilnikNapedu::MOVE::HOLD_DOWN);
      break;
    case  OTWARTA:
    case  ZATRZYMANA:
    case  ZAMYKA_SIE:
      hamulec->hamuj(false);
      getSilnik()->setMove(SilnikNapedu::MOVE::DOWN);
      //bool zezwolenie =
      stanBramy = (mozna) ? ZAMYKA_SIE : ZAMKNIETA;
      break;
    case  OTWIERA_SIE:
      getSilnik()->setMove(SilnikNapedu::MOVE::FLOAT); // zatrzymaj a potem opusc
      hamulec->hamuj(false);
      stanBramy = ZATRZYMANA;
      break;
    case  USZKODZONA:
      getSilnik()->setMove(SilnikNapedu::MOVE::HOLD_DOWN);
      break;
    default:
      stanBramy = ZATRZYMANA;
      break;
    }
    return stanBramy;
  }

  StanBramy zatrzymaj(){
    getSilnik()->setMove(SilnikNapedu::MOVE::DOWN);
    hamulec->hamuj(true);
    stanBramy = ZATRZYMANA;
    return stanBramy;
  }

  StanBramy getStanBramy() const{
    return stanBramy;
  }

  void setNaped(NAPED nowyTypNapedu){
    typNapedu = nowyTypNapedu;
    switch(typNapedu){
    case VIC_012x: hamulec->setMode(Hamulec::MODE::PRZECIWNIE, false); break;
    case VIC_0701: hamulec->setMode(Hamulec::MODE::NORMALNIE, false); break;
    case VIC_042x: hamulec->setMode(Hamulec::MODE::OFF, false); break;
    case VIC_010x: hamulec->setMode(Hamulec::MODE::OFF, false); break;
    case VIC_040x: hamulec->setMode(Hamulec::MODE::OFF, false); break;
    case NIEOKRESLONY:
    default:    return  hamulec->setMode(Hamulec::MODE::OFF, false); break;
    }
  }

  bool gotoSafePosition(){
    silnik24VDC->setMove(SilnikNapedu::MOVE::FLOAT);
    silnik230VAC->setMove(SilnikNapedu::MOVE::FLOAT);
    hamulec->setMode(Hamulec::MODE::OFF, false);
    return true;
  }

  //void poll();

  // StanBramy ustalStanBramy();

};

extern Sterownik * sterM;

#endif /* STEROWNIK_H_ */

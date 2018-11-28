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
#include "Parameter.h"

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

  bool awaria = false;


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

  void fixOutputs(){
    wewy->gpioOutOtwarte.setOutput(stanBramy == OTWARTA);
    wewy->gpioOutZamkniete.setOutput(stanBramy == ZAMKNIETA);
    setBuzzer((stanBramy == ZAMYKA_SIE)||(stanBramy == OTWIERA_SIE));
    if (stanBramy == USZKODZONA) awaria = true;
  }


  StanBramy check(){
    if (isOtwarte()) stanBramy = OTWARTA;
    if (isZamkniete()) stanBramy = ZAMKNIETA;
    if (isOtwarte() && isZamkniete()){
      stanBramy = USZKODZONA;
    }else{
      if (stanBramy == USZKODZONA) stanBramy = ZATRZYMANA;
    }

    switch(stanBramy){
    case  OTWARTA:
      getSilnik()->setMove(SilnikNapedu::MOVE::HOLD_DOWN);
      hamulec->hamuj(true);
      if(!isOtwarte()){
        stanBramy = ZATRZYMANA;
      }
      break;
    case  ZAMKNIETA:
      getSilnik()->setMove(SilnikNapedu::MOVE::HOLD_DOWN);
      hamulec->hamuj(true);
      if(!isZamkniete()){
        stanBramy = ZATRZYMANA;
      }
      break;
    case  OTWIERA_SIE:
      hamulec->hamuj(false);
      getSilnik()->setMove(SilnikNapedu::MOVE::UP);
      break;
    case  ZAMYKA_SIE:
      hamulec->hamuj(false);
      getSilnik()->setMove(SilnikNapedu::MOVE::DOWN);
      break;
    case  ZATRZYMANA:
      getSilnik()->setMove(SilnikNapedu::MOVE::HOLD_DOWN);
      hamulec->hamuj(true);
      break;
    case  USZKODZONA:
      getSilnik()->setMove(SilnikNapedu::MOVE::HOLD_DOWN);
      hamulec->hamuj(true);
      break;
    default:
      stanBramy = ZATRZYMANA;
      getSilnik()->setMove(SilnikNapedu::MOVE::HOLD_DOWN);
      hamulec->hamuj(true);
      break;
    }
    fixOutputs();
    return stanBramy;
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
  inline bool isAlarmAkustyczny()const{ return !(wewy->gpioInAlarmAkust.getInput()); }
  inline bool isRezerwa1()const{ return !(wewy->gpioInRezerwa1.getInput()); }
  inline bool isRezerwa2()const{ return !(wewy->gpioInRezerwa2.getInput()); }

  inline void setBuzzer(bool enable){ wewy->gpioOutBuzer.setOutput(enable); }

  inline bool isAwaria()const{ return awaria; }

  void setAwaria(bool enable){ awaria = enable; }

  StanBramy podnies(){
    bool mozna = (      // mozna podniesc jesli
        (!isOtwarte())    // nie jest otwarte
        && (!isZakazOtwierania()) // nie ma zakazu otwierania
        && (!isPozar())   // nie ma pozaru
    );

    switch(stanBramy){
    case  OTWARTA:
      break;
    case  ZAMKNIETA:
    case  ZATRZYMANA:
      if (mozna){
        stanBramy = OTWIERA_SIE; // otwierac, jesli mozna
        uint32_t licznik = Parameter::getValue(Parameter::Nazwa::LICZNIK);
        Parameter::setValue(Parameter::Nazwa::LICZNIK, uint16_t(licznik + 1));
      }
      break;
    case  OTWIERA_SIE:
      if (!mozna) stanBramy = ZATRZYMANA; // otwierac, a jesli nie mozna to zatrzymac i juz
      break;
    case  ZAMYKA_SIE:
      stanBramy = ZATRZYMANA; // najpierw zatrzymac
      break;
    case  USZKODZONA: // poczekac, az uszkodzenie ustapi
      break;
    default:
      stanBramy = ZATRZYMANA;
      break;
    }
    check();
    fixOutputs();
    return stanBramy;
  }

  StanBramy opusc(){
    bool mozna = (      // mozna podniesc jesli
        (!isZamkniete())    // nie jest zamkniete
        && (!isZakazZamykania()) // nie ma zakazu zamykania
    );

    switch(stanBramy){
    case  ZAMKNIETA:  // zamknietej nie trzeba zamykac
      break;
    case  OTWARTA:
    case  ZATRZYMANA:
      if (mozna){
        stanBramy = ZAMYKA_SIE; // zamykac, jesli mozna
        uint32_t licznik = Parameter::getValue(Parameter::Nazwa::LICZNIK);
        Parameter::setValue(Parameter::Nazwa::LICZNIK,uint16_t(licznik + 1));
      }
      break;
    case  OTWIERA_SIE:
      stanBramy = ZATRZYMANA; // najpierw zatrzymac
      break;
    case  ZAMYKA_SIE:
      if (!mozna) stanBramy = ZATRZYMANA; // zamykac, a jesli nie mozna to zatrzymac i juz
      break;
    case  USZKODZONA: // poczekac, az uszkodzenie ustapi
      break;
    default:
      stanBramy = ZATRZYMANA;
      break;
    }
    check();
    fixOutputs();
    return stanBramy;
  }

  StanBramy zatrzymaj(){
    stanBramy = ZATRZYMANA;
    check();
    fixOutputs();
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

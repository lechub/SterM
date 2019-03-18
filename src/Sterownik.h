/*
 * Sterownik.h
 *
 *  Created on: 13.09.2016
 *      Author: lechu
 */

#ifndef STEROWNIK_H_
#define STEROWNIK_H_

//#include "Led.h"
#include <VEprom.h>
#include "Silnik230VAC.h"

#include "Silnik24VDC.h"
#include "Hamulec.h"
#include "Pinout.h"

class Sterownik {

private:

  //	Praca praca = Praca();

public:

  typedef enum {
    POSRODKU,   // miedzy dolem a gora
    OTWARTA,  // górna krańcówka włączona
    ZAMKNIETA,   // dolna krańcówka włączona
    USZKODZONA,   // stan niewłaściwy, np. obie krańcówki włączone
  }Pozycja;

  typedef enum {
    STOP,   // zatrzymany silnik
    OTWORZYC,   // silnik idzie do góry,
    ZAMKNAC,   // silnik idzie w dół,
  }Ruch;


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
  Pozycja pozycja = POSRODKU;
  Ruch ruch = STOP;

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
//    wewy->gpioOutOtwarte.setOutput(pozycja == OTWARTA);
//    wewy->gpioOutZamkniete.setOutput(pozycja == ZAMKNIETA);
    if (pozycja == USZKODZONA){
      awaria = true;
    }else{
      awaria = false;
    }

    bool awaria = isAwaria();
    getSilnik()->gotoSafePosition(awaria);
    hamulec->gotoSafePosition(awaria);

  }


  Pozycja checkPozycja(){
    if (isOtwarte() && isZamkniete()){
      pozycja = USZKODZONA;
    }else{
      if (isOtwarte()){
        pozycja = OTWARTA;
      }else if (isZamkniete()){
        pozycja = ZAMKNIETA;
      }else{
        pozycja = POSRODKU;
      }
    }
    fixOutputs();

    return pozycja;
  }

  void checkRuch(){
    checkPozycja();
    switch(ruch){
    case  Ruch::OTWORZYC:
      if (isOpenPossible()){
        hamulec->hamuj(false);
        getSilnik()->setMove(SilnikNapedu::MOVE::UP);
      }else{
        getSilnik()->setMove(SilnikNapedu::MOVE::HOLD_DOWN);
        hamulec->hamuj(true);
      }
      break;
    case  Ruch::ZAMKNAC:
      if (isClosePossible()){
        hamulec->hamuj(false);
        getSilnik()->setMove(SilnikNapedu::MOVE::DOWN);
      }else{
        getSilnik()->setMove(SilnikNapedu::MOVE::HOLD_DOWN);
        hamulec->hamuj(true);
      }
      break;
    case  Ruch::STOP:
    default:
      ruch = Ruch::STOP;
      getSilnik()->setMove(SilnikNapedu::MOVE::HOLD_DOWN);
      hamulec->hamuj(true);
      break;
    }
  }


  void checkRelays(){
    wewy->gpioOutOtwarte.setOutput(!isOtwarte());
    wewy->gpioOutZamkniete.setOutput(!isZamkniete());
    wewy->gpioOutRelSprawny.setOutput(!isAwaria()); // Awaria ma odwrocona logike
    wewy->gpioOutPozar.setOutput(wewy->gpioInPozar.getInput());
    bool alarmAkust = false;
    if (isPozar()) alarmAkust = true;
    if (isAlarmAkustyczny() && (!isZamkniete())) alarmAkust = true;
    wewy->gpioOutSygnAkust.setOutput(alarmAkust);
  }


  void initNaped(){
    switch(typNapedu){
    case VIC_012x:
      silnik24VDC->setType1234(false);
      hamulec->setMode(Hamulec::MODE::NORMALNIE , false);
      break;
    case VIC_042x:
      silnik24VDC->setType1234(false);
      hamulec->setMode(Hamulec::MODE::OFF, false);
      break;
    case VIC_0701:
      silnik24VDC->setType1234(false);
      hamulec->setMode(Hamulec::MODE::PRZECIWNIE, false);
      break;
    case VIC_010x:
      hamulec->setMode(Hamulec::MODE::OFF, false);
      silnik24VDC->setType1234(true);
      break;
    case VIC_040x:
      hamulec->setMode(Hamulec::MODE::OFF, false);
      silnik24VDC->setType1234(false);
      break;
    case NIEOKRESLONY:
    default:
      silnik24VDC->setType1234(false);
      hamulec->setMode(Hamulec::MODE::OFF, false); break;
    }
    //hamulec->init();
  }


public:
  Sterownik(Silnik24VDC * silnik24, Silnik230VAC * silnik230, Hamulec * hamulecSilnika){
    silnik24VDC = silnik24;
    silnik230VAC = silnik230;
    hamulec = hamulecSilnika;
  }

  bool init(){
    wewy = pins;
    silnik24VDC->init();
    silnik230VAC->init();
    hamulec->init();
    typNapedu = (NAPED)VEprom::readWord(VEprom::VirtAdres::NAPED);
    initNaped();
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

  inline bool isSoftStartUse() const{
    switch(getTypNapedu()){
    case VIC_0701: return true;
    case VIC_012x:
    case VIC_042x:
    case VIC_010x:
    case VIC_040x:
    case NIEOKRESLONY:
    default:    return false;
    }
  }

  inline bool isBrakeAtStop(){
    return getTypNapedu() == NAPED::VIC_012x;
  }

  inline NAPED getTypNapedu() const{ return typNapedu; }

  inline bool isOtwarte()const{ return wewy->gpioInKrancOtwarte.getInput(); }
  inline bool isZamkniete()const{ return wewy->gpioInKrancZamkniete.getInput(); }
  inline bool isZakazOtwierania()const{ return wewy->gpioInZakazOtwierania.getInput(); }
  inline bool isZakazZamykania()const{ return wewy->gpioInZakazZamykania.getInput(); }
  inline bool isPozar()const{ return !wewy->gpioInPozar.getInput(); }
  inline bool isAlarmAkustyczny()const{ return !(wewy->gpioInAlarmAkust.getInput()); }
  inline bool isPozarOrAlarmAkust()const{ return isPozar() || isAlarmAkustyczny(); }
  inline bool isRezerwa1()const{ return !(wewy->gpioInRezerwa1.getInput()); }
  inline bool isRezerwa2()const{ return !(wewy->gpioInRezerwa2.getInput()); }

  inline void setBuzzer(bool enable){ wewy->gpioOutBuzer.setOutput(enable); }

  inline bool isAwaria()const{ return awaria; }

  inline bool isOpenPossible(){
    return (      // mozna podniesc jesli
        (!isOtwarte())    // nie jest otwarte
        && (!isZakazOtwierania()) // nie ma zakazu otwierania
        && (!isPozarOrAlarmAkust())   // nie ma pozaru
    );
  }

  inline bool isClosePossible(){
    return (      // mozna podniesc jesli
        (!isZamkniete())    // nie jest zamkniete
        && (!isZakazZamykania()) // nie ma zakazu zamykania
    );
  }

  Ruch getRuch(){ return ruch; }

  bool isMotorOn(){
    SilnikNapedu::MOVE move = getSilnik()->getMove();
    return ((move == SilnikNapedu::MOVE::DOWN) || (move == SilnikNapedu::MOVE::UP));
  }

  void setAwaria(bool enable){ awaria = enable; }

  Pozycja podnies(){
    if (ruch != Ruch::OTWORZYC) VEprom::addToValue(VEprom::VirtAdres::LICZNIK, 1);
    ruch = Ruch::OTWORZYC;
    checkRuch();
    return pozycja;
  }

  Pozycja opusc(){
    if (ruch != Ruch::ZAMKNAC) VEprom::addToValue(VEprom::VirtAdres::LICZNIK, 1);
    ruch = Ruch::ZAMKNAC;
    checkRuch();
    return pozycja;
  }

  Pozycja zatrzymaj(){
    ruch = Ruch::STOP;
    checkRuch();
    return pozycja;
  }

  void poll(){
    checkRuch();
    checkRelays();
  }

  void setNaped(NAPED nowyTypNapedu){
    zatrzymaj();
    typNapedu = nowyTypNapedu;
    VEprom::writeWord(VEprom::VirtAdres::NAPED, typNapedu);
    initNaped();
  }


  static NAPED getPrevNaped(NAPED naped){
    uint8_t typ = (uint8_t)naped;
     if (--typ <= NAPED::NIEOKRESLONY){
       typ = int8_t(NAPED::VIC_0701);
     }
     return (NAPED)typ;
   }

   static NAPED getNextNaped(NAPED naped){
     uint8_t typ = (uint8_t)naped;
     if (++typ > NAPED::VIC_0701){
       typ = (int8_t)NAPED::VIC_012x;
     }
     return (NAPED)typ;
   }

  void setPrevNaped(){
    setNaped(getPrevNaped(getTypNapedu()));
  }

  void setNextNaped(){
    setNaped(getNextNaped(getTypNapedu()));
  }

  const char * getOpisNapedu()const{ return getOpisNapedu(typNapedu); }

  static const char * getOpisNapedu(NAPED naped){
    switch(naped){  //-----1234567890123456
    case VIC_012x: return "VIC-012x"; break;
    case VIC_0701: return "VIC-0701"; break;
    case VIC_042x: return "VIC-042x"; break;
    case VIC_010x: return "VIC-010x"; break;
    case VIC_040x: return "VIC-040x"; break;
    case NIEOKRESLONY:
    default:       return "NIEOKRESLONY"; break;
    }
  }

  bool gotoSafePosition(bool enable){
    silnik24VDC->gotoSafePosition(enable);
    silnik230VAC->gotoSafePosition(enable);
    hamulec->gotoSafePosition(enable);
    if (enable) setNaped(typNapedu);
    return true;
  }

};

extern Sterownik * sterM;

#endif /* STEROWNIK_H_ */

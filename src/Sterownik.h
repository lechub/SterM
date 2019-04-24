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
#include "Events.h"
#include "ConnectionZB40.h"

class Sterownik :public OVCdetector {

private:

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
  volatile bool inverterIsOn = false;

  volatile NAPED typNapedu = NIEOKRESLONY;
  Pozycja pozycja = POSRODKU;
  Ruch ruch = STOP;

  //bool awaria = false;


  SilnikNapedu * getSilnik() const{
    SilnikNapedu * sn;
    if (isTyp230VAC()){
      sn = silnik230VAC;
    }else{
      sn = silnik24VDC;
    }
    return sn;
  }

  void initOVC();


  void fixOutputs(){
    bool bladKrancowek = (pozycja == USZKODZONA);
    Events::setEvent(Events::Name::BladKrancowek, bladKrancowek);
    ovcIrqEnable(false);
    getSilnik()->gotoSafePosition(bladKrancowek);
    hamulec->gotoSafePosition(bladKrancowek);
    ovcIrqEnable(false);
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

  void makeSilnikMove(SilnikNapedu::MOVE move){
    ovcIrqEnable(false);
    getSilnik()->setMove(move);
    ovcIrqEnable(true);
  }

  void hamuj(bool hamowanie){
    ovcIrqEnable(false);
    hamulec->hamuj(hamowanie);
    ovcIrqEnable(true);
  }

  void checkRuch(){
    checkPozycja();
    switch(ruch){
    case  Ruch::OTWORZYC:
      if (isOpenPossible()){
        hamuj(false);
        makeSilnikMove(SilnikNapedu::MOVE::UP);
      }else{
        makeSilnikMove(SilnikNapedu::MOVE::HOLD_DOWN);
        hamulec->hamuj(true);
      }
      break;
    case  Ruch::ZAMKNAC:
      if (isClosePossible()){
        hamuj(false);
        makeSilnikMove(SilnikNapedu::MOVE::DOWN);
      }else{
        makeSilnikMove(SilnikNapedu::MOVE::HOLD_DOWN);
        hamuj(true);
      }
      break;
    case  Ruch::STOP:
    default:
      ruch = Ruch::STOP;
      makeSilnikMove(SilnikNapedu::MOVE::HOLD_DOWN);
      hamuj(true);
      break;
    }
  }


  void initNaped(){
    ovcIrqEnable(false);
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
    ovcIrqEnable(true);
  }


public:
  Sterownik(Silnik24VDC * silnik24, Silnik230VAC * silnik230, Hamulec * hamulecSilnika){
    silnik24VDC = silnik24;
    silnik230VAC = silnik230;
    hamulec = hamulecSilnika;
  }

  bool init(){
    wewy = pins;
    initOVC();
    silnik24VDC->init();
    silnik230VAC->init();
    hamulec->init();
    typNapedu = (NAPED)VEprom::readWord(VEprom::VirtAdres::NAPED);
    if (typNapedu == NAPED::NIEOKRESLONY) typNapedu = NAPED::VIC_040x;
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
  inline bool checkAlarm(){
    bool alPoz = wewy->gpioInPozar.getInput();
    Events::setEvent(Events::Name::AlarmPozarowy, alPoz);
    bool alAku = !(wewy->gpioInAlarmAkust.getInput());
    Events::setEvent(Events::Name::AlarmAkustyczny, alAku);
    return alPoz || alAku;
  }

  inline bool isPozarOrAlarmAkust(){ return checkAlarm(); }
  inline bool isRezerwa1()const{ return !(wewy->gpioInRezerwa1.getInput()); }
  inline bool isRezerwa2()const{ return !(wewy->gpioInRezerwa2.getInput()); }

  inline void setBuzzer(bool enable){ wewy->gpioOutBuzer.setOutput(enable); }

  inline bool isAwaria()const{
    return Events::isAwaria(Event::Priority::Awaria, true);
  }

  bool isAwariaSieci230VAC(){
    if (!zb40->isZB40InUse()){
      bool siecOK = wewy->gpioInSiec230VAC.getInput();
      Events::getEvent(Events::Name::Brak230VAC)->setActive(!siecOK);
    }
    return Events::getEvent(Events::Name::Brak230VAC)->isActive();
  }

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

  //void setAwaria(bool enable){ awaria = enable; }

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
    ovcCheck();
    checkRuch();

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


  void setInverter(bool enable){
    if (!inverterIsOn){ // wylaczyć detekcje przeciążenia przy starcie przetwornicy
      ovcIrqEnable(false);
    }
    wewy->gpioWlaczInwerter.setOutput(enable);
    wewy->gpioWlaczBypass.setOutput(enable);
    inverterIsOn = enable;
  }


  bool gotoSafePosition(bool enable){
    ovcIrqEnable(false);
    silnik24VDC->gotoSafePosition(enable);
    silnik230VAC->gotoSafePosition(enable);
    hamulec->gotoSafePosition(enable);
    if (enable) setNaped(typNapedu);
    ovcIrqEnable(true);
    return true;
  }

  // wylaczenie obciazenia dla usuniecia OVC
  virtual void ovcHold(bool state){
    if (state){
      makeSilnikMove(SilnikNapedu::MOVE::FLOAT);
      if (isTyp230VAC()) wewy->gpioWlaczInwerter.setOutput(false);
    }
    Events::setEvent(Events::Name::PrzeciazenieNapedu, state);

  }



  // dla zainicjowania innych akcji przy OVC
  virtual void ovcStart(){ };

  virtual void ovcIrqEnable(bool state){
    if (state) {
      NVIC_EnableIRQ(EXTI4_15_IRQn);
    }else{
      NVIC_DisableIRQ(EXTI4_15_IRQn);
    }
  }


};


extern Sterownik * sterM;

#ifdef __cplusplus
extern "C" {
#endif

void EXTI4_15_IRQHandler(void); // przerwanie od detekcji OVC

#ifdef __cplusplus
}
#endif

#endif /* STEROWNIK_H_ */

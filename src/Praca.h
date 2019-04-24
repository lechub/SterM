/*
 * Praca.h
 *
 *  Created on: 20 lis 2018
 *      Author: lechu
 */

#ifndef PRACA_H_
#define PRACA_H_

#include "Pinout.h"
#include "Sterownik.h"
#include "Front.h"
#include "HMI.h"


class Praca {
public:
  static constexpr uint32_t TIME_POLL_PERIOD_MS = 53; // odstep dla kolejki odpytywania
  static constexpr uint32_t TIME_MOTOR_DELAY_MS = 500;  // Opóźnienie włączenia silnika w stosunku do

  //  typedef enum {
  //    STANDBY,   // bez zasilania 24V
  //    TYLKO_HAMULEC,  // zasilanie hamulca dla VIC-012x
  //    PRZED_SILNIKIEM,   // opoźnienie dla silnika
  //    PRACA_SILNIKA,   // pracuje silnik
  //    PO_SILNIKU,    // opoźnienie po wyłączeniu silnika
  //  }State24V;

  typedef enum {
    STOI,   // zatrzymany silnik
    OTWIERANIE,   // silnik idzie do góry,
    ZAMYKANIE,   // silnik idzie w dół,
  }JOB;

private:
  Pinout * wewy = nullptr;

  //State24V state24v = STANDBY;
  JOB job = JOB::STOI;
  uint32_t motorDelayMs = 500;
  bool isOpenedOn230Fail = false;
  bool is230Fail = false;



public:

  void init(){
    wewy = pins;
    //sterM->init();
    sterM->gotoSafePosition(false);
  }

  //  bool isAwariaSieci230VAC(){
  //    return !wewy->gpioInSiec230VAC.getInput();
  //  }

  inline bool isOpenRequest(){
    if (!sterM->isOpenPossible()) return false;
    bool openInputs = (!wewy->gpioInOtworz.getInput())     // otworz sygnalem OTWORZ
                        || (!wewy->gpioInKluczII.getInput());   // otworz sygnalem Klucz_II
    bool openFront = (HMI::getInstance()->front->getState() == Front::State::UP);   // otworz z klawiatury na drzwiach
    if (openInputs) HMI::getInstance()->front->pushStop();

    return openInputs || openFront;
  }

  inline bool isCloseRequest(){
    if (!sterM->isClosePossible()) return false;
    bool closeInputs = (!wewy->gpioInKluczI.getInput());   // zamknij sygnalem KL_I
    bool closeEvent = (sterM->isPozarOrAlarmAkust()) || (Events::getEvent(Events::Name::NiskiStanAkumulatora)->isActive());
    bool closeFront = (HMI::getInstance()->front->getState() == Front::State::DOWN);   // zamknij
    if (closeInputs || closeEvent) HMI::getInstance()->front->pushStop();
    return   closeInputs || closeEvent || closeFront;          // zamknij sygnalem pozaru

  }

  void checkRelays(){
    //sterM->
    wewy->gpioOutOtwarte.setOutput(!sterM->isOtwarte());
    wewy->gpioOutZamkniete.setOutput(!sterM->isZamkniete());
    wewy->gpioOutRelSprawny.setOutput(!Events::isAwaria(Event::Priority::Usterka, true));
    bool alAku = Events::getEvent(Events::Name::AlarmAkustyczny)->isActive();
    bool alPoz = Events::getEvent(Events::Name::AlarmPozarowy)->isActive();
    wewy->gpioOutPozar.setOutput(alPoz);// alarm == Events::Alarm::AlarmPozarowy);
    bool sygnalAkust = alPoz;
    if ((alAku) && (!sterM->isZamkniete())) sygnalAkust = true;
    wewy->gpioOutSygnAkust.setOutput(sygnalAkust);
  }

  void poll(){
    sterM->poll();   // tutaj jest check

    // otwarcie, zamkniecie lub zatrzymanie

    bool moveRequest = ( isOpenRequest() || isCloseRequest());

    if (!sterM->isAwariaSieci230VAC()){ isOpenedOn230Fail = false; }   // reset licznika otwarc dla awarii sieci

    bool inverterInUse = sterM->isTyp230VAC() && sterM->isAwariaSieci230VAC() && moveRequest;
    sterM->setInverter(inverterInUse);

    bool turnBuzzerOn = false;

    if (moveRequest){
      motorDelayMs = (motorDelayMs < TIME_POLL_PERIOD_MS) ? 0 : motorDelayMs - TIME_POLL_PERIOD_MS;
      if (motorDelayMs == 0){
        if(isCloseRequest()){
          if (job != JOB::ZAMYKANIE){
            job = JOB::ZAMYKANIE;
            motorDelayMs = (sterM->isPozarOrAlarmAkust()) ?                     // opoznienie wlaczenia napedu
                VEprom::readWord(VEprom::VirtAdres::OPOZN_ZAMK_POZAR) * 1000ul + TIME_MOTOR_DELAY_MS:  // jesli pozar to opoznienie pozarowe
                TIME_MOTOR_DELAY_MS;        // bez pozaru, to zwykle opoznienie 0.5 s
            sterM->zatrzymaj();
          }else{
//            wewy->gpioWlaczInwerter.setOutput(inverterInUse);
//            wewy->gpioWlaczBypass.setOutput(inverterInUse);
            sterM->opusc();
            turnBuzzerOn = true;
          }
        }else if(isOpenRequest()){
          if (job != JOB::OTWIERANIE){    // nalezy otwierac
            if (sterM->isAwariaSieci230VAC()){   // gdy brak sieci to tylko 1 raz
              if (isOpenedOn230Fail){
                job = JOB::STOI;
              }else{
                isOpenedOn230Fail = true;
                job = JOB::OTWIERANIE;
              }
            }else{
              job = JOB::OTWIERANIE;
            }
            motorDelayMs = TIME_MOTOR_DELAY_MS;
            sterM->zatrzymaj();
          }else{
//            wewy->gpioWlaczInwerter.setOutput(inverterInUse);
//            wewy->gpioWlaczBypass.setOutput(inverterInUse);
            sterM->podnies();
            turnBuzzerOn = true;
          }
        }else{
          job = JOB::STOI;
          sterM->zatrzymaj();
          motorDelayMs = TIME_MOTOR_DELAY_MS;
        }
      }
    }else{
      job = JOB::STOI;
      sterM->zatrzymaj();
      motorDelayMs = TIME_MOTOR_DELAY_MS;
    }
    constexpr uint32_t maxDelay = TIME_MOTOR_DELAY_MS + MAX_OPOZN_ZAMK_POZ_sek * 1000;
    if (motorDelayMs > (maxDelay)) motorDelayMs = maxDelay;

    sterM->setBuzzer(turnBuzzerOn);

    checkLEDs();
    checkRelays();

  }

  void inline checkLEDs(){
    // mruganie diodą pozar
    bool alPoz = Events::getEvent(Events::Name::AlarmPozarowy)->isActive();//::Alarm alarm = sterM->getAlarm();
    bool alAkus = Events::getEvent(Events::Name::AlarmAkustyczny)->isActive();//::Alarm alarm = sterM->getAlarm();
    if (alPoz){
      wewy->ledPozar.set(Led::Mode::MRUGA_FAST);
    }else if(alAkus){
      wewy->ledPozar.set(Led::Mode::MRUGA_SLOW);
    }else{
      wewy->ledPozar.set(Led::Mode::ZGASZONA);
    }

    // mruganie diodą awarii
    if (Events::isAwaria(Event::Priority::Awaria, true)){
      wewy->ledAwaria.set(Led::Mode::MRUGA_FAST); // sygnalizacja na LED-ie
    }else if (Events::isAwaria(Event::Priority::Usterka, true)){
      wewy->ledAwaria.set(Led::Mode::PULSUJE);
    }else{
      wewy->ledAwaria.set(Led::Mode::ZGASZONA);
    }

    // mruganie diodą gotowosc
    if (sterM->isAwariaSieci230VAC()){
      wewy->ledGotowosc.set(Led::Mode::PULSUJE); // sygnalizacja na LED-ie
    }else if (sterM->isMotorOn()){
      wewy->ledGotowosc.set(Led::Mode::MRUGA_SLOW);
    }else{
      wewy->ledGotowosc.set(Led::Mode::SWIECI);
    }
  }

  inline JOB getJob()const { return job; }

};

extern Praca *praca;

#endif /* PRACA_H_ */



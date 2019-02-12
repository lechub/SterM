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



public:

  void init(){
    wewy = &pins;
    sterM->init();
    sterM->gotoSafePosition(false);
  }

  bool isAwariaSieci230VAC(){
    return !wewy->gpioInSiec230VAC.getInput();
  }

  inline bool isOpenRequest(){
    if (!sterM->isOpenPossible()) return false;
    return (!wewy->gpioInOtworz.getInput())     // otworz sygnalem OTWORZ
        || (!wewy->gpioInKluczII.getInput());   // otworz sygnalem Klucz_II
  }

  inline bool isCloseRequest(){
    if (!sterM->isClosePossible()) return false;
    return (!wewy->gpioInKluczI.getInput())   // zamknij sygnalem KL_I
        || (sterM->isPozar());               // zamknij sygnalem pozaru
  }



  void poll(){
    sterM->poll();   // tutaj jest check



    // otwarcie, zamkniecie lub zatrzymanie

    bool zas24Necessary = ( isOpenRequest() || isCloseRequest() || sterM->isBrakeAtStop()  );

   // wewy->gpioWlaczZasNaped.setOutput(zas24Necessary);
    wewy->gpioWlaczInwerter.setOutput(zas24Necessary && sterM->isTyp230VAC() && isAwariaSieci230VAC());

    bool turnBuzzerOn = false;

    if (zas24Necessary){
      motorDelayMs = (motorDelayMs < TIME_POLL_PERIOD_MS) ? 0 : motorDelayMs - TIME_POLL_PERIOD_MS;
      if (motorDelayMs == 0){
        if(isCloseRequest()){
          if (job != JOB::ZAMYKANIE){
            job = JOB::ZAMYKANIE;
            motorDelayMs = TIME_MOTOR_DELAY_MS;
            sterM->zatrzymaj();
          }else{
            sterM->opusc();
            turnBuzzerOn = true;
          }
        }else if(isOpenRequest()){
          if (job != JOB::OTWIERANIE){
            job = JOB::OTWIERANIE;
            motorDelayMs = TIME_MOTOR_DELAY_MS;
            sterM->zatrzymaj();
          }else{
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
    if (motorDelayMs > TIME_MOTOR_DELAY_MS) motorDelayMs = TIME_MOTOR_DELAY_MS;

    sterM->setBuzzer(turnBuzzerOn);

    //    // włączanie przekaźnika od akumulatora i inwertera
    //    if (sterM->isMotorOn() || sterM->isBrakeAtStop()){
    //      wewy->gpioWlaczZasNaped.setOutputUp();    // jesli ruszamy brama, to trzeba napiecia
    //
    //      // inwerter wlaczamy tylko gdy awaria sieci 230VAC
    //      wewy->gpioWlaczInwerter.setOutput(sterM->isTyp230VAC() && isAwariaSieci230VAC());
    //    }else{
    //      wewy->gpioWlaczZasNaped.setOutputDown();
    //      wewy->gpioWlaczInwerter.setOutputDown();
    //    }
    //
    //  // otwarcie, zamkniecie lub zatrzymanie
    //
    //    if ((!wewy->gpioInOtworz.getInput()) || (!wewy->gpioInKluczII.getInput())){  // otworz sygnalem OTWORZ i Klucz_II
    //      sterM->podnies();
    //    }else  if ((!wewy->gpioInKluczI.getInput()) || (sterM->isPozar())){  // zamknij sygnalem KL_I lub pozarem
    //      sterM->opusc();
    //    }else{
    //      sterM->zatrzymaj();
    //    }
    //

    // propagacja sygnału pożarowego
    wewy->gpioOutPozar.setOutput(sterM->isPozar());

    // sygnalizator akustyczny dziala gdy jest pozar i nie ma sygnalu alarmAkustyczny
    wewy->gpioOutSygnAkust.setOutput((!sterM->isAlarmAkustyczny())&&(sterM->isPozar()));



    // mruganie diodą awaria
    if (sterM->isAwaria()){
      wewy->ledAwaria.set(Led::Mode::MRUGA_FAST);
    }else {
      wewy->ledAwaria.set(Led::Mode::ZGASZONA);
    }

    // mruganie diodą pracy buforowej
    if (isAwariaSieci230VAC()){
      wewy->ledPracaAku.set(Led::Mode::MRUGA_SLOW); // sygnalizacja na LED-ie
    }else{
      wewy->ledPracaAku.set(Led::Mode::ZGASZONA);
    }

  }

  inline JOB getJob()const { return job; }

};

extern Praca *praca;

#endif /* PRACA_H_ */

//  if (zas24Necessary){
//      motorDelayMs = (motorDelayMs < TIME_POLL_PERIOD_MS) ? 0 : motorDelayMs - TIME_POLL_PERIOD_MS;
//      if (motorDelayMs == 0){
//        switch(ruch){
//        case Sterownik::Ruch::ZAMKNAC:
//          if(isCloseRequest()){
//            sterM->opusc();
//            turnBuzzerOn = true;
//          }else{
//            motorDelayMs = TIME_MOTOR_DELAY_MS;
//            ruch = Sterownik::Ruch::STOP;
//          }
//          break;
//        case Sterownik::Ruch::OTWORZYC:
//          if(isOpenRequest()){
//            sterM->podnies();
//            turnBuzzerOn = true;
//          }else{
//            motorDelayMs = TIME_MOTOR_DELAY_MS;
//            ruch = Sterownik::Ruch::STOP;
//          }
//          break;
//        case Sterownik::Ruch::STOP:
//        default:
//          if (isOpenRequest()) ruch = Sterownik::Ruch::OTWORZYC;
//          else if (isCloseRequest()) ruch = Sterownik::Ruch::ZAMKNAC;
//          else{
//            motorDelayMs = TIME_MOTOR_DELAY_MS;
//            ruch = Sterownik::Ruch::STOP;
//          }
//          break;
//        }
//      }else{
//        ruch = Sterownik::Ruch::STOP;
//      }
//    }else{
//      motorDelayMs = TIME_MOTOR_DELAY_MS;
//      ruch = Sterownik::Ruch::STOP;
//    }
//    if (motorDelayMs > TIME_MOTOR_DELAY_MS) motorDelayMs = TIME_MOTOR_DELAY_MS;
//    if (ruch == Sterownik::Ruch::STOP) sterM->zatrzymaj();
//
//    sterM->setBuzzer(turnBuzzerOn);
//


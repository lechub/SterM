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
  static constexpr uint32_t TIME_POLL_PERIOD_MS = 53;



private:
  Pinout * wewy = nullptr;


  bool isAwariaSieci230VAC(){
    return !wewy->gpioInSiec230VAC.getInput();
  }

public:

  void init(){
    wewy = &pins;
  }



  void poll(){
    sterM->poll();   // tutaj jest check

    // włączanie przekaźnika od akumulatora i inwertera
    if (sterM->isMotorOn()){
      wewy->gpioWlaczZasNaped.setOutputUp();    // jesli ruszamy brama, to trzeba napiecia
      if (sterM->isTyp230VAC() && isAwariaSieci230VAC()){ // inwerter wlaczamy tylko gdy awaria sieci 230VAC
        wewy->gpioWlaczInwerter.setOutputUp();
      }
    }else{
      wewy->gpioWlaczZasNaped.setOutputDown();
      wewy->gpioWlaczInwerter.setOutputDown();
    }

    if (isAwariaSieci230VAC()){   // obsluga bypassa, inwertera i/lub akumulatora
      wewy->ledPracaAku.set(Led::Mode::MRUGA_SLOW); // sygnalizacja na LED-ie
    }else{
      wewy->ledPracaAku.set(Led::Mode::ZGASZONA);
    }

    wewy->gpioOutPozar.setOutput(sterM->isPozar());
    // sygnalizator akustyczny dziala gdy jest pozar i nie ma sygnalu alarmAkustyczny
    wewy->gpioOutSygnAkust.setOutput((!sterM->isAlarmAkustyczny())&&(sterM->isPozar()));

   // Sterownik::Ruch ruch = sterM->getRuch();
    if ((!wewy->gpioInOtworz.getInput()) || (!wewy->gpioInKluczII.getInput())){  // otworz sygnalem OTWORZ
      sterM->podnies();
      //ruch = Sterownik::Ruch::OTWIERA_SIE;
    }else  if ((!wewy->gpioInKluczI.getInput()) || (sterM->isPozar())){  // zamknij sygnalem KL_I lub pozarem
      sterM->opusc();
      //ruch = Sterownik::Ruch::ZAMYKA_SIE;
    }else{
      sterM->zatrzymaj();
    }

//    if ((!wewy->gpioInOtworz.getInput()) || (!wewy->gpioInKluczII.getInput())){  // otworz sygnalem OTWORZ
//      sterM->podnies();
//    }else{
//      if (sterM->getRuch() == Sterownik::Ruch::OTWIERA_SIE){
//        sterM->zatrzymaj();
//      }
//    }

//    if ((!wewy->gpioInKluczI.getInput()) || (sterM->isPozar())){  // zamknij sygnalem KL_I lub pozarem
//      sterM->opusc();
//    }else{
//      if (sterM->getRuch() == Sterownik::Ruch::ZAMYKA_SIE){
//        sterM->zatrzymaj();
//      }
//    }

    if (sterM->isAwaria()){
      wewy->ledAwaria.set(Led::Mode::MRUGA_FAST);
    }else {
      wewy->ledAwaria.set(Led::Mode::ZGASZONA);
    }


  }



  /**
   *
 // wyjscia
  wewy->gpioOutPozar;
  wewy->gpioOutZamkniete;
  wewy->gpioOutOtwarte;
  wewy->gpioOutSygnAku;
  wewy->gpioOutZakazZamykania;
  wewy->gpioOutBuzer;
  wewy->gpioWlaczBypass;
  wewy->gpioWlacz230Zamknij;
  wewy->gpioWlacz230Otworz;
  wewy->gpioWlaczInwerter;
  wewy->gpioWlaczZasNaped;
  wewy->ledAlarm;
  wewy->ledPracaAku;

  // wejscia
  wewy->gpioInBtnBACK;
  wewy->gpioInBtnLEFT;
  wewy->gpioInBtnRIGHT;
  wewy->gpioInBtnENTER;
  wewy->gpioInKrancZamkniete;
  wewy->gpioInKrancOtwarte;
  wewy->gpioInZakazOtwierania;
  wewy->gpioInOtworz;
  wewy->gpioInKluczI;
  wewy->gpioInKluczII;
  wewy->gpioInPozarIn;
  wewy->gpioInAlarmAku;
  wewy->gpioInSiec230VAC;
  wewy->gpioInRezerwa1;
  wewy->gpioInRezerwa2;

  wewy->gpioLcdReset;
  wewy->gpioLcdBackLight;

   * */
};

extern Praca *praca;

#endif /* PRACA_H_ */



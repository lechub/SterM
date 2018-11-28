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
    Sterownik::StanBramy brama = sterM->getStanBramy();

    if (isAwariaSieci230VAC()){   // obsluga bypassa, inwertera i/lub akumulatora
      wewy->ledPracaAku.set(Led::Mode::MRUGA_SLOW); // sygnalizacja na LED-ie
      switch(brama){
      case  Sterownik::StanBramy::OTWIERA_SIE:
      case  Sterownik::StanBramy::ZAMYKA_SIE:
        wewy->gpioWlaczZasNaped.setOutputUp();
        if (sterM->isTyp230VAC()){
          wewy->gpioWlaczInwerter.setOutputUp();
        }
        break;
      default:
        wewy->gpioWlaczZasNaped.setOutputDown();
        wewy->gpioWlaczInwerter.setOutputDown();
        break;
      }
    }else{
      wewy->ledPracaAku.set(Led::Mode::PULSUJE); // sygnalizacja na LED-ie
      wewy->gpioWlaczZasNaped.setOutputDown();
      wewy->gpioWlaczInwerter.setOutputDown();
    }


      wewy->gpioOutPozar.setOutput(sterM->isPozar());
      // sygnalizator akustyczny dziala gdy jest pozar i nie ma sygnalu alarmAkustyczny
      wewy->gpioOutSygnAkust.setOutput((!sterM->isAlarmAkustyczny())&&(sterM->isPozar()));

    if (!wewy->gpioInOtworz.getInput()){  // otworz sygnalem OTWORZ
      sterM->podnies();
    }

    if (!wewy->gpioInKluczII.getInput()){  // otworz sygnalem KL_II
      sterM->podnies();
    }

    if (!wewy->gpioInKluczI.getInput()){  // zamknij sygnalem KL_I
      sterM->opusc();
    }

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



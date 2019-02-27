/*
 * Menu.h
 *
 *  Created on: 04.07.2017
 *      Author: lechu
 */

#ifndef MENU_H_
#define MENU_H_

#include <VEprom.h>
#include "Keyboard.h"
#include "FrameBuffer.h"
#include "Sterownik.h"
#include "Password.h"
#include "Settings.h"
//#include "HMI.h"

class Menu {


public:

  static constexpr char CHAR_IN_ACTIVE = '*';
  static constexpr char CHAR_IN_NOACTIVE = '-';

  /*
   * e_START
   * |
   * e_MAIN
   * |-# e_HASLO
   * |   |-[12345]
   * |      |-<> e_UST_NAPED <> e_RESET_LICZNIKA   <>  e_UST_ZEGAR   <> e_UST_HASLO
   * |-<> e_WYJSCIA  <> e_WEJSCIA
   * */


  typedef enum{
    e_INIT,
    e_MAIN,
    e_WEJSCIA,
    e_WYJSCIA,
    e_HASLO,
    e_uNAPED,
    e_uNAPED_USTAW,
    e_uRESET_LICZNIKA,
    e_uZEGAR,
    e_uZEGAR_USTAW,
    e_uNOWE_HASLO,
    e_uHASLO_USTAW,
  }EKRAN;



  //HMI * hmi;

private:

  static constexpr uint32_t REFRESH_DELAY_MS = 338;

  Sterownik * ster;
  Keyboard * keys;
  FrameBuffer * lcd;
  //Praca * praca;
  Password pass;
  Sterownik::NAPED napedM = Sterownik::NAPED::NIEOKRESLONY;

  uint32_t refreshDelay;

  EKRAN ekran;
  //Parameter::Nazwa editParam = Parameter::Nazwa::NONE;
  uint16_t editValue = 0;
  bool editMode = false;

public:

  void init(Sterownik * pSter, Keyboard * pKeys, FrameBuffer * pLcd){
    ster = pSter;
    keys = pKeys;
    lcd = pLcd;
    //praca = ster->getPraca();
    pass.init();

    ekran = EKRAN::e_INIT;
    //goToEkran(EKRAN::e_AUTOMAT);
  }


  void goToEkran(EKRAN nowyEkran){
    ekran = nowyEkran;
    if (ekran == EKRAN::e_INIT) sterM->gotoSafePosition(false);
    if (ekran == EKRAN::e_uNAPED) sterM->gotoSafePosition(true);
    editMode = false;

    showEkran();
  }

  //	void goToEdit(Parameter::Nazwa param){
  //		editParam = param;
  //		editValue = Parameter::getValue(editParam);
  //		editMode = true;
  //		showEkran(editValue);
  //	}

  void printPattern(const char * pattern, uint32_t value);


  bool processHaslo(Keyboard::Key key);


//  // tylko wyswietlenie - w trybie przegladania
//  void showEkran(){
//    editMode = false;
//    uint16_t val = 1;
//    switch(ekran){
//    case e_START:
//      val = VEprom::readWord(VEprom::VirtAdres::NAPED);
//      break;
//    case e_1_NAPED:
//      break;
//    case e_1_NAPED_WYBOR:
//      break;
//    default:
//      break;
//    }
//
//    showEkran(val);
//  }

  bool edit(Keyboard::Key key);

  void showEkran();

  void poll();


};


#endif /* MENU_H_ */

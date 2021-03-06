/*
 * Menu.h
 *
 *  Created on: 04.07.2017
 *      Author: lechu
 */

#ifndef MENU_H_
#define MENU_H_

#include "Keyboard.h"
#include "FrameBuffer.h"
#include "Sterownik.h"
//#include ""
//#include "HMI.h"
#include "Parameter.h"

class Menu {

public:
  typedef enum{
    e_START,
    e_1_NAPED,
    e_1_NAPED_WYBOR,

  }EKRAN;



  //HMI * hmi;

private:

  static constexpr uint32_t REFRESH_DELAY_MS = 338;

  Sterownik * ster;
  Keyboard * keys;
  FrameBuffer * lcd;
  //Praca * praca;

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

    ekran = EKRAN::e_START;
    //goToEkran(EKRAN::e_AUTOMAT);
  }


  void goToEkran(EKRAN nowyEkran){
    ekran = nowyEkran;
    if (ekran == EKRAN::e_START) sterM->gotoSafePosition(false);
    if (ekran == EKRAN::e_1_NAPED_WYBOR) sterM->gotoSafePosition(true);
    editMode = false;

    //		switch(ekran){
    //		case e_AUTOMAT:				 break;
    //
    //		default:
    // break;
    //		}
    showEkran();
  }

  //	void goToEdit(Parameter::Nazwa param){
  //		editParam = param;
  //		editValue = Parameter::getValue(editParam);
  //		editMode = true;
  //		showEkran(editValue);
  //	}

  void printPattern(const char * pattern, uint32_t value);


  // tylko wyswietlenie - w trybie przegladania
  void showEkran(){
    editMode = false;
    uint16_t val = 1;
    switch(ekran){
    case e_START:
      val = (uint16_t)Parameter::getValue(Parameter::Nazwa::LICZNIK);
      break;
    case e_1_NAPED:
      break;
    case e_1_NAPED_WYBOR:
      break;
    default:
      break;
    }

    showEkran(val);
  }

  bool edit(Keyboard::Key key);

  void showEkran(uint16_t val);

  void poll();


};


#endif /* MENU_H_ */

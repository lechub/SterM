/*
 * Menu.cpp
 *
 *  Created on: 18 paź 2017
 *      Author: lechu
 */

#include "Menu.h"

#include <ctype.h>
#include <VEprom.h>
#include "Keyboard.h"
#include "FrameBuffer.h"
#include "HMI.h"
#include "Praca.h"

void Menu::poll(){

  Keyboard::Key key = keys->getKey();

  switch(ekran){
  case e_START:
  {
    switch(key){
    case Keyboard::Key::ENTER: goToEkran(EKRAN::e_1_NAPED_WYBOR);  break;
    case Keyboard::Key::ENTER_CANCEL:  // zerowanie licznika
      VEprom::writeWord(VEprom::VirtAdres::LICZNIK, 0);
      break;
    case Keyboard::Key::LEFT:
      VEprom::subtractFromValue(VEprom::VirtAdres::LICZNIK, 1);
      break;
    case Keyboard::Key::RIGHT:
      VEprom::addToValue(VEprom::VirtAdres::LICZNIK, 1);
      break;
    default:
      break;
    }
    break;
  }
  case e_1_NAPED:
  {
    //Sterownik::NAPED naped = sterM->getTypNapedu();
    switch(key){
    case Keyboard::Key::ENTER: goToEkran(EKRAN::e_1_NAPED_WYBOR);  break;
    case Keyboard::Key::CANCEL: goToEkran(EKRAN::e_START);  break;
    case Keyboard::Key::LEFT:
    case Keyboard::Key::RIGHT:
    default:
      break;
    }
    break;
  }
  case e_1_NAPED_WYBOR:
  {
    switch(key){
    case Keyboard::Key::ENTER:
    case Keyboard::Key::CANCEL:
      goToEkran(EKRAN::e_START);
      break;
    case Keyboard::Key::LEFT:  sterM->setPrevNaped(); break;
    case Keyboard::Key::RIGHT:  sterM->setNextNaped(); break;
    default:
      break;
    }
    break;
  }

  break;
  default: break;
  }

  showEkran();

}

// switch(ekran){
//  case e_START:
//  {
//    refreshDelay += HMI::TIME_PERIOD_MONITOR_MS;
//    if (refreshDelay > REFRESH_DELAY_MS){
//      showEkran((uint16_t)Parameter::getValue(Parameter::Nazwa::LICZNIK));
//      refreshDelay = 0;
//    }
//    return;
//    //goToEkran(EKRAN::e_START);
//  }
//  default: break;
//  }

void Menu::printPattern(const char * pattern, uint32_t value){
  lcd->printNumbersWithPattern(pattern, value);
  if (editMode){
    const char * ptr = pattern;
    while (*ptr != '\0'){ptr++;}
    char znak;
    uint16_t offset = 0;
    do{
      znak = char(*ptr);
      if (isdigit(znak)) break;
      ptr--;
      offset++;
    }while (ptr >= pattern);
    lcd->cursorLeft(offset);
    lcd->cursorMode(FrameBuffer::CursorMode::BLINK);
  }else{
    lcd->cursorMode(FrameBuffer::CursorMode::HIDDEN);
  }
}


static inline char getJobSymbol(){
  Praca::JOB job = praca->getJob();
  switch(job){
  case Praca::JOB::OTWIERANIE: return 'O';
  case Praca::JOB::ZAMYKANIE:  return 'Z';
  case Praca::JOB::STOI:
  default:  return '-';
  }
}


void Menu::showEkran(uint16_t val){
  //Parameter::Nazwa param = Parameter::Nazwa::NONE;

  switch(ekran){
  case e_START:///------->1234567890123456<
  {
    lcd->gotoXY(0,0);
    //--------------------------->1234567890123456<
    lcd->printNumbersWithPattern("LICZNIK: 000000 ", val);
//    //----------0123456789012345
//    lcd->printXY(0,1, "    MARTECH     ");
    lcd->clearLine(1);
    //--------------->1234567890123456<
    lcd->printXY(0, 1, "[");
    lcd->print( sterM->getOpisNapedu());
    lcd->print("]");
    lcd->gotoXY(11,1);
    lcd->print(getJobSymbol());
    lcd->print(sterM->isPozar() ? 'P' : '-');
    lcd->print(sterM->isAlarmAkustyczny() ? 'S' : '-');
    lcd->print(sterM->isAwaria() ? 'A' : '-');
    lcd->print(praca->isAwariaSieci230VAC() ? 'V' : '-');
    break;
  }
  case e_1_NAPED:{
    //--------------->1234567890123456<
    lcd->printXY(0,0," <Wybor napedu>  " );
    Sterownik::NAPED npd = sterM->getTypNapedu();
    lcd->clearLine(1);
    //--------------->1234567890123456<
    lcd->printXY(0, 1, sterM->getOpisNapedu(npd) );
    break;
  }
  case e_1_NAPED_WYBOR:
  {
    //--------------->1234567890123456<
    lcd->printXY(0,0," Wybierz naped: " );
    Sterownik::NAPED npd = sterM->getTypNapedu();
    lcd->clearLine(1);
    //--------------->1234567890123456<
    lcd->printXY(0, 1, "<");
    lcd->print( sterM->getOpisNapedu(npd));
    lcd->print(">");
    break;
  }
  case e_WEJSCIA:
  {
    //--------------->|****|----|*-*-|<
    //--------------->1234567890123456<
    lcd->gotoXY(0,0);
    sterM->wewy->gpioInKrancZamkniete.getInput() ?
        lcd->print("-") : lcd->print("*");
    sterM->wewy->gpioInKrancOtwarte.getInput() ?
        lcd->print("-") : lcd->print("*");
    sterM->wewy->gpioInZakazZamykania.getInput() ?
        lcd->print("-") : lcd->print("*");
    sterM->wewy->gpioInZakazOtwierania.getInput() ?
        lcd->print("-") : lcd->print("*");
    sterM->wewy->gpioInKluczI.getInput() ?
        lcd->print("-") : lcd->print("*");
    sterM->wewy->gpioInKluczII.getInput() ?
        lcd->print("-") : lcd->print("*");
    sterM->wewy->gpioInPozar.getInput() ?
        lcd->print("-") : lcd->print("*");

    lcd->print(" ");  // przerwa

    sterM->wewy->gpioInKrancZamkniete.getInput() ?
        lcd->print("_") : lcd->print("*");
    sterM->wewy->gpioInKrancOtwarte.getInput() ?
        lcd->print("_") : lcd->print("*");
    sterM->wewy->gpioInZakazZamykania.getInput() ?
        lcd->print("_") : lcd->print("*");
    sterM->wewy->gpioInZakazOtwierania.getInput() ?
        lcd->print("_") : lcd->print("*");
    sterM->wewy->gpioInKluczI.getInput() ?
        lcd->print("_") : lcd->print("*");
    sterM->wewy->gpioInKluczII.getInput() ?
        lcd->print("_") : lcd->print("*");
    sterM->wewy->gpioInPozar.getInput() ?
        lcd->print("_") : lcd->print("*");

    lcd->clearLine(1);
    //--------------->1234567890123456<
    lcd->printXY(0, 1, "<");
    lcd->print( sterM->getOpisNapedu(npd));
    lcd->print(">");
    break;
  }
  ,
     e_WYJSCIA,
  break;
  default:
    break;
  }
}

bool Menu::edit(Keyboard::Key key){
  return true;
}


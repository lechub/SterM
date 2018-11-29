/*
 * Menu.cpp
 *
 *  Created on: 18 paź 2017
 *      Author: lechu
 */

#include "Menu.h"

#include <ctype.h>
#include "Keyboard.h"
#include "FrameBuffer.h"
#include "HMI.h"
#include "Parameter.h"

void Menu::poll(){

  Keyboard::Key key = keys->getKey();

  switch(ekran){
  case e_START:
  {
    switch(key){
    case Keyboard::Key::ENTER: ekran = EKRAN::e_1_NAPED;  break;
    case Keyboard::Key::ENTER_CANCEL:  // zerowanie licznika
      Parameter::setValue(Parameter::Nazwa::LICZNIK, 0);
      break;
    case Keyboard::Key::LEFT:
      Parameter::setValue(Parameter::Nazwa::LICZNIK, (uint16_t)(Parameter::getValue(Parameter::Nazwa::LICZNIK)-1));
      break;
    case Keyboard::Key::RIGHT:
      Parameter::setValue(Parameter::Nazwa::LICZNIK, (uint16_t)(Parameter::getValue(Parameter::Nazwa::LICZNIK)+1));
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
    case Keyboard::Key::ENTER: ekran = EKRAN::e_1_NAPED_WYBOR;  break;
    case Keyboard::Key::CANCEL: ekran = EKRAN::e_START;  break;
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
      ekran = EKRAN::e_1_NAPED;
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


void Menu::showEkran(uint16_t val){
  //Parameter::Nazwa param = Parameter::Nazwa::NONE;

  switch(ekran){
  case e_START:///------->1234567890123456<
  {
    lcd->gotoXY(0,0);
    //--------------------------->1234567890123456<
    lcd->printNumbersWithPattern("LICZNIK: 000000 ", val);
    lcd->gotoXY(0,1);
    //----------0123456789012345
    lcd->print("    MARTECH     ");
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
  break;
  default:
    break;
  }
}

bool Menu::edit(Keyboard::Key key){
  return true;
}


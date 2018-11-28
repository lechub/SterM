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
  switch(key){
  case Keyboard::Key::ENTER:
    Parameter::setValue(Parameter::Nazwa::LICZNIK, (uint16_t)(QuickTask::getCounter() % 1000));
    break;
  case Keyboard::Key::CANCEL:
    Parameter::setValue(Parameter::Nazwa::LICZNIK, 0);
    break;
  case Keyboard::Key::LEFT:
    Parameter::setValue(Parameter::Nazwa::LICZNIK, (uint16_t)(Parameter::getValue(Parameter::Nazwa::LICZNIK)-1));
    break;
  case Keyboard::Key::RIGHT:
    Parameter::setValue(Parameter::Nazwa::LICZNIK, (uint16_t)(Parameter::getValue(Parameter::Nazwa::LICZNIK)+1));
    break;
  case Keyboard::Key::NONE:
  default:
    break;
  }


  //  if (editMode){
  //    edit(key);
  //    return;
  //  }

  switch(ekran){
  case e_AUTOMAT:
  {
    {
      refreshDelay += HMI::TIME_PERIOD_MONITOR_MS;
      if (refreshDelay > REFRESH_DELAY_MS){
        showEkran((uint16_t)Parameter::getValue(Parameter::Nazwa::LICZNIK));
        refreshDelay = 0;
      }
      return;
    }
    //goToEkran(EKRAN::e_AUTOMAT);
  }
  default: break;
  }

}




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
  case e_AUTOMAT:///------->1234567890123456<
  {

    lcd->gotoXY(0,0);
    //--------------------------->1234567890123456<
    lcd->printNumbersWithPattern("LICZNIK: 000000 ", val);
    lcd->gotoXY(0,1);
    //----------0123456789012345
    lcd->print("    MARTECH     ");
  }break;

  }
}

bool Menu::edit(Keyboard::Key key){
  return true;
}


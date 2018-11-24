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
//#include "Parameter.h"

void Menu::poll(){
  Keyboard::Key key = keys->getKey();
  if (key == Keyboard::Key::NONE){
    refreshDelay += HMI::TIME_PERIOD_MONITOR_MS;
    if (refreshDelay > REFRESH_DELAY_MS){
      uint16_t val = 1;// Parameter::getValue(editParam);
      showEkran(val);
      refreshDelay = 0;
    }
    return;
  }
  refreshDelay = 0;



  if (editMode){
    edit(key);
    return;
  }

  // odrzucanie CANCEL z impulsatora  -zeby nie wychodzil od razu z opcji menu
  static bool enterToMem = false;
  if ((enterToMem)&&(key == Keyboard::Key::CANCEL)){
    enterToMem = false;
    return;
  }
  enterToMem = (key == Keyboard::Key::ENTER);

  switch(ekran){
  case e_AUTOMAT:
  {
    switch (key){
    case Keyboard::Key::ENTER:	break;
    case Keyboard::Key::RIGHT: break;
    case Keyboard::Key::LEFT: 	 break;
    case Keyboard::Key::CANCEL:
    default:					goToEkran(EKRAN::e_AUTOMAT); break;
    }
  }break;

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
    lcd->printNumbersWithPattern("hbjkbhjkC",77);
    lcd->gotoXY(1,0);
    //printPattern(patt,val);
  }break;

  }
}

bool Menu::edit(Keyboard::Key key){
 return true;
}


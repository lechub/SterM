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

  // przywracanie ekranu glownego jesli klawiatura jest nieuzywana
  if ((key == Keyboard::Key::NONE)&&
      (!HMI::getInstance()->isBackLightInUse())&&
      (ekran != e_MAIN)){
    goToEkran(EKRAN::e_MAIN);
  }

  switch(ekran){

  case e_INIT:{ // jakikolwiek klawisz albo czas startup
    if ( key != Keyboard::Key::NONE) goToEkran(EKRAN::e_MAIN);
    if (!HMI::getInstance()->isStartup()) goToEkran(EKRAN::e_MAIN);
    break;
  }

  case e_MAIN:  {
    switch(key){
    case Keyboard::Key::ENTER:{
      pass.clear();
      goToEkran(EKRAN::e_HASLO);
      break;
    }
    case Keyboard::Key::RIGHT: goToEkran(EKRAN::e_WEJSCIA);  break;
    case Keyboard::Key::LEFT: goToEkran(EKRAN::e_WYJSCIA);  break;
    default: break;
    }
    break;
  }
  case e_WEJSCIA:  {
    switch(key){
    case Keyboard::Key::RIGHT: goToEkran(EKRAN::e_WYJSCIA);  break;
    case Keyboard::Key::LEFT: goToEkran(EKRAN::e_MAIN);  break;
    case Keyboard::Key::CANCEL: goToEkran(EKRAN::e_MAIN);  break;
    default: break;
    }
    break;
  }
  case e_WYJSCIA:  {
    switch(key){
    case Keyboard::Key::RIGHT: goToEkran(EKRAN::e_MAIN);  break;
    case Keyboard::Key::LEFT: goToEkran(EKRAN::e_WEJSCIA);  break;
    case Keyboard::Key::CANCEL: goToEkran(EKRAN::e_MAIN);  break;
    default: break;
    }
    break;
  }

  case e_HASLO:  {
    if (key == Keyboard::Key::NONE) break;
    Password::Status stat = pass.collect(key);
    switch(stat){
    case Password::Status::MATCH: goToEkran(EKRAN::e_uNAPED); break;
    case Password::Status::FULL: goToEkran(EKRAN::e_MAIN); break;
    default: break;
    }
    break;
  }

  case e_uNAPED:  {
    switch(key){
    case Keyboard::Key::ENTER:{
      napedM = sterM->getTypNapedu();
      goToEkran(EKRAN::e_uNAPED_USTAW);break;
    }
    case Keyboard::Key::CANCEL: goToEkran(EKRAN::e_MAIN);break;
    case Keyboard::Key::LEFT:  goToEkran(EKRAN::e_uNOWE_HASLO); break;
    case Keyboard::Key::RIGHT: goToEkran(EKRAN::e_uRESET_LICZNIKA); break;
    default:
      break;
    }
    break;
  }

  case e_uNAPED_USTAW:  {
    switch(key){
    case Keyboard::Key::ENTER:{
      sterM->setNaped(napedM);
    }
    // no break;
    case Keyboard::Key::CANCEL:
      goToEkran(EKRAN::e_MAIN);
      break;
    case Keyboard::Key::LEFT:  napedM = Sterownik::getPrevNaped(napedM); break;
    case Keyboard::Key::RIGHT:  napedM = Sterownik::getNextNaped(napedM); break;
    default:
      break;
    }
    break;
  }

  case e_uRESET_LICZNIKA:     {
    switch(key){
    case Keyboard::Key::ENTER:{
      VEprom::writeWord(VEprom::VirtAdres::LICZNIK, 0);
      goToEkran(EKRAN::e_MAIN);
      break;
    }
    case Keyboard::Key::CANCEL: goToEkran(EKRAN::e_MAIN); break;
    case Keyboard::Key::LEFT:  goToEkran(EKRAN::e_uNAPED); break;
    case Keyboard::Key::RIGHT: goToEkran(EKRAN::e_uZEGAR); break;
    default:
      break;
    }
    break;
  }

  case e_uZEGAR:       {
    switch(key){
    case Keyboard::Key::ENTER: goToEkran(EKRAN::e_uZEGAR_USTAW); break;
    case Keyboard::Key::CANCEL: goToEkran(EKRAN::e_MAIN); break;
    case Keyboard::Key::LEFT:  goToEkran(EKRAN::e_uRESET_LICZNIKA); break;
    case Keyboard::Key::RIGHT: goToEkran(EKRAN::e_uNOWE_HASLO); break;
    default:
      break;
    }
    break;
  }

  case e_uZEGAR_USTAW:{ // ToDo zrealizowac
    //if (key != Keyboard::Key::NONE)    goToEkran(EKRAN::e_MAIN);
    goToEkran(EKRAN::e_MAIN);
    break;
  }

  case e_uNOWE_HASLO:     {
    switch(key){
    case Keyboard::Key::ENTER:{
      pass.clear();
      goToEkran(EKRAN::e_uHASLO_USTAW); break;
    }
    case Keyboard::Key::CANCEL: goToEkran(EKRAN::e_MAIN); break;
    case Keyboard::Key::LEFT:  goToEkran(EKRAN::e_uZEGAR); break;
    case Keyboard::Key::RIGHT: goToEkran(EKRAN::e_uNAPED); break;
    default:
      break;
    }
    break;
  }

  case e_uHASLO_USTAW:  {
    Password::Status stat = pass.collect(key);
    switch(stat){
    case Password::Status::MATCH:
    case Password::Status::FULL:
    {
      pass.save();
      goToEkran(EKRAN::e_MAIN);
      break;
    }
    default: break;
    }
    break;
  }

  default: break;
  }

  showEkran();

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

static inline char getJobSymbol(){
  Praca::JOB job = praca->getJob();
  switch(job){
  case Praca::JOB::OTWIERANIE: return 'O';
  case Praca::JOB::ZAMYKANIE:  return 'Z';
  case Praca::JOB::STOI:
  default:  return '-';
  }
}


/*
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
 */
void Menu::showEkran(){

  switch(ekran){

  case e_INIT: {
    //---------------->1234567890123456<
    lcd->printXY(0, 0," STEROWNIK BRAM ");
    lcd->gotoXY(0, 1);
    //--------------------------->1234567890123456<
    lcd->printNumbersWithPattern("WERSJA: 000.", getProgMainVersion());
    lcd->printNumbersWithPattern(            "000 ", getProgSubVersion());
  }
  break;

  case e_MAIN:  {
    lcd->gotoXY(0,0);
    //--------------------------->1234567890123456<
    lcd->printNumbersWithPattern("LICZNIK: 000000 ", VEprom::readWord(VEprom::VirtAdres::LICZNIK));
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
  case e_WEJSCIA: {
    //--------------->|****|----|*-*-|<
    //--------------->1234567890123456<
    lcd->printXY(0,0, "     WEJSCIA    ");  // przerwa
    lcd->gotoXY(0,1);
    lcd->print("|");  // przerwa
    lcd->print(pins->gpioInKrancZamkniete.getInput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print(pins->gpioInKrancOtwarte.getInput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print(pins->gpioInZakazZamykania.getInput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print(pins->gpioInZakazOtwierania.getInput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print("|");  // przerwa
    lcd->print(pins->gpioInOtworz.getInput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print(pins->gpioInKluczI.getInput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print(pins->gpioInKluczII.getInput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print(pins->gpioInPozar.getInput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print("|");  // przerwa
    lcd->print(pins->gpioInAlarmAkust.getInput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print(pins->gpioInSiec230VAC.getInput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print(pins->gpioInRezerwa1.getInput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print(pins->gpioInRezerwa2.getInput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print("|");  // przerwa
  }
  break;

  case e_WYJSCIA:{
    //--------->|****|----|*-*-|<
    //--------->1234567890123456<
    lcd->gotoXY(0,0);
    lcd->print("     WYJSCIA    ");  // przerwa
    lcd->gotoXY(0,1);
    lcd->print("WY:|");  // przerwa
    lcd->print(pins->gpioOutZamkniete.getOutput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print(pins->gpioOutOtwarte.getOutput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print(pins->gpioOutPozar.getOutput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print(pins->gpioOutRelSprawny.getOutput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print("| SA:|");  // przerwa
    lcd->print(pins->gpioOutSygnAkust.getOutput() ? CHAR_IN_NOACTIVE : CHAR_IN_ACTIVE);
    lcd->print("|");  // przerwa
  }
  break;

  case e_HASLO:
  {
    //---------------->1234567890123456<
    lcd->printXY(0, 0,"  PODAJ HASLO:  ");  // przerwa
    //----------------->1234567890123456<
    char values[6];
    pass.getAsChars(values);
    lcd->gotoXY(0, 1);
    //--------------------------->1234567890123456<
    lcd->printSymbolsWithPattern("    [-----]     ", values, '-');
  }
  break;

  //  case e_1_NAPED:{
  //    //--------------->1234567890123456<
  //    lcd->printXY(0,0," <Wybor napedu>  " );
  //    Sterownik::NAPED npd = sterM->getTypNapedu();
  //    lcd->clearLine(1);
  //    //--------------->1234567890123456<
  //    lcd->printXY(0, 1, sterM->getOpisNapedu(npd) );
  //    break;
  //  }

  case e_uNAPED:
  {
    //---------------->1234567890123456<
    lcd->printXY(0, 0,"  USTAW_NAPED:  ");  // przerwa
    lcd->gotoXY(0, 1);
    lcd->print(      "   #-WEJSCIE    ");  // przerwa
  }
  break;
  case e_uNAPED_USTAW:
  {
    //--------------->1234567890123456<
    lcd->printXY(0,0," Wybierz naped: " );
    lcd->clearLine(1);
    lcd->printXY(0, 1, "<");
    lcd->print(Sterownik::getOpisNapedu(napedM));
    lcd->print(">");
    break;
  }

  case e_uRESET_LICZNIKA:
  {
    //------------ --->1234567890123456<
    lcd->printXY(0, 0," RESET LICZNIKA:");  // przerwa
    lcd->gotoXY(0, 1);
    lcd->print(      "   #-RESETUJ    ");  // przerwa
  }
  break;
  case e_uZEGAR:
  case e_uZEGAR_USTAW:
  {
    //--------- ------>1234567890123456<
    lcd->printXY(0, 0,"  USTAW ZEGAR   ");  // przerwa
    lcd->gotoXY(0, 1);
    lcd->print(      "  #-ROZPOCZNIJ  ");  // przerwa
  }
  break;
  case e_uNOWE_HASLO:
  {
    //---------- ----->1234567890123456<
    lcd->printXY(0, 0,"  USTAW HASLO   ");  // przerwa
    lcd->gotoXY(0, 1);
    lcd->print(      "  #-ROZPOCZNIJ  ");  // przerwa
  }
  break;
  case e_uHASLO_USTAW:
    //--------- -------->1234567890123456<
    lcd->printXY(0, 0, "  NOWE HASLO:   ");
    char values[6];
    pass.getAsChars(values);
    lcd->gotoXY(0, 1);
    //--------------------------->1234567890123456<
    lcd->printSymbolsWithPattern("    [-----]     ", values, '-');
    break;

  default:
    break;
  }
}




bool Menu::edit(Keyboard::Key key){
  return true;
}


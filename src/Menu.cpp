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
    }break;
    case Keyboard::Key::ENTER_CANCEL:{
      pass.clearService();
      goToEkran(EKRAN::e_HASLO_SERWIS);
    }break;
    case Keyboard::Key::RIGHT: goToEkran(EKRAN::e_WEJSCIA);  break;
    case Keyboard::Key::LEFT: goToEkran(EKRAN::e_BLEDY);  break;
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
    case Keyboard::Key::RIGHT: goToEkran(EKRAN::e_FRONT);  break;
    case Keyboard::Key::LEFT: goToEkran(EKRAN::e_WEJSCIA);  break;
    case Keyboard::Key::CANCEL: goToEkran(EKRAN::e_MAIN);  break;
    default: break;
    }
    break;
  }
  case e_FRONT:  {
    switch(key){
    case Keyboard::Key::RIGHT: goToEkran(EKRAN::e_BLEDY);  break;
    case Keyboard::Key::LEFT: goToEkran(EKRAN::e_WYJSCIA);  break;
    case Keyboard::Key::CANCEL: goToEkran(EKRAN::e_MAIN);  break;
    default: break;
    }
  }break;

  case e_HASLO:  {
    switch(key){
    case Keyboard::Key::ENTER:
    case Keyboard::Key::CANCEL:
    case Keyboard::Key::LEFT:
    case Keyboard::Key::RIGHT:{
      Password::Status stat = pass.collect(key);
      switch(stat){
      case Password::Status::MATCH: goToEkran(EKRAN::e_uNAPED); break;
      case Password::Status::FULL: goToEkran(EKRAN::e_MAIN); break;
      default: break;
      }
    }break;
    default:  break;
    }
  }break;

  case e_HASLO_SERWIS:  {
    switch(key){
    case Keyboard::Key::ENTER:
    case Keyboard::Key::CANCEL:
    case Keyboard::Key::LEFT:
    case Keyboard::Key::RIGHT:{
      Password::Status stat = pass.collectService(key);
      switch(stat){
      case Password::Status::MATCH: goToEkran(EKRAN::e_uNOWE_HASLO); break;
      case Password::Status::FULL: goToEkran(EKRAN::e_MAIN); break;
      default: break;
      }
    }break;
    default: break;
    }
  }break;

  case e_uNAPED:  {
    switch(key){
    case Keyboard::Key::ENTER:{
      tmpValue = (uint16_t)(sterM->getTypNapedu());
      goToEkran(EKRAN::e_uNAPED_USTAW);break;
    }
    case Keyboard::Key::CANCEL: goToEkran(EKRAN::e_MAIN);break;
    case Keyboard::Key::LEFT:  goToEkran(EKRAN::e_uOPOZN_ZAM_POZ); break;
    case Keyboard::Key::RIGHT: goToEkran(EKRAN::e_uRESET_LICZNIKA); break;
    default:
      break;
    }
    break;
  }

  case e_uNAPED_USTAW:  {
    switch(key){
    case Keyboard::Key::ENTER:{
      Sterownik::NAPED nap = (Sterownik::NAPED)(tmpValue);
      sterM->setNaped(nap);
      goToEkran(EKRAN::e_MAIN);
      break;
    }
    case Keyboard::Key::CANCEL:
      goToEkran(EKRAN::e_MAIN);
      break;
    case Keyboard::Key::LEFT:  tmpValue = Sterownik::getPrevNaped((Sterownik::NAPED)(tmpValue)); break;
    case Keyboard::Key::RIGHT:  tmpValue = Sterownik::getNextNaped((Sterownik::NAPED)(tmpValue)); break;
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
    case Keyboard::Key::RIGHT: goToEkran(EKRAN::e_uOPOZN_ZAM_POZ); break;
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

  case e_uOPOZN_ZAM_POZ:  {
    switch(key){
    case Keyboard::Key::ENTER:{
      tmpValue = VEprom::readWord(VEprom::VirtAdres::OPOZN_ZAMK_POZAR);
      if (tmpValue > MAX_OPOZN_ZAMK_POZ_sek) tmpValue = 1;
      goToEkran(EKRAN::e_uOPOZN_Z_P_USTAW);
      break;
    }
    case Keyboard::Key::CANCEL: goToEkran(EKRAN::e_MAIN);break;
    case Keyboard::Key::LEFT:  goToEkran(EKRAN::e_uNOWE_HASLO); break;
    case Keyboard::Key::RIGHT: goToEkran(EKRAN::e_uNAPED); break;
    default:
      break;
    }
    break;
  }

  case e_uOPOZN_Z_P_USTAW: {
    switch(key){
    case Keyboard::Key::ENTER:{
      VEprom::writeWord(VEprom::VirtAdres::OPOZN_ZAMK_POZAR, (uint16_t)tmpValue);
      goToEkran(EKRAN::e_MAIN);
      break;
    }
    case Keyboard::Key::CANCEL: goToEkran(EKRAN::e_MAIN);break;
    case Keyboard::Key::LEFT:
      if (tmpValue > 0) tmpValue--;
      break;
    case Keyboard::Key::RIGHT:
      if (tmpValue < MAX_OPOZN_ZAMK_POZ_sek) tmpValue++;
      break;
    default:
      break;
    }
    break;

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

  case e_BLEDY:  {
    switch(key){
    case Keyboard::Key::RIGHT: goToEkran(EKRAN::e_MAIN);  break;
    case Keyboard::Key::LEFT: goToEkran(EKRAN::e_FRONT);  break;
    case Keyboard::Key::CANCEL: goToEkran(EKRAN::e_MAIN);  break;
    default: break;
    }
  }break;

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
    //Events::Alarm alarm = sterM->getAlarm();
    lcd->print(Events::getEvent(Events::Name::AlarmPozarowy)->isActive() ? 'P' : '-');
    lcd->print(Events::getEvent(Events::Name::AlarmAkustyczny)->isActive() ? 'S' : '-');
    lcd->print(sterM->isAwaria() ? 'A' : '-');
    lcd->print(sterM->isAwariaSieci230VAC() ? 'V' : '-');
    break;
  }
  case e_WEJSCIA: {
    //--------------->|****|----|*-*-|<
    //--------------->1234567890123456<
    lcd->printXY(0,0, "     WEJSCIA    ");  // przerwa
    lcd->gotoXY(0,1);
    lcd->print("|");  // przerwa
    lcd->print(pins->gpioInKrancZamkniete.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print(pins->gpioInKrancOtwarte.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print(pins->gpioInZakazZamykania.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print(pins->gpioInZakazOtwierania.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print("|");  // przerwa
    lcd->print(pins->gpioInOtworz.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print(pins->gpioInKluczI.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print(pins->gpioInKluczII.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print(pins->gpioInPozar.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print("|");  // przerwa
    lcd->print(pins->gpioInAlarmAkust.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print(pins->gpioInSiec230VAC.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print(pins->gpioInRezerwa1.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print(pins->gpioInRezerwa2.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print("|");  // przerwa
  }
  break;

  case e_WYJSCIA:{
    //----------------->1234567890123456<
    lcd->printXY(0, 0, "     WYJSCIA    ");  // przerwa
    lcd->gotoXY(0,1);
    lcd->print("WY:|");  // przerwa
    lcd->print(pins->gpioOutZamkniete.getOutput() ? CHAR_OPEN : CHAR_SHORT);
    lcd->print(pins->gpioOutOtwarte.getOutput() ? CHAR_OPEN : CHAR_SHORT);
    lcd->print(pins->gpioOutPozar.getOutput() ? CHAR_OPEN : CHAR_SHORT);
    lcd->print(pins->gpioOutRelSprawny.getOutput() ? CHAR_SHORT : CHAR_OPEN);
    lcd->print("| SA:|");  // przerwa
    lcd->print(pins->gpioOutSygnAkust.getOutput() ? CHAR_ACTIVE : CHAR_INACTIVE);
    lcd->print("|");  // przerwa
  }
  break;


  case e_FRONT:{
    //----------------->1234567890123456<
    lcd->printXY(0, 0, " KLAW. FRONT    ");  // przerwa
    lcd->gotoXY(0,1);
    lcd->print("kl:");  // przerwa
    lcd->print(pins->stacyjka.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print(" g:");  // przerwa
    lcd->print(pins->keyUp.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print(" s:");  // przerwa
    lcd->print(pins->keyStop.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
    lcd->print(" d:");  // przerwa
    lcd->print(pins->keyDown.getInput() ? CHAR_INACTIVE : CHAR_ACTIVE);
  }
  break;

  case e_HASLO:  {
    //---------------->1234567890123456<
    lcd->printXY(0, 0,"  PODAJ HASLO:  ");  // przerwa
    //----------------->1234567890123456<
    char values[6];
    pass.getAsChars(values);
    lcd->gotoXY(0, 1);
    //--------------------------->1234567890123456<
    lcd->printSymbolsWithPattern("    [-----]     ", values, '-');
    break;
  }

  case e_HASLO_SERWIS:  {
    //---------------->1234567890123456<
    lcd->printXY(0, 0," HASLO SERWISOWE");  // przerwa
    //----------------->1234567890123456<
    char values[Password::ServicePassLength];
    pass.getServiceChars(values);
    lcd->gotoXY(0, 1);
    //--------------------------->1234567890123456<
    lcd->printSymbolsWithPattern("   [--------]   ", values, '-');
  }break;

  case e_uNAPED:
  {
    //----------------->1234567890123456<
    lcd->printXY(0, 0, "  USTAW_NAPED:  ");  // przerwa
    lcd->printXY(0, 1, "   #-WEJSCIE    ");  // przerwa
  }
  break;
  case e_uNAPED_USTAW:
  {
    //--------------->1234567890123456<
    lcd->printXY(0,0," Wybierz naped: " );
    lcd->clearLine(1);
    lcd->printXY(0, 1, "<");
    lcd->print(Sterownik::getOpisNapedu((Sterownik::NAPED)tmpValue));
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
    lcd->printXY(0, 1,"  #-ROZPOCZNIJ  ");  // przerwa
  }
  break;
  case e_uHASLO_USTAW:{
    //----------------->1234567890123456<
    lcd->printXY(0, 0, "  NOWE HASLO:   ");
    char values[6];
    pass.getAsChars(values);
    lcd->gotoXY(0, 1);
    //--------------------------->1234567890123456<
    lcd->printSymbolsWithPattern("    [-----]     ", values, '-');
    break;
  }
  case e_uOPOZN_ZAM_POZ:
    //----------------->1234567890123456<
    lcd->printXY(0, 0, "OPOZN. ZAMYK. P.");
    lcd->printXY(0, 1,"  #-ROZPOCZNIJ  ");  // przerwa
    break;

  case e_uOPOZN_Z_P_USTAW:{
    //----------------->1234567890123456<
    lcd->printXY(0, 0, "OPOZN. ZAMYK. P.");
    //char charBuf[8];
    //my_itoa((int32_t)(tmpValue & 0x0ffff), charBuf);
    lcd->gotoXY(0, 1);
    //--------------------------->1234567890123456<
    lcd->printNumbersWithPattern("  [000] sekund  ", tmpValue & 0x0ffff);
    break;
  }
  case e_BLEDY:{
    //----------------->1234567890123456<
    lcd->clearScreen();
    lcd->gotoXY(0, 0);
    Events::Iterator iter = Events::getIterator();
    bool noEvents = true;
    while(true){
      Event * ev = Events::getNextEvent(iter);
      if (ev == nullptr) break;
      if (ev->value != 0){
        noEvents = false;
        if (!(lcd->printToEnd(ev->name))) break;
        if (!(lcd->printToEnd(", "))) break;
      }
    }
    if (noEvents){
      lcd->print(" BRAK ZDARZEN");
    }
    break;
  }

  default:
    break;
  }
}


//bool Menu::edit(Keyboard::Key key){
//  return true;
//}


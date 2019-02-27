/*
 * Password.h
 *
 *  Created on: 26 lut 2019
 *      Author: lechu
 */

#ifndef PASSWORD_H_
#define PASSWORD_H_

#include "systemDefs.h"
#include "VEprom.h"
#include "Keyboard.h"

class Password {
public:
  static constexpr uint16_t DEFAULT_LENGTH = 5;
  static constexpr uint16_t MAX_LENGTH = 16/3;  //max 5 bo znaki kodowane sa na 3 bitach w 16 bitowym slowie
  static constexpr uint16_t DEFAULT_PASSWORD =
      (Keyboard::Key::ENTER << (3*4))  |
      (Keyboard::Key::CANCEL << (3*3))  |
      (Keyboard::Key::LEFT << (3*2))  |
      (Keyboard::Key::RIGHT << (3*1))  |
      (Keyboard::Key::ENTER << (3*0)) ;       // E><B

  typedef enum{
    NOT_FULL,
    FULL,
    MATCH,
  }Status;


private:
  uint16_t length;
  uint16_t password;
  uint8_t index;



  void add(Keyboard::Key key){
    if (index >= length) return;
    uint8_t offset = uint8_t(index * 3);
    password &= uint16_t(~(0b0111 << offset));
    password |= uint16_t((key & 0b0111) << offset);
    index++;
  }

  char getCharAt(uint8_t pos){
      if (pos >= length) return '-';
      uint8_t offset = uint8_t(pos * 3);
      Keyboard::Key key = Keyboard::Key((password >> offset) & 0b0111);
      switch(key){
      case Keyboard::Key::ENTER: return 'E';
      case Keyboard::Key::CANCEL: return 'B';
      case Keyboard::Key::LEFT: return '<';
      case Keyboard::Key::RIGHT: return '>';
      default: return '-';
      }
    }

  bool isFull(){
    return (index >= length);
  }

  bool isMatch(){
    if (!isFull()) return false;
    uint32_t saved = VEprom::readWord(VEprom::PASSWORD);
    return password == uint16_t(saved);
  }

  static inline void save(uint16_t newPass){
    VEprom::writeWord(VEprom::VirtAdres::PASSWORD, newPass);
  }


public:

  Password(){
    password = 0;
    length = DEFAULT_LENGTH;
    index = 0;
  }


  void init(){
    uint16_t pass = (uint16_t)VEprom::readWord(VEprom::PASSWORD);
    if ((pass == 0) || (pass == 0xffff)){
      save(DEFAULT_PASSWORD);
    }
    clear();
  }

  void clear(){
     password = 0;
     index = 0;
   }

  void save(){
    save(password);
  }

  uint16_t get()const {
    return password;
  }

  void set(uint16_t newPass) {
    password = newPass;
  }


  Status getStatus(){
    if (isFull()){
      if (isMatch()){
        return Status::MATCH;
      }
      return Status::FULL;
    }
    return Status::NOT_FULL;
  }

  Status collect(Keyboard::Key key){
    if (key != Keyboard::Key::NONE){
      add(key);
    }
    return getStatus();

  }

  void getAsChars(char * tab){
    for (uint8_t i = 0; i < length; i++){
      tab[i] = getCharAt(i);
    }
  }

};

#endif /* PASSWORD_H_ */

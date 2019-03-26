/*
 * Led.h
 *
 *  Created on: 24 sty 2014
 *      Author: lechu
 */

#ifndef LED_H_
#define LED_H_

#include "Gpio.h"

class Led : public Gpio{
public:
  static constexpr uint32_t TIME_DEFAULT_TICK_INTERVAL_MS = 100;

  typedef	enum {SWIECI, ZGASZONA, MRUGA_SLOW, MRUGA_MEDIUM, MRUGA_FAST, PULSUJE, PULSUJE_NIEROWNO} Mode;
  typedef	enum {ACTIVEHIGH, ACTIVELOW} Type;

protected:
  uint32_t tickMs;
  Mode tryb;
  Type typ;

  inline void preinit(){
    typ = Type::ACTIVEHIGH;
    tryb = Mode::ZGASZONA;
    tickMs = 0;
  }

public:
  //  Led(GPIO_TypeDef* gpioPtr, uint8_t pin_Nr):Gpio(gpioPtr, pin_Nr){
  //    ticks = 0;
  //  }

  Led(GPIO_TypeDef* gpioPtr, uint8_t pin_Nr) noexcept :Gpio(gpioPtr, pin_Nr){
    preinit();
  }

  Led(GPIO_TypeDef* gpioPtr, uint8_t pin_Nr, Type type, Mode mode):Gpio(gpioPtr, pin_Nr){
    typ = type;
    tryb = mode;
    tickMs = 0;
  }

  void init(Type type, Mode mode){
    typ = type;
    tryb = mode;
    tickMs = 0;
    Gpio::setup(GpioMode::OUTPUT, Gpio::GpioOType::PushPull, GpioPuPd::NoPull, GpioSpeed::LowSpeed);
  }

  void init(){
    tickMs = 0;
    Gpio::setup(GpioMode::OUTPUT, Gpio::GpioOType::PushPull, GpioPuPd::NoPull, GpioSpeed::LowSpeed);
  }

  void set(Mode sw){tryb = sw;}
  void turnOn(void){set(SWIECI); tick();}
  void turnOff(void){set(ZGASZONA); tick();}

  void tick(uint32_t milis){
    tickMs +=milis;
    if (tickMs >= (10 * TIME_DEFAULT_TICK_INTERVAL_MS)){
      tickMs = tickMs % TIME_DEFAULT_TICK_INTERVAL_MS;
    }
    uint8_t tick = uint8_t(tickMs / TIME_DEFAULT_TICK_INTERVAL_MS);
    switch (tryb){
    case SWIECI:
      Gpio::setOutputUp();
      break;
    case ZGASZONA:
      Gpio::setOutputDown();
      break;
    case MRUGA_SLOW:
      setOutput(tick < 5 ); // pol na pol 0,1,2,3,4, , , , , ,
      break;
    case MRUGA_MEDIUM:
      setOutput((tick >> 1) % 2 ); // 0,1, , ,4,5, , ,8,9
      break;
    case MRUGA_FAST:
      setOutput(tick % 2 ); // co drugi 0, ,2, ,4, ,5, ,7, ,9 swieci
//      if ((tickMs == 1) || (tickMs == 2) || (tickMs == 5) || (tickMs == 6)) setOutputUp();
//      else setOutputDown();
      break;
    case PULSUJE:
      setOutput(!tick); // 0, , , , , , , , , , swieci
//      if ((ticks == 2) || (ticks == 6)) setOutputDown(); else setOutputUp();
      break;
    case PULSUJE_NIEROWNO:
      setOutput((!(tick % 4))||(!(tick % 3))); // 0, , ,3,4, ,6, ,8,9 swieci
//      if ((ticks == 1) || (ticks == 5) || (ticks == 7)) setOutputDown(); else setOutputUp();
      break;
    default:
      tryb = MRUGA_MEDIUM;
      break;
    }
  }

  void tick(void){
   tick(TIME_DEFAULT_TICK_INTERVAL_MS);
  }
};

#endif /* LED_H_ */

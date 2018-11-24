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
	typedef	enum {SWIECI, ZGASZONA, MRUGA_SLOW, MRUGA_FAST, PULSUJE, PULSUJE_NIEROWNO} Mode;
	typedef	enum {ACTIVEHIGH, ACTIVELOW} Type;

protected:
	uint8_t ticks;
	Mode tryb;
	Type typ;
public:
	Led(GPIO_TypeDef* gpioPtr, uint8_t pin_Nr, Type type, Mode mode):Gpio(gpioPtr, pin_Nr){
		typ = type;
		tryb = mode;
		ticks = 0;
		Gpio::setup(GpioMode::OUTPUT, Gpio::GpioOType::PushPull, GpioPuPd::NoPull, GpioSpeed::LowSpeed);
		//setMODE(Gpio::GpioMode::OUTPUT);
		//setSpeed(Gpio::GpioSpeed::Speed1);
		//setOType(Gpio::GpioOType::PushPull);
	}

	void set(Mode sw){tryb = sw;}
	void turnOn(void){set(SWIECI); tick();}
	void turnOff(void){set(ZGASZONA); tick();}
	void tick(void){
		ticks++;
		if (ticks >= 8) ticks = 0;

		switch (tryb){
		case SWIECI:
			Gpio::setOutputUp();
			//bitSet();
			break;
		case ZGASZONA:
			Gpio::setOutputDown();
			//bitClear();
			break;
		case MRUGA_SLOW:
			setOutput(ticks < 4);	// pol na pol
			//if (ticks < 4) bitSet(); else bitClear();
			break;
		case MRUGA_FAST:
			if ((ticks == 1) || (ticks == 2) || (ticks == 5) || (ticks == 6)) setOutputUp();
			else setOutputDown();
			break;
		case PULSUJE:
			if ((ticks == 2) || (ticks == 6)) setOutputDown(); else setOutputUp();
			break;
		case PULSUJE_NIEROWNO:
			if ((ticks == 1) || (ticks == 5) || (ticks == 7)) setOutputDown(); else setOutputUp();
			break;
		default:
			tryb = MRUGA_FAST;
			break;
		}
	}
};

#endif /* LED_H_ */

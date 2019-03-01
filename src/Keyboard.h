/*
 * Keyboard.h
 *
 *  Created on: 15.09.2016
 *      Author: lechu
 */


#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "Button.h"
#include "Fifo.h"
#include "QuickTask.h"
#include "Gpio.h"



class Keyboard {

public:
	static constexpr uint16_t BUFFERSIZE = 4;	// rozmiar bufora znakow
	static constexpr uint32_t TIME_PERIOD_KEYB_MS = 10;

	typedef enum{
		NONE = 0,
		ENTER, CANCEL,
		RIGHT, LEFT,
		ENTER_CANCEL,
		LEFT_RIGHT,
	}Key;


private:
	// bufor znakow - od najstarszego na najmlodszego. Brak znaku to 'Key::NONE'
	uint8_t buffer[BUFFERSIZE];
	Fifo fifo = Fifo(buffer, BUFFERSIZE);

	/**
	 * kasuje ostatni znak, przesuwa wszystkie o jeden i wpisuje najnowszy na koniec
	 */
	void addToBuffer(Key key){
		if (key == NONE) return;
		fifo.put((uint8_t)key);
	}

	Key getFromBuffer(){
		while(fifo.isNotEmpty()){
			Key result = (Key)fifo.get();
			if (result != Key::NONE) return result;	// pomijac przypadkowe NONE az bedzie wlasciwy klawisz
		}
		return Key::NONE;
	}

	Gpio *keyBACK;// =  Gpio(GPIOC,6, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	Gpio *keyLEFT;// =  Gpio(GPIOC,5, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	Gpio *keyRIGHT;// =  Gpio(GPIOC,8, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	Gpio *keyENTER;// =  Gpio(GPIOA,12, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);

	// unbuffered key state
	Key getPressedKey(){
	  Key key = Key::NONE;
		if (!keyRIGHT->getInput()) key = Key::RIGHT;
		if (!keyENTER->getInput()) key = Key::ENTER;
		if (!keyBACK->getInput()) key = Key::CANCEL;
		if (!keyLEFT->getInput()) key = Key::LEFT;
		if ((!keyRIGHT->getInput()) && (!keyLEFT->getInput())) key = Key::LEFT_RIGHT;
    if ((!keyENTER->getInput()) && (!keyBACK->getInput())) key = Key::ENTER_CANCEL;
		return key;
	}



public:
	Keyboard(Gpio *keyBack, Gpio *keyLeft, Gpio *keyRight, Gpio *keyEnter) {
	  keyBACK = keyBack;
	  keyLEFT = keyLeft;
	  keyRIGHT = keyRight;
	  keyENTER = keyEnter;

		for (uint16_t i = 0; i<BUFFERSIZE; i++)	buffer[i] = NONE;
	}

	void init(){
	  keyBACK->setup(Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::LowSpeed);
	  keyLEFT->setupFromClone(keyBACK);
	  keyRIGHT->setupFromClone(keyBACK);
	  keyENTER->setupFromClone(keyBACK);
	}

	void co10ms(){
		static Key oldKey = NONE;
		Key newKey = getUnbufferedKey();
		if (oldKey != newKey){
			oldKey = newKey;
			if (newKey != NONE) addToBuffer(newKey);
		}
	}


	bool haveBufferedKey() {
		return fifo.isNotEmpty();
	}

	Key getKey(){
		return getFromBuffer();
	}

	Key getUnbufferedKey(){
		return getPressedKey();
	}


};

extern Keyboard keys;

#endif /* KEYBOARD_H_ */

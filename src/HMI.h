/*
 * HMI.h
 *
 *  Created on: 16.09.2016
 *      Author: lechu
 */

#ifndef HMI_H_
#define HMI_H_

#include "Sterownik.h"
#include <string.h>
#include "Keyboard.h"
#include "Menu.h"
#include "ST7032iFB.h"
#include "Front.h"

class HMI {
public:
	static constexpr uint32_t CZAS_WYJSCIA_Z_MENU_MS = 30 * 1000;	// pol minuty
	static constexpr uint32_t TIME_PERIOD_MONITOR_MS = 5;

  static constexpr uint32_t TIME_BACKLIGHT_ACTIVE_MS = 2 * 60 * 1000;    // 2 minuty po jakimkolwiek klawiszu
  static constexpr uint32_t TIME_STARTUP_INDICATION_MS = 5000;

private:

	uint32_t startupDelayMs = 0;
	uint32_t backlightTimeMs = 0;

public:
	Sterownik * ster = nullptr;
	FrameBuffer * lcd = nullptr;
	Keyboard * keyboard = nullptr;
	Front * front;
	Menu * menu = nullptr;
  Led * ledPozarRed = nullptr;
  Led * ledAwariaYellow = nullptr;
  Led * ledGotowoscGreen = nullptr;
  Gpio * backLight = nullptr;

	HMI(){;}

	void init(Sterownik * sterownik, Keyboard * pkeys, Front * frontKeys, ST7032iFB * lcdDriver, Menu * pmenu, Led * ledPozar_Red, Led * ledAwaria_Yellow, Led * ledGotowosc_Green) {
		ster = sterownik;
		lcd = lcdDriver->getFrameBuffer();
		keyboard = pkeys;
		front = frontKeys;
		backLight = lcdDriver->getBackLight();
    ledPozarRed = ledPozar_Red;
    ledAwariaYellow = ledAwaria_Yellow;
    ledGotowoscGreen = ledGotowosc_Green;

		keyboard->init();
		menu = pmenu;
		menu->init(ster, keyboard, lcd);

		ledPozarRed->init(Led::Type::ACTIVEHIGH, Led::Mode::SWIECI );
		ledAwariaYellow->init(Led::Type::ACTIVEHIGH, Led::Mode::SWIECI );
		ledGotowoscGreen->init(Led::Type::ACTIVEHIGH, Led::Mode::SWIECI );

		//startupDelayMs = 0L;
		//backlightTimeMs = 0L;
	}

	void setBackLight(bool enable){ backLight->setOutput(enable); }

	void poll(){

	  // Po uruchomieniu mrugamy diodami przez @HMI::TIME_STARTUP_INDICATION_MS
	  if (isStartup()){
	    startupDelayMs += TIME_PERIOD_MONITOR_MS;
	    ledPozarRed->set(Led::Mode::PULSUJE_NIEROWNO);
      ledAwariaYellow->set(Led::Mode::PULSUJE_NIEROWNO);
      ledGotowoscGreen->set(Led::Mode::PULSUJE_NIEROWNO);
	  }

	  // wlaczac podswietlenie na TIME_BACKLIGHT_ACTIVE_MS milisekund po nacisnieciu jakiegos klawisza
	  if (keyboard->getUnbufferedKey() !=  Keyboard::Key::NONE){
	    backlightTimeMs = 0;
	  }
	  if (backlightTimeMs < TIME_BACKLIGHT_ACTIVE_MS){
	    backlightTimeMs += TIME_PERIOD_MONITOR_MS;
	    setBackLight(true);
	  }else {
	    setBackLight(false);
	  }

		// sterowanie LED-ami
    ledPozarRed->tick(TIME_PERIOD_MONITOR_MS);
    ledAwariaYellow->tick(TIME_PERIOD_MONITOR_MS);
    ledGotowoscGreen->tick(TIME_PERIOD_MONITOR_MS);

    // reszta kolejki odpytywania
    keyboard->co10ms();
    front->poll();
    menu->poll();
	}

	static HMI * getInstance();

	inline bool isStartup(){
	  return startupDelayMs < TIME_STARTUP_INDICATION_MS;
	}

  inline bool isBackLightInUse(){
    return backLight->getOutput();
  }

};	// class HMI



#endif /* HMI_H_ */

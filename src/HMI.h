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

class HMI {
public:
	static constexpr uint32_t CZAS_WYJSCIA_Z_MENU_MS = 30 * 1000;	// pol minuty
	static constexpr uint32_t TIME_PERIOD_MONITOR_MS = 5;

  static constexpr uint32_t TIME_BACKLIGHT_ACTIVE_MS = 10000;    // 10 sekund po jakimkolwiek klawiszu
  static constexpr uint32_t TIME_STARTUP_INDICATION_MS = 5000;

private:

	uint32_t startupDelayMs = 0;
	uint32_t backlightTimeMs = 0;

public:
	Sterownik * ster = nullptr;
	FrameBuffer * lcd = nullptr;
	Keyboard * keyboard = nullptr;
	Menu * menu = nullptr;
  Led * ledAlarm = nullptr;
  Led * ledPracaAku = nullptr;
  Gpio * backLight = nullptr;

	HMI(){;}

//	HMI(Sterownik * psterownik, Keyboard * pkeys, FrameBuffer * plcd, Menu * pmenu, Led * ledAlarmRed, Led * ledPracaAkuYellow){
//		init(psterownik, pkeys, plcd, pmenu, ledAlarmRed, ledPracaAkuYellow);
//	}

	void init(Sterownik * sterownik, Keyboard * pkeys, FrameBuffer * plcd, Gpio * gpioBackLight, Menu * pmenu, Led * ledAlarmRed, Led * ledPracaAkuYellow) {
		ster = sterownik;
		lcd = plcd;
		keyboard = pkeys;
		backLight = gpioBackLight;
    ledAlarm = ledAlarmRed;
    ledPracaAku = ledPracaAkuYellow;
		menu = pmenu;
		menu->init(ster, keyboard, lcd);
		//startupDelayMs = 0L;
		//backlightTimeMs = 0L;
	}


	void setBackLight(bool enable){ backLight->setOutput(enable); }

	void monitor(){

	  // Po uruchomieniu mrugamy diodami przez @HMI::TIME_STARTUP_INDICATION_MS
	  if (startupDelayMs < TIME_STARTUP_INDICATION_MS){
	    startupDelayMs += TIME_PERIOD_MONITOR_MS;
      ledAlarm->set(Led::Mode::PULSUJE_NIEROWNO);
      ledPracaAku->set(Led::Mode::PULSUJE_NIEROWNO);
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
    ledAlarm->tick(TIME_PERIOD_MONITOR_MS);
    ledPracaAku->tick(TIME_PERIOD_MONITOR_MS);

    // reszta kolejki odpytywania
    menu->poll();
	}

	static HMI * getInstance();


};	// class HMI



#endif /* HMI_H_ */

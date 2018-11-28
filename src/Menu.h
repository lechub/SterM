/*
 * Menu.h
 *
 *  Created on: 04.07.2017
 *      Author: lechu
 */

#ifndef MENU_H_
#define MENU_H_

#include "Keyboard.h"
#include "FrameBuffer.h"
#include "Sterownik.h"
//#include ""
//#include "HMI.h"
//#include "Parameter.h"

class Menu {

public:
	typedef enum{
		e_AUTOMAT,

	}EKRAN;



	//HMI * hmi;

private:

	static constexpr uint32_t REFRESH_DELAY_MS = 338;

	Sterownik * ster;
	Keyboard * keys;
	FrameBuffer * lcd;
	//Praca * praca;

	uint32_t refreshDelay;

	EKRAN ekran;
	//Parameter::Nazwa editParam = Parameter::Nazwa::NONE;
	uint16_t editValue = 0;
	bool editMode = false;

public:

	void init(Sterownik * pSter, Keyboard * pKeys, FrameBuffer * pLcd){
		ster = pSter;
		keys = pKeys;
		lcd = pLcd;
		//praca = ster->getPraca();

		ekran = EKRAN::e_AUTOMAT;
		//goToEkran(EKRAN::e_AUTOMAT);
	}


	void goToEkran(EKRAN nowyEkran){
		ekran = nowyEkran;
		editMode = false;

//		switch(ekran){
//		case e_AUTOMAT:				 break;
//
//		default:
// break;
//		}
		showEkran();
	}

//	void goToEdit(Parameter::Nazwa param){
//		editParam = param;
//		editValue = Parameter::getValue(editParam);
//		editMode = true;
//		showEkran(editValue);
//	}

	void printPattern(const char * pattern, uint32_t value);


// tylko wyswietlenie - w trybie przegladania
	void showEkran(){
		editMode = false;
		uint16_t val = 1;// Parameter::getValue(editParam);
		showEkran(val);
	}

	bool edit(Keyboard::Key key);

	void showEkran(uint16_t val);

	void poll();


};


#endif /* MENU_H_ */

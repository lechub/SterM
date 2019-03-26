/*
 * PkkbL1.h
 *
 *  Created on: 10 lut 2014
 *      Author: lechu
 */

#ifndef PKKBL1_H_
#define PKKBL1_H_

#include "Pkkb.h"
#include "Fifo.h"

class PkkbL1 {
public:
	typedef enum {
		PKKB_SOF = 0xAA, /** Znacznik poczatku transmisji, tzn. poczatku ramki*/
		PKKB_EOF = 0xAB, /** Znacznik konca transmisji */
		PKKB_ESC = 0xAC /** Znacznik podmiany dla nastepnego znaku */
	} PKKB_CtrlChar;

	typedef enum {
		PKKB_ESC_SOF = 0x01, /** kod SOF dla zakodowanego bajtu */
		PKKB_ESC_EOF = 0x02, /** kod EOF dla zakodowanego bajtu */
		PKKB_ESC_ESC = 0x03 /** kod ESC dla zakodowanego bajtu */
	} PKKB_EncodedChar;

private:

	/** Procedurka zamienia zakodowane po ESC znaki na prawidlowe.
	 @param data Bajt do odkodowania
	 @return Bajt po odkodowaniu
	 */
	static unsigned char esc_decode(unsigned char data) {
		switch (data) {
		case PKKB_ESC_SOF:
			return PKKB_SOF;
			break;
		case PKKB_ESC_EOF:
			return PKKB_EOF;
			break;
		case PKKB_ESC_ESC:
			return PKKB_ESC;
			break;
		}
		return data;
	}

	static bool escFlag;

	// ************************************************************************

	/** Procedurka koduje znaki sterujace pojawiajace sie po ESC.
	 @param data Bajt do zakodowania
	 @return Bajt po zakodowaniu
	 */
	static unsigned char esc_encode(unsigned char data) {
		switch (data) {
		case PKKB_SOF:
			return PKKB_ESC_SOF;
			break;
		case PKKB_EOF:
			return PKKB_ESC_EOF;
			break;
		case PKKB_ESC:
			return PKKB_ESC_ESC;
			break;
			//default break;
		}
		return data;
	}

	// ************************************************************************

	/** Sprawdza, czy bajt jest znakiem specjalnym wymaganym do zakodowania.
	 @param data Bajt do zbadania
	 @return 1 jesli to znak specjalny i 0 gdy zwykly.
	 */
	static bool is_ctrl_code(unsigned char data) {
		return ((data == PKKB_SOF) || (data == PKKB_EOF) || (data == PKKB_ESC));
	}

public:

	static bool encodeFrame(Fifo *frameIn, Fifo *frameOut) {
		uint8_t znak;
		if (!frameOut->put(PKKB_CtrlChar::PKKB_SOF))
			return false;
		while (frameIn->isNotEmpty()) {
			znak = (uint8_t)frameIn->get();
			if (is_ctrl_code(znak)) {
				if (!frameOut->put(PKKB_CtrlChar::PKKB_ESC))
					return false;
				znak = esc_encode(znak);
			}
			if (!frameOut->put(znak))
				return false;
		}
		if (!frameOut->put(PKKB_CtrlChar::PKKB_EOF))
			return false;
		return true;
	}

	static bool decodeFrame(Fifo *frameIn, Fifo *frameOut) {
		uint8_t znak;
		while (frameIn->isNotEmpty()) {	// odszukaj SOF
			znak = (uint8_t) frameIn->get();
			if (znak == PKKB_CtrlChar::PKKB_SOF)
				break;
		}	// odczytano SOF
		if (frameIn->isEmpty())	return false;	// brak reszty ramki?
		while (frameIn->isNotEmpty()) {
			znak = (uint8_t)frameIn->get();
			switch(znak){
			case PKKB_CtrlChar::PKKB_ESC: znak = esc_decode((uint8_t)frameIn->get()); break;
			case PKKB_CtrlChar::PKKB_EOF: return true;
			case PKKB_CtrlChar::PKKB_SOF: return false;
			}
			if (!frameOut->put(znak)) return false;
		}
		return false;
	}

	// dodaje kolejne bajty razem z dekodowaniem L1. Zwraca true, gdy odebrana cala ramka.
	static bool decodeByte(Fifo * frameOut, uint8_t byteIn){
		if (escFlag){
			frameOut->put(esc_decode(byteIn));
			escFlag = false;
			return false;
		}
		if (byteIn == PKKB_CtrlChar::PKKB_ESC){
			escFlag = true;
			return false;
		}
		escFlag = false;
		if (byteIn == PKKB_CtrlChar::PKKB_EOF){ return true; }
		if (byteIn == PKKB_CtrlChar::PKKB_SOF){frameOut->flush();  return false;}

		frameOut->put(byteIn);
		return false;
	}

};

#endif /* PKKBL1_H_ */

/*
 * PkkbSlave.h
 *
 *  Created on: 13 lut 2014
 *      Author: lechu
 */

#ifndef PKKBSLAVE_H_
#define PKKBSLAVE_H_

//#include "Pkkb.h"
#include "PkkbL2.h"
#include "PkkbInterface.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t	indexRead(uint8_t index);
extern bool	indexWrite(uint8_t index,uint8_t val);
extern bool stringRead(uint8_t strIndex, Fifo *result);
extern bool stringWrite(uint8_t strIndex, Fifo *toWrite);

extern uint8_t getDeviceType();
extern uint8_t getDeviceSubType();

#ifdef __cplusplus
}
#endif

class PkkbSlave {
public:

private:
	Pkkb::PkkbResult lastResult;
	PkkbInterface * pkkbInterface;
	//uint8_t * adresPtr;
public:
	PkkbSlave(PkkbInterface *iface) {
		//adresPtr = adres;
		pkkbInterface = iface;
		lastResult = Pkkb::RESULT_OK;
	}

	// odczyt 1 bajtu pamieci wirtualnej o okreslonym indeksie
	uint8_t read_byte(Pkkb::IndexMapTmplt index) {
		switch (index) {
		case Pkkb::INDEX_PKKB_STATUS: return 0;
		case Pkkb::INDEX_ERROR: return 0;
		case Pkkb::INDEX_PKKB_MODE: return 0;
		case Pkkb::INDEX_ADRES:
			return pkkbInterface->getAdres();
			//return *adresPtr; //break;
		case Pkkb::INDEX_DEVICE_TYPE:
			return	getDeviceType();
		case Pkkb::INDEX_DEVICE_SUBTYPE:
			return getDeviceSubType();
		case Pkkb::INDEX_HEADER_COPY: return 0;
		case Pkkb::INDEX_UNUSED7: return 0;
		case Pkkb::INDEX_UNUSED8: return 0;
		case Pkkb::INDEX_UNUSED9: return 0;
			return 0;
		case Pkkb::INDEX_DATA:
		default:
			return indexRead(index);
			//return indexRead((IndexMap) index);
			//return PKKB_INDEX_NOT_EXIST_ERROR;
			break;
		}
		return false;
	}

	// zapis 1 bajtu pamieci wirtualnej pod okreslonym indeksem
	bool write_byte(Pkkb::IndexMapTmplt index, uint8_t val) {
		switch (index) {
		case Pkkb::INDEX_PKKB_STATUS:
		case Pkkb::INDEX_ERROR:
			if (val == 0) lastResult = Pkkb::PkkbResult::RESULT_OK;
			break;
		case Pkkb::INDEX_PKKB_MODE:
		case Pkkb::INDEX_ADRES:
			//*adresPtr = val;
			pkkbInterface->setAdres(val);
			break;
		case Pkkb::INDEX_DEVICE_TYPE:
		case Pkkb::INDEX_DEVICE_SUBTYPE:
		case Pkkb::INDEX_HEADER_COPY:
		case Pkkb::INDEX_UNUSED7:
		case Pkkb::INDEX_UNUSED8:
		case Pkkb::INDEX_UNUSED9:
			lastResult = Pkkb::WRITE_PROTECT_ERROR;
			return false;
		case Pkkb::INDEX_DATA:
		default:
			if (!indexWrite(index, val)) {
				lastResult = Pkkb::WRITE_PROTECT_ERROR;
				return false;
			} else
				return true;
		}
		return false;
	}


	// procedura automatycznej odpowiedzi slave'a na ramke mastera
	bool autoAnswer(Fifo *frameIn, Fifo *frameOut) {
		bool wynik = true;
		while (frameIn->isNotEmpty()) {
			Pkkb::PkkbCommands komenda = (Pkkb::PkkbCommands) frameIn->get();
			uint8_t indeks, dlugosc;
			switch (komenda) {
			case Pkkb::READ: {
				frameOut->put(Pkkb::ANSWER);				// bajt <ANSWER>
				frameOut->put(read_byte((Pkkb::IndexMapTmplt) frameIn->get()));	// bajt danych spod zadanego indeksu
				break;
			}
			case Pkkb::WRITE:
				indeks = frameIn->get();
				if (!write_byte((Pkkb::IndexMapTmplt) indeks, frameIn->get()))
					wynik = false;
						// INDEX
				//level2.answerACKEX(wynik);
				break;
			case Pkkb::READBLOCK:
				frameOut->put(Pkkb::ANSWER);				// bajt <ANSWER>
				indeks = frameIn->get();
				dlugosc = (Pkkb::IndexMapTmplt) frameIn->get();
				while (dlugosc > 0) {
					frameOut->put(read_byte((Pkkb::IndexMapTmplt) indeks));	// bajt danych spod zadanego indeksu
					dlugosc--;
					indeks++;
				}
				//level2.answer(&ramka, result);
				break;
			case Pkkb::WRITEBLOCK:
				indeks = frameIn->get();
				wynik = true;
				for (dlugosc = frameIn->get(); dlugosc > 0;
						dlugosc--, indeks++) {
					if (!write_byte((Pkkb::IndexMapTmplt) indeks,
							frameOut->put(frameIn->get())))
						wynik = false;
				}
				//answerACKEX(wynik);
				break;
			case Pkkb::READSTRING:
				frameOut->put(Pkkb::ANSWER);				// bajt <ANSWER>
				indeks = frameIn->get();
				//dlugosc = frameIn->get();
				stringRead(indeks,frameOut);	// bajt danych spod zadanego indeksu
				//level2.answer(&ramka, result);
				break;

			case Pkkb::WRITESTRING:
				indeks = frameIn->get();
				wynik = true;
				wynik =  stringWrite(indeks, frameIn);
				//answerACKEX(wynik);
				break;
			case Pkkb::ANSWER:
				wynik = false;
				// od mastera nie powinien dostawac komendy <ANSWER>
				break;
			case Pkkb::ASSIGN:
				frameIn->get();
				// ToDo dorobic nadawanie adresu
				break;
			default:
				break;
			}
			return wynik;
		}
		return false;
	}

};

#endif /* PKKBSLAVE_H_ */

/*
 * Pkkb.h
 *
 *  Created on: 12 lut 2014
 *      Author: lechu
 */

#ifndef PKKB_H_
#define PKKB_H_

#include <stdint.h>

//#ifdef __cplusplus

class Pkkb {
public:
	const static uint32_t MAXFRAMESIZE = 300;	// 256 + troche na naglowki, przekodowania itp.
	const static uint8_t PKKB_MASTER_ADRESS = 0x00;
	const static uint8_t PKKB_BROADCAST_ADRESS = 0xFF;
	const static uint8_t PKKB_UNUSED_ADRESS = 0xFE;

	typedef enum {
		READ = 0x01,		// odczyt 1 bajtu: READ, INDEX
		WRITE = 0x02,		//	- zapis 1 bajtu: WRITE, INDEX, VALUE
		READBLOCK = 0x03,		//	- odczyt bloku danych:  READBLOCK, INDEX, LENGTH
		WRITEBLOCK = 0x04,		//	- zapis bloku danych: WRITEBLOCK, INDEX, LENGTH, dane....
		ANSWER = 0x05,		// - tylko odpowiedz: ANSWER, [dane...]
		READSTRING = 0x06,		// - odczyt tekstu: READSTRING, INDEX
		WRITESTRING = 0x07,		// - zapis tekstu: WRITESTRING, INDEX, dane...
		ASSIGN = 0x10		//wejscie w procedure nadawania adresu
	} PkkbCommands;

	typedef enum {
		RESULT_OK, WRITE_PROTECT_ERROR,	// proba zapisu pod index "tylko do odczytu"
		INDEX_NOT_EXIST_ERROR,	// proba odczytu/zapisu pod nieistniejacy index
		SEND_ACCES_ERROR,		// proba wysylania ramki sie nie powiodla
		RECEIVE_NOT_COMPLETED,	// brak ramki do odebrania
		BUFFER_OVERRUN,			// za malo miejsca, zeby odebrac ramke
		POSITIVE_ANSWER,
		NEGATIVE_ANSWER,
		UNKNOWN_ERROR				// nieznany blad protokolu
	} PkkbResult;

	typedef enum {
		INDEX_PKKB_STATUS	= 0,	// Status protokolu PKKB
		INDEX_ERROR 		= 1,	// Ostatni niezatwierdzony blad
		INDEX_PKKB_MODE 	= 2,	// tryb pracy protokolu PKKB
		INDEX_ADRES 		= 3,// Adres urzadzenia zmiana tylko poprzez procedure nadawania adresu
		INDEX_DEVICE_TYPE 	= 4,// Typ urzadzenia - jeden z wymienionych w DeviceType
		INDEX_DEVICE_SUBTYPE = 5,	// Podtyp - zalezny od urzadzenia lub 0
		INDEX_HEADER_COPY 	= 6,	// kopia ostatniego naglowka PKKB
		INDEX_UNUSED7 		= 7,
		INDEX_UNUSED8 		= 8,
		INDEX_UNUSED9 		= 9,
		INDEX_DATA 			= 10	// odtad zaczynaja sie specyficzne dla kazdego urzadzenia indeksy i zmienne
	// tu dopisywac dalsze
	} IndexMapTmplt;

	typedef enum {
		STR_NAME = 0,	// Nazwa urzadzenia, np. KBZB-38
		STR_SUBNAME = 1,	// np. 5A+2,5A 24Ah
		STR_DESCRIPTION = 2,// Rozwiniecie nazwy np. "Zasilacz buforowy do urz. ppoz."
		STR_COMPANY = 3,
		STR_COMPANY2 = 4,
		STR_VERSION = 5,
		STR_AUTHOR = 6,
		STR_UNUSED7 = 7,
		STR_UNUSED8 = 8,
		STR_UNUSED9 = 9,
		STR_DATA = 10	// tu dopisywac dalsze, specyficzne dla urzadzenia
	} StringMapTmplt;

};


//#endif /* __cplusplus */

#endif /* PKKB_H_ */

/*
 * PkkbL2.h
 *
 *  Created on: 11 lut 2014
 *      Author: lechu
 */

#ifndef PKKBL2_H_
#define PKKBL2_H_

#include "Pkkb.h"
//#include "PkkbL1.h"
#include "Fifo.h"
//#include "PkkbInterface.h"



// ******************************************************************************************

class PkkbL2 {
public:
	constexpr static uint8_t CRC_SEED = 0x6A;
	typedef enum {
		ACK_POSITIVE, ACK_NEGATIVE, NO_ACK
	} ACKstatus;

	typedef struct pkkb_hdr1 {
		unsigned char POZ_NEG :1;//-> 1 - potwierdzenie pozytywne OK / 0 - potwierdzenie negatywne BLAD	- mozna uzywac do potwierdzenia przyjecia ramki itp.
		unsigned char ACKEX :1;	// 1 - ramka z odpowiedzia (odpowiedz na bicie POZ_NEG)/ 0 - zwykla ramka (POZ_NEG nie ma znaczenia)
		unsigned char KONTYN :1;// 1 - to jest kontynuacja ciagu danych - wtedy nie trzeba powtarzac adresow - bedzie 1 bajt numeru ramki/ 0 - nowa ramka, bez numeru
		unsigned char DL_CRC :2;	//  - dlugosc danych i CRC w ramce
		//00	- bez CRC i bez okreslenia dlugosci ramki. Ramke limituje znak EOF
		//01	- bedzie dlugosc ramki 1 bajt, bez sumy CRC
		//10	- bedzie dlugosc ramki 1 bajt, suma CRC 1 bajtowa za trescia
		//11	- bedzie dlugosc ramki 1 bajt, suma CRC 4 bajtowa za trescia
		unsigned char ADRN :1;	// 1 - bedzie adres nadawcy / 0 - brak ()
		unsigned char ADRO :1;// 1 - bedzie adres odbiorcy / 0 - brak (ramka bez adresu!)
		unsigned char NEXT :1;//1 - bedzie drugi bajt HEADER (obecnie nieuzywany) / 0 - tylko 1 bajt HEADER
	} PKKB_Header1Bits;

	typedef union {
		PKKB_Header1Bits bits;
		uint8_t byte;
	} PKKB_Header1;

private:
	// ******************************************************************************************

	uint8_t crcChecksum;
	//PKKB_Header1 headerIn;
	//PKKB_Header1 headerOut;
	uint8_t adresLokalny;	// Adres tego urzadzenia
	uint8_t adresZdalny;	// Adres zdalnego urzadzenia
	uint8_t dlug;
	uint32_t errorL2Counter;

	ACKstatus ackStatus;

	void dodajCRC(unsigned char znak) {
		crcChecksum = (uint8_t)(crcChecksum + znak);
	}
//	void dodajCRC32(unsigned char znak) {
//			switch (headerIn.bits.DL_CRC) {
//			case 0b11:
//				crcChecksum += (uint8_t)znak;
//				break;
//				//	case 0b11:
//				//		crcChecksum + = znak;	//ToDo potem zmienic na rzeczywiste 32 bity
//			default:
//				break;
//			}
//		}

	void resetCRC() {
		crcChecksum = CRC_SEED;
	}

	bool checkCRC(uint8_t crc) {
		if (crcChecksum == crc) return true;
		return false;
	}

public:
	PkkbL2() {
		//		PkkbInterface * pkkbInterface = pkkbIface;
		crcChecksum = 0;
		//		adres = pkkbInterface->getAdres();
		adresLokalny = adresZdalny = dlug = 0;
		errorL2Counter = 0;
		ackStatus = NO_ACK;
	}

	void setAdresLokalny(uint8_t address){adresLokalny = address; }
	void setAdresZdalny(uint8_t address){adresZdalny = address; }
	uint8_t getAdresLokalny(){return adresLokalny;}
	uint8_t getAdresZdalny(){return adresZdalny;}

	void makeHeaderForMSZ(PKKB_Header1 * header){
		header->bits.NEXT = 0;
		header->bits.ADRO = 1;
		header->bits.ADRN = 0;
		header->bits.DL_CRC = 0b10;
		header->bits.KONTYN = 0;
		header->bits.ACKEX = 0;
		header->bits.POZ_NEG = 0;
	}

  static void makeHeaderP2P(PKKB_Header1 * header){
//    header->bits.NEXT = 0;
//    header->bits.ADRO = 0;
//    header->bits.ADRN = 0;
//    header->bits.DL_CRC = 0b00;
//    header->bits.KONTYN = 0;
//    header->bits.ACKEX = 0;
//    header->bits.POZ_NEG = 0;
    header->byte = 0;
  }

	//	bool encodeFrame(Fifo *frameIn, Fifo *FrameOut, bool ack, bool answer) {
	//		PKKB_Header1	header;
	//		header.bits.POZ_NEG = answer;
	//		header.bits.ACKEX = ack;	// slave nie zada potwierdzen
	//		header.bits.DL_CRC = 0b10;	//headerIn.bits.DL_CRC;
	//		header.bits.KONTYN = false;// na razie nie przewidujemy dlugich ramek
	//		header.bits.NEXT = false;// na razie nie przewidujemy drugiego bajtu Header
	//		header.bits.ADRN = headerIn.bits.ADRO;
	//		header.bits.ADRO = headerIn.bits.ADRN;
	//		return encodeFrame(frameIn, FrameOut, &header);
	//	}

	bool encodeFrame(Fifo *frameIn, Fifo *FrameOut, PKKB_Header1 * headerOut ) {
		resetCRC();
		dodajCRC(headerOut->byte);
		if (!FrameOut->put(headerOut->byte)) return false;

		if (headerOut->bits.ADRO) {
			dodajCRC(adresZdalny);
			FrameOut->put(adresZdalny);
		}

		if (headerOut->bits.ADRN) {
			dodajCRC(adresLokalny);
			FrameOut->put(adresLokalny);
		}

		if (headerOut->bits.DL_CRC != 0b00) {
			uint8_t tmp = (uint8_t)frameIn->get_count();
			dodajCRC(tmp);
			FrameOut->put(tmp);
		}

		while (frameIn->isNotEmpty()) {
			uint8_t tmp = (uint8_t) frameIn->get();
			dodajCRC(tmp);
			FrameOut->put(tmp);
		}

		if (headerOut->bits.DL_CRC == 0b10) {
			FrameOut->put(crcChecksum);
		} else if (headerOut->bits.DL_CRC == 0b11) {
			// ToDo zmienic potem na crc 4-bitowe!
			FrameOut->put((uint8_t) crcChecksum);
			FrameOut->put((uint8_t) (crcChecksum + 1));
			FrameOut->put((uint8_t) (crcChecksum + 2));
			FrameOut->put((uint8_t) (crcChecksum + 3));
		}

		return true;
	}

	bool decodeFrame(Fifo *frameIn, Fifo *result, PKKB_Header1 * headerIn) {
		uint8_t znak;
		if (frameIn == nullptr)
			return true;
		resetCRC();		// reset sumy CRC
		znak = (uint8_t) frameIn->get();
		headerIn->byte = znak;		// pierwszy bajt ramki L2 - header1
		dodajCRC(znak);

		if (headerIn->bits.NEXT) {
			dodajCRC(znak = (uint8_t) frameIn->get());
		}	// na razie olac, ale wliczyc do CRC

		if (headerIn->bits.ADRO) {
			znak = (uint8_t)frameIn->get();
			if (!((znak == adresLokalny) || (znak == Pkkb::PKKB_BROADCAST_ADRESS)
					|| (znak == Pkkb::PKKB_UNUSED_ADRESS))) {
				return false;
			}
		}

		if (headerIn->bits.ADRN) {
			adresZdalny = znak = (uint8_t) frameIn->get();
			dodajCRC(znak);
		}
		if (headerIn->bits.ACKEX) {
			if (headerIn->bits.POZ_NEG)
				ackStatus = ACKstatus::ACK_NEGATIVE;
			else
				ackStatus = ACKstatus::ACK_POSITIVE;
		} else
			ackStatus = ACKstatus::NO_ACK;

		if (headerIn->bits.DL_CRC == 0b00) {	//00	- bez CRC i bez okreslenia dlugosci ramki. Ramke limituje znak EOF
			result->putAll(frameIn);
			return result;
		}

		//01	- bedzie dlugosc ramki 1 bajt, bez sumy CRC
		//10	- bedzie dlugosc ramki 1 bajt, suma CRC 1 bajtowa za trescia
		//11	- bedzie dlugosc ramki 1 bajt, suma CRC 4 bajtowa za trescia
		dlug = (uint8_t)frameIn->get();
		dodajCRC((uint8_t) dlug);

		for (int ilosc = dlug; ilosc > 0; ilosc--) {
			dodajCRC(znak = (uint8_t) frameIn->get());
			result->put(znak);
		}

		if (headerIn->bits.DL_CRC == 0b10) {
			znak = (uint8_t) frameIn->get();
			if (znak == crcChecksum) {
				return true;
			} else {
				return false;
			}
		}
		if (headerIn->bits.DL_CRC == 0b11) {
			znak = (uint8_t) frameIn->get();
			if (znak == crcChecksum) {
				return true;
			} else {
				//errorL2Counter++;
				return false;
			}
		}
		// DL_CRC == 0b01
		return true;

	}

};

#endif /* PKKBL2_H_ */

/*
 * KabeDevices.h
 *
 *  Created on: 24 sty 2014
 *      Author: lechu
 */

#ifndef KABEDEVICES_H_
#define KABEDEVICES_H_





//----------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------


typedef enum {
	KABE_TYP_NIEZNANY		= 0x00,
	KABE_TYP_MASTER			= 0x01,	// potem jeszcze oznaczenie w kodzie ASCII, np. KBPK-01 v1.0
	KABE_TYP_ZASILACZ		= 0x02,	// potem jeszcze oznaczenie zasilacza w kodzie ASCII i wersja , np. "KBZB-35 v1.0"
	KABE_TYP_MODUL_SYGNAL	= 0x03,	//	modul sygnalizacyjny. Typ tylko dla porzadku, nie zaimplementowano w nim PKKB
	KABE_TYP_REPEATER		= 0x04,	// ??? Nikt juz nie pamieta o tym
	KABE_TYP_CZUJNIK_PALIWA = 0x05,	// Czyjnik paliwa do helikoptera. Typ tylko dla porzadku, nie zaimplementowano w nim PKKB
	KABE_TYP_HELIKOPTER		= 0x06,	// Master helikoptera UAV2. J.w. - bez PKKB
	KABE_TYP_ZAS_HELI_V1	= 0x07,	// Zasilacz helikoptera. J.w. - bez PKKB
	KABE_TYP_ZAS_KBPK_01	= 0x08,	// Panel kontrolny do zasilaczy. J.w. - bez PKKB
	KABE_TYP_ZAS_KBZB_36	= 0x09,	// Zasilacz KBZB36.  J.w. - bez PKKB
	KABE_TYP_ZAS_KBZB_33	= 0x0A,	// Zasilacz KBZB33.  J.w. - bez PKKB
	KABE_TYP_ZAS_KBZB_35	= 0x0B,	// Zasilacz KBZB35.  J.w. - bez PKKB
	KABE_TYP_ZAS_KBZB_38	= 0x0C,	// Zasilacz ppoz. KBZB38. Pierwszy obslugujacy PKKB
	KABE_TYP_ZAS_KBZB_40	= 0x0D, // Zasilacz ppoz. KB-EKO.
	KABE_TYP_ZAS_KBZB_39	= 0x0E, // Zasilacz buforowy bez certyfikatu.
	KABE_TYP_FUTURE			= 0x0F, // znacznik dla ostatniego obslugiwanego typu
} DeviceType;


//----------------------------------------------------
#ifdef __cplusplus
}
#endif
//----------------------------------------------------


#endif /* KABEDEVICES_H_ */

/*
 * ZbEKO.h
 *
 *  Created on: 07.04.2017
 *      Author: lechu
 */

#ifndef KBZB40_H_
#define KBZB40_H_

#include <stdint.h>

//#include "PkkbMaster.h"


class KBZB40 {

	/*************************************************************************
	 * Wyliczenie trybow pracy akumulatora
	 * */
public:

	static constexpr char * constTypeDescr = "KBZB-40";

	//	static constexpr uint32_t PKKB_BUFFER_SIZE = 100;

	typedef enum {
		AKU_INIT			= 0,	///< uruchomienie modulu
		AKU_LADOWANIE		= 1,	///< ladowanie akumulatora
		AKU_KONSERWACJA		= 2,	///< konserwacja akumulatora
		AKU_OCZEKIWANIE 	= 3,	///< akumulator nie jest ladowany
		AKU_PRZED_TESTEM	= 4,	///< czynnosci przed testem akumulatora, obnizenie napiecia zasilania itp.
		AKU_TEST 			= 5,	///< test akumulatora - dociazenie
		AKU_PO_TESCIE		= 6,	///< zakonczenie testu akumulatora
		AKU_POWROT_PO_TESCIE = 7,	///< czynnosci kontrolne po tescie i powrot do poprzedniego trybu
		AKU_PRACA_Z_AKU		= 8,	///< praca bez napiecia sieciowego
		AKU_ODLACZENIE		= 9,	///< zabezpieczenie podnapieciowe akumulatora
		AKU_DIAGNOSTYKA		= 10,	///< tryb diagnostyczny - tylko do serwisu
		AKU_RESTART			= 11,	///< stan przejsciowy do ladowania po INIT lub pracy z aku
		AKU_POWROT_SIECI	= 12,	///<
		AKU_BLAD_MM			= 13,
		AKU_BEZ_AKU			= 20,	///< praca tylko z zasilacza ale bez aku
		AKU_MM_TEST			= 21,
		AKU_PO_TESCIE_MM	= 22,
		AKU_AWARIA			= 23,
	} TrybAku;

	/*************************************************************************
	 * bajt statusowy obwodu akumulatora
	 * */
	typedef union {
		uint8_t		bajt;
		struct{								// bajt informacyjny o akumulatorze (1 - true) oznacza zaistnienie zdarzenia
			uint8_t	brakAku			:1;		///< nie wykryto akumulatora lub złe połączenie akumulatora lub przepalony bezpiecznik akumulatora
			uint8_t	niskieUAku		:1;		///< niskie napiecie na akumulatorze (ponizej 12V)
			uint8_t	wysokieUAku		:1;		///< zbyt wysokie napiecie na akumulatorze
			uint8_t	wysokaRwewn		:1;		///< zbyt wysoka rezystancja wewnetrzna akumulatora
			uint8_t	niskiIDociaz	:1;		///< zbyt niski prad pomiarowy w tescie rezystancji wewnetrznej
			uint8_t	wysokiIDociaz	:1;		///< zbyt wysoki prad pomiarowy w tescie rezystancji wewnetrznej
			uint8_t	wysokiPrad		:1;		///< zbyt wysoki prad akumulatora
			uint8_t bladCzujnikaT	:1;		///< blad nieokreslony
		}flag;
	}AkuStatusFlags;

	/*************************************************************************
	 * Bajt statusowy zasilacza
	 * */
	typedef union {
		uint8_t		bajt;
		struct{								///< bajt informacyjny o akumulatorze (1 - true) oznacza zaistnienie zdarzenia
			uint8_t	brakUwy			:1;		///< brak napiecia na wyjsciu
			uint8_t	zaWysokieUwy	:1;		///< Zbyt wysokie napiecie na wyjsciu
			uint8_t	zaWysokiI		:1;		///< zbyt duzy prad wyjsciowy
			uint8_t	brakSieci230V	:1;		///< brak napiecia sieciowego 230VAC
			uint8_t	bladBezpiecznikow	:1;	///< blad bezpiecznikow
			uint8_t	bladKlucza			:1;		///< problem z kluczem aku
			uint8_t	bladModuluMM	:1;		///< zarezerwowane
			uint8_t	nieokreslonyBlad	:1;	// zarezerwowane
		}flag;
	}ZasStatusFlags;

	/*************************************************************************
	 * Bajt zbiorczych bledow dla zasilacza
	 * */
	typedef union{
		uint8_t		bajt;
		struct{								// flagi bledow: (1)'true' to blad/problem, a  (0)'false' to brak bledu/problemu
			uint8_t	aku			:1;		///<  dla toru akumulatora
			uint8_t	out			:1;		///<  dla napiec wyjsciowych
			uint8_t	bezpieczniki :1;	///<  dla bezpiecznikow i modulu rozbicia zasilania
			uint8_t	u230V		:1;		///<  dla sieci 230VAC
			uint8_t	ladowarka	:1;		///<  dla ladowarki akumulatora
			uint8_t	dociaz		:1;		///<  dla toru pomiarowego rezystancji akumualtora
			uint8_t	torPomiarowy :1;	///<  dla innych torow pomiarowych - wartosci poza dopuszczalnymi zakresami
			uint8_t	inny		:1;		///<  kazdy inny blad
		}flag;
	}ZBEKOErrorFlags;

	/*************************************************************************
	 * Bajt statusowy bezpiecznikow
	 * */
	typedef union {
		uint8_t	bajt;
		struct{
			uint8_t	unused		:1;	///< true (1) oznacza ze wpiety, false (0) oznacza ze nieuzywany
			uint8_t	mainFuse1	:1;		///< true (1) oznacza dobry, false (0) oznacza przepalony
			uint8_t	mainFuse2	:1;		///< true (1) oznacza dobry, false (0) oznacza przepalony
			uint8_t	akuFuse		:1;		///< true (1) oznacza dobry, false (0) oznacza przepalony
		}bits;
	}BezpiecznikiStatus;


	/*************************************************************************
	 * Wyliczenie wersji modulow sterujacych
	 * */
	typedef enum {
		MODEL_MS_LowPower	= 0,	// dla MM 1A i 2,5A - rezystory pomiarowe IZas i IAku to 20 mOhm
		MODEL_MS_MidPower	= 1,
		MODEL_MS_HiPower	= 2,	// dla MM 5A i 10A - rezystory pomiarowe IZas i IAku to 5 mOhm
	} ModelMS;	// Modul Sterujacy

	static constexpr uint32_t CurrentSelf_ModelLoMid = 30;
	static constexpr uint32_t CurrentSelf_ModelHi = 70;


	/*************************************************************************
	 * Wyliczenie wersji modulow sterujacych
	 * */
	typedef enum {
		VERSION_24V	= 0,	// dla MM 1A i 2,5A - rezystory pomiarowe IZas i IAku to 20 mOhm
		VERSION_12V	= 1,	// dla MM 5A i 10A - rezystory pomiarowe IZas i IAku to 5 mOhm
	} VoltageVersion;	// Modul Sterujacy


	typedef enum {
		MODEL_M35W	= 1,	///< Modul mocy 1A
		MODEL_M50W	= 2,	///< Modul mocy 1A
		MODEL_M75W	= 3,	///< Modul mocy 2,5A
		MODEL_M100W	= 4,	///< Modul mocy 5A
		MODEL_M150W	= 5,	///< Modul mocy 5A
		MODEL_M240W	= 6,	///< Modul mocy 8,5A
	} ModelMM;	// Modul Mocy


	static inline int32_t getNominalCurrent(ModelMM mm){
		switch(mm){
		case ModelMM::MODEL_M35W: return 1200;
		case ModelMM::MODEL_M50W: return 1800;
		case ModelMM::MODEL_M75W: return 2700;
		case ModelMM::MODEL_M100W: return 3700;
		case ModelMM::MODEL_M150W: return 5500;
		case ModelMM::MODEL_M240W: return 8800;
		default:
			break;
		}
		return 999;
	}

	static inline const char * getNominalCurrentTxt(ModelMM mm){
		switch(mm){
		case ModelMM::MODEL_M35W: return "1.2";
		case ModelMM::MODEL_M50W: return "1.8";
		case ModelMM::MODEL_M75W: return "2.7";
		case ModelMM::MODEL_M100W: return "3.7";
		case ModelMM::MODEL_M150W: return "5.5";
		case ModelMM::MODEL_M240W: return "8.8";
		default:
			break;
		}
		return "??";
	}

	/*************************************************************************
	 * Wyliczenie indeksow bajtowych do komunikacji
	 * Stosowane konwencje zapisu liczb:
	 * K1) Napiecie z zakresu 0V..65V na dwoch bajtach i pomnozone przez 100,
	 * 	 	czyli np. 24,68V * 100 = 2468 => 2468/256 = 9; 2468%256 = 164 : H=9, L=164
	 *
	 * K2) Prad z zakresu -30A..30A na dwoch bajtach w U2 i pomnozone przez 100,
	 * 			czyli np. -10A * 100 = 64536 ^= FC18
	 * */
	typedef enum {
		INDEX_ZB_EKO_STATUS = 10,	///< INDEX_DATA,	// 10
		INDEX_ZB_EKO_ERRORS = 11,	///<  zawiera flagi KBZB38ErrorFlags
		INDEX_AKU_STATUS	= 12,	///<  zawiera flagi AkuStatusFlags
		INDEX_ZAS_STATUS	= 13,	///<  zawiera flagi ZasStatusFlags
		INDEX_BEZP_STATUS	= 14,	///<  zawiera flagi BezpiecznikiStatus,
		INDEX_RESERVED0		= 15,	///<  zawiera flagi ModRozbiciaZas
		INDEX_RESERVED1		= 16,	///<  zarezerwowany
		INDEX_SIEC230V 		= 17,	///<  wartosc rozna od zera oznacza obecnosc zasilania 230VAC
		INDEX_OUT_U_L		= 18,	///<  wartosc napiecia wyjsciowego w miliwoltach (K1) - mlodszy bajt
		INDEX_OUT_U_H		= 19,	///<  j.w. ale starszy bajt
		INDEX_OUT_I_L		= 20,	///<  wartosc pradu wyjsciowego w miliamperach - mlodszy bajt (moze byc ujemne w U2!!!)
		INDEX_OUT_I_H		= 21,	///<  j.w. ale starszy bajt
		INDEX_AKU_U_L		= 22,	///<  wartosc napiecia akumulatora w miliwoltach - mlodszy bajt
		INDEX_AKU_U_H		= 23,	///<  j.w. ale starszy bajt
		INDEX_AKU_I_L		= 24,	///<  wartosc pradu wyjsciowego w miliamperach - mlodszy bajt (moze byc ujemne w U2!!!)
		INDEX_AKU_I_H		= 25,	///<  j.w. ale starszy bajt
		INDEX_AKU_REZYST_L	= 26,	///<  rezystancja obwodu akumulatora w miliohmach - mlodszy bajt
		INDEX_AKU_REZYST_H	= 27,	///<  j.w. ale starszy bajt
		INDEX_AKU_TEMPERATURE_L = 28,	///<  temperatura akumulatora odczytana na czujniku DS18B20 - mlodszy bajt
		INDEX_AKU_TEMPERATURE_H	= 29,	///<  j.w. ale starszy bajt
		INDEX_RESERVED2		= 30,	///<  jesli 0x00 to brak czujnika, wartosc != 0 oznacza ze czujnik jest wpiety
		INDEX_AKU_MODE		= 31,	///<  tryb pracy akumulatora zdefiniowany w TrybAku - do celow serwisowych
		INDEX_FORCE_TEST	= 32,	///<  wymuszenie testu akumulatora - do celow serwisowych
		INDEX_FORCE_RESET	= 33,	///<  wymuszenie restartu trybu prqacy akumulatora - do celow serwisowych
		INDEX_INTERNAL_FLAG1 = 34,	///<  flagi do celow serwisowych
		INDEX_INTERNAL_FLAG2 = 35,	///<  j.w.
		INDEX_CALIBRATION_START = 36,	///<  bajt sterujacy do procedury kalibracji
		INDEX_CALIBRATION_ENABLE = 37,	///<  bajt sterujacy do procedury kalibracji
		INDEX_USER_WRITE_ENABLE = 38,	///<  bajt sterujacy do zapisu danych uzytkownika
		INDEX_MODEL_MS			= 39,	///<  typ modulu sterujacego - z wyliczenia ModulMS
		INDEX_RESERVED3			= 40,	///<  typ modulu ladujacego - z wyliczenia ModulML
		INDEX_MODEL_MM			= 41,	///<  typ modulu mocy - z wyliczenia ModulMM
		INDEX_AKU_CAPACITY		= 42,
		INDEX_LAST_BYTE				///<  znacznik konca indeksow
	} IndexMap;


	/*************************************************************************
	 * Wyliczenie indeksow tekstowych (stringow) do komunikacji
	 * */
	typedef enum {
		STR_SERIALNO	= 10,	///< Numer seryjny zasilacza - max. 14 znakow ascii
		STR_CPUID		= 11,	///< Numer identyfikacyjny procesora - max. 16 znakow ascii
		STR_USER		= 12,	///< Tekst(oznaczenie) uzytkownika  - max. 20 znakow ascii
		STR_OUT			= 13,	///< Tekst informacyjny o wyjsciu: UOUT, UOUT1, UOUT2 oraz prady IZAS,
		STR_AKU 		= 14,	///< Tekst informacyjny o akumulatorze: UAKU, prad Aku IAKU, prad ladowania ILAD
		STR_INTERNAL	= 15,	///< do celow serwisowych
		STR_RESERVED 	= 16,	///< zarezerwowane
		STR_CONFIGURATION = 17,	///< ciag kalibracyjny - do celow serwisowych
		STR_ADC_INFO = 18,	///< do celow serwisowych
		STR_CALIBRATION = 19,	///< do celow serwisowych
		STR_LAST_STRING	= 20	///< znacznik konca indeksow tekstowych
	} StringMap;


	static constexpr uint8_t		serialLength = 20;
	static constexpr uint8_t		cpuIdLength = 24;
	static constexpr uint8_t		userStrLength = 20;
	static constexpr uint8_t		configLength = 78;
	static constexpr uint8_t		calibrLength = 36;

	typedef struct __attribute__((packed)){
		uint8_t		mem[IndexMap::INDEX_LAST_BYTE];
		char		serialNo[serialLength + 1];
		char		cpuId[cpuIdLength + 1];
		char		userStr[userStrLength + 1];
		char		configString[configLength + 1];
		char		calibrString[calibrLength + 1];
	}ZbEkoData;

};

#endif /* KBZB40_H_ */

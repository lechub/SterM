/*
 * Pinout.h
 *
 *      Author: lechu
 */

#ifndef PINOUT_H_
#define PINOUT_H_

#include "Gpio.h"
#include "Led.h"
#include "Pomiar.h"

#define PCB_VERSION 3

class Pinout {

public:

#if PCB_VERSION == 1
  //Wyjscia
  Gpio gpioOutPozar = Gpio(GPIOA, 0);
  Gpio gpioOutZamkniete = Gpio(GPIOA, 1);
  Gpio gpioOutOtwarte = Gpio(GPIOA, 2);
  Gpio gpioOutSygnAkust = Gpio(GPIOA, 3);
  Gpio gpioOutBuzer = Gpio(GPIOC, 4);

  Gpio gpioWlaczBypass = Gpio(GPIOA, 8);
  Gpio gpioWlacz230Zamknij = Gpio(GPIOC, 6);
  Gpio gpioWlacz230Otworz = Gpio(GPIOC, 9);
  Gpio gpioWlaczInwerter = Gpio(GPIOC, 7);
  Gpio gpioWlaczZasNaped = Gpio(GPIOC, 8);


  Gpio gpioWyj1H = Gpio(GPIOD, 15);
  Gpio gpioWyj2H = Gpio(GPIOD, 14);
  Gpio gpioWyj3H = Gpio(GPIOD, 13);
  Gpio gpioWyj4H = Gpio(GPIOD, 12);
  Gpio gpioWyj1L = Gpio(GPIOD, 11);
  Gpio gpioWyj2L = Gpio(GPIOD, 10);
  Gpio gpioWyj3L = Gpio(GPIOD, 9);
  Gpio gpioWyj4L = Gpio(GPIOD, 8);

  Gpio gpioInBtnBACK = Gpio(GPIOA, 7);
  Gpio gpioInBtnLEFT = Gpio(GPIOB, 0);
  Gpio gpioInBtnRIGHT = Gpio(GPIOB, 1);
  Gpio gpioInBtnENTER = Gpio(GPIOB, 2);


  Gpio gpioInKrancZamkniete = Gpio(GPIOA, 4);
  Gpio gpioInKrancOtwarte = Gpio(GPIOA, 5);

  Gpio gpioInZakazZamykania = Gpio(GPIOA, 6);
  Gpio gpioInZakazOtwierania = Gpio(GPIOE, 7);
  Gpio gpioInOtworz = Gpio(GPIOE, 8);
  Gpio gpioInKluczI = Gpio(GPIOE, 9);
  Gpio gpioInKluczII = Gpio(GPIOE, 10);
  Gpio gpioInPozar = Gpio(GPIOE, 11);
  Gpio gpioInAlarmAkust = Gpio(GPIOE, 12);
  Gpio gpioInSiec230VAC = Gpio(GPIOE, 13);
  Gpio gpioInRezerwa1 = Gpio(GPIOE, 14);
  Gpio gpioInRezerwa2 = Gpio(GPIOE, 15);

  Gpio gpioLcdReset = Gpio(GPIOC, 5);
  Gpio gpioLcdBackLight = Gpio(GPIOE, 2);

  Gpio gpioUart1Dir = Gpio(GPIOA, 12);
  Gpio gpioUart1Tx = Gpio(GPIOB, 6);
  Gpio gpioUart1Rx = Gpio(GPIOB, 7);

  Gpio gpioUart2Dir = Gpio(GPIOD, 4);
  Gpio gpioUart2Tx = Gpio(GPIOD, 5);
  Gpio gpioUart2Rx = Gpio(GPIOD, 6);

  Gpio gpioSCL = Gpio(GPIOB, 10);
  Gpio gpioSDA = Gpio(GPIOB, 11);

  Led ledPozar = Led(GPIOF, 2); //  Gpio gpioLedAwaria = Gpio(GPIOF, 2);
  Led ledAwaria = Led(GPIOF, 3); //  Gpio gpioLedPracaAku = Gpio(GPIOF, 3);

  // to jest w Hardware/ADC
  //  Gpio gpioPomSygnL = Gpio(GPIOC, 0);
  //  Gpio gpioPomZas24V = Gpio(GPIOC, 1);

#elif PCB_VERSION == 3

  //Wyjscia
   Gpio gpioOutPozar      = Gpio(GPIOF, 9);
   Gpio gpioOutZamkniete  = Gpio(GPIOF, 3);
   Gpio gpioOutOtwarte    = Gpio(GPIOF, 2);
   Gpio gpioOutRelSprawny = Gpio(GPIOE, 5);
   Gpio gpioOutSygnAkust  = Gpio(GPIOE, 4);
   Gpio gpioOutBuzer      = Gpio(GPIOA, 11);

   Gpio gpioWlacz230Zamknij = Gpio(GPIOC, 6);
   Gpio gpioWlacz230Otworz = Gpio(GPIOC, 9);
   Gpio gpioWlaczBypass = Gpio(GPIOC, 7);
   Gpio gpioWlaczInwerter = Gpio(GPIOA, 8);
//   Gpio gpioWlaczZasNaped = Gpio(GPIOC, 8);


   Gpio gpioWyj1H = Gpio(GPIOD, 15);
   Gpio gpioWyj2H = Gpio(GPIOD, 14);
   Gpio gpioWyj3H = Gpio(GPIOE, 9);
   Gpio gpioWyj4H = Gpio(GPIOE, 11);
   Gpio gpioWyj1L = Gpio(GPIOD, 13);
   Gpio gpioWyj2L = Gpio(GPIOD, 12);
   Gpio gpioWyj3L = Gpio(GPIOE, 8);
   Gpio gpioWyj4L = Gpio(GPIOE, 10);

   Gpio gpioInBtnBACK = Gpio(GPIOC, 4);
   Gpio gpioInBtnLEFT = Gpio(GPIOB, 0);
   Gpio gpioInBtnRIGHT = Gpio(GPIOB, 1);
   Gpio gpioInBtnENTER = Gpio(GPIOB, 2);

   Gpio gpioInKrancZamkniete = Gpio(GPIOB, 15);
   Gpio gpioInKrancOtwarte = Gpio(GPIOB, 14);
   Gpio gpioInZakazZamykania = Gpio(GPIOB, 13);
   Gpio gpioInZakazOtwierania = Gpio(GPIOB, 12);
   Gpio gpioInOtworz = Gpio(GPIOE, 15);
   Gpio gpioInKluczI = Gpio(GPIOE, 14);
   Gpio gpioInKluczII = Gpio(GPIOE, 13);
   Gpio gpioInPozar = Gpio(GPIOE, 12);
   Gpio gpioInAlarmAkust = Gpio(GPIOA, 7);
   Gpio gpioInSiec230VAC = Gpio(GPIOA, 6);
   Gpio gpioInRezerwa1 = Gpio(GPIOA, 5);
   Gpio gpioInRezerwa2 = Gpio(GPIOA, 4);

   Gpio gpioLcdReset = Gpio(GPIOC, 5);
   Gpio gpioLcdBackLight = Gpio(GPIOE, 2);

   Gpio gpioUart1Dir = Gpio(GPIOA, 12);
   Gpio gpioUart1Tx = Gpio(GPIOB, 6);
   Gpio gpioUart1Rx = Gpio(GPIOB, 7);

   Gpio gpioUart2Dir = Gpio(GPIOD, 4);
   Gpio gpioUart2Tx = Gpio(GPIOD, 5);
   Gpio gpioUart2Rx = Gpio(GPIOD, 6);

   Gpio gpioSCL = Gpio(GPIOB, 10);
   Gpio gpioSDA = Gpio(GPIOB, 11);

   Led ledPozar     = Led(GPIOA, 0);
   Led ledAwaria  = Led(GPIOA, 1);
   Led ledGotowosc  = Led(GPIOA, 2);

   Gpio stacyjka    = Gpio(GPIOD, 8);
   Gpio keyDown     = Gpio(GPIOD, 9);
   Gpio keyStop     = Gpio(GPIOD, 10);
   Gpio keyUp       = Gpio(GPIOD, 11);

   Gpio gpioOVCdetect = Gpio(GPIOA, 15);

   // to jest w Hardware/ADC
   //  Gpio gpioPomSygnL = Gpio(GPIOF, 10);
   //  Gpio gpioPomW1 = Gpio(GPIOC, 0);
   //  Gpio gpioPomW2 = Gpio(GPIOC, 1);
   //  Gpio gpioPomW3 = Gpio(GPIOC, 2);
   //  Gpio gpioPomW4 = Gpio(GPIOC, 3);
   //  Gpio gpioPomCurr = Gpio(GPIOA, 3);




#endif


  void init(){

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN
        |RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN
        |RCC_AHBENR_GPIOEEN | RCC_AHBENR_GPIOFEN
        ;
    READ_REG_AND_FORGET(RCC->AHBENR);

    gpioOutPozar.setup( Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::LowSpeed);
    gpioOutZamkniete.setupFromClone(&gpioOutPozar);
    gpioOutOtwarte.setupFromClone(&gpioOutPozar);
    gpioOutRelSprawny.setupFromClone(&gpioOutPozar);
    gpioOutSygnAkust.setupFromClone(&gpioOutPozar);
    gpioOutBuzer.setupFromClone(&gpioOutPozar);

    gpioWlacz230Zamknij.setupFromClone(&gpioOutPozar);
    gpioWlacz230Otworz.setupFromClone(&gpioOutPozar);
    gpioWlaczBypass.setupFromClone(&gpioOutPozar);
    gpioWlaczInwerter.setupFromClone(&gpioOutPozar);
    //gpioWlaczZasNaped.setupFromClone(&gpioOutPozar);

    // sterowanie silnikiem i hamulcem samo zainicjuje gpio
//    gpioWyj1H.setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
//    gpioWyj2H.setupFromClone(&gpioWyj1H);
//    gpioWyj3H.setupFromClone(&gpioWyj1H);
//    gpioWyj4H.setupFromClone(&gpioWyj1H);
//    gpioWyj1L.setupFromClone(&gpioWyj1H);
//    gpioWyj2L.setupFromClone(&gpioWyj1H);
//    gpioWyj3L.setupFromClone(&gpioWyj1H);
//    gpioWyj4L.setupFromClone(&gpioWyj1H);

    gpioInBtnBACK.setup(Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::LowSpeed);
    gpioInBtnLEFT.setupFromClone(&gpioInBtnBACK);
    gpioInBtnRIGHT.setupFromClone(&gpioInBtnBACK);
    gpioInBtnENTER.setupFromClone(&gpioInBtnBACK);

    gpioInKrancZamkniete.setupFromClone(&gpioInBtnBACK);
    gpioInKrancOtwarte.setupFromClone(&gpioInBtnBACK);

    gpioInZakazZamykania.setupFromClone(&gpioInBtnBACK);
    gpioInZakazOtwierania.setupFromClone(&gpioInBtnBACK);
    gpioInOtworz.setupFromClone(&gpioInBtnBACK);
    gpioInKluczI.setupFromClone(&gpioInBtnBACK);
    gpioInKluczII.setupFromClone(&gpioInBtnBACK);
    gpioInPozar.setupFromClone(&gpioInBtnBACK);
    gpioInAlarmAkust.setupFromClone(&gpioInBtnBACK);
    gpioInSiec230VAC.setupFromClone(&gpioInBtnBACK);
    gpioInRezerwa1.setupFromClone(&gpioInBtnBACK);
    gpioInRezerwa2.setupFromClone(&gpioInBtnBACK);

    stacyjka.setupFromClone(&gpioInBtnBACK);
    keyDown.setupFromClone(&gpioInBtnBACK);
    keyStop.setupFromClone(&gpioInBtnBACK);
    keyUp.setupFromClone(&gpioInBtnBACK);



    // zainicjowane w HMI
//    ledAwaria.init(Led::Type::ACTIVEHIGH, Led::Mode::SWIECI );
//    ledPracaAku.init(Led::Type::ACTIVEHIGH, Led::Mode::SWIECI );
//    ledOK.init(Led::Type::ACTIVEHIGH, Led::Mode::SWIECI );

  }

};

extern Pinout * pins;

#endif /* PINOUT_H_ */

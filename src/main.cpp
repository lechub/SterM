/*

 */

// ----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"


#include "QuickTask.h"
#include "Hardware.h"
#include "Sterownik.h"
#include "Pinout.h"
#include "Keyboard.h"
#include "HMI.h"
#include "ST7032iFB.h"
#include "I2C.h"
#include "Hamulec.h"
#include "Silnik24VDC.h"
#include "VEprom.h"
#include "Praca.h"

// ----------------------------------------------------------------------------


void hBridgePoll();

Pinout pinout = Pinout();
Pinout * pins = &pinout;

Keyboard keys = Keyboard(&pinout.gpioInBtnBACK, &pinout.gpioInBtnLEFT, &pinout.gpioInBtnRIGHT, &pinout.gpioInBtnENTER);

HBridge hBridgeHamulec  = HBridge(&pinout.gpioWyj1H, &pinout.gpioWyj1L, &pinout.gpioWyj2H, &pinout.gpioWyj2L);
HBridge hBridgeSilnik   = HBridge(&pinout.gpioWyj3H, &pinout.gpioWyj3L, &pinout.gpioWyj4H, &pinout.gpioWyj4L);

Hamulec hamulec = Hamulec(&hBridgeHamulec);

Silnik24VDC silnik24VDC = Silnik24VDC(&hBridgeHamulec, &hBridgeSilnik);
Silnik230VAC silnik230VAC = Silnik230VAC(&pinout.gpioWlacz230Otworz, &pinout.gpioWlacz230Zamknij);

I2C_TypeDef * const I2C_FOR_LCD = I2C2;
I2C::DeviceDefs i2cDefs;
I2C * i2c = I2C::getInstance();
ST7032iFB st7032iDriver = ST7032iFB();

Sterownik sterBramDym = Sterownik(&silnik24VDC, &silnik230VAC, &hamulec);
Sterownik * sterM = &sterBramDym;

Praca job = Praca();
Praca *praca = &job;

//volatile uint8_t lcdFrameBuffer[ST7032iFB::LCD_COL][ST7032iFB::LCD_ROW];
//FrameBuffer lcd = FrameBuffer(ST7032iFB::LCD_COL, ST7032iFB::LCD_ROW, (uint8_t*)lcdFrameBuffer);

Menu menu = Menu();


/***
 * procedura sterowania mostkami H. docelowo przenieść do przerwania (co 200us)
 */
void hBridgePoll(){
  hBridgeHamulec.poll();
  hBridgeSilnik.poll();
}


/** Wywolanie metody monitor() */
void static inline keyb_callback(){  keys.co10ms(); }

// periodycznie wykonywana funkcja monitor() opakowana w aku_callback()
QuickTask keybQtsk(QuickTask::QT_PERIODIC, keyb_callback, Keyboard::TIME_PERIOD_KEYB_MS);

// periodycznie wykonywana funkcja monitor() opakowana w aku_callback()
QuickTask hBridgeQtsk(QuickTask::QT_PERIODIC, hBridgePoll, HBridge::TIME_POLL_PERIOD_MS);


// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wunused-parameter"
//#pragma GCC diagnostic ignored "-Wmissing-declarations"
//#pragma GCC diagnostic ignored "-Wreturn-type"



//int main(int argc, char* argv[]) {
void main(void) {
  // At this stage the system clock should have already been configured
  // at high speed.

  QuickTask::hold(true);

  Hardware::init();

  pins->init();

  QuickTask::delayMsWithStoppedTasks(100);
  VEprom::init();

  //  for(int i = 0; i < 5; i++ ){
  //    pins.ledAlarm.toggleOutput();
  //    pins.ledPracaAku.toggleOutput();
  //    QuickTask::delayMsWithStoppedTasks(1000);
  //  }

  {
    i2cDefs.base = I2C_FOR_LCD;
    i2cDefs.i2cFreqkHz = 100;
    i2cDefs.sda = &pins->gpioSDA;
    i2cDefs.scl = &pins->gpioSCL;
    i2c->init(&i2cDefs);
    st7032iDriver.init(i2c, &pins->gpioLcdBackLight, &pins->gpioLcdReset);
  }

  HMI * hmi = HMI::getInstance();
  hmi->init(sterM, &keys, &st7032iDriver, &menu, &pins->ledPozar, &pins->ledAwaria1, &pins->ledGotowosc);

  hmi->lcd->clearScreen();

  // hmi->lcd->lcd_ON();

  hmi->lcd->cursorMode(FrameBuffer::CursorMode::HIDDEN);

  sterM->init();
  praca->init();

  //  if (!Parameter::initEepromMemory()){
  //    //---------------------->1234567890123456<
  //    hmi->lcd->printXY(1,0,  "  BLAD EEPROM!  ");
  //    while(true){;}
  //  }

  //---------------------->1234567890123456<
//  hmi->lcd->printXY(0,0,  " Sterownik bram ");//  i2c->dirtyDelayMs(500);
//  hmi->lcd->printXY(0,1,  "pozarowych v.1.0");
//  QuickTask::delayMsWithActiveTasks(2000);

  hmi->menu->goToEkran(Menu::EKRAN::e_INIT);
  QuickTask::hold(false);
  do{
    Hardware::WDOG_Reload();          // przeladowanie Watch-doga
    QuickTask::poll();                // przegląd i uruchamianie tasków
    //komunikacja();  // w tle, bez opoznien

  } while (true);     // do konca swiata
}

#ifdef  USE_FULL_ASSERT

/**
 * @brief  The assert_param macro is used for function's parameters check.
 * @param  expr: If expr is false, it calls assert_failed function which reports
 *         the name of the source file and the source line number of the call
 *         that failed. If expr is true, it returns no value.
 * @retval None
 */
#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
void assert_failed(uint8_t* file, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------

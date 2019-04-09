/*

 */

// ----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"


#include "QuickTask.h"
#include "Hardware.h"
#include "Sterownik.h"
#include "HMI.h"
#include "Hamulec.h"
#include "Silnik24VDC.h"
#include "VEprom.h"
#include "Praca.h"
#include "ConnectionZB40.h"
#include "ConnectionAux.h"

// ----------------------------------------------------------------------------


void hBridgePoll();

Pinout pinout = Pinout();
Pinout * pins = &pinout;

Keyboard keys = Keyboard(&pinout.gpioInBtnBACK, &pinout.gpioInBtnLEFT, &pinout.gpioInBtnRIGHT, &pinout.gpioInBtnENTER);

Front front = Front(&pins->keyUp, &pins->keyStop, &pins->keyDown, &pins->stacyjka);

HBridgeGPIO hBridgeHamulec  = HBridgeGPIO(&pinout.gpioWyj1H, &pinout.gpioWyj1L, &pinout.gpioWyj2H, &pinout.gpioWyj2L);
//HBridgeGPIO hBridgeSilnik   = HBridgeGPIO(&pinout.gpioWyj3H, &pinout.gpioWyj3L, &pinout.gpioWyj4H, &pinout.gpioWyj4L);
HBridgePWM hBridgeSilnik   = HBridgePWM();

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

Menu menu = Menu();

//RS485::InitStruct rsIS = { USART1, RS485::USART_DEFAULT_BAUD_RATE};

RS485 rsZB40 = RS485();
RS485 rsAux = RS485();

ConnectionZB40 conZB40 = ConnectionZB40(&rsZB40);
ConnectionAux conAux = ConnectionAux(&rsAux);

ConnectionZB40 * zb40 = &conZB40;

///** realizacja komunikacji ZB-40 i zewnętrznej */
void static inline rs485_callback(){
  conZB40.poll();
  conAux.poll();
}

//// periodycznie wykonywana funkcja rs485_callback() opakowana w aku_callback()
QuickTask connectQtsk(QuickTask::QT_PERIODIC, rs485_callback, 13);

/***
 * procedura sterowania mostkami H. docelowo przenieść do przerwania (co 200us)
 */
void hBridgePoll(){
  hBridgeHamulec.poll();
  hBridgeSilnik.poll();
}



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

  QuickTask::hold(true);

  Hardware::init();

  pins->init();

  QuickTask::delayMsWithStoppedTasks(100);
  VEprom::init();

  {
    i2cDefs.base = I2C_FOR_LCD;
    i2cDefs.i2cFreqkHz = 100;
    i2cDefs.sda = &pins->gpioSDA;
    i2cDefs.scl = &pins->gpioSCL;
    i2c->init(&i2cDefs);
    st7032iDriver.init(i2c, &pins->gpioLcdBackLight, &pins->gpioLcdReset);
  }

  HMI * hmi = HMI::getInstance();
  hmi->init(sterM, &keys, &front, &st7032iDriver, &menu, &pins->ledPozar, &pins->ledAwaria, &pins->ledGotowosc);

  hmi->lcd->clearScreen();

  // hmi->lcd->lcd_ON();

  hmi->lcd->cursorMode(FrameBuffer::CursorMode::HIDDEN);

  sterM->init();
  praca->init();

  rsZB40.setup(USART2, RS485::USART_ZB40_BAUD_RATE);
  rsAux.setup(USART1, RS485::USART_DEFAULT_BAUD_RATE);

  hmi->menu->goToEkran(Menu::EKRAN::e_INIT);
  QuickTask::hold(false);   // uruchomienie QuickTask-ów
  do{
    Hardware::WDOG_Reload();          // przeladowanie Watch-doga
    QuickTask::poll();                // przegląd i uruchamianie tasków
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

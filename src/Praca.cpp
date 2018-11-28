/*
 * Praca.cpp
 *
 *  Created on: 20 lis 2018
 *      Author: lechu
 */

#include "Praca.h"
#include "QuickTask.h"

void  pracaCallback();
/** Wywolanie metody monitor() */
void  pracaCallback(){
  praca->poll();
}

// periodycznie wykonywana funkcja monitor() opakowana w aku_callback()
QuickTask pracaQtsk(QuickTask::QT_PERIODIC, pracaCallback, Praca::TIME_POLL_PERIOD_MS);


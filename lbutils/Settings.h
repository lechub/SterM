/*
 * Settings.h
 *
 *  Created on: 26 lut 2019
 *      Author: lechu
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "systemDefs.h"

// użyty procesor: STM32F072V8T6
// ram:   16kB
// flash: 64kB z czego 4kB jest na eeprom

#define EEPROM_START_ADDRESS ((uint32_t)0x0800F000)

#define PROG_MAIN_VERSION 2
#define PROG_SUB_VERSION 1

static const uint16_t SOTWARE_VERSION = PROG_MAIN_VERSION * 1000 + PROG_SUB_VERSION;

static const uint32_t OVC_SILENCE_MS = 2000;

static const uint16_t MAX_OPOZN_ZAMK_POZ_sek = 2 * 60;  // maksymalnie 2 minuty opoznienia

static inline uint16_t getProgMainVersion(){ return PROG_MAIN_VERSION; }
static inline uint16_t getProgSubVersion(){ return PROG_SUB_VERSION; }

#endif /* SETTINGS_H_ */

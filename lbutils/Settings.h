/*
 * Settings.h
 *
 *  Created on: 26 lut 2019
 *      Author: lechu
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "systemDefs.h"

#define PROG_MAIN_VERSION 2
#define PROG_SUB_VERSION 1

static constexpr uint16_t SOTWARE_VERSION = PROG_MAIN_VERSION * 1000 + PROG_SUB_VERSION;

static inline uint16_t getProgMainVersion(){ return PROG_MAIN_VERSION; }
static inline uint16_t getProgSubVersion(){ return PROG_SUB_VERSION; }

#endif /* SETTINGS_H_ */

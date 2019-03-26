/*
 * PkkbSlave.cpp
 *
 *  Created on: 13 lut 2014
 *      Author: lechu
 */

#include "PkkbSlave.h"

uint8_t __attribute__((weak)) readIndex(uint8_t index) { return 0; }

bool __attribute__((weak)) writeIndex(uint8_t index, uint8_t val) {	return false; }

//uint8_t __attribute__((weak)) getDeviceType(){ return 0;}

//uint8_t __attribute__((weak)) getDeviceSubType(){ return 0;}

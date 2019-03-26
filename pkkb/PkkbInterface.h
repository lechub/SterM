/*
 * PkkbInterface.h
 *
 *  Created on: 28 maj 2015
 *      Author: lechu
 */

#ifndef PKKBINTERFACE_H_
#define PKKBINTERFACE_H_

#include <stdint.h>


class PkkbInterface {
public:
	virtual void setAdres(uint8_t) = 0;
	virtual uint8_t getAdres() = 0;
	//virtual ~PkkbInterface();
};

#endif /* PKKBINTERFACE_H_ */

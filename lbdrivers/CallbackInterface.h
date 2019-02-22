/*
 * I2CCallback.h
 *
 *  Created on: 21 lut 2019
 *      Author: lechu
 */

#ifndef CALLBACKINTERFACE_H_
#define CALLBACKINTERFACE_H_

class CallbackInterface {
public:
  virtual void callbackFunction() = 0;
  virtual ~CallbackInterface(){};
};

#endif /* CALLBACKINTERFACE_H_ */

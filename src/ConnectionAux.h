/*
 * ConnectionAux.h
 *
 *  Created on: 22 mar 2019
 *      Author: lechu
 */

#ifndef CONNECTIONAUX_H_
#define CONNECTIONAUX_H_

#include "RS485.h"

class ConnectionAux {
private:
  RS485 * port = nullptr;

public:

  ConnectionAux(RS485 * portRS485) :port(portRS485) { ; }

  void poll(){
    ;
  }
};

#endif /* CONNECTIONAUX_H_ */

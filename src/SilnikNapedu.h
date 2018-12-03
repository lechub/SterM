/*
 * SilnikNapedu.h
 *
 *  Created on: 15 lis 2018
 *      Author: lechu
 */

#ifndef SILNIKNAPEDU_H_
#define SILNIKNAPEDU_H_

class SilnikNapedu {
protected:
  bool safePosition = true;
public:
  typedef enum{
    UP,     // naped idzie do gory
    DOWN,   // naped idzie w dol
    FLOAT,  // naped bez zasilania
    HOLD_UP,  // oba bieguny na plusie
    HOLD_DOWN,  // oba bieguny na minusie
  }MOVE;

 virtual void setMove(MOVE newMove) = 0;
 virtual MOVE getMove() = 0;
 virtual bool gotoSafePosition(bool enable) = 0;

 bool isInSafePosition(){ return safePosition; }

 virtual ~SilnikNapedu(){}
};

#endif /* SILNIKNAPEDU_H_ */

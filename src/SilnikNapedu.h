/*
 * SilnikNapedu.h
 *
 *  Created on: 15 lis 2018
 *      Author: lechu
 */

#ifndef SILNIKNAPEDU_H_
#define SILNIKNAPEDU_H_

class SilnikNapedu {

public:


  typedef enum{
    UP,     // naped idzie do gory
    DOWN,   // naped idzie w dol
    FLOAT,  // naped bez zasilania
    HOLD_UP,  // oba bieguny na plusie
    HOLD_DOWN,  // oba bieguny na minusie
  }MOVE;

protected:
  volatile bool safePosition = true;
  volatile MOVE move = MOVE::FLOAT;

public:

  virtual void setMove(MOVE newMove) = 0;
  virtual inline MOVE getMove() const{
    return move;
  }
  //virtual void ovcHold() = 0;

  virtual inline bool gotoSafePosition(bool enable){
    safePosition = enable;
    if (safePosition) setMove(MOVE::FLOAT);
    return true;
  }

  inline bool isInSafePosition()const{ return safePosition; }

  virtual ~SilnikNapedu(){}
};

#endif /* SILNIKNAPEDU_H_ */

/*
 * ConnectionZB40.h
 *
 *  Created on: 22 mar 2019
 *      Author: lechu
 */

#ifndef CONNECTIONZB40_H_
#define CONNECTIONZB40_H_

#include "RS485.h"
#include "PkkbMaster.h"
#include "PkkbL2.h"
#include "PkkbL1.h"
#include "QuickTask.h"
#include "KBZB40.h"

class ConnectionZB40 {
public:
  static constexpr uint32_t FRAME_INTERVAL_MS = 1000; // co sekunde transmisja
  static constexpr uint32_t MAX_CONNECT_TRY = 3; // tyle utraconych ramek oznacza brak łączności
  static constexpr uint32_t GET_INDEX_FRAME_LENGTH = 10; // tyle utraconych ramek oznacza brak łączności

private:
  RS485 * port = nullptr;
  uint32_t lastFrame = 0;
  uint8_t ZB40ExistTry = 0;
  bool ZB40Necessary = true;
  //PkkbMaster master = PkkbMaster();
  PkkbL2 pkkbL2 = PkkbL2();

  bool receiveZB40Status(){
    if (port->isEmpty()){
      if (ZB40ExistTry < MAX_CONNECT_TRY) ZB40ExistTry++;
      return false;
    }
    {
      uint8_t buff1[GET_INDEX_FRAME_LENGTH];
      Fifo frame1 = Fifo(buff1, GET_INDEX_FRAME_LENGTH);
      uint8_t buff2[GET_INDEX_FRAME_LENGTH];
      Fifo frame2 = Fifo(buff2, GET_INDEX_FRAME_LENGTH);

      while (!port->isEmpty()){
        PkkbL1::decodeByte(&frame1, (uint8_t)port->get());
      }
      PkkbL2::PKKB_Header1 header;
      pkkbL2.makeHeaderP2P(&header);
      pkkbL2.decodeFrame(&frame1, &frame2, &header);
    }
    return true;
  }

  bool sendForZB40Status(){
    {
      uint8_t buff1[GET_INDEX_FRAME_LENGTH];
      Fifo frame1 = Fifo(buff1, GET_INDEX_FRAME_LENGTH);
      uint8_t buff2[GET_INDEX_FRAME_LENGTH];
      Fifo frame2 = Fifo(buff2, GET_INDEX_FRAME_LENGTH);

      if (!PkkbMaster::makeFrameReadIndex(&frame1, KBZB40::IndexMap::INDEX_AKU_STATUS)){//Pkkb::IndexMapTmplt::INDEX_DEVICE_TYPE
        return false;
      }
      PkkbL2::PKKB_Header1 header;
      pkkbL2.makeHeaderP2P(&header);
      //    pkkbL2.setAdresZdalny(Pkkb::PKKB_BROADCAST_ADRESS);
      if (!pkkbL2.encodeFrame(&frame1, &frame2, &header)) return false;
      frame1.flush();
      if (!PkkbL1::encodeFrame(&frame2, &frame1)) return false;
      port->putAll(&frame1); // wysylanie ramki PKKB z pytaniem
    }
    return true;
  }

public:

  ConnectionZB40(RS485 * portRS485) :port(portRS485) { ; }

  void poll(){
    if (!ZB40Necessary) return;

    if (QuickTask::isOlderThanMs(lastFrame, 1000)){
      lastFrame = QuickTask::getCounter();
      receiveZB40Status();
      sendForZB40Status();
    }
  }

  inline bool isZB40Online()const{ return ZB40ExistTry < MAX_CONNECT_TRY; }

  inline void setZB40Cooperation(bool enable){
    ZB40Necessary = enable;
    port->flush();
  }

};

#endif /* CONNECTIONZB40_H_ */

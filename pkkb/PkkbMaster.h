/*
 * PkkbMaster.h
 *
 *  Created on: 13 lut 2014
 *      Author: lechu
 */

#ifndef PKKBMASTER_H_
#define PKKBMASTER_H_

#include "Pkkb.h"
#include "Fifo.h"

class PkkbMaster {

public:

	static bool makeFrameReadIndex(Fifo *result, uint8_t indexNr){
		if (!result->put(Pkkb::PkkbCommands::READ)) return false;
		if (!result->put(indexNr)) return false;
		return true;
	}

	static bool makeFrameReadIndexBlock(Fifo *result, uint8_t indexNr, uint8_t blockLength){
		if (!result->put(Pkkb::PkkbCommands::READBLOCK)) return false;
		if (!result->put(indexNr)) return false;
		if (!result->put(blockLength)) return false;
		return true;
	}

	static bool makeFrameWriteIndex(Fifo *result, uint8_t indexNr, uint8_t data){
		if (!result->put(Pkkb::PkkbCommands::WRITE)) return false;
		if (!result->put(indexNr)) return false;
		if (!result->put(data)) return false;
		return true;
	}

	static bool makeFrameWriteIndexBlock(Fifo *dane, Fifo * result, uint8_t indexNr){
		if (!result->put(Pkkb::PkkbCommands::WRITEBLOCK)) return false;
		if (!result->put(indexNr)) return false;
		if (!result->put((uint8_t)dane->get_count())) return false;
		if (!result->putAll(dane)) return false;
		return true;
	}

	static bool makeFrameReadString(Fifo *result, uint8_t strNr){
		if (!result->put(Pkkb::PkkbCommands::READSTRING)) return false;
		if (!result->put(strNr)) return false;
		return true;
	}

	static bool makeFrameWriteString(Fifo *dane, Fifo *result, uint8_t strNr){
		if (!result->put(Pkkb::PkkbCommands::WRITESTRING)) return false;
		if (!result->put(strNr)) return false;
		if (!result->putAll(dane)) return false;
		return true;
	}

};



#endif /* PKKBMASTER_H_ */

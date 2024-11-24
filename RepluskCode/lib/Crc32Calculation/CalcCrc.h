/*
 * CalcCrc.h
 *
 *  Created on: 24.05.2021
 *      Author: harald
 * 
 * Crc calculation class.
 * Adapt DEFAULT_CRC32_POLY to your needs.
 * 
 * usage:
 * uint32_t CRC_Calculate(uint32_t pBuffer[], uint32_t BufferLength){
 *   CalcCrc calcCrc;
 *   calcCrc.resetCrc(); // Set default Crc initvalue
 *   // Update crc with 4 bytes at a time
 *   for (uint32_t i = 0U; i < BufferLength; i++) {
 *     calcCrc.calcCrcUpdate(pBuffer[i]);
 *   }
 *   return calcCrc.GetActCrc();
 * }
 */

#ifndef CALCCRC_H_
#define CALCCRC_H_

#include <stdint.h>
#include <vector>

using namespace std;

class CalcCrc {
public:
  CalcCrc() : actCrc{0} {}
  virtual ~CalcCrc(){};
	 uint32_t GetActCrc() { return actCrc; };

  void resetCrc(void) { actCrc = DEFAULT_CRC_INITVALUE; };

  uint32_t calcCrcUpdate(uint32_t data) {
    uint8_t i;
    actCrc = actCrc ^ data;
    for (i = 0; i < 32; i++) {
      if (actCrc & 0x80000000)
        actCrc = (actCrc << 1) ^ DEFAULT_CRC32_POLY;
      else
        actCrc <<= 1;
    }
    return actCrc;
  };

  uint32_t crcAccumulate(uint32_t pBuffer[], uint32_t BufferLength) {
    uint32_t i;
    for (i = 0U; i < BufferLength; i++) {
      calcCrcUpdate(pBuffer[i]);
    }
    return actCrc;
  };

  uint32_t crcCalculate(vector<uint32_t> buffer){
    resetCrc();
    for (uint32_t item : buffer) {
      calcCrcUpdate(item);
    }
    return actCrc;
  };

  uint32_t crcCalculate(uint32_t pBuffer[], uint32_t BufferLength) {
    resetCrc();
    for (uint32_t i = 0U; i < BufferLength; i++) {
      calcCrcUpdate(pBuffer[i]);
    }
    return actCrc;
  }

private:
  uint32_t actCrc; // current crc-value
  static constexpr uint32_t DEFAULT_CRC32_POLY = 0x04C11DB7U;
  static constexpr uint32_t DEFAULT_CRC_INITVALUE = 0xFFFFFFFFU;
};

#endif /* CALCCRC_H_ */

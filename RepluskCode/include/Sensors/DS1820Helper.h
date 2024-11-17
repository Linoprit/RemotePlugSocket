#ifndef DS1820_HELPER_H
#define DS1820_HELPER_H

#include <DallasTemperature.h>
#include <OneWire.h>
#include <AppTypes.h>


#pragma once

namespace msmnt {

class Ds1820Helper {
public:
  //Ds1820Helper(){};
  Ds1820Helper(uint8_t oneWirePinOut, uint8_t resolution);
  virtual ~Ds1820Helper(){};

  void InitHardware();
  void Cycle();

private:
  OneWire _oneWire;
  DallasTemperature _sensors;
  uint8_t _resolution;
  //ToDo: store IDs somwhere else later
  SensorId _sensorIds[4]; 
};

} // namespace msmnt
#endif
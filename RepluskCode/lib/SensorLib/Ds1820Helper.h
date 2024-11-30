#ifndef DS1820HELPER_H
#define DS1820HELPER_H

// #pragma once

#include "Measurement.h"
#include "MeasurementPivot.h"
#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

namespace msmnt {

class Ds1820Helper {
public:
  Ds1820Helper(Measurement::SensorChannel channel, uint8_t onwWirePin,
               uint8_t resolution, MeasurementPivot *measurementPivot);
  virtual ~Ds1820Helper(){};

  void initHardware();
  void cycle();

private:
  OneWire _oneWireCh;
  DallasTemperature _sensorsCh;  
  Measurement::SensorChannel _channel;
  MeasurementPivot *_measurementPivot;
  uint8_t _resolution;
};
} // namespace msmnt
#endif
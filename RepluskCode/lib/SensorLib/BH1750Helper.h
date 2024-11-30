#ifndef BH1750_HELPER_H
#define BH1750_HELPER_H

#pragma once

#include <BH1750.h>
#include <Measurement.h>
#include <MeasurementPivot.h>

namespace msmnt {

class BH1750Helper {
public:
  BH1750Helper(Measurement::SensorChannel channel,
               MeasurementPivot *measurementPivot);
  virtual ~BH1750Helper(){};

  void InitHardware();
  void Cycle();

private:
  BH1750 *_lightMeter;
  Measurement::SensorChannel _channel;
  MeasurementPivot *_measurementPivot;
  Measurement::SensorId _lightMeterId;
};
} // namespace msmnt

#endif

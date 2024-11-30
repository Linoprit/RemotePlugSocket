#ifndef APPLICATION_SENSORS_SENSORS_H_
#define APPLICATION_SENSORS_SENSORS_H_

#include "MeasurementPivot.h"
#include <Arduino.h>
#include <Ds1820Helper.h>
#include <BH1750Helper.h>


namespace msmnt {

class Sensors {
public:
  Sensors();
  virtual ~Sensors(){};

  void init(void);
  void initHardware(void);
  static Sensors &instance(void);

  void cycle(void);
  MeasurementPivot *getMeasurementPivot() { return &_measurementPivot; };
  bool saveSensorIdTable();

private:
  MeasurementPivot _measurementPivot;
  
  BH1750Helper _bh1750;
  Ds1820Helper _ds1820Ch1;

  uint_fast8_t _updateCount;
};

} // namespace msmnt
#endif
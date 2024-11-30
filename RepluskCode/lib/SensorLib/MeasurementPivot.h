#ifndef MEASUREMENTARRAY_H
#define MEASUREMENTARRAY_H

#pragma once

#include "Measurement.h"

namespace msmnt {

typedef std::array<Measurement, MAX_SENSORS> MeasurementArray;

/* We assure:
    - After the first invalid sensId there are no valid sensIds anymore.
    - A sensorId is unique in the array.
*/
class MeasurementPivot {
public:
  enum SensorKindType {
    BME_TEMP_76 = 1,
    BME_HUMIDITY_76 = 2,
    BME_PRESS_76 = 3,
    BME_TEMP_77 = 4,
    BME_HUMIDITY_77 = 5,
    BME_PRESS_77 = 6,
    BH_LIGHT_23 = 7,
    BH_LIGHT_5C = 8,

    RELAY_01 = 10,
    RELAY_02 = 11,
  };

  MeasurementPivot(){
    _indexIter = 0;
  };
  ~MeasurementPivot(){};

  Measurement *GetMeasurement(uint8_t index) {
    return &_measurementArray.at(index);
  };

  // Copys the values from config into the corresponding Measurement.
  // Returns false, if sensId is not found.
  // The flag 'configChanged' will be set. If the config table will be
  // saved to FS later, then this value will be considered.
  bool UpdateConfig(SensorConfigType config);

  // Needs to be called after init, to avoid saving configs, 
  // that hasnt been touched. 
  void ResetConfigChangedFlags();

  // Generates an Id for Relay, or I2C-sensors (i.e. BME), based on the MAC
  // address.
  static Measurement::SensorId CreateId(SensorKindType sensorAddressType);
  // Prints the contents of the whole Array to serial
  void Dump();
  // Looks for the corresponding measurements, adds the value in the rotating
  // values-array and updates the meanValue. If the Id wasn't found, false
  // will be returned.
  bool UpdateValue(Measurement::SensorId sensId, float value);
  // Looks for the next free slot and stores the sensId there.
  // Returns true, if a free slot was found. If the Id was already in the array,
  // false is returned, and the Id is not stored (avoid double entries).
  bool StoreSensId(
      Measurement::SensorId sensId, Measurement::SensorChannel sensorChannel,
      Measurement::SensorType sensorType = Measurement::SensorType::TEMP);
  bool StoreSensId(
      Measurement::SensorIdArray sensIdArray,
      Measurement::SensorChannel sensorChannel,
      Measurement::SensorType sensorType = Measurement::SensorType::TEMP);
  // Sets the iterator to zero. The iterator is used by all GetNext* functions.
  void ResetIter() { _indexIter = 0; };
  // Returns the pointer to the measurement, where _indexIter points to.
  // _indexIter is automatically incremented.
  Measurement *GetNextMeasurement();
  // Looks for the first empty element in the array.
  // Returns nullptr, if there is no free slot.
  Measurement *FindNextFreeSlot();
  // Looks for a given address and returns a pointer to the measurement.
  // Returns nullptr, if address wasn't found.
  Measurement *FindSensIdOrEmpty(Measurement::SensorId sensId);

private:
  MeasurementArray _measurementArray;
  uint_fast8_t
      _indexIter; // points to the next Measurement, returned by GetNext*
};

} // namespace msmnt
#endif // MEASUREMENTARRAY_H
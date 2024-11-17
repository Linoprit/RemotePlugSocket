/*
 * AppTypes.h
 *
 *  Created on: 15.04.2021
 *      Author: harald
 */

#ifndef FRAMEWORK_TYPES_APPTYPES_H_
#define FRAMEWORK_TYPES_APPTYPES_H_

#include <cstring>
#include <stdint.h>

using namespace std;

constexpr int _SUCCESS_ = 0;
constexpr int _FAIL_ = -1;

constexpr bool _on_ = true;
constexpr bool _off_ = false;

static constexpr uint16_t SENS_ID_LEN = 8; // DS1820 addr-len in bytes

enum SensorType {
  TEMP = 0,
  HUMIDITY = 1,
  PRESS = 2,
  RELAY = 3,
  ILLUMINANCE = 4, // lux

};

// Is the same as DallasTemperature::DeviceAddress
typedef uint8_t SensorIdArray[SENS_ID_LEN];
// SensorIdArray cast to Integer
typedef uint64_t SensorId;

static SensorId CastArrayToSensId(uint8_t *address) {
  SensorId *temp = (SensorId *)address;
  return *temp;
}

static uint8_t *CastSensIdToArray(SensorId *sensId) {
  return (uint8_t *)sensId;
}

static bool CompareSensorId(SensorIdArray address_left,
                            SensorIdArray address_right) {
  for (uint_fast8_t i = 0; i < SENS_ID_LEN; i++) {
    if (address_left[i] != address_right[i]) {
      return false;
    }
  }
  return true;
}

#endif /* FRAMEWORK_TYPES_APPTYPES_H_ */

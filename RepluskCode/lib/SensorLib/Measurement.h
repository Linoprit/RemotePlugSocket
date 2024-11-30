#ifndef APPLICATION_SENSORS_MEASUREMENTS_H_
#define APPLICATION_SENSORS_MEASUREMENTS_H_

#include <Arduino.h>
#include <Config.h>
#include <algorithm> // std::min
#include <array>
#include <cstring>
#include <limits>
#include <math.h>
#include <stdint.h>
#include <OsHelpers.h>
#include <Logger.h>


namespace msmnt {

/* We have to make sure:
    - Every instance in initialised with INVALID_SENS_ID.
    - A measurement with INVALID_SENS_ID contains no valid data.
*/
class __attribute__((__packed__)) Measurement {
public:
  static constexpr uint16_t SHORTNAME_LEN = 8;
  static constexpr uint16_t SENS_ID_LEN = 8;
  static constexpr uint16_t INVLD_TEMPERATURE = 255;
  static constexpr uint16_t MIN_TEMP_HYSTERESIS = 1;
  static constexpr uint16_t SENSOR_ID_LEN = 8;
  static constexpr uint16_t VALUES_BUFF_LEN = 5;
  static constexpr uint64_t INVALID_SENS_ID =
      std::numeric_limits<uint64_t>::max();
  static constexpr float TEMP_INVAL_MIN = -55.0f;
  static constexpr float TEMP_INVAL_MAX = 50.0f;
  static constexpr float HUM_INVAL_MIN = 5.0f;
  static constexpr float HUM_INVAL_MAX = 99.0f;
  static constexpr float PRS_INVAL_MIN = 500.0f;
  static constexpr float PRS_INVAL_MAX = 1100.0f;

  typedef uint8_t
      SensorIdArray[SENS_ID_LEN]; // same as DallasTemperature::DeviceAddress
  typedef uint64_t SensorId;      // SensorIdArray casted to Integer

  enum SensorType {
    TEMP = 0,
    HUMIDITY = 1,
    PRESS = 2,
    RELAY = 3,
    LIGHT = 4,

  };
  enum RelayChannel {
    RL_NONE = 0,
    REL_01 = 1,
    REL_02 = 2,
  };

  // Don't forget: used in array Sensors::_isChannelActive
  enum SensorChannel {
    SC_NONE = 0,
    CH_1 = 1,  // OneWire Channel 1
    CH_2 = 2,  // OneWire Channwl 2
    I2C_1 = 3, // I2C (BME280, ...)
    I2C_2 = 4, // unused
    GPIO = 5,  // Digital Pin (Relay)
    Last = GPIO, // Marks the last entry
  };

  SensorId sensorId = INVALID_SENS_ID; // 8 bytes integer.
  float meanValue = NAN;               // Mean value of values array.
  uint32_t lastUpdateTick = 0;         // in seconds.
  float minVal = 0.0f;        // switch relay on, if actValue below this.
  float maxVal = 0.0f;        // switch relay off, if actValue above this.
  SensorType sensType = TEMP; // Which type is the sensor.
  SensorChannel sensChan =
      SC_NONE; // Where the sensor is connected to. Relevant to OW devices.
  RelayChannel relayNr = RL_NONE; // which channel to switch; 0 = no relay.
  float values[VALUES_BUFF_LEN] = {0.0f}; // last measurement i.
  uint8_t valueIndex = 0; // pos, where the next value is stored to.
  char shortname[Measurement::SHORTNAME_LEN] = {'-'}; // without \0.
  bool configChanged = false; // indicator, if config was touched after init.

  Measurement() { InitValueArray(); }
  virtual ~Measurement() {}

  bool UpdateValue(float value) {
    if (!isInRange(value)) {
      return false;
    }

    values[valueIndex] = value;
    IncrementValueIdx();

    float sum = 0.0f;
    float count = 0.0f;
    for (uint_fast8_t i = 0; i < VALUES_BUFF_LEN; i++) {
      if (isnanf(values[i])) {
        break;
      }
      sum += values[i];
      count += 1.0f;
    }
    if (count > 0.0) {
      meanValue = sum / count;
    }
    return true;
  }

  bool isInRange(float value) {
    switch (sensType) {
    case SensorType::TEMP:
      if ((value < TEMP_INVAL_MIN) || (value > TEMP_INVAL_MAX)) {
        return false;
      }
      break;
    case SensorType::HUMIDITY:
      if ((value < HUM_INVAL_MIN) || (value > HUM_INVAL_MAX)) {
        return false;
      }
      break;
    case SensorType::PRESS:
      if ((value < PRS_INVAL_MIN) || (value > PRS_INVAL_MAX)) {
        return false;
      }
      break;
    case SensorType::RELAY:
      if ((value < 0.0) || (value > 1.0)) {
        return false;
      }
      break;
    default:
      break;
    }
    return true;
  }

  bool isTimeout() {
    // timeval timeVal;
    // gettimeofday(&timeVal, NULL);
    //uint32_t curTimeSec = timeVal.tv_sec;

    uint32_t curTimeSec = OsHelpers::GetTickSeconds();
    uint32_t valueTimeSec = lastUpdateTick + MEASUREMENT_TIMEOUT_SEC;

    if (valueTimeSec < curTimeSec) {
      return true;
    }
    return false;
  }

  // Make a clean string, the maximum length of shortname-attribute.
  static std::string MakeShortname(char *shortnameChar) {
    std::string tmp =
        std::string(shortnameChar, msmnt::Measurement::SHORTNAME_LEN);
    return tmp;
  }

  static void CopyToShortname(char *oldShortname, char *newShortname) {
    CopyToShortname(oldShortname, MakeShortname(newShortname));
  }

  // Copies the newShortname into oldShortname. Removes string-terminators and
  // pads the result with spaces.
  static void CopyToShortname(char *oldShortname, std::string newShortname) {
    const uint16_t len = msmnt::Measurement::SHORTNAME_LEN;
    memset(oldShortname, ' ', len);
    uint16_t strLength = std::min((uint16_t)newShortname.length(), len);
    memcpy(oldShortname, newShortname.c_str(), strLength);
    // eliminate string terminators
    for (uint16_t i = 0; i < len; i++) {
      if (oldShortname[i] == '\0') {
        oldShortname[i] = ' ';
      }
    }
  }

  void CopyToShortname(char *newShortname) {
    CopyToShortname(shortname, newShortname);
  }

  float GetMeanValue() { return meanValue; }

  bool IsInvalidSensId() { return sensorId == INVALID_SENS_ID; }

  static bool IsInvalidSensId(SensorId id) { return id == INVALID_SENS_ID; }

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

  std::string GetShortname() { return std::string(shortname, SHORTNAME_LEN); }

  static std::string DumpSensId(SensorId sensId) {
    SensorIdArray *sensIdArray = (SensorIdArray *)&sensId;
    return DumpSensIdArray(*sensIdArray);
  }
  static std::string DumpSensIdArray(SensorIdArray sensIdArray) {
    char buffer[25];
    sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", sensIdArray[0],
            sensIdArray[1], sensIdArray[2], sensIdArray[3], sensIdArray[4],
            sensIdArray[5], sensIdArray[6], sensIdArray[7]);
    return std::string(buffer);
  }

  static std::string DumpSensChannel(SensorChannel sensChan) {
    switch (sensChan) {
    case SensorChannel::CH_1:
      return "CH_1";
      break;
    case SensorChannel::CH_2:
      return "CH_2";
      break;
    case SensorChannel::GPIO:
      return "GPIO";
      break;
    case SensorChannel::I2C_1:
      return "I2C_1";
      break;
    case SensorChannel::I2C_2:
      return "I2C_2";
      break;
    case SensorChannel::SC_NONE:
      return "NONE";
      break;
    }
    return "Error";
  }

  std::string DumpSensType(){
    return DumpSensType(sensType);
  }

  static std::string DumpSensType(SensorType sensType) {
    switch (sensType) {
    case SensorType::TEMP:
      return "TEMP";
      break;
    case SensorType::HUMIDITY:
      return "HUMID";
      break;
    case SensorType::PRESS:
      return "PRESS";
      break;
    case SensorType::RELAY:
      return "RELAY";
      break;
    case SensorType::LIGHT:
      return "LIGHT";
      break;
    }
    return "Error";
  }

  void GetConfigAsCmd(char *buffer) {
    sprintf(buffer, "setSensId %llu %.2f %.2f %i %i \"%s\"\r\n", sensorId,
            minVal, maxVal, sensType, relayNr, GetShortname().c_str());
    // setSensId 16789727009993785128 3.5 5.0 0 0 "Test 003"
  }

  void Dump() {
    Logger::Log("sensId = %llu ", sensorId);
    Logger::Log("(%s)\n", DumpSensId(sensorId).c_str());

    Logger::Log("shortname = %s", GetShortname().c_str());
    Logger::Log("\tsensType = %s\n", DumpSensType(sensType).c_str());

    Logger::Log("meanValue = %.02f", meanValue);
    Logger::Log("\tlastUpdateTick = %lu\n", lastUpdateTick);

    Logger::Log("valueIndex = %i\t\t", valueIndex);
    Logger::Log("values = ");
    for (uint_fast8_t i = 0; i < VALUES_BUFF_LEN; i++) {
      Logger::Log("%.2f ", values[i]);
    }
    Logger::Log("\nminVal = %.2f\t\t", minVal);
    Logger::Log("maxVal = %.2f\n", maxVal);
   
    Logger::Log("sensChan = %s", DumpSensChannel(sensChan).c_str());
    Logger::Log("\t\trelayNr = %i\n", relayNr);

    Logger::Log("configChanged = ");
    if (configChanged) {
      Logger::Log("true");
    } else {
      Logger::Log("false");
    }

    Logger::Log("\tisTimeout = ");
    if (isTimeout()) {
      Logger::Log("true\n");
    } else {
      Logger::Log("false\n");
    }
  }

private:
  void IncrementValueIdx() {
    valueIndex++;
    if (valueIndex >= VALUES_BUFF_LEN) {
      valueIndex = 0;
    }
  }

  void InitValueArray() {
    for (uint_fast8_t i = 0; i < VALUES_BUFF_LEN; i++) {
      values[i] = NAN;
    }
  }
};

// used to update measurement in the array
typedef struct {
  Measurement::SensorId sensorId;
  float value;
  uint32_t lastUpdateTick;
} MeasurementType;

class SensorConfigType {
public:
  Measurement::SensorId sensorId;
  float minVal;
  float maxVal;
  Measurement::SensorType sensType;
  Measurement::RelayChannel relayNr; // 0 = no relay
  char shortname[Measurement::SHORTNAME_LEN];

  void CopyToShortname(char *newShortname) {
    Measurement::CopyToShortname(shortname, newShortname);
  }
};

} // namespace msmnt
#endif /* APPLICATION_SENSORS_MEASUREMENTS_H_ */
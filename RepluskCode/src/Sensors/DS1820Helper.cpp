#include <Arduino.h>
#include <Logger.h>
#include <Sensors/DS1820Helper.h>

namespace msmnt {

using namespace std;

Ds1820Helper::Ds1820Helper(uint8_t oneWirePinOut, uint8_t resolution)
    : _oneWire(oneWirePinOut), _sensors(&_oneWire), _resolution(resolution) {
    }

void Ds1820Helper::InitHardware(void) {
  uint8_t address[8];
  uint8_t sensorCount = 0;
  _sensors.begin();
  _sensors.setResolution(_resolution);
  _oneWire.reset_search();

  Logger::Log("DS1820 sensors present: %i.\n", _sensors.getDS18Count());

  while (_oneWire.search(address)) {
    Logger::Log("ROM = %s\n", Logger::DumpSensIdArray(address).c_str());

    if (OneWire::crc8(address, 7) != address[7]) {
      Logger::Log("CRC is not valid!\n");
      continue;
    }

    // ToDo: later, we store the sensor-address there
    //_measurementPivot->StoreSensId(address, _channel);
    // ToDo: Array-overflow is not avoided
    _sensorIds[sensorCount] = CastArrayToSensId(address);

    sensorCount++;
  }
  Logger::Log("Registered sensors: %i\n", sensorCount);
}

void Ds1820Helper::Cycle() {
  DallasTemperature::request_t req = _sensors.requestTemperatures();
  // delay(200);

  if (!req.result) {
    Logger::Log("request Temp failed.");
    return;
  }

  for (uint_fast8_t i = 0; i < 4; i++) {

    float temperature = _sensors.getTempC(CastSensIdToArray(&_sensorIds[i]));
    if (temperature <= DEVICE_DISCONNECTED_C) {
      continue;
    }

    Logger::Log("Temp: %.02f °C\n", temperature);

  }

  // _measurementPivot->ResetIter();

  // Measurement *actMeasurement;
  // for (uint_fast8_t i = 0; i < MAX_SENSORS; i++) {
  //   actMeasurement = _measurementPivot->GetNextMeasurement();
  //   if (actMeasurement == nullptr) { // end of list
  //     break;
  //   }
  //   if (actMeasurement->sensChan != _channel) { // other channel
  //     continue;
  //   }

  //   Measurement::SensorId sensId = actMeasurement->sensorId;
  //   float temperature =
  //       _sensors.getTempC(Measurement::CastSensIdToArray(&sensId));
  //   if (temperature <= DEVICE_DISCONNECTED_C) {
  //     continue;
  //   }

  //   bool updResult = _measurementPivot->UpdateValue(sensId, temperature);
  //   if (!updResult) {
  //     string idStr = Measurement::DumpSensId(sensId);
  //     string shortName = string(actMeasurement->shortname);
  //     Logger::Log("Updating value failed: %s / %s. Value: %.02f\n",
  //     idStr.c_str(),
  //           shortName.c_str(), temperature);
  //   }
  // }
}

} // namespace msmnt
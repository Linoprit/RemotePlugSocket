#include "Ds1820Helper.h"
#include <Logger.h>

namespace msmnt {

using namespace std;

Ds1820Helper::Ds1820Helper(Measurement::SensorChannel channel,
                           uint8_t oneWirePinOut, uint8_t resolution,
                           MeasurementPivot *measurementPivot)
    : _oneWireCh(oneWirePinOut), _sensorsCh(&_oneWireCh), _channel(channel),
      _measurementPivot(measurementPivot) {
  _resolution = resolution;
}

void Ds1820Helper::initHardware(void) {
  uint8_t address[8];
  uint8_t sensorCount = 0;
  _sensorsCh.begin();
  _sensorsCh.setResolution(_resolution);
  _oneWireCh.reset_search();

  Logger::Log("Channel %s, DS1820 sensors present: %i.\n",
        Measurement::DumpSensChannel(_channel).c_str(),
        _sensorsCh.getDS18Count());

  while (_oneWireCh.search(address)) {
    Logger::Log("ROM = %s\n", Measurement::DumpSensIdArray(address).c_str());

    if (OneWire::crc8(address, 7) != address[7]) {
      Logger::Log("CRC is not valid!\n");
      continue;
    }
    _measurementPivot->StoreSensId(address, _channel);
    sensorCount++;
  }
  Logger::Log("Registered sensors: %i\n", sensorCount);
}

void Ds1820Helper::cycle() {
  DallasTemperature::request_t req = _sensorsCh.requestTemperatures();
  // delay(200);

  if (!req.result) {
    Logger::Log("request Temp failed.");
    return;
  }

  _measurementPivot->ResetIter();

  Measurement *actMeasurement;
  for (uint_fast8_t i = 0; i < MAX_SENSORS; i++) {
    actMeasurement = _measurementPivot->GetNextMeasurement();
    if (actMeasurement == nullptr) { // end of list
      break;
    }
    if (actMeasurement->sensChan != _channel) { // other channel
      continue;
    }

    Measurement::SensorId sensId = actMeasurement->sensorId;
    float temperature =
        _sensorsCh.getTempC(Measurement::CastSensIdToArray(&sensId));
    if (temperature <= DEVICE_DISCONNECTED_C) {
      continue;
    }

    bool updResult = _measurementPivot->UpdateValue(sensId, temperature);
    if (!updResult) {
      string idStr = Measurement::DumpSensId(sensId);
      string shortName = string(actMeasurement->shortname);
      Logger::Log("Updating value failed: %s / %s. Value: %.02f\n",
                  idStr.c_str(), shortName.c_str(), temperature);
    }
  }
}

} // namespace msmnt
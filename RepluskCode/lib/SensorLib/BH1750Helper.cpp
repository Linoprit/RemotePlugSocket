#include <Arduino.h>
#include <BH1750Helper.h>
#include <Config.h>
#include <Logger.h>
#include <Wire.h>

namespace msmnt {

BH1750Helper::BH1750Helper(Measurement::SensorChannel channel,
                           MeasurementPivot *measurementPivot)
    : _channel(channel), _measurementPivot(measurementPivot) {
  if (BH1750_I2CADDR == 0x23) {
    _lightMeterId = MeasurementPivot::CreateId(
        MeasurementPivot::SensorKindType::BH_LIGHT_23);
  } else {
    _lightMeterId = MeasurementPivot::CreateId(
        MeasurementPivot::SensorKindType::BH_LIGHT_5C);
  }
  _measurementPivot->StoreSensId(_lightMeterId, _channel,
                                 Measurement::SensorType::LIGHT);
}

void BH1750Helper::InitHardware() {
  Wire.begin();
  _lightMeter = new BH1750(BH1750_I2CADDR);

  if (_lightMeter->begin(BH1750::ONE_TIME_HIGH_RES_MODE_2)) {
    Logger::Log("BH1750 Advanced begin\n");
  } else {
    Logger::Log("Error initialising BH1750\n");
  }
}

void BH1750Helper::Cycle() {

  while (!_lightMeter->measurementReady(true)) {
    yield();
  }

  float lux = _lightMeter->readLightLevel();
  _measurementPivot->UpdateValue(_lightMeterId, lux);

  _lightMeter->configure(BH1750::ONE_TIME_HIGH_RES_MODE);
}

} // namespace msmnt
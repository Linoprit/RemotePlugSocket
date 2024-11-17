#include <Arduino.h>
#include <Sensors/BH1750Helper.h>
#include <Logger.h>
#include <Wire.h>


namespace msmnt {

BH1750Helper::BH1750Helper() {}

void BH1750Helper::InitHardware() {

  Wire.begin();

  _lightMeter = new BH1750(0x23);

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
  Logger::Log("Light: %.02f lux\n", lux);
  _lightMeter->configure(BH1750::ONE_TIME_HIGH_RES_MODE);


}

} // namespace msmnt
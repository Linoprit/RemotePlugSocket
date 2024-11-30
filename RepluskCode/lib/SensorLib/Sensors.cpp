#include "Sensors.h"
#include <LittleFsHelpers.h>
#include <sys/time.h>
#include <MqttHelper.h>


namespace msmnt {

void Sensors::init(void) { new (&instance()) Sensors(); }

Sensors &Sensors::instance(void) {
  static Sensors sensors;
  return sensors;
}

Sensors::Sensors()
    : _ds1820Ch1(Measurement::CH_1, ONE_WIRE_PIN_CH1, DS18B20_PRECISION,
                 &_measurementPivot),
      _bh1750(Measurement::I2C_1, &_measurementPivot) {}

void Sensors::initHardware(void) {
  _ds1820Ch1.initHardware();
  _bh1750.InitHardware();
  delay(100);

  // MqttSetup and readIdTable push commands to the interpreter-queue.
  // Thats, why this must happen in the same task.
  nvm::LittleFsHelpers::instance().readIdTable();
  _measurementPivot.ResetConfigChangedFlags();
}

void Sensors::cycle(void) {
  _ds1820Ch1.cycle();
  _bh1750.Cycle();

  _updateCount++;
  if (_updateCount >= Measurement::VALUES_BUFF_LEN) {
    wifi::MqttHelper::instance().pubishMeasurements(&_measurementPivot);
    _updateCount = 0;
  }
}

bool Sensors::saveSensorIdTable() {
  return nvm::LittleFsHelpers::instance().saveSensIdTable(&_measurementPivot);
}

} // namespace msmnt

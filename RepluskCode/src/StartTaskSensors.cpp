#include "TasksCommon.h"
#include <Arduino.h>
#include <Sensors/BH1750Helper.h>
#include <Sensors/DS1820Helper.h>
#include <Config.h>
#include <DigitalIo/DigitalIo.h>


// #include "Config.h"
// #include <Sensors/Sensors.h>

void startSensorsTask(void *unused_arg) {
  // msmnt::Sensors::instance().init();
  // msmnt::Sensors::instance().initHardware();

  msmnt::BH1750Helper LightMeter;
  LightMeter.InitHardware();

  msmnt::Ds1820Helper Temperatures(ONE_WIRE_PIN_CH1, DS18B20_PRECISION);
  Temperatures.InitHardware();

  while (true) {
    // msmnt::Sensors::instance().cycle();
    // delay(MEASURETASK_CYCLE);

    LightMeter.Cycle();
    Temperatures.Cycle();

    digitIo::DigitalIo::TglPin(LED_DEBUG);
    digitIo::DigitalIo::TglPin(LED_STATUS);

    delay(1000);
  }
}
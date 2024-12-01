#include "TasksCommon.h"
#include <Arduino.h>
#include <BH1750Helper.h>
#include <Ds1820Helper.h>
#include <Config.h>
#include <DigitalIo.h>
#include "Config.h"
#include <Sensors.h>



void startSensorsTask(void *unused_arg) {
  msmnt::Sensors::instance().init();
  msmnt::Sensors::instance().initHardware();

  while (true) {
    msmnt::Sensors::instance().cycle();
    delay(MEASURETASK_CYCLE);

    // digitIo::DigitalIo::TglPin(LED_DEBUG);
    // digitIo::DigitalIo::TglPin(LED_STATUS);
    //ndelay(1000);
  }
}
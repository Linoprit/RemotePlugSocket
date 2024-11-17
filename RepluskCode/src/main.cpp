#include <Arduino.h>
#include <Config.h>
#include <Buttons/Button_Machine.h>
#include <DigitalIo/DigitalIo.h>
#include <Logger.h>
#include <TasksCommon.h>

TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t comLineTaskHandle = NULL;

digitIo::DigitalIo dio;

void setup() {
  Logger::Init();

  xTaskCreate(startComLineTask, "ComLineTask", 3024, NULL, 1,
              &comLineTaskHandle);
  delay(500);

  xTaskCreate(startSensorsTask, "SENSOR_TASK", 3024, NULL, 1,
              &sensorTaskHandle);

  dio.InitHardware();

  Logger::Log("Init done.\n");
}

void loop() {
  static uint32_t cycleCount = 0;

  Logger::Cycle();

  dio.Cycle();

  if (cycleCount >= 500) {
    cycleCount = 0;
    Logger::Log("Button state is: ");
    if (dio.GetPushButtonState() == digitIo::EventEnum::evntReleased) {
      Logger::Log("Released.\n");
    } else if (dio.GetPushButtonState() == digitIo::EventEnum::evntPressed) {
      Logger::Log("Pressed.\n");
    } else if (dio.GetPushButtonState() == digitIo::EventEnum::evntHeld) {
      Logger::Log("Held.\n");
    }
  }

  cycleCount+= 10;
  delay(10);
}

#include <Arduino.h>
// #include <Buttons/Button_Machine.h>
#include <CommandLine.h>
// #include <Config.h>
// #include <DigitalIo.h>
#include <LittleFsHelpers.h>
#include <Logger.h>
#include <MqttHelper.h>
#include <StateMachine.h>
#include <TasksCommon.h>

TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t comLineTaskHandle = NULL;



void setup() {
  Logger::Init();

  xTaskCreate(startComLineTask, "ComLineTask", 3024, NULL, 1,
              &comLineTaskHandle);

  nvm::LittleFsHelpers::instance().init();
  nvm::LittleFsHelpers::instance().initHardware();

  wifi::MqttHelper::instance().init();
  wifi::MqttHelper::instance().MqttSetup();
  delay(200);

  xTaskCreate(startSensorsTask, "SENSOR_TASK", 3024, NULL, 1,
              &sensorTaskHandle);

  delay(500);

  stm::StateMachine::instance().init();
  stm::StateMachine::instance().initHardware();

  Logger::Log("Init done.");
  cLine::CommandLine::instance().putChar('\r');
}

void loop() {
  stm::StateMachine::instance().cycle();
  delay(10);

  //
  // ToDo: do something with the ButtonState
  // if (cycleCount >= 500) {
  //   cycleCount = 0;
  //   Logger::Log("Button state is: ");
  //   if (dio.GetPushButtonState() == digitIo::EventEnum::evntReleased) {
  //     Logger::Log("Released.\n");
  //   } else if (dio.GetPushButtonState() == digitIo::EventEnum::evntPressed)
  //   {
  //     Logger::Log("Pressed.\n");
  //   } else if (dio.GetPushButtonState() == digitIo::EventEnum::evntHeld) {
  //     Logger::Log("Held.\n");
  //   }
  // }
}

// ToDo Logging auch auf Mqtt publishen.

/*
HowTo:
1. Switch als Online melden:
mosquitto_pub -h 192.168.178.103 -u "mosquitto" -P "public" -t
HoBuero/sens/switch/available -m "1"

2. State des SW melden:
mosquitto_pub -h 192.168.178.103 -u "mosquitto" -P "public" -t
HoBuero/sens/switch/0 -m "0"

3. Empfang ON oder OFF:
mosquitto_sub -h 192.168.178.103 -u "mosquitto" -P "public" -t
HoBuero/cmd/switch/set switch 0 1 switch 0 0 <switch> <Relais-Id> <on/off>

4. Rückmeldung des States
mosquitto_pub -h 192.168.178.103 -u "mosquitto" -P "public" -t
HoBuero/sens/switch/0 -m "1" mosquitto_pub -h 192.168.178.103 -u "mosquitto" -P
"public" -t HoBuero/sens/switch/0 -m "0"
*/
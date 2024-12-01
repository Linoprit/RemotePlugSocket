/*
 *  Created on: 02.06.2014
 *      Author: harald
 */

#include <Arduino.h>
#include <Buttons/Button_Machine.h>
#include <CommandLine.h>
#include <Config.h>
#include <LittleFsHelpers.h>
#include <Logger.h>
#include <MqttHelper.h>
#include <StateMachine.h>
#include <TasksCommon.h>

namespace stm {

StateMachine::StateMachine()
    : _currState(StateEnm::aus), _isEntry(true), _buttonWasHeld(false)
      //_buttonWasPressed(false) 
			{}

void StateMachine::initHardware() {
  _dio.InitHardware();
  reportAvailable();
}

void StateMachine::reportAvailable() {
  wifi::MqttHelper::instance().reportRelaysAvailability(true);
  for (int32_t i = 0; i < MAX_RELAY_COUNT; i++) {
    bool state = digitIo::DigitalIo::GetRelayState(i);
    wifi::MqttHelper::instance().reportRelayState(i, state);
  }
}

void StateMachine::cycle() {
  _dio.Cycle(); // pushbutton

  switch (_currState) {
  case StateEnm::aus:
    stAus();

    if (tsButtonLong()) {
      switchState(StateEnm::manOn);
    } else if (tsConnected()) {
      switchState(StateEnm::stauto);
    }
    break;
  case StateEnm::manOff:
    stManuallyOff();

    if (tsButtonLong()) {
      switchState(StateEnm::aus);
    }
    break;
  case StateEnm::manOn:
    stManuallyOn();

    if (tsButtonLong()) {
      switchState(StateEnm::manOff);
    }
    break;
  case StateEnm::stauto:
    stAuto();

    if (tsButtonLong()) {
      switchState(StateEnm::manOn);
    } else if (tsDisConnected()) {
      switchState(StateEnm::aus);
    } 
    break;

  default:
    break;
  }
}

void StateMachine::switchState(StateEnm newState) {
  _currState = newState;
  _isEntry = true;
}

/* -------------------- States -------------------- */
void StateMachine::stAus() {
  if (_isEntry) {
    _isEntry = false;
    _dio.SetPin(LED_STATUS, false);
    Logger::Log("\nState is AUS.");
  }
  /* code */
}

void StateMachine::stManuallyOff() {
  if (_isEntry) {
    _isEntry = false;
    for (int32_t i = 0; i < MAX_RELAY_COUNT; i++) {
      _dio.SetRelayState(i, false);
    }
    Logger::Log("\nState is ManOFF.");
  }
  /* state cycle code */
}

void StateMachine::stManuallyOn() {
  if (_isEntry) {
    _isEntry = false;
    _dio.SetPin(LED_STATUS, false);
    wifi::MqttHelper::instance().reportRelaysAvailability(false);
    for (int32_t i = 0; i < MAX_RELAY_COUNT; i++) {
      _dio.SetRelayState(i, true);
    }
    Logger::Log("\nState is ManON.");
  }
  /* state cycle code */
}

void StateMachine::stAuto() {
  if (_isEntry) {
    _isEntry = false;
    _dio.SetPin(LED_STATUS, true);
    reportAvailable();
    Logger::Log("\nState is AUTO.");
  }
  /* code */
}

/* -------------------- Transitions -------------------- */
// bool StateMachine::tsButtonPressed() {
//   if (_dio.ButtonPress()) {
//     _buttonWasPressed = true;
//   }
//   if (_dio.ButtonReleased() && _buttonWasPressed) {
//     _buttonWasPressed = false;
//     return true;
//   }
//   return false;
// };

bool StateMachine::tsButtonLong() {
  if (_dio.ButtonLongPress()) {
    _buttonWasHeld = true;
  }
  if (_dio.ButtonReleased() && _buttonWasHeld) {
    _buttonWasHeld = false;
		//_buttonWasPressed = false;
    return true;
  }
  return false;
};

bool StateMachine::tsConnected() {
  if (wifi::MqttHelper::instance().isMqttConnected()) {
    return true;
  }
  return false;
};

bool StateMachine::tsDisConnected() {
  if (!wifi::MqttHelper::instance().isMqttConnected()) {
    return true;
  }
  return false;
};

} // namespace stm
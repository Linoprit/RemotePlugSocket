/*
 * Button_State_Machine.cpp
 *
 *  Created on: 06.01.2017
 *      Author: harald
 */

#include <Buttons/Button_Machine.h>
#include <OsHelpers.h>

namespace digitIo {

ButtonMachine::ButtonMachine(uint8_t pin, bool groundIsOn) {
  _state = off;
  _oldTicks = 0;
  _lastEvent = EventEnum::evntReleased;
  _oldEvent = EventEnum::evntReleased;
  _pin = pin;
  _groundIsOn = groundIsOn;
}

EventEnum ButtonMachine::Cycle() {
  bool buttonIsPressed;
  if (_groundIsOn) {
    buttonIsPressed = !digitalRead(_pin);
  } else {
    buttonIsPressed = digitalRead(_pin);
  }

  if ((_state == off) && (buttonIsPressed)) {
    _state = on;
    _oldTicks = OsHelpers::GetTickMillis();
    _oldEvent = _lastEvent;
    _lastEvent = evntReleased;
  }

  else if ((_state == on) && (buttonIsPressed) &&
           (OsHelpers::GetTickMillis() - _oldTicks > debounceTicks)) {
    _state = pressed;
    _oldEvent = _lastEvent;
    _lastEvent = evntPressed;
  }

  else if ((_state == pressed) && (buttonIsPressed) &&
           (OsHelpers::GetTickMillis() - _oldTicks > btnHeldTicks)) {
    _state = held;
    _oldEvent = _lastEvent;
    _lastEvent = evntHeld;
  }

  else if (((_state == pressed) || (_state == held)) && (!buttonIsPressed)) {
    _state = off;
    _oldEvent = _lastEvent;
    _lastEvent = evntReleased;
  }

  else if ((_state == on) && (!buttonIsPressed)) {
    _state = off;
    _oldEvent = _lastEvent;
    _lastEvent = evntReleased;
  }
  return _lastEvent;
}

} // namespace digitIo

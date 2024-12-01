/*
 * Button_State_Machine.h
 *
 *  Created on: 06.01.2017
 *      Author: harald
 */

#ifndef BUTTON_MACHINE_H_
#define BUTTON_MACHINE_H_

#pragma once

#include <stdint.h>

namespace digitIo {

constexpr uint8_t debounceTicks = 20;
constexpr uint16_t btnHeldTicks = 500;

enum EventEnum { evntReleased, evntPressed, evntHeld };

class ButtonMachine {
public:
  // groundIsOn = true, means that the pin goes to ground, if button is pressed.
  ButtonMachine(uint8_t pin, bool groundIsOn);
  virtual ~ButtonMachine(){};

  EventEnum LastEvent() { return _lastEvent; };
  EventEnum OldEvent() { return _oldEvent; };

  EventEnum Cycle();

private:
  enum StateEnum { off, on, pressed, held };

  EventEnum _lastEvent;
  EventEnum _oldEvent;
  StateEnum _state;
  uint32_t _oldTicks;
  uint8_t _pin;
  bool _groundIsOn;
};

} // namespace digitIo

#endif /* BUTTON_MACHINE_H_ */

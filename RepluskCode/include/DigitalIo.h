#ifndef DIGITALIO_H
#define DIGITALIO_H

#pragma once

#include <stdint.h>
#include <Buttons/Button_Machine.h>

namespace digitIo {

class DigitalIo {
public:
  DigitalIo();
  virtual ~DigitalIo(){};

  void InitHardware();
  void Cycle();

  bool ButtonLongPress();
  bool ButtonPress();
  bool ButtonReleased();

  static void SetPin(uint8_t pin, bool state);
  static void TglPin(uint8_t pin);
  
  static void SetRelayState(uint8_t relayNr, uint8_t state);
  static bool GetRelayState(uint8_t relayNr);
  static uint8_t ToRelayChannel(uint8_t relayNr);
  static uint8_t ToRelayNr(uint8_t relayChannel);

private:
  ButtonMachine _pushButton;
};

} // namespace digitIo

#endif
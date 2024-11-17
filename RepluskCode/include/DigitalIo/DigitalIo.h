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

  EventEnum GetPushButtonState(){ return _pushButton.LastEvent; }
  static void SetPin(uint8_t pin, bool state);
  static void TglPin(uint8_t pin);

private:
  ButtonMachine _pushButton;
};

} // namespace digitIo

#endif
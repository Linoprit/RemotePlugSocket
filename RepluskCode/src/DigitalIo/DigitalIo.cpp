#include <AppTypes.h>
#include <Arduino.h>
#include <Config.h>
#include <DigitalIo/DigitalIo.h>

namespace digitIo {

DigitalIo::DigitalIo() : _pushButton{ButtonMachine(PUSH_BTTN, true)} {};

void DigitalIo::InitHardware() {
  pinMode(LED_STATUS, OUTPUT);
  pinMode(LED_DEBUG, OUTPUT);
  pinMode(RELAY_CH1, OUTPUT);
  pinMode(RELAY_CH2, OUTPUT);
  pinMode(RELAY_CH3, OUTPUT);
  pinMode(RELAY_CH4, OUTPUT);

  pinMode(PUSH_BTTN, INPUT_PULLUP);

  SetPin(RELAY_CH1, _on_);
  SetPin(RELAY_CH2, _on_);
  SetPin(RELAY_CH3, _on_);
  SetPin(RELAY_CH4, _on_);
}

void DigitalIo::Cycle() {
  _pushButton.Cycle();
}

void DigitalIo::SetPin(uint8_t pin, bool state) {
  digitalWrite(pin, (uint8_t)state);
}
void DigitalIo::TglPin(uint8_t pin) { digitalWrite(pin, !digitalRead(pin)); }

} // namespace digitIo
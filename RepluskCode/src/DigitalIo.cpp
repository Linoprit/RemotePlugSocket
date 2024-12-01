#include <AppTypes.h>
#include <Arduino.h>
#include <Config.h>
#include <DigitalIo.h>
#include <Logger.h>

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

  // Relaystates are inverted
  SetPin(RELAY_CH1, _on_);
  SetPin(RELAY_CH2, _on_);
  SetPin(RELAY_CH3, _on_);
  SetPin(RELAY_CH4, _on_);
}

void DigitalIo::Cycle() { _pushButton.Cycle(); }

void DigitalIo::SetPin(uint8_t pin, bool state) {
  digitalWrite(pin, (uint8_t)state);
}
void DigitalIo::TglPin(uint8_t pin) { digitalWrite(pin, !digitalRead(pin)); }

bool DigitalIo::ButtonLongPress() {
  return _pushButton.LastEvent() == EventEnum::evntHeld;
}

bool DigitalIo::ButtonPress() {
  return _pushButton.LastEvent() == EventEnum::evntPressed;
}

bool DigitalIo::ButtonReleased() {
  return _pushButton.LastEvent() == EventEnum::evntReleased;
}

void DigitalIo::SetRelayState(uint8_t relayNr, uint8_t state) {
  uint8_t relayChannel = ToRelayChannel(relayNr);
  SetPin(relayChannel, !(bool)state);
}

bool DigitalIo::GetRelayState(uint8_t relayNr) {
  uint8_t relayChannel = ToRelayChannel(relayNr);
  return !digitalRead(relayChannel);
}

uint8_t DigitalIo::ToRelayChannel(uint8_t relayNr) {
  switch (relayNr) {
  case 0:
    return RELAY_CH1;
  case 1:
    return RELAY_CH2;
  case 2:
    return RELAY_CH3;
  case 3:
    return RELAY_CH4;
  default:
    return 255;
  }
}

uint8_t DigitalIo::ToRelayNr(uint8_t relayChannel) {
  switch (relayChannel) {
  case RELAY_CH1:
    return 0;
  case RELAY_CH2:
    return 1;
  case RELAY_CH3:
    return 2;
  case RELAY_CH4:
    return 3;
  default:
    return 255;
  }
}

} // namespace digitIo
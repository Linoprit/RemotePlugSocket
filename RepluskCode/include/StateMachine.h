/*
 *  Created on: 02.06.2014
 *      Author: harald
 */

#ifndef LIGHTBAR_MACHINE_H_
#define LIGHTBAR_MACHINE_H_

#pragma once

#include <DigitalIo.h>
#include <new>
#include <stdint.h>

namespace stm {

class StateMachine {
public:
  enum StateEnm { aus, manOff, manOn, stauto };

  StateMachine();
  ~StateMachine(){};

  void init(void) { new (&instance()) StateMachine(); };
  static StateMachine &instance(void) {
    static StateMachine stateMachine;
    return stateMachine;
  };
	void initHardware();

  StateEnm getCurrentState() { return _currState; }
  void cycle();

private:
  uint32_t _oldTickSeconds;
  StateEnm _currState;
  bool _isEntry;
	bool _buttonWasHeld;
	//bool _buttonWasPressed;
  digitIo::DigitalIo _dio;

  void switchState(StateEnm newState);
	void reportIsAvailable();
  void reportAvailableCycle();

  void stAus();
  void stManuallyOff();
  void stManuallyOn();
  void stAuto();

  //bool tsButtonPressed();
  bool tsButtonLong();
  bool tsConnected();
  bool tsDisConnected();
};

} // namespace stm
#endif /* LIGHTBAR_MACHINE_H_ */

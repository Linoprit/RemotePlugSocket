#include "TasksCommon.h"
#include <Arduino.h>
#include <CommandLine/Interpreter.h>
// #include <ComLineConfig.h>
#include <CommandLine.h>

void startComLineTask(void *unused_arg) {

  cLine::Interpreter interpreter;
  cLine::CommandLine::instance().init(&interpreter);
  cLine::CommandLine::instance().splash();

  while (true) {

    Logger::Cycle();

    // CommandLine loop
    int incomingByte = Serial.read();
    while (incomingByte != -1) {
      cLine::CommandLine::instance().putChar(incomingByte);
      incomingByte = Serial.read();
    }
    cLine::CommandLine::instance().cycle();
    delay(10);
  }
}
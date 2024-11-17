#include <Arduino.h>
#include "TasksCommon.h"
//#include <CommandLine/ComLineConfig.h>
//#include <CommandLine/CommandLine.h>

void startComLineTask(void *unused_arg)
{

  //cLine::CommandLine::instance().init();
  //cLine::CommandLine::instance().splash();

  while (true)
  {
   // CommandLine loop
    //int incomingByte = Serial.read();
    //while (incomingByte != -1) {
    //  cLine::CommandLine::instance().putChar(incomingByte);
    //  incomingByte = Serial.read();
    //}
    //cLine::CommandLine::instance().cycle();
    //delay(10);
    
    delay(1000);
  }
}
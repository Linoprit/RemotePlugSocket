#ifndef LOGGER_H
#define LOGGER_H

#pragma once

#include <AppTypes.h>
#include <stdio.h>
#include <string>

using namespace std;

class Logger {
public:
  Logger() {};
  virtual ~Logger() {};

  void static Init();
  static int Cycle(void);
  static int Log(const char *format, ...);
  static void Putchar(char character);

  static std::string DumpSensIdArray(SensorIdArray sensIdArray) {
    char buffer[25];
    sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", sensIdArray[0],
            sensIdArray[1], sensIdArray[2], sensIdArray[3], sensIdArray[4],
            sensIdArray[5], sensIdArray[6], sensIdArray[7]);
    return std::string(buffer);
  }

  static std::string DumpSensId(SensorId sensId) {
    SensorIdArray *sensIdArray = (SensorIdArray *)&sensId;
    return DumpSensIdArray(*sensIdArray);
  }

private:
};

#endif
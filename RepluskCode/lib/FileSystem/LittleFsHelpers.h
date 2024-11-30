#ifndef LITTLEFSHELPERS_H
#define LITTLEFSHELPERS_H

#pragma once

// https://raw.githubusercontent.com/RuiSantosdotme/Random-Nerd-Tutorials/master/Projects/ESP32/ESP32_Testing_LittleFS.ino
// Adapted from:
// https://github.com/espressif/arduino-esp32/blob/master/libraries/LittleFS/examples/LITTLEFS_test/LITTLEFS_test.ino
// Project details:
// https://RandomNerdTutorials.com/esp32-write-data-littlefs-arduino/

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <Config.h>
#include <MeasurementPivot.h>

//  You only need to format LittleFS the first time you run a
//  test or else use the LITTLEFS plugin to create a partition
//  https://github.com/lorol/arduino-esp32littlefs-plugin

// CommandLine's input queue
extern volatile QueueHandle_t keyBufferQueue;

namespace nvm {

#define FORMAT_LITTLEFS_IF_FAILED false

class LittleFsHelpers {
public:
  LittleFsHelpers();
  virtual ~LittleFsHelpers(){};

  void init(void);
  void initHardware(void);
  static LittleFsHelpers &instance(void);

  void readConfigFile(const char *filename);
  void readIdTable() { readConfigFile(idTableFile); };
  bool saveSensIdTable(msmnt::MeasurementPivot *measurementPivot);
  void readMqttConf() { readConfigFile(mqttConfFile); };

  void listDir(const char *dirname, uint8_t levels);
  void createDir(const char *path);
  void removeDir(const char *path);
  void readFile(const char *path);
  void writeFile(const char *path, const char *message);
  void appendFile(const char *path, const char *message);
  void renameFile(const char *path1, const char *path2);
  void deleteFile(const char *path);

  void testFileIO(const char *path);
  void doTheTest();

private:
  fs::FS &_LittleFS;
};
} // namespace nvm
#endif
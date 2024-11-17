#include "CommandLine/ComLineConfig.h"
#include "CommandLine/CommandLine.h"
#include "LittleFsHelpers.h"
#include "Sensors/Measurement.h"
#include <Config.h>
#include <Wifi/MqLog.h>

namespace nvm {

void LittleFsHelpers::init(void) { new (&instance()) LittleFsHelpers(); }

LittleFsHelpers &LittleFsHelpers::instance(void) {
  static LittleFsHelpers littleFsHelpers;
  return littleFsHelpers;
}

LittleFsHelpers::LittleFsHelpers() : _LittleFS{LittleFS} {}

void LittleFsHelpers::initHardware() {
  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    MqLog("LittleFS Mount Failed\n");
  }
}

void LittleFsHelpers::readConfigFile(const char *filename) {
  // Read file line by line
  File file = LittleFS.open(filename);
  if (!file || file.isDirectory()) {
    MqLog("Failed to open idTableFile.\n");
    return;
  }
  while (file.available()) {
    String line = file.readStringUntil('\n');

    for (uint_fast8_t i = 0; i < line.length(); i++) {
      uint8_t chr = (uint8_t)line.c_str()[i];
      xQueueSendToBack(keyBufferQueue, &chr, 50);
    }
    if (line.c_str()[line.length() - 1] != cLine::_KEY_ENTER) {
      uint8_t chr = cLine::_KEY_ENTER;
      xQueueSendToBack(keyBufferQueue, &chr, 50);      
    }

    delay(60); // CommandLine-task must process the buffer
  }
  file.close();
}

bool LittleFsHelpers::saveSensIdTable(
    msmnt::MeasurementPivot *measurementPivot) {

  File file = LittleFS.open(idTableFile, FILE_APPEND);
  if (!file || file.isDirectory()) {
    MqLog("Failed to open idTableFile.\n");
    return false;
  }

  measurementPivot->ResetIter();
  msmnt::Measurement *actMsmnt = measurementPivot->GetNextMeasurement();
  while (actMsmnt != nullptr) {
    if (actMsmnt->configChanged) {

      char msgBuff[cLine::COMMAND_BUFFER_LEN];
      actMsmnt->GetConfigAsCmd(msgBuff);

      String message = String(msgBuff);

      if (file.print(message)) {
        actMsmnt->configChanged = false;
      } else {
        MqLog("- append failed\n");
        return false;
      }
    }
    actMsmnt = measurementPivot->GetNextMeasurement();
  }
  file.close();
  return true;
}

void LittleFsHelpers::listDir(const char *dirname, uint8_t levels) {
  MqLog("Listing directory: %s\r\n", dirname);

  File root = _LittleFS.open(dirname);
  if (!root) {
    MqLog("- failed to open directory\n");
    return;
  }
  if (!root.isDirectory()) {
    MqLog(" - not a directory\n");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      MqLog("  DIR: %s\n", file.name());
      if (levels) {
        listDir(file.path(), levels - 1);
      }
    } else {
      MqLog("  FILE: %s\tSIZE: %lu\n", file.name(), file.size());
    }
    file = root.openNextFile();
  }
}

void LittleFsHelpers::createDir(const char *path) {
  MqLog("Creating Dir: %s\n", path);
  if (!_LittleFS.mkdir(path)) {
    MqLog("mkdir failed\n");
  }
}

void LittleFsHelpers::removeDir(const char *path) {
  MqLog("Removing Dir: %s\n", path);
  if (!_LittleFS.rmdir(path)) {
    MqLog("rmdir failed\n");
  }
}

void LittleFsHelpers::readFile(const char *path) {
  File file = _LittleFS.open(path);
  if (!file || file.isDirectory()) {
    MqLog("- failed to open file for reading\n");
    return;
  }

  Serial.println(); 
  while (file.available()) {
    String line = file.readStringUntil('\n');

    if (line.c_str()[line.length() - 1] != '\n' ){
      line += '\n';
    }
    
    MqLog("%s", line.c_str());
    delay(20);
  }
  file.close();
}

void LittleFsHelpers::writeFile(const char *path, const char *message) {
  File file = _LittleFS.open(path, FILE_WRITE);
  if (!file) {
    MqLog("- failed to open file for writing\n");
    return;
  }
  if (!file.print(message)) {
    MqLog("- write failed\n");
  }
  file.close();
}

void LittleFsHelpers::appendFile(const char *path, const char *message) {
  File file = _LittleFS.open(path, FILE_APPEND);
  if (!file) {
    MqLog("- failed to open file for appending\n");
    return;
  }
  if (!file.print(message)) {
    MqLog("- append failed\n");
  }
  file.close();
}

void LittleFsHelpers::renameFile(const char *path1, const char *path2) {
  MqLog("Renaming file %s to %s\r\n", path1, path2);
  if (!_LittleFS.rename(path1, path2)) {
    MqLog("- rename failed\n");
  }
}

void LittleFsHelpers::deleteFile(const char *path) {
  MqLog("Deleting file: %s\r\n", path);
  if (!_LittleFS.remove(path)) {
    MqLog("- delete failed\n");
  }
}
} // namespace nvm
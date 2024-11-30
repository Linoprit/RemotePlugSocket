#include "MeasurementPivot.h"
#include <OsHelpers.h>
#include <sys/time.h>

namespace msmnt {

Measurement *MeasurementPivot::GetNextMeasurement() {
  if (_indexIter >= MAX_SENSORS) {
    return nullptr;
  }
  if (Measurement::IsInvalidSensId(_measurementArray[_indexIter].sensorId)) {
    return nullptr;
  }
  return GetMeasurement(_indexIter++);
}

Measurement *MeasurementPivot::FindSensIdOrEmpty(Measurement::SensorId sensId) {
  for (uint_fast8_t i = 0; i < MAX_SENSORS; i++) {
    Measurement *actMeasurement = &_measurementArray[i];
    if (actMeasurement->IsInvalidSensId()) {
      return nullptr;
    }
    if (sensId == actMeasurement->sensorId) {
      return actMeasurement;
    }
  }
  return nullptr;
}

Measurement *MeasurementPivot::FindNextFreeSlot() {
  for (uint_fast8_t i = 0; i < MAX_SENSORS; i++) {
    Measurement *actMeasurement = &_measurementArray[i];
    if (actMeasurement->IsInvalidSensId()) {
      return actMeasurement;
    }
  }
  return nullptr;
}

bool MeasurementPivot::StoreSensId(Measurement::SensorId sensId,
                                   Measurement::SensorChannel sensorChannel,
                                   Measurement::SensorType sensorType) {
  Measurement *actMeasurement = FindSensIdOrEmpty(sensId);
  if (actMeasurement != nullptr) {
    return false;
  }
  actMeasurement = FindNextFreeSlot();
  if (actMeasurement == nullptr) {
    return false;
  }

  actMeasurement->sensorId = sensId;
  actMeasurement->sensChan = sensorChannel;
  actMeasurement->sensType = sensorType;
  return true;
}

bool MeasurementPivot::StoreSensId(Measurement::SensorIdArray sensIdArray,
                                   Measurement::SensorChannel sensorChannel,
                                   Measurement::SensorType sensorType) {
  return StoreSensId(Measurement::CastArrayToSensId(sensIdArray), sensorChannel,
                     sensorType);
}

bool MeasurementPivot::UpdateValue(Measurement::SensorId sensId, float value) {
  Measurement *actMeasurement = FindSensIdOrEmpty(sensId);
  if (actMeasurement == nullptr) {
    return false;
  }

  if (actMeasurement->UpdateValue(value)) {
    timeval timeVal;
    gettimeofday(&timeVal, NULL);
    actMeasurement->lastUpdateTick = timeVal.tv_sec;
    return true;
  }

  return false;
}

void MeasurementPivot::Dump() {
  ResetIter();
  Measurement *actMeasurement = GetNextMeasurement();
  while (actMeasurement != nullptr) {
    delay(30); // wait for MqLog
    actMeasurement->Dump();
    actMeasurement = GetNextMeasurement();
  }
}

Measurement::SensorId
MeasurementPivot::CreateId(SensorKindType sensorAddressType) {
  uint64_t sensorId = 0;
  uint8_t *mac = (uint8_t *)&sensorId; // the last two bytes should stay empty

  OsHelpers::GetMacAddress(mac);
  mac[7] = sensorAddressType;

  return sensorId;
}

void MeasurementPivot::ResetConfigChangedFlags() {
  ResetIter();
  Measurement *actMeasurement = GetNextMeasurement();
  while (actMeasurement != nullptr) {
    actMeasurement->configChanged = false;
    actMeasurement = GetNextMeasurement();
  }
}

bool MeasurementPivot::UpdateConfig(SensorConfigType config) {
  Measurement *actMeasurement = FindSensIdOrEmpty(config.sensorId);
  if (actMeasurement == nullptr) {
    return false;
  }
  actMeasurement->minVal = config.minVal;
  actMeasurement->maxVal = config.maxVal;
  actMeasurement->sensType = config.sensType;
  actMeasurement->relayNr = (Measurement::RelayChannel)config.relayNr;
  memcpy(actMeasurement->shortname, config.shortname,
         Measurement::SHORTNAME_LEN);
  actMeasurement->configChanged = true;
  return true;
}
} // namespace msmnt

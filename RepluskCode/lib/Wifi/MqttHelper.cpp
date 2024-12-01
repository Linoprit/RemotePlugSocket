#include <AppTypes.h>
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <CommandLine.h>
#include <LittleFsHelpers.h>
#include <Logger.h>
#include <MeasurementPivot.h>
#include <MqttHelper.h>
#include <OsHelpers.h>
#include <WiFi.h>

namespace wifi {

using namespace msmnt;

bool MqttHelper::_mqttIsConnected = false;

/// begin Mqtt callbacks and timer
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

void connectToMqtt() {
  Logger::Log("Connecting to MQTT...\n");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
  // Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event) {
  case SYSTEM_EVENT_STA_GOT_IP:
    Logger::Log("\nWIFI is connected, local IP: %i.%i.%i.%i\n",
                WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2],
                WiFi.localIP()[3]);
    connectToMqtt();
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Logger::Log("WiFi lost connection\n");
    xTimerStop(
        mqttReconnectTimer,
        0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    xTimerStart(wifiReconnectTimer, 0);
    break;
  }
}

void onMqttConnect(bool sessionPresent) {
  Logger::Log("Connected to MQTT. Session present: %i\n", sessionPresent);

  uint16_t result =
      mqttClient.subscribe(wifi::MqttHelper::instance()._mqttSubCmd, 1);

  Logger::Log("Subscribing to '%s', got ID %lu\n",
              wifi::MqttHelper::instance()._mqttSubCmd, result);

  MqttHelper::instance().setMqttConnected(true);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Logger::Log("Disconnected from MQTT.\n");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
  MqttHelper::instance().setMqttConnected(false);
}

void onMqttPublish(uint16_t packetId) {
  Logger::Log("Pub ack. Id: %lu\n", packetId);
}

void connectToWifi() {
  Logger::Log("Reconnecting to Wi-Fi...\n");
  nvm::LittleFsHelpers::instance().readMqttConf();
}

void onMqttMessage(char *topic, char *payload,
                   AsyncMqttClientMessageProperties properties, size_t len,
                   size_t index, size_t total) {

  for (uint_fast8_t i = 0; i < total; i++) {
    xQueueSendToBack(keyBufferQueue, &payload[i], 50);
  }
  if (payload[total - 1] != cLine::_KEY_ENTER) {
    uint8_t chr = cLine::_KEY_ENTER;
    xQueueSendToBack(keyBufferQueue, &chr, 50);
  }
}
/// end Mqtt callbacks and timer

void MqttHelper::init(void) { new (&instance()) MqttHelper(); }

MqttHelper &MqttHelper::instance(void) {
  static MqttHelper mqttHelper;
  return mqttHelper;
}

MqttHelper::MqttHelper() {
  _mqttPubLog = new char[MQTT_PATHLEN];
  _mqttPubSens = new char[MQTT_PATHLEN];
  _mqttSubCmd = new char[MQTT_PATHLEN];
  _DoSerialPrint = true;

  char noneBuff[] = "none";
  strcpy(_mqttPubLog, noneBuff);
  strcpy(_mqttPubSens, noneBuff);
  strcpy(_mqttSubCmd, noneBuff);
}

void MqttHelper::MqttSetup() {
  mqttReconnectTimer =
      xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0,
                   reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer =
      xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0,
                   reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  // mqttClient.onSubscribe(onMqttSubscribe);
  // mqttClient.onUnsubscribe(onMqttUnsubscribe);
  // mqttClient.onPublish(onMqttPublish);
  mqttClient.onMessage(onMqttMessage);

  // Connection to Wifi is done by reading the MqttConfig file.
  nvm::LittleFsHelpers::instance().readMqttConf();

  mqttClient.setServer(_mqttHost, _mqttPort);
  // If your broker requires authentication (username and password), set them
  // below
  mqttClient.setCredentials("mosquitto",
                            "public"); // TODO make this configurable

  WiFi.onEvent(WiFiEvent);
}

void MqttHelper::printMqttConf() {
  if (WiFi.status() == WL_CONNECTED) {
    Logger::Log("\nWIFI is connected, local IP: %i.%i.%i.%i\n",
                WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2],
                WiFi.localIP()[3]);
  } else {
    Logger::Log("\nWifi is not  connected.\n");
  }
  Logger::Log("MqttHost: %i.%i.%i.%i:%lu\n", _mqttHost[0], _mqttHost[1],
              _mqttHost[2], _mqttHost[3], _mqttPort);
  Logger::Log("Mqtt publish sensordata: '%s'\n", _mqttPubSens);
  Logger::Log("Mqtt publish logs: '%s'\n", _mqttPubLog);
  Logger::Log("Mqtt subscribe commands: '%s'\n", _mqttSubCmd);
}

void MqttHelper::pubishMeasurements(MeasurementPivot *measurementPivot) {
  uint8_t publishCount = 0;

  // Calculation length: uint64: 20, float: 6, spaces: 1, EOL: 1
  char buff[30];

  measurementPivot->ResetIter();
  Measurement *actMeasurement = measurementPivot->GetNextMeasurement();

  while (actMeasurement != nullptr) {
    if ((!actMeasurement->isTimeout()) &&
        (!isnanf(actMeasurement->meanValue))) {

      std::string topic = std::string(_mqttPubSens) + "/" +
                          trim(actMeasurement->GetShortname());

      sprintf(buff, "{\"%s\":%.02f}\0", actMeasurement->DumpSensType().c_str(),
              actMeasurement->meanValue);

      uint16_t packetIdPub1 = mqttClient.publish(topic.c_str(), 1, true, buff);
      publishCount++;
      // '{"temp":{value_tmp}, "prs":{value_prs}, "hum":{value_hum} }'
      // Serial.printf("Pub on topic '%s' at QoS 1, Id: %lu\n", topic.c_str(),
      //               packetIdPub1);
    }
    actMeasurement = measurementPivot->GetNextMeasurement();
  }
  Logger::Log("(%lu) Published %i sensors.\n", OsHelpers::GetTickSeconds(),
              publishCount);
}

void MqttHelper::reportRelayState(uint8_t relayNr, bool state) {
  std::string topic =
      std::string(_mqttPubSens) + "/switch/" + std::to_string(relayNr);
  std::string stateStr = std::to_string(state);

  mqttClient.publish(topic.c_str(), 1, true, stateStr.c_str());
}

void MqttHelper::reportRelaysAvailability(bool isAvailable) {
  std::string topic = std::string(_mqttPubSens) + "/switch/available";
  std::string isAvailStr = std::to_string(isAvailable);

  mqttClient.publish(topic.c_str(), 1, true, isAvailStr.c_str());
}

std::string MqttHelper::trim(const std::string &str,
                             const std::string &whitespace) {
  const auto strBegin = str.find_first_not_of(whitespace);
  if (strBegin == std::string::npos)
    return ""; // no content

  const auto strEnd = str.find_last_not_of(whitespace);
  const auto strRange = strEnd - strBegin + 1;

  return str.substr(strBegin, strRange);
}

int MqttHelper::publishLog(uint8_t *message, uint16_t size) {
  if (_DoSerialPrint) {
    Serial.printf((char *)message);
  }

  uint16_t packetIdPub1 =
      mqttClient.publish(_mqttPubLog, 1, true, (char *)message);
  if (packetIdPub1 == 0) {
    return _FAIL_;
  }
  return _SUCCESS_;
}

int MqttHelper::publishLog(std::string message) {
  return publishLog((uint8_t *)message.c_str(), message.length());
}

void MqttHelper::copyMqttPath(char *buffer, std::string spot,
                              const char *mqttSuffix, size_t mqttLen) {
  uint8_t spotLen = strlen(spot.c_str());

  if ((spotLen + mqttLen) > MQTT_PATHLEN) {
    spotLen = MQTT_PATHLEN - mqttLen;
    Logger::Log("\nWARNING: Spotname is too long. It will be truncated.\n");
  }

  memcpy(buffer, spot.c_str(), spotLen);
  memcpy(&buffer[spotLen], mqttSuffix, mqttLen);
}

} // namespace wifi
#include "Logger.h"
#include <AppTypes.h>
#include <Arduino.h>
#include <cstring>
#include <stdarg.h>
#include <stdio.h>

constexpr uint16_t TX_BUFF_LEN = 1280;//1024;

volatile QueueHandle_t LogBufQueue = xQueueCreate(TX_BUFF_LEN, sizeof(int8_t));

constexpr uint16_t TMP_BUFF_LEN = 512;
uint8_t txBuff[TX_BUFF_LEN];

void Logger::Init(){
  Serial.begin(115200);
}

int Logger::Cycle(void) {
  char character;
  uint16_t tx_act_pos = 0;

  while (xQueueReceive(LogBufQueue, &character, 10) == pdTRUE) {
    txBuff[tx_act_pos] = character;
    tx_act_pos++;
    if (tx_act_pos >= TX_BUFF_LEN){
      break;
    }
  }
  txBuff[tx_act_pos] = '\0';

  if (tx_act_pos > 0) {

    Serial.printf((char *)txBuff);
    return _SUCCESS_; // 

    // uint8_t result = _FAIL_;
    // result = wifi::MqttHelper::instance().publishLog(txBuff, tx_act_pos);
    // return result;
  }

  return _SUCCESS_;
}

void Logger::Putchar(char character) {
  xQueueSendToBack(LogBufQueue, &character, 20);
}

int Logger::Log(const char *format, ...) {
  uint8_t tmpBuff[TMP_BUFF_LEN];

  va_list arg;
  va_start(arg, format);
  uint16_t tmpLen = vsnprintf((char *)&tmpBuff, TMP_BUFF_LEN, format, arg);
  va_end(arg);

  for (uint16_t i = 0; i < tmpLen; i++) {
    if (xQueueSendToBack(LogBufQueue, &tmpBuff[i], 20) != pdTRUE) {
      return _FAIL_;
    }
  }

  return _SUCCESS_;
}

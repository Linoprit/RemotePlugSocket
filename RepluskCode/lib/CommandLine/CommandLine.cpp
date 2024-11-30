/*
 * commandLine.cpp
 *
 *  Created on: 05.11.2020
 *      Author: harald
 *  Inspired by Cat (https://github.com/ShareCat/STM32CommandLine)
 */

#include <Arduino.h>
#include <CommandLine.h>
#include <AppTypes.h>
#include <new>

using namespace std;

volatile QueueHandle_t keyBufferQueue =
    xQueueCreate(cLine::KEY_BUFFER_LEN, sizeof(int8_t));

namespace cLine {

void CommandLine::init(IfcInterpreter* interpreter) { 
  new (&instance()) CommandLine(interpreter); 
  }

CommandLine &CommandLine::instance(void) {
  static CommandLine commandLine;
  return commandLine;
}

CommandLine::CommandLine(IfcInterpreter* interpreter) : 
              _cmdPos(0), _keyBufferQueue(keyBufferQueue) {
  _flagInitIsDone = false;
  _cmdBuffer.fill('\0');
  _interpreter = interpreter;
  // termDisplayClear();
}

void CommandLine::splash(void) {
  termHighLight();
  // termResetCursor();
  Logger::Log("\n\n*****************************************\n");
  Logger::Log("**    Stm32 Command Line Interpreter   **\n");
  Logger::Log("**            V1.0 / 2021              **\n");
  Logger::Log("*****************************************\n");
  termUnHighLight();
  Logger::Log("\n");
  termPrompt();
  _flagInitIsDone = true;
}

void CommandLine::cycle(void) {
  uint8_t actChar = '\0';

  while (readNextChar(actChar) == _SUCCESS_) {
    if (actChar == _KEY_ENTER) {
      procEnter();
    } else if ((actChar == _KEY_BACKSPACE) || (actChar == _KEY_BACKSPACE_ALT)) {
      procBackspace();
    } else if (actChar == _KEY_ESCAPE) {
      int result = readNextChar(actChar);
      if ((result == _SUCCESS_) && (actChar == _KEY_SQBRACELEFT)) {
        procSqrEscKeys();
      }
      // Minicom sends different END-key sequence
      else if ((result == _SUCCESS_) && (actChar == _KEY_END_MINICOM1)) {
        result = readNextChar(actChar);
        if ((result == _SUCCESS_) && (actChar == _KEY_END_MINICOM2)) {
          procEnd();
        }
      } else {
        procEscape();
      }
    } else {
      accumulateChar(actChar);
    }
  }
}

int CommandLine::readNextChar(uint8_t &chr) {
  uint8_t key = 0;
  if (xQueueReceive(_keyBufferQueue, &key, 5)) {
    chr = key;
    return _SUCCESS_;
  }
  return _FAIL_;
}

void CommandLine::accumulateChar(uint8_t chr) {
  if (_cmdPos == COMMAND_BUFFER_LEN - 1) {
    return;
  }
  if (_cmdBuffer.at(_cmdPos + 1) != '\0') {
    moveCmdRight(_cmdPos);
  }
  _cmdBuffer.at(_cmdPos) = chr;
  incCmdPos();
  termInsert(1u);
  Logger::Putchar(chr);
}

void CommandLine::procSqrEscKeys(void) {
  uint8_t actChar = '\0';
  int result = readNextChar(actChar);

  if ((result == _SUCCESS_) && (actChar == _KEY_UP)) {
    procArrowUp();
  } else if ((result == _SUCCESS_) && (actChar == _KEY_DOWN)) {
    procArrowDown();
  } else if ((result == _SUCCESS_) && (actChar == _KEY_LEFT)) {
    procArrowLeft();
  } else if ((result == _SUCCESS_) && (actChar == _KEY_RIGHT)) {
    procArrowRight();
  }

  procFourByteEscKeys(actChar);
}

void CommandLine::procFourByteEscKeys(uint8_t actChar) {
  uint8_t seqSuffix = '\0';
  uint8_t result = readNextChar(seqSuffix);

  if ((result == _SUCCESS_) && (seqSuffix == _SEQ_SUFFIX)) {
    if ((result == _SUCCESS_) && (actChar == _KEY_POS1)) {
      procPos1();
    } else if ((result == _SUCCESS_) && (actChar == _KEY_DEL)) {
      procDel();
    } else if ((result == _SUCCESS_) && (actChar == _KEY_END)) {
      procEnd();
    } else if ((result == _SUCCESS_) && (actChar == _KEY_PGUP)) {
      procPGUP();
    } else if ((result == _SUCCESS_) && (actChar == _KEY_PGDN)) {
      procPGDN();
    }
  }
}

void CommandLine::moveCmdRight(uint8_t startPos) {
  uint8_t actPos = startPos;
  uint8_t actChar = _cmdBuffer.at(actPos);

  while (_cmdBuffer.at(actPos) != '\0') {
    uint8_t nextChar = _cmdBuffer.at(actPos + 1);
    _cmdBuffer.at(actPos + 1) = actChar;
    actChar = nextChar;
    actPos++;
  }
}

void CommandLine::moveCmdLeft(uint8_t startPos) {
  uint8_t actPos = startPos;
  while (_cmdBuffer.at(actPos) != '\0') {
    _cmdBuffer.at(actPos) = _cmdBuffer.at(actPos + 1);
    actPos++;
  }
}

void CommandLine::procEnter(void) {
  _history.add(_cmdBuffer);
  _history.resShowPos();
  if (_cmdBuffer.at(0) == '\0') { // line empty?
    Logger::Log("\n\0");
  } else {
    bool result = _interpreter->doit(_cmdBuffer);
    if (!result) {
      Logger::Log("\nUnknown command.\n\0");
    } else {
      Logger::Log("\n** DONE **\n\0"); //  remember TI-99/4A?
    }
  }
  termPrompt();
  resCmdPos();
  _cmdBuffer.fill('\0');
}

void CommandLine::procBackspace(void) {
  if (_cmdPos != 0) {
    decCmdPos();
    termMoveLeft(1u);
    termDelete(1u);
    moveCmdLeft(_cmdPos);
  }
}

void CommandLine::procDel(void) {
  moveCmdLeft(_cmdPos);
  termDelete(1u);
}

void CommandLine::syncCmdPos(void) {
  _cmdPos = 0;
  while ((_cmdPos < COMMAND_BUFFER_LEN) && (_cmdBuffer.at(_cmdPos) != '\0')) {
    incCmdPos();
  }
}

void CommandLine::procArrowUp(void) {
  if (_history.getInsertPos() == _history.getShowPos()) {
    _history.add(_cmdBuffer);
  }
  _cmdBuffer = _history.showUp();
  termPos1();
  termEraseLine(2u);
  termPrompt();
  Logger::Log("%s", _cmdBuffer.data());
  syncCmdPos();
}

void CommandLine::procArrowDown(void) {
  if (_history.getInsertPos() == _history.getShowPos()) {
    return;
  }
  _cmdBuffer = _history.showDown();
  termPos1();
  termEraseLine(2u);
  termPrompt();
  Logger::Log("%s", _cmdBuffer.data());
  syncCmdPos();
}

void CommandLine::procArrowLeft(void) {
  if (_cmdPos != 0) {
    termMoveLeft(1u);
    decCmdPos();
  }
}

void CommandLine::procArrowRight(void) {
  if ((_cmdPos < COMMAND_BUFFER_LEN) && (_cmdBuffer.at(_cmdPos) != '\0')) {
    termMoveRight(1u);
    incCmdPos();
  }
}

void CommandLine::procEnd(void) {
  while ((_cmdPos < COMMAND_BUFFER_LEN) && (_cmdBuffer.at(_cmdPos) != '\0')) {
    termMoveRight(1u);
    incCmdPos();
  }
}

void CommandLine::procPos1(void) {
  termPos1();
  termPrompt();
  resCmdPos();
}

void CommandLine::procEscape(void) {
  // unused
}

void CommandLine::procPGUP(void) {
  // unused
}

void CommandLine::procPGDN(void) {
  // unused
}

} // namespace cLine

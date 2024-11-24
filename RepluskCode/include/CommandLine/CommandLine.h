/*
 * commandLine.h
 *
 *  Created on: 05.11.2020
 *      Author: harald
 */

#ifndef SYSTEM_COMMANDLINE_H_
#define SYSTEM_COMMANDLINE_H_

#include "ComLineConfig.h"
#include "History.h"
#include <Arduino.h>
#include <CommandLine/Interpreter.h>
#include <Logger.h>


namespace cLine {

constexpr uint8_t _KEY_ENTER = '\r';         // [enter]
constexpr uint8_t _KEY_BACKSPACE_ALT = 127;  // [del]
constexpr uint8_t _KEY_BACKSPACE = 8;        // [backspace]
constexpr uint8_t _KEY_ESCAPE = '\x1b';      // [esc]
constexpr uint8_t _KEY_SQBRACELEFT = '\x5b'; // [ [ ] square brace left

// three bytes escaped keys, preceded by ESC[ ('\x1b\x5b')
constexpr uint8_t _KEY_UP = '\x41';    // [up]    0x1b 0x5b 0x41
constexpr uint8_t _KEY_DOWN = '\x42';  // [down]  0x1b 0x5b 0x42
constexpr uint8_t _KEY_RIGHT = '\x43'; // [right] 0x1b 0x5b 0x43
constexpr uint8_t _KEY_LEFT = '\x44';  // [left]  0x1b 0x5b 0x44

// four bytes escaped keys, preceded by ESC[, finalized by 'm'
constexpr uint8_t _KEY_POS1 = '\x31'; // [pos1]
constexpr uint8_t _KEY_DEL = '\x33';
constexpr uint8_t _KEY_END = '\x34'; // [end]
constexpr uint8_t _KEY_PGUP = '\x35';
constexpr uint8_t _KEY_PGDN = '\x36';
constexpr uint8_t _SEQ_SUFFIX = '\x7e'; // ends four bytes sequence

// for the END-key, Minicom sends a different sequence (dec 27 79 70)
constexpr uint8_t _KEY_END_MINICOM1 = '\x4f';
constexpr uint8_t _KEY_END_MINICOM2 = '\x46';

class CommandLine {
public:
  CommandLine();
  virtual ~CommandLine(){};

  void init();
  static CommandLine &instance(void);
  void splash(void);
  void cycle(void);

  bool isInitDone(void) { return _flagInitIsDone; }
  Interpreter *getInterpreter() { return &_interpret; }

  // must be called by the serial callback
  inline void putChar(uint8_t chr) {
    xQueueSendToBack(_keyBufferQueue, &chr, 5);

    // if (!_keyBuffer.isFull()) {
    //   _keyBuffer.enqueue(chr);
    // }
  }

  // Terminal control functions
  static void termDisplayClear() {
    Serial.printf("\033[2J");
  }; // terminal clear all
  static void termResetCursor() { Serial.printf("\033[H"); }; // cursor reset
  static void termHideCursor() {
    Serial.printf("\033[?25l");
  }; // cursor invisible
  static void termShowCursor() {
    Serial.printf("\033[?25h");
  };                                                         // cursor visible
  static void termHighLight() { Serial.printf("\033[7m"); }; // reverse display
  static void termUnHighLight() {
    Serial.printf("\033[27m");
  };                                                      // normal display
  static void termPos1() { Serial.printf("\r\033[1A"); }; // cursor to pos1
  static void termDelete(uint8_t n) {
    Serial.printf("\033[%dP", n);
  }; // Del n chars
  static void termInsert(uint8_t n) {
    Serial.printf("\033[%d@", n);
  }; // Ins n chars
  static void termMoveUp(uint8_t n) {
    Serial.printf("\033[%dA", n);
  }; // cursor move up
  static void termMoveDown(uint8_t n) {
    Serial.printf("\033[%dB", n);
  }; // cursor move down
  static void termMoveLeft(uint8_t n) {
    Serial.printf("\033[%dD", n);
  }; // cursor move left
  static void termMoveRight(uint8_t n) {
    Serial.printf("\033[%dC", n);
  }; // cursor move right
  static void termMoveToCol(uint8_t n) {
    Serial.printf("\0338%dG", n);
  }; // Move cursor to column n
  static void termInsLines(uint8_t n) {
    Serial.printf("\0338%dL", n);
  }; // Insert n blank lines
  static void termDelLines(uint8_t n) {
    Serial.printf("\0338%dM", n);
  }; // Delete n lines
  static void termEraseChars(uint8_t n) {
    Serial.printf("\033[%dX", n);
  }; // Erase n characters
  static void termMoveUpRows(uint8_t n) {
    Serial.printf("\033[%dF", n);
  }; // Move cursor up n rows, to col 1
  static void termMoveDownRows(uint8_t n) {
    Serial.printf("\033[%dE", n);
  }; // Move cursor down n rows, to col 1
  static void termMoveTo(uint8_t x, uint8_t y) {
    Serial.printf("\033[%d;%dH", x, y);
  }; // cursor move to
     //	0 erase from cursor to end of display,
     //  1 erase from start of display to cursor,
     // 2 erase display
  static void termEraseDisplay(uint8_t n) { Serial.printf("\033[%dJ", n); };
  //	0 erase from cursor to end of line,
  //  1 erase from start of line to cursor,
  //  2 erase line
  static void termEraseLine(uint8_t n) { Serial.printf("\033[%dK", n); };
  static void termPrompt() { Logger::Putchar('>'); };

protected:
  // SimpleQueue<uint8_t> _keyBuffer;
  QueueHandle_t _keyBufferQueue;

  History _history;
  Interpreter _interpret;
  CmdBufferType _cmdBuffer;
  uint8_t _cmdPos;
  bool _flagInitIsDone;

  void incCmdPos(void) {
    if (_cmdPos < COMMAND_BUFFER_LEN)
      _cmdPos++;
  };
  void decCmdPos(void) {
    if (_cmdPos > 0)
      _cmdPos--;
  };
  void resCmdPos(void) { _cmdPos = 0; };
  void moveCmdRight(uint8_t startPos);
  void moveCmdLeft(uint8_t startPos);
  int readNextChar(uint8_t &chr);
  void accumulateChar(uint8_t chr);
  void procSqrEscKeys(void);
  void procFourByteEscKeys(uint8_t actChar);
  void procEnter(void);
  void procBackspace(void);
  void procEscape(void);
  void procArrowUp(void);
  void procArrowDown(void);
  void procArrowLeft(void);
  void procArrowRight(void);
  void procEnd(void);
  void procPos1(void);
  void procDel(void);
  void procPGUP(void);
  void procPGDN(void);
  void syncCmdPos(void);
};

} // namespace cLine

#endif /* SYSTEM_COMMANDLINE_H_ */

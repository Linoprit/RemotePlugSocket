/*
 * ComLineInterpreter.h
 *
 *  Created on: 07.11.2020
 *      Author: harald
 */

#ifndef SYSTEM_COMMANDLINE_INTERPRETER_H_
#define SYSTEM_COMMANDLINE_INTERPRETER_H_

#include "Lexer.h"

namespace cLine {

class Interpreter {
public:
  Interpreter();
  virtual ~Interpreter(){};

  bool doit(CmdBufferType comLine);

private:
  bool getMacAddress();
  bool setMqttHost(Lexer *lex);
  bool setMqttPort(Lexer *lex);
  bool startWifi(Lexer *lex);
  bool stopWifi(Lexer *lex);
  bool setMqttSpot(Lexer *lex);
  bool getMqttConf();

  bool storeWifi(Lexer *lex);
  bool storeMqttSpot(Lexer *lex);
  bool storeMqttHost(Lexer *lex);
  bool storeMqttPort(Lexer *lex);

  bool listDir(Lexer *lex);
  bool readFile(Lexer *lex);
  
  // bool tstRelay(Lexer *lex);
  // bool doMsmntCycle(Lexer *lex);
  // bool printMeasures(Lexer *lex);
  // bool saveSensIdTable(Lexer *lex);
  // bool setSensId(Lexer *lex);
  // bool getSensIdTable(Lexer *lex);

  bool getStationId(Lexer *lex);
  bool calcHash(Lexer *lex);
  bool shutup();
  bool talk();
};

} /* namespace cLine */

#endif /* SYSTEM_COMMANDLINE_INTERPRETER_H_ */

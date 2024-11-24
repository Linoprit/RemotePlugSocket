/*
 * ComLineInterpreter.cpp
 *
 *  Created on: 07.11.2020
 *      Author: harald
 */

#include <CommandLine/CommandLine.h>
#include <CommandLine/Interpreter.h>
#include "Wifi/MqttHelper.h"
#include <CommandLine/ComLineConfig.h>
#include <CrcSocket.h>
#include <LittleFsHelpers.h>
#include <IPAddress.h>
#include <OsHelpers.h>
#include <Logger.h>
#include <cstring>

namespace cLine {

Interpreter::Interpreter() {}

bool Interpreter::doit(CmdBufferType comLine) {
  bool result = false;
  Lexer lex(&comLine);

  CmdToken *cmdToken = (CmdToken *)lex.getNextToken();
  if (cmdToken->getType() != Token::Command) {
    return false;
  }
  uint32_t cmd = cmdToken->getVal();

  //@formatter:off
  // clang-format off
  if (cmd == 1050090700) {    result = calcHash(&lex);  				} // calcHash
  else if (cmd == 2948963465) {    result = getStationId(&lex);  		} // getStationId
  else if (cmd == 1996702945) {    result = listDir(&lex);          } // listDir
  else if (cmd == 3796804437) {    result = readFile(&lex);         } // readFile  
  
  else if (cmd == 1759482888) {    result = getMacAddress();        } // getMacAddress
  else if (cmd == 3926160061) {    result = setMqttHost(&lex);      } // setMqttHost
  else if (cmd == 1095354772) {    result = setMqttPort(&lex);      } // setMqttPort
  else if (cmd == 2336016021) {    result = startWifi(&lex);        } // startWifi
  else if (cmd == 3573871370) {    result = stopWifi(&lex);         } // stopWifi
  else if (cmd == 3570528819) {    result = setMqttSpot(&lex);      } // setMqttSpot
  else if (cmd == 477230859)  {    result = getMqttConf();          } // getMqttConf

  else if (cmd == 2393500523)  {    result = storeWifi(&lex);       } // storeWifi
  else if (cmd == 4252320804)  {    result = storeMqttSpot(&lex);   } // storeMqttSpot
  else if (cmd == 3509612507)  {    result = storeMqttHost(&lex);   } // storeMqttHost
  else if (cmd == 3851424486)  {    result = storeMqttPort(&lex);   } // storeMqttPort

  else if (cmd == 959926194) 	{    result = shutup();  							} // shutup
  else if (cmd == 1639364146) {    result = talk();  								} // talk
  else if (cmd == 1676458703) {		 OsHelpers::SYSTEM_REBOOT();  		} // reboot
  else if (cmd == 1973435441) {    OsHelpers::SYSTEM_EXIT();  			} // exit
	else if (cmd == 3288008560) {		 CommandLine::termDisplayClear();    
																	 CommandLine::termResetCursor();
																	 result = true;										} //	clear
  // clang-format on
  //@formatter:on
  
  return result;
}

bool Interpreter::storeWifi(Lexer *lex) {
  ChrToken *chrToken = (ChrToken *)lex->getNextToken();
  if (chrToken->getType() != Token::String) {
    Logger::Log("\nUsage: storeWifi \"<SSID>\" \"<PASS>\".\n");
    return false;
  }
  std::string ssid = chrToken->getVal();

  chrToken = (ChrToken *)lex->getNextToken();
  if (chrToken->getType() != Token::String) {
    Logger::Log("\nUsage: storeWifi \"<SSID>\" \"<PASS>\".\n");
    return false;
  }
  std::string pass = chrToken->getVal();

  std::string cmd = "startWifi \"" + ssid + "\" \"" + pass + "\"\r\n";
  nvm::LittleFsHelpers::instance().appendFile(mqttConfFile, cmd.c_str());

  return true;
}

bool Interpreter::storeMqttSpot(Lexer *lex) {
  ChrToken *chrToken = (ChrToken *)lex->getNextToken();
  if (chrToken->getType() != Token::String) {
    Logger::Log("\nUsage: storeMqttSpot \"<Spotname>\".\n");
    return false;
  }
  std::string spot = chrToken->getVal();

  std::string cmd = "setMqttSpot \"" + spot + "\"\r\n";
  nvm::LittleFsHelpers::instance().appendFile(mqttConfFile, cmd.c_str());

  return true;
}

bool Interpreter::storeMqttHost(Lexer *lex) {
  IPAddress hostAddress;

  for (uint_fast8_t i = 0; i < 4; i++) {
    UInt64Token *intToken = (UInt64Token *)lex->getNextToken();
    if (intToken->getType() != Token::UInt64) {
      Logger::Log("\nUsage: storeMqttHost <int> <int> <int> <int>. (i.e. 192 "
                    "168 15 2)\n");
      return false;
    }
    hostAddress[i] = (uint8_t)intToken->getVal();
  }

  char buff[64];
  sprintf(buff, "setMqttHost %i %i %i %i\r\n", hostAddress[0], hostAddress[1],
          hostAddress[2], hostAddress[3]);

  nvm::LittleFsHelpers::instance().appendFile(mqttConfFile, buff);

  return true;
}

bool Interpreter::storeMqttPort(Lexer *lex) {
  UInt64Token *intToken = (UInt64Token *)lex->getNextToken();
  if (intToken->getType() != Token::UInt64) {
    Logger::Log("\nUsage: storeMqttPort <int>.");
    return false;
  }
  uint16_t port = (uint16_t)intToken->getVal();

  char buff[64];
  sprintf(buff, "setMqttPort %lu \r\n", port);

  nvm::LittleFsHelpers::instance().appendFile(mqttConfFile, buff);

  return true;
}

bool Interpreter::setMqttHost(Lexer *lex) {
  IPAddress hostAddress;

  for (uint_fast8_t i = 0; i < 4; i++) {
    UInt64Token *intToken = (UInt64Token *)lex->getNextToken();
    if (intToken->getType() != Token::UInt64) {
      Logger::Log("\nUsage: setMqttHost <int> <int> <int> <int>. (i.e. 192 "
                    "168 15 2)\n");
      return false;
    }
    hostAddress[i] = (uint8_t)intToken->getVal();
  }

  wifi::MqttHelper::instance().setMqttHost(hostAddress);
  return true;
}

bool Interpreter::setMqttPort(Lexer *lex) {
  uint16_t port;

  UInt64Token *intToken = (UInt64Token *)lex->getNextToken();
  if (intToken->getType() != Token::UInt64) {
    Logger::Log("\nUsage: setMqttPort <long int>.\n");
    return false;
  }
  port = (uint16_t)intToken->getVal();

  wifi::MqttHelper::instance().setMqttPort(port);
  return true;
}

bool Interpreter::getMacAddress() {
  uint8_t macBuff[6];
  OsHelpers::GetMacAddress(macBuff);
  Logger::Log("\nDEC %i:%i:%i:%i:%i:%i ", macBuff[0], macBuff[1], macBuff[2],
                macBuff[3], macBuff[4], macBuff[5]);
  Logger::Log("(HEX %02x:%02x:%02x:%02x:%02x:%02x)\n", macBuff[0], macBuff[1],
        macBuff[2], macBuff[3], macBuff[4], macBuff[5]);
  return true;
}

bool Interpreter::startWifi(Lexer *lex) {
  ChrToken *chrToken = (ChrToken *)lex->getNextToken();
  if (chrToken->getType() != Token::String) {
    Logger::Log("\nUsage: startWifi \"<SSID>\" \"<PASS>\".\n");
    return false;
  }
  std::string ssid = chrToken->getVal();

  chrToken = (ChrToken *)lex->getNextToken();
  if (chrToken->getType() != Token::String) {
    Logger::Log("\nUsage: startWifi \"<SSID>\" \"<PASS>\".\n");
    return false;
  }
  std::string pass = chrToken->getVal();

  wifi::MqttHelper::instance().startWifi(ssid, pass);
  return true;
}
bool Interpreter::stopWifi(Lexer *lex) {
  wifi::MqttHelper::instance().stopWifi();
  return true;
}

bool Interpreter::getMqttConf() {
  wifi::MqttHelper::instance().printMqttConf();
  return true;
}

bool Interpreter::setMqttSpot(Lexer *lex) {
  ChrToken *chrToken = (ChrToken *)lex->getNextToken();
  if (chrToken->getType() != Token::String) {
    Logger::Log("\nUsage: setMqttSpot \"<spotName>\". (i.e. 'Lager')\n");
    return false;
  }

  wifi::MqttHelper::instance().setMqttSpot(chrToken->getVal());
  return true;
}

bool Interpreter::shutup() {
  wifi::MqttHelper::instance().stopSerialPrint();
  Logger::Log("\nSerial print is inhibited.\n");
  return true;
}

bool Interpreter::talk() {
  wifi::MqttHelper::instance().startSerialPrint();
  Logger::Log("\nSerial print is active.\n");
  return true;
}

bool Interpreter::listDir(Lexer *lex) {
  Logger::Log("\n");
  nvm::LittleFsHelpers::instance().listDir("/", 3);
  return true;
}

bool Interpreter::readFile(Lexer *lex) {
  ChrToken *chrToken = (ChrToken *)lex->getNextToken();
  if (chrToken->getType() != Token::String) {
    Logger::Log(
        "\nUsage: readFile \"<filename>\". (don't forget the slash!)\n");
    return false;
  }
  Logger::Log("\n");
  nvm::LittleFsHelpers::instance().readFile(chrToken->getVal());
  return true;
}

bool Interpreter::calcHash(Lexer *lex) {
  CmdToken *cmdToken = (CmdToken *)lex->getNextToken();
  if (cmdToken->getType() != Token::Command) {
    Logger::Log("\nUsage: calcHash <commandString>\n");
    return false;
  }
  Logger::Log("\nHash = %lu\n", cmdToken->getVal());

  return true;
}

bool Interpreter::getStationId(Lexer *lex) {
  uint64_t sensorId = 0;
  uint8_t *mac = (uint8_t *)&sensorId; // the last two bytes should stay empty
  OsHelpers::GetMacAddress(mac);
  std::string stationId = Logger::DumpSensId(sensorId);

  Logger::Log("\nStation ID = %llu (%s)\n", sensorId, stationId.c_str());
  return true;
}

} /* namespace cLine */

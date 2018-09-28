#ifndef LOGGER_H
#define LOGGER_H

#include "singleton.h"

#include <fstream>
#include <string>
#include <ctime>

class Logger : public Singleton<Logger>
{
public:
  void Init() override
  {
    _logFile.open("debug-log.txt");    
  }

  virtual ~Logger()
  {
    _logFile.close();
  }

  void Print(const std::string& stringToPrint)
  {
    if (!Disabled)
    {
      time_t now = time(nullptr);
      tm *ltm = localtime(&now);

      _logFile << ltm->tm_hour << ":";
      _logFile << ltm->tm_min << ":";
      _logFile << ltm->tm_sec << " *** ";

      _logFile << stringToPrint;
    }
  }

  // IF set to true all printing is ignored
  bool Disabled = false;

private:
  std::ofstream _logFile;
};

#endif
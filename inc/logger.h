#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <ctime>

#include "singleton.h"

class Logger : public Singleton<Logger>
{
  public:
    void Init() override
    {
    }

    void Prepare(bool disabled)
    {
      _disabled = disabled;

      if (!_disabled)
      {
        _logFile.open("debug-log.txt");
      }
    }

    virtual ~Logger()
    {
      if (_logFile.is_open())
      {
        _logFile.close();
      }
    }

    void Print(const std::string& stringToPrint)
    {
      if (!_disabled)
      {
        time_t now = time(nullptr);
        tm *ltm = localtime(&now);

        _logFile << ltm->tm_hour << ":";
        _logFile << ltm->tm_min << ":";
        _logFile << ltm->tm_sec << " *** ";

        _logFile << stringToPrint;
      }
    }

  private:
    std::ofstream _logFile;

    bool _disabled = true;
};

#endif

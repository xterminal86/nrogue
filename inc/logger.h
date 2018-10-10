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

    void Prepare(bool enabled)
    {
      _enabled = enabled;

      if (_enabled)
      {
        _logFile.open("debug-log.txt");
        Print("Log started");
      }
    }

    virtual ~Logger()
    {
      if (_logFile.is_open())
      {
        Print("Log ended");
        _logFile.close();
      }
    }

    void Print(const std::string& stringToPrint)
    {
      if (_enabled)
      {
        time_t now = time(nullptr);
        tm *ltm = localtime(&now);

        _logFile << ltm->tm_hour << ":";
        _logFile << ltm->tm_min << ":";
        _logFile << ltm->tm_sec << " *** ";

        _logFile << stringToPrint << "\n";
      }
    }

  private:
    std::ofstream _logFile;

    bool _enabled = true;
};

#endif

#include "logger.h"

void Logger::Init()
{
}

void Logger::Prepare(bool enabled)
{
  _enabled = enabled;

  if (_enabled)
  {
    _logFile.open("debug-log.txt");
    Print("Log started");
  }
}

void Logger::Print(const std::string &stringToPrint)
{
  if (_enabled)
  {
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);

    if (ltm->tm_hour >= 0 && ltm->tm_hour < 10)
    {
      _logFile << "0";
    }

    _logFile << ltm->tm_hour << ":";

    if (ltm->tm_min >= 0 && ltm->tm_min < 10)
    {
      _logFile << "0";
    }

    _logFile << ltm->tm_min << ":";

    if (ltm->tm_sec >= 0 && ltm->tm_sec < 10)
    {
      _logFile << "0";
    }

    _logFile << ltm->tm_sec << " *** ";

    _logFile << stringToPrint << "\n";

    _logFile.flush();
  }
}

Logger::~Logger()
{
  if (_logFile.is_open())
  {
    Print("Log ended");
    _logFile.close();
  }
}

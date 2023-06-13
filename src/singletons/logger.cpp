#include "logger.h"

#include <ctime>

#include "util.h"

void Logger::InitSpecific()
{
}

// =============================================================================

void Logger::Prepare(bool enabled)
{
  _enabled = enabled;

  if (_enabled)
  {
    _logFile.open("debug-log.txt");
    Print("Log started");
  }
}

// =============================================================================

void Logger::Print(const std::string &stringToPrint, bool error)
{
  if (_enabled)
  {
    auto str = Util::GetCurrentDateTimeString();
    _logFile << str << (error ? " !!! " : " *** ");

    _logFile << stringToPrint << "\n";

    _logFile.flush();
  }
}

// =============================================================================

Logger::~Logger()
{
  if (_logFile.is_open())
  {
    Print("Log ended");
    _logFile.close();
  }
}

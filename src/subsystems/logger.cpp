#include "logger.h"

#include <ctime>

#include "util.h"

// =============================================================================

void Logger::Prepare(bool enabled)
{
  if (_started)
  {
    return;
  }

  _enabled = enabled;

  if (_enabled)
  {
    _logFile.open("debug-log.txt");
    Print("Log started");

#ifdef DEBUG_BUILD
    auto str = Util::StringFormat("World seed is 0x%lX", Game::gRng.Seed);
    DebugLog("%s\n\n", str.data());
    Print(str);
#endif

    _started = true;
  }
}

// =============================================================================

void Logger::Print(const std::string &stringToPrint, bool error)
{
  if (_enabled)
  {
    auto str = Util::GetCurrentDateTimeString();
    _logFile << " --- " << str << " --- ";

    if (error)
    {
      _logFile << "!!! ERROR !!! ";
    }

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

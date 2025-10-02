#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

#include "util.h"

class Logger
{
  public:
    ~Logger();

    void Prepare(bool enabled);
    void Print(const std::string& stringToPrint, bool error = false);

    template <typename ... Args>
    void Printf(const std::string& format, Args ... args)
    {
      // FIXME: Util has not been declared bs
      //std::string str = Util::StringFormat(format, args ...);
      //Print(str);
    }

  private:
    std::ofstream _logFile;

    bool _enabled = true;

    bool _started = false;
};

#endif

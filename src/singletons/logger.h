#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

#include "singleton.h"

class Logger : public Singleton<Logger>
{
  public:
    virtual ~Logger();

    void Init() override;
    void Prepare(bool enabled);
    void Print(const std::string& stringToPrint);

  private:
    std::ofstream _logFile;

    bool _enabled = true;
};

#endif

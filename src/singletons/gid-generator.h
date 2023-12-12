#ifndef GIDGENERATOR_H
#define GIDGENERATOR_H

#include <cstdint>

#include "singleton.h"

class GID : public Singleton<GID>
{
  public:
    const uint64_t& GenerateGlobalId();
    const uint64_t& GetCurrentGlobalId();

  protected:
    void InitSpecific() override;

  private:
    const uint64_t kStartingValue = 0;

    uint64_t _globalId = kStartingValue;
};

#endif // GIDGENERATOR_H

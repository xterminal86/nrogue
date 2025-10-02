#ifndef GIDGENERATOR_H
#define GIDGENERATOR_H

#include <cstdint>

class GID
{
  public:
    void Init();

    const uint64_t& GenerateGlobalId();
    const uint64_t& GetCurrentGlobalId();

  private:
    const uint64_t kStartingValue = 0;

    uint64_t _globalId = kStartingValue;

    bool _initialized = false;
};

#endif // GIDGENERATOR_H

#ifndef RNG_H
#define RNG_H

#include <chrono>
#include <random>

#include "singleton.h"

class RNG : public Singleton<RNG>
{
  public:
    void Init() override;

    std::mt19937_64 Random;
};

#endif // RNG_H

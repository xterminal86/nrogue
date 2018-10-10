#ifndef RNG_H
#define RNG_H

#include <chrono>
#include <random>

#include "singleton.h"

class RNG : public Singleton<RNG>
{
  public:
    void Init() override;

    void SetSeed(unsigned long long seed);

    std::mt19937_64 Random;

    unsigned long long Seed;
};

#endif // RNG_H

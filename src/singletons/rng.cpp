#include "rng.h"

void RNG::Init()
{
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  Random.seed(seed);
  Seed = seed;

  _initialized = true;
}

void RNG::SetSeed(unsigned long long seed)
{
  Seed = seed;
  Random.seed(Seed);
}

int RNG::RandomRange(int min, int max)
{
  if (min == max)
  {
    return min;
  }

  int trueMin = std::min(min, max);
  int trueMax = std::max(min, max);

  int d = std::abs(trueMax - trueMin);

  int random = Random() % d;

  return trueMin + random;
}

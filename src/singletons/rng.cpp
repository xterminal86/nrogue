#include "rng.h"

#include "constants.h"

void RNG::Init()
{
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  Random.seed(seed);
  Seed = seed;

  _initialized = true;
}

void RNG::SetSeed(const std::string& hexValue)
{
  bool isSeedValid = true;
  if (hexValue.empty())
  {
    printf("Seed value is empty, generating random\n");
    isSeedValid = false;
  }
  else if (hexValue.length() > sizeof(unsigned long long))
  {
    printf("!!! Seed value is too big (%lu digits against system's max of %lu), generating random\n", hexValue.length(), sizeof(unsigned long long) * 2);
    isSeedValid = false;
  }
  else
  {
    for (auto& c : hexValue)
    {
      if (GlobalConstants::HexChars.find(c) == std::string::npos)
      {
        printf("!!! Invalid seed value ('%s'), generating random\n", hexValue.data());
        isSeedValid = false;
        break;
      }
    }
  }

  if (!isSeedValid)
  {
    Seed = std::chrono::system_clock::now().time_since_epoch().count();
  }
  else
  {
    unsigned long long value = std::stoull(hexValue, 0, 16);
    Seed = value;
  }

  Random.seed(Seed);
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

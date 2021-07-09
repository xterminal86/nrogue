#include "rng.h"

#include "constants.h"
#include "util.h"

void RNG::Init()
{
  if (_initialized)
  {
    return;
  }

  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  Random.seed(seed);
  Seed = seed;

  GenerateSeedString("<seed was randomized>");

  _initialized = true;
}

void RNG::SetSeed(const std::string& string)
{
  bool isSeedValid = true;

  if (string.empty())
  {
    //DebugLog("Seed value is empty, generating random\n");
    isSeedValid = false;
  }
  else
  {
    Seed = _hasher(string);
    GenerateSeedString(string);
  }

  if (!isSeedValid)
  {
    Seed = std::chrono::system_clock::now().time_since_epoch().count();    
    GenerateSeedString("<seed was randomized>");
  }

  Random.seed(Seed);
}

void RNG::SetSeed(size_t seed)
{
  Seed = seed;
  Random.seed(Seed);

  GenerateSeedString("<seed was set by value>");
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

const SeedString& RNG::GetSeedString()
{
  return _seedString;
}

void RNG::GenerateSeedString(const std::string& str)
{
  std::stringstream ss;

  ss << "0x" << std::uppercase << std::hex << Seed;

  _seedString = { str, ss.str() };
}

std::string RNG::GetSeedAsHex()
{
  std::stringstream ss;

  ss << std::hex << std::uppercase << Seed;

  return ss.str();
}

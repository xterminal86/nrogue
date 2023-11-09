#ifndef RNG_H
#define RNG_H

#include <chrono>
#include <random>
#include <functional>

#include "singleton.h"

using SeedString = std::pair<std::string, std::string>;

class RNG : public Singleton<RNG>
{
  public:
    void SetSeed(size_t seed);
    void SetSeed(const std::string& string);

    std::string GetSeedAsHex();

    int RandomRange(int min, int max);

    const SeedString& GetSeedString();

    std::mt19937_64 Random;

    size_t Seed = 0;

  protected:
    void InitSpecific() override;

  private:
    std::hash<std::string> _hasher;

    void GenerateSeedString(const std::string& str = std::string());

    SeedString _seedString;
};

#endif // RNG_H

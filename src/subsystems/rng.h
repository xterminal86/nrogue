#ifndef RNG_H
#define RNG_H

#include <chrono>
#include <random>
#include <functional>

using SeedString = std::pair<std::string, std::string>;

class RNG
{
  public:
    void Init();

    void SetSeed(size_t seed);
    void SetSeed(const std::string& string);

    std::string GetSeedAsHex();

    int RandomRange(int min, int max);

    const SeedString& GetSeedString();

    std::mt19937_64 Random;

    size_t Seed = 0;

  private:
    std::hash<std::string> _hasher;

    void GenerateSeedString(const std::string& str = std::string());

    SeedString _seedString;

    bool _initialized = false;
};

#endif // RNG_H

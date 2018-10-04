#ifndef AIDUMMY_H
#define AIDUMMY_H

#include <chrono>
#include <random>

#include "component.h"

class AIDummy : public Component
{
  public:
    AIDummy()
    {
      _hash = typeid(*this).hash_code();
      auto seed = std::chrono::system_clock::now().time_since_epoch().count();
      _rng.seed(seed);
    }

    void Update() override;

  private:
    std::mt19937 _rng;
};

#endif // AIDUMMY_H

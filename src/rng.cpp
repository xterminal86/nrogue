#include "rng.h"

void RNG::Init()
{
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  Random.seed(seed);
}

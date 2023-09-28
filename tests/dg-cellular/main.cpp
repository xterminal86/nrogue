#include "level-builder.h"
#include "rng.h"

int main(int argc, char* argv[])
{
  if (argc < 7)
  {
    printf("Usage: %s "
           "<map_x> <map_y> "
           "<wall_chance> "
           "<birth_threshold> "
           "<death_threshold> "
           "<iterations>\n", argv[0]);
    printf("Example params for 100x100 map: 40, 5, 4, 12\n");
    return 1;
  }

  RNG::Instance().Init();

  Position mapSize;
  int wallChance;
  int threshold;
  int deathThreshold;
  int iterations;

  std::vector<int*> params =
  {
    &mapSize.Y,
    &mapSize.X,
    &wallChance,
    &threshold,
    &deathThreshold,
    &iterations
  };

  int cnt = 1;

  for (int* item : params)
  {
    *item = std::stoi(argv[cnt]);
    cnt++;
  }

  LevelBuilder lb;

  lb.CellularAutomataMethod(mapSize,
                            wallChance,
                            threshold,
                            deathThreshold,
                            iterations);

  std::string mapRaw = lb.GetMapRawString();

  printf("%s\n", mapRaw.data());
  printf("empty / total = %.2f\n", lb.GetEmptyPercent() * 100.0);

  return 0;
}

#include "level-builder.h"
#include "rng.h"

int main(int argc, char* argv[])
{
  if (argc < 6)
  {
    printf("Usage: %s "
           "<map_x> <map_y> "
           "<room_size_min> <room_size_max> <max_iterations>\n",
           argv[0]);
    return 1;
  }

  RNG::Instance().Init();

  //RNG::Instance().SetSeed(1694631412025388189);

  printf("seed is %lu\n", RNG::Instance().Seed);

  Position mapSize;
  Position roomSizes;
  int iterations;

  std::vector<int*> params =
  {
    &mapSize.Y,
    &mapSize.X,
    &roomSizes.Y,
    &roomSizes.X,
    &iterations
  };

  int cnt = 1;

  for (int* item : params)
  {
    *item = std::stoi(argv[cnt]);
    cnt++;
  }

  LevelBuilder lb;

  lb.RoomsMethod(mapSize, roomSizes, iterations);

  std::string mapRaw = lb.GetMapRawString();

  printf("%s\n", mapRaw.data());
  printf("empty / total = %.2f\n", lb.GetEmptyPercent() * 100.0);

  return 0;
}

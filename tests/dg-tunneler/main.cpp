#include "level-builder.h"
#include "rng.h"

int main(int argc, char* argv[])
{
  if (argc < 8)
  {
    printf("Usage: %s <map_x> <map_y> "
           "<iterations> <tunnel_min> "
           "<tunnel_max> <start_x> <start_y>\n", argv[0]);
    return 1;
  }

  RNG::Instance().Init();

  Position mapSize;
  int iterations;
  Position tunnelMinMax;
  Position start;

  std::vector<int*> params =
  {
    &mapSize.X,
    &mapSize.Y,
    &iterations,
    &tunnelMinMax.X,
    &tunnelMinMax.Y,
    &start.X,
    &start.Y
  };

  int cnt = 1;

  for (int* item : params)
  {
    *item = std::stoi(argv[cnt]);
    cnt++;
  }

  LevelBuilder lb;

  lb.TunnelerMethod(mapSize, iterations, tunnelMinMax, start);

  std::string mapRaw = lb.GetMapRawString();

  printf("%s\n", mapRaw.data());
  printf("empty / walls = %.2f%%\n", lb.GetFillingRatio() * 100.0);

  return 0;
}

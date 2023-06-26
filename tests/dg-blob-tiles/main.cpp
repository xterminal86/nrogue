#include "level-builder.h"
#include "rng.h"

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    printf("Usage: %s <map_x> <map_y>\n", argv[0]);
    return 1;
  }

  RNG::Instance().Init();

  Position mapSize;

  std::vector<int*> params =
  {
    &mapSize.X,
    &mapSize.Y,
  };

  int cnt = 1;

  for (int* item : params)
  {
    *item = std::stoi(argv[cnt]);
    cnt++;
  }

  LevelBuilder lb;

  lb.FromBlobTiles(mapSize.X, mapSize.Y);

  std::string mapRaw = lb.GetMapRawString();

  printf("%s\n", mapRaw.data());
  printf("empty / total = %.2f\n", lb.GetEmptyPercent() * 100.0);

  return 0;
}

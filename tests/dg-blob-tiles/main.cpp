#include "level-builder.h"
#include "rng.h"

int main(int argc, char* argv[])
{
  if (argc < 5)
  {
    printf("Usage: %s <map_x> <map_y> "
           "<tile_size_factor> "
           "<wall_size_factor> "
           "[<post_process>]\n", argv[0]);
    return 1;
  }

  RNG::Instance().Init();
  //RNG::Instance().SetSeed(1);

  Position mapSize;
  int tileSizeFactor;
  int wallSizeFactor;
  int postProcess = 0;

  std::vector<int*> params =
  {
    &mapSize.Y,
    &mapSize.X,
    &tileSizeFactor,
    &wallSizeFactor,
    &postProcess
  };

  int paramsCnt = (argc > 6) ? 6 : argc;

  for (int i = 1; i < paramsCnt; i++)
  {
    *params[i - 1] = std::stoi(argv[i]);
  }

  LevelBuilder lb;

  lb.FromBlobTiles(mapSize.X, mapSize.Y,
                   tileSizeFactor,
                   wallSizeFactor,
                   (postProcess != 0));

  std::string mapRaw = lb.GetMapRawString();

  printf("%s\n", mapRaw.data());
  printf("empty / total = %.2f\n", lb.GetEmptyPercent() * 100.0);

  return 0;
}

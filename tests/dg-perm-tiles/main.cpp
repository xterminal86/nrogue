#include "level-builder.h"
#include "rng.h"

int main(int argc, char* argv[])
{
  if (argc < 6)
  {
    printf("Usage: %s "
           "<map_x> <map_y> "
           "<tileset_index> "
           "<post_process> "
           "<remove_bias>\n", argv[0]);
    return 1;
  }

  RNG::Instance().Init();

  Position mapSize;
  int tilesetIndex;
  int postProcess;
  int removeBias;

  std::vector<int*> params =
  {
    &mapSize.Y,
    &mapSize.X,
    &tilesetIndex,
    &postProcess,
    &removeBias
  };

  int cnt = 1;

  for (int* item : params)
  {
    *item = std::stoi(argv[cnt]);
    cnt++;
  }

  LevelBuilder lb;

  lb.FromPermutationTilesMethod(mapSize, tilesetIndex, postProcess, removeBias);

  std::string mapRaw = lb.GetMapRawString();

  printf("%s\n", mapRaw.data());
  printf("empty / total = %.2f\n", lb.GetEmptyPercent() * 100.0);

  return 0;
}

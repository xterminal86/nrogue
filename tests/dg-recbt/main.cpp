#include "level-builder.h"
#include "rng.h"

int main(int argc, char* argv[])
{
  if (argc < 8)
  {
    printf("Usage: %s "
           "<map_x> <map_y> "
           "<start_x> <start_y> "
           "<empty_cells_min> <empty_cells_max> "
           "<passes>\n", argv[0]);
    return 1;
  }

  RNG::Instance().Init();

  Position mapSize;
  Position start;
  RemovalParams removalParams;

  std::vector<int*> params =
  {
    &mapSize.X,
    &mapSize.Y,
    &start.X,
    &start.Y,
    &removalParams.EmptyCellsAroundMin,
    &removalParams.EmptyCellsAroundMax,
    &removalParams.Passes
  };

  int cnt = 1;

  for (int* item : params)
  {
    *item = std::stoi(argv[cnt]);
    cnt++;
  }

  LevelBuilder lb;

  lb.RecursiveBacktrackerMethod(mapSize, start, removalParams);

  std::string mapRaw = lb.GetMapRawString();

  printf("%s\n", mapRaw.data());
  printf("empty / total = %.2f\n", lb.GetEmptyPercent() * 100.0);

  return 0;
}

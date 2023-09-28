#include "level-builder.h"
#include "rng.h"

int main(int argc, char* argv[])
{
  if (argc < 8)
  {
    printf("Usage: %s "
           "<map_x> <map_y> "
           "<tunnel_min> <tunnel_max> "
           "<start_x> <start_y> "
           "<post_process_flag>\n", argv[0]);
    return 1;
  }

  RNG::Instance().Init();

  Position mapSize;
  Position tunnelMinMax;
  Position start;
  int postProcess;

  std::vector<int*> params =
  {
    &mapSize.Y,
    &mapSize.X,
    &tunnelMinMax.X,
    &tunnelMinMax.Y,
    &start.Y,
    &start.X,
    &postProcess
  };

  int cnt = 1;

  for (int* item : params)
  {
    *item = std::stoi(argv[cnt]);
    cnt++;
  }

  LevelBuilder lb;

  lb.BacktrackingTunnelerMethod(mapSize, tunnelMinMax, start, (postProcess != 0));

  std::string mapRaw = lb.GetMapRawString();

  printf("%s\n", mapRaw.data());
  printf("empty / total = %.2f\n", lb.GetEmptyPercent() * 100.0);

  return 0;
}

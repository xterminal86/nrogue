#include "level-builder.h"
#include "rng.h"

int main(int argc, char* argv[])
{
  if (argc < 6)
  {
    printf("Usage: %s "
           "<map_x> <map_y> "
           "<split_x> <split_y> "
           "<min_room_size>\n",
           argv[0]);
    return 1;
  }

  RNG::Instance().Init();
  RNG::Instance().SetSeed(1);

  Position mapSize;
  Position splitRatio;
  int minRoomSize;

  std::vector<int*> params =
  {
    &mapSize.X,
    &mapSize.Y,
    &splitRatio.X,
    &splitRatio.Y,
    &minRoomSize
  };

  int cnt = 1;

  for (int* item : params)
  {
    *item = std::stoi(argv[cnt]);
    cnt++;
  }

  LevelBuilder lb;

  lb.BSPRoomsMethod(mapSize, splitRatio, minRoomSize);

  auto rooms = lb.GetEmptyRooms();

  if (!rooms.empty())
  {
    printf("Found %lu rooms:\n\n", rooms.size());

    for (auto& i : rooms)
    {
      printf("[%i;%i] - [%i;%i]\n", i.first.X, i.first.Y, i.second.X, i.second.Y);
    }
  }

  std::string mapRaw = lb.GetMapRawString();

  printf("%s\n", mapRaw.data());
  printf("empty / total = %.2f\n", lb.GetEmptyPercent() * 100.0);

  return 0;
}

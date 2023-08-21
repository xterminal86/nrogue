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
  //RNG::Instance().SetSeed(1);

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

  auto emptyRooms = lb.GetEmptyRooms();

  printf("Found %lu empty rooms\n", emptyRooms.size());

  for (auto& i : emptyRooms)
  {
    printf("[%d;%d] - [%d;%d] (%d)\n", i.X1, i.Y1, i.X2, i.Y2, i.Area());
  }

  TransformedRoomsWeights weights =
  {
    { TransformedRoom::EMPTY,     { 1, 0 } },
    { TransformedRoom::TREASURY,  { 5, 2 } },
    { TransformedRoom::STORAGE,   { 4, 0 } },
    { TransformedRoom::FLOODED,   { 3, 0 } },
    { TransformedRoom::CHESTROOM, { 2, 1 } },
  };

  lb.TransformRooms(weights);

  std::string mapRaw = lb.GetMapRawString();

  printf("%s\n", mapRaw.data());
  printf("empty / total = %.2f\n", lb.GetEmptyPercent() * 100.0);

  return 0;
}

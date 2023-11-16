#include "level-builder.h"
#include "rng.h"

int main(int argc, char* argv[])
{
  if (argc < 6)
  {
    printf("Usage: %s "
           "<map_x> <map_y> "
           "<room_size_min> <room_size_max> "
           "<door_chance>\n", argv[0]);
    return 1;
  }

  RNG::Instance().Init();

  printf("seed is %llu\n", RNG::Instance().Seed);

  Position mapSize;
  Position roomSizes;

  int doorChance;

  std::vector<int*> params =
  {
    &mapSize.Y,
    &mapSize.X,
    &roomSizes.Y,
    &roomSizes.X,
    &doorChance
  };

  int cnt = 1;

  for (int* item : params)
  {
    *item = std::stoi(argv[cnt]);
    cnt++;
  }

  FeatureRoomsWeights weights =
  {
    { FeatureRoomType::EMPTY,    { 10, 0 } },
    { FeatureRoomType::DIAMOND,  {  3, 3 } },
    { FeatureRoomType::FLOODED,  {  1, 3 } },
    { FeatureRoomType::GARDEN,   {  3, 3 } },
    { FeatureRoomType::PILLARS,  {  5, 0 } },
    { FeatureRoomType::ROUND,    {  5, 3 } },
    { FeatureRoomType::POND,     {  3, 3 } },
    { FeatureRoomType::FOUNTAIN, {  3, 2 } },
    { FeatureRoomType::SHRINE,   { 10, 1 } }
  };

  LevelBuilder lb;

  lb.FeatureRoomsMethod(mapSize,
                        roomSizes,
                        weights,
                        doorChance,
                        mapSize.X * mapSize.Y);

  std::string mapRaw = lb.GetMapRawString();

  printf("%s\n", mapRaw.data());
  printf("empty / total = %.2f\n", lb.GetEmptyPercent() * 100.0);

  return 0;
}

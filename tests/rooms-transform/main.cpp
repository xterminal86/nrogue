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
  //RNG::Instance().SetSeed(1692708011872159643);
  //RNG::Instance().SetSeed(1692710624278623825);
  //RNG::Instance().SetSeed(1692729112495450837);
  //RNG::Instance().SetSeed(1692729726453276167);
  //RNG::Instance().SetSeed(1692903483318007350);
  //RNG::Instance().SetSeed(1692903870204678915);
  //RNG::Instance().SetSeed(1692904310499041332);
  //RNG::Instance().SetSeed(1692970517326539720);
  //RNG::Instance().SetSeed(1692973124684611547);
  //RNG::Instance().SetSeed(1692974725102378573);
  //RNG::Instance().SetSeed(1692976718046812796);
  //RNG::Instance().SetSeed(1692977200693175939);
  //RNG::Instance().SetSeed(1692978491065214259);

  printf("seed is %lu\n", RNG::Instance().Seed);

  Position mapSize;
  Position splitRatio;
  int minRoomSize;

  std::vector<int*> params =
  {
    &mapSize.Y,
    &mapSize.X,
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
    printf("[%2d;%2d] - [%2d;%2d] (%4d) %5d x %-5d\n",
           i.X1, i.Y1, i.X2, i.Y2, i.Area(), i.Width(), i.Height());
  }

  TransformedRoomsWeights weights =
  {
    { TransformedRoom::EMPTY,     {  1, 0 } },
    { TransformedRoom::TREASURY,  {  5, 1 } },
    { TransformedRoom::SHRINE,    { 10, 1 } },
    { TransformedRoom::STORAGE,   {  5, 2 } },
    { TransformedRoom::CHESTROOM, {  5, 1 } },
  };

  lb.TransformRooms(weights);

  std::string mapRaw = lb.GetMapRawString();

  printf("%s\n", mapRaw.data());
  printf("empty / total = %.2f\n", lb.GetEmptyPercent() * 100.0);

  return 0;
}

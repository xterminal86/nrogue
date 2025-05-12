#include "level-builder.h"
#include "rng.h"

void PostProcessForPrinting(LevelBuilder& lb)
{
  auto gm = lb.GeneratedMap();

  for (auto& line : gm)
  {
    for (MapCell& cell : line)
    {
      switch (cell.ZoneMarker)
      {
        case TransformedRoom::CHESTROOM:
        {
          //
          // This will always pass, although it seems that default value for
          // enum is always 0, unless explicitly assigned to the variant.
          // That's why we need additional check.
          //
          if (std::holds_alternative<GameObjectType>(cell.ObjectHere))
          {
            GameObjectType t = std::get<GameObjectType>(cell.ObjectHere);
            if (t == GameObjectType::CONTAINER)
            {
              lb.GetMapRaw()[cell.Coordinates.X][cell.Coordinates.Y] = 'C';
            }
          }
        }
        break;

        // ---------------------------------------------------------------------

        default:
          // Not doing shit.
          break;
      }
    }
  }
}

// =============================================================================

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

  printf("seed is %llu\n", RNG::Instance().Seed);

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
    //{ TransformedRoom::EMPTY,           {  1, 0 } },
    //{ TransformedRoom::TREASURY,        {  5, 1 } },
    //{ TransformedRoom::STORAGE,         {  5, 2 } },
    //{ TransformedRoom::FLOODED,         { 10, 1 } },
    { TransformedRoom::CHESTROOM,       {  5, 1 } },
    //{ TransformedRoom::GRAVEYARD,       {  5, 1 } },
    //{ TransformedRoom::LIBRARY,         {  5, 1 } },
    //{ TransformedRoom::SHRINE,          { 10, 1 } },
    //{ TransformedRoom::HALLOWED_GROUND, { 10, 1 } },
    //{ TransformedRoom::CURSED_GROUND,   { 10, 1 } },
    //{ TransformedRoom::BATTLEFIELD,     { 10, 1 } },
    //{ TransformedRoom::ZOO,             { 10, 1 } },
  };

  lb.TransformRooms(weights);

  PostProcessForPrinting(lb);

  std::string mapRaw = lb.GetMapRawString();

  printf("%s\n", mapRaw.data());
  printf("empty / total = %.2f\n", lb.GetEmptyPercent() * 100.0);

  return 0;
}

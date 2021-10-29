#include "map-level-town.h"

#include "player.h"
#include "rect.h"
#include "game-objects-factory.h"
#include "items-factory.h"
#include "game-object-info.h"
#include "door-component.h"
#include "trigger-component.h"
#include "trigger-handlers.h"
#include "pathfinder.h"
#include "printer.h"
#include "application.h"

MapLevelTown::MapLevelTown(int sizeX, int sizeY, MapType type, int dungeonLevel) :
  MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
  for (auto& kvp : _iterationMap)
  {
    for (auto& i : kvp.second)
    {
      auto l = Util::DecodeMap(i);
      _layoutsForLevel.push_back(l);
    }
  }

  /*
  _layoutsForLevel =
  {
    // Common houses
    // 0
    {
      "##+##",
      "#...#",
      "#...#",
      "#B..#",
      "##-##"
    },
    // 1
    {
      "#+#####",
      "#..#.B#",
      "|..+..|",
      "#..#..#",
      "#######"
    },
    // 2
    {
      "###+###",
      "#.....#",
      "#.....#",
      "|..#+##",
      "#..#..#",
      "#..#.B#",
      "#######"
    },
    // Rich residents
    // 3
    {
      "####+######-###",
      "#.......#.....#",
      "|.......+.....|",
      "#.......#.....#",
      "##+###+##.....#",
      "#...#...#.....#",
      "#...#...#.....|",
      "#B..#B..#.....#",
      "##-###-####-###"
    },
    // 4
    {
      "#########-#####",
      "#......#.....B#",
      "|......#......|",
      "#......#......#",
      "##+########+###",
      "#   #ggggggggg#",
      "#   #g#mm#mm#g#",
      "+    gm~~~~~mg#",
      "#   #gm~~F~~mg#",
      "+    gm~~~~~mg#",
      "#   #g#mm#mm#g#",
      "#   #ggggggggg#",
      "##+########+###",
      "#......#......#",
      "|......#......|",
      "#......#.....B#",
      "#########-#####"
    },
    // 5
    {
      "####+##########",
      "#ggg ggg#....B#",
      "#gTg gTg#.....|",
      "#ggg ggg#.....#",
      "#ggg ggg#.....#",
      "#gTg gTg#.....|",
      "#ggg ggg#.....#",
      "####+######+###",
      "#.......#.....#",
      "#.......#.....#",
      "|.......+.....|",
      "#.......#.....#",
      "#.......#.....#",
      "###############"
    },
    // Blacksmith
    // 6
    {
       "#########",
       "# _  #.B#",
     R"(#/.\_#..#)",
       "#...T#..#",
       "+...T#..#",
       "#....+..#",
       "#########"
    },
    // Church
    // 7
    {
      "................####-####........",
      "................#       #........",
      "................#       #........",
      "................|       |........",
      "####-###-###-####       #........",
      "#               +       #........",
      "#  P P P P P P #############-####",
      "#    h h h h h h        #       #",
      "+    h h h h h h        #       |",
      "#                       A   /   |",
      "+    h h h h h h        #       |",
      "#    h h h h h h        #       #",
      "#  P P P P P P #############-####",
      "#               +       #........",
      "####-###-###-####       #........",
      "................|       |........",
      "................#       #........",
      "................#       #........",
      "................####-####........",
    },
    // Garden
    // 8
    {
      "T.T.T.T.T.T.T.T.T.T",
      "........~~~........",
      "#####..~WWW~..#####",
      "#~~~#.~WWWWW~.#~~~#",
      "#~F~#.~WWWWW~.#~F~#",
      "#~~~#.~WWWWW~.#~~~#",
      "#####..~WWW~..#####",
      "........~~~........",
      "T.T.T.T.T.T.T.T.T.T",
    },
    // Portal square
    // 9
    {
      ".......",
      ".#~t~#.",
      ".~~t~~.",
      ".ttPtt.",
      ".~~t~~.",
      ".#~t~#.",
      ".......",
    },
    // Mine entrance
    // 10
    {
      "#######",
      "#.....#",
      "#.#.#.#",
      "+.....#",
      "#.#.#.#",
      "#.....#",
      "#######",
    }
  };
  */
}

void MapLevelTown::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

void MapLevelTown::CreateLevel()
{
  Peaceful = true;

  VisibilityRadius = GlobalConstants::MaxVisibilityRadius;

  GameObjectInfo t;
  t.Set(false, false, '.', Colors::ShadesOfGrey::Four, Colors::BlackColor, "Ground");

  Rect r(0, 0, MapSize.X - 1, MapSize.Y - 1);

  FillArea(r.X1, r.Y1, r.X2, r.Y2, t);

  t.Set(true, true, ' ', Colors::BlackColor, Colors::ShadesOfGrey::Six, "Walls");

  auto bounds = r.GetBoundaryElements();
  for (auto& pos : bounds)
  {
    InsertStaticObject(pos.X, pos.Y, t);
  }

  ReplaceGroundWithGrass();

  LevelStart.X = 5;
  LevelStart.Y = 8;

  _playerRef->PosX = 5;
  _playerRef->PosY = 8;

  AdjustCamera();

  CreatePlayerHouse();

  // Bydlo (that includes you, btw) ;-)

  int numHouses = 4;

  auto rotatedRoom = Util::RotateRoomLayout(_layoutsForLevel[0], RoomLayoutRotation::CCW_180);

  int offset = 15;
  for (int i = 0; i < numHouses; i++)
  {
    CreateRoom(18 + offset * i, 3, rotatedRoom);
  }

  CreateBlacksmith(78, 3, _layoutsForLevel[6]);

  // Majors

  CreateRoom(5, 20, _layoutsForLevel[3]);

  auto room = Util::RotateRoomLayout(_layoutsForLevel[3], RoomLayoutRotation::CCW_270);
  CreateRoom(5, 32, room);

  CreateRoom(25, 30, _layoutsForLevel[4]);
  CreateRoom(45, 33, _layoutsForLevel[5]);

  // Other stuff

  CreateChurch(63, 15);

  PlaceGarden(33, 20);

  PlacePortalSquare(22, 21);

  PlaceMineEntrance(88, 41);

  CreateTownGates();

  BuildRoads();

  RecordEmptyCells();

  LevelExit.X = 91;
  LevelExit.Y = 44;

  GameObjectsFactory::Instance().CreateStairs(this, LevelExit.X, LevelExit.Y, '>', MapType::MINES_1);

  // *** FIXME: debug

  // All scrolls
  /*
  int count = 0;
  for (auto& item : GlobalConstants::ScrollValidSpellTypes)
  {
    auto scroll = GameObjectsFactory::Instance().CreateScroll(1 + count, 1, item, ItemPrefix::UNCURSED);
    ItemComponent* ic = scroll->GetComponent<ItemComponent>();
    ic->Data.IsIdentified = false;
    InsertGameObject(scroll);
    count++;
  }
  */

  // All potions
  /*
  count = 0;
  auto map = GlobalConstants::PotionNameByType;
  for (auto& kvp : map)
  {
    auto go = GameObjectsFactory::Instance().CreatePotion(kvp.first);
    go->PosX = 1 + count;
    go->PosY = 1;
    ItemComponent* ic = go->GetComponent<ItemComponent>();
    ic->Data.IsIdentified = false;
    InsertGameObject(go);
    count++;
  }
  */

  // Random wands

  /*
  for (size_t x = 1; x < 16; x++)
  {
    auto wand = GameObjectsFactory::Instance().CreateRandomWand();
    ItemComponent* ic = wand->GetComponent<ItemComponent>();
    ic->Data.IsIdentified = true;
    wand->PosX = x;
    wand->PosY = 1;
    InsertGameObject(wand);
  }
  */

  /*
  // Rings

  ItemQuality q = ItemQuality::EXCEPTIONAL;

  std::map<ItemQuality, int> multByQ =
  {
    { ItemQuality::DAMAGED,      1 },
    { ItemQuality::FLAWED,       2 },
    { ItemQuality::NORMAL,       3 },
    { ItemQuality::FINE,         4 },
    { ItemQuality::EXCEPTIONAL,  5 },
  };

  for (int i = 0; i < 10; i++)
  {
    int min = 20 + 5 * (multByQ[q] - 1);
    int max = 21 + 20 * multByQ[q];
    int percentage = RNG::Instance().RandomRange(min, max);

    ItemBonusStruct bs;
    bs.Type = ItemBonusType::THORNS;
    bs.Id = _playerRef->ObjectId();
    bs.Duration = -1;
    bs.BonusValue = percentage;

    auto ring = GameObjectsFactory::Instance().CreateAccessory(1 + i, 1, EquipmentCategory::RING, { bs }, ItemPrefix::UNCURSED, q);
    InsertGameObject(ring);
  }
  */

  /*
  ItemBonusStruct bs;
  bs.Type = ItemBonusType::REGEN;
  bs.BonusValue = 1;
  bs.Period = 5;

  auto ring1 = GameObjectsFactory::Instance().CreateAccessory(1, 1, EquipmentCategory::RING, { bs }, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
  auto ring2 = GameObjectsFactory::Instance().CreateAccessory(1, 1, EquipmentCategory::RING, { bs }, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
  InsertGameObject(ring1);
  InsertGameObject(ring2);
  */

  /*
  for (int i = 0; i < 20; i++)
  {
    int val = RNG::Instance().RandomRange(0, 2);
    EquipmentCategory cat = (val == 0) ? EquipmentCategory::NECK : EquipmentCategory::RING;
    auto ring = GameObjectsFactory::Instance().CreateRandomAccessory(i + 1, 1, cat);
    InsertGameObject(ring);
  }
  */

  /*
  auto scroll = GameObjectsFactory::Instance().CreateScroll(1, 1, SpellType::LIGHT, ItemPrefix::UNCURSED);
  InsertGameObject(scroll);

  ItemBonusStruct bs = { ItemBonusType::HP, 5, 0, false };

  auto ring1 = GameObjectsFactory::Instance().CreateAccessory(1, 1, EquipmentCategory::RING, { bs }, ItemPrefix::UNCURSED);
  InsertGameObject(ring1);

  auto ring2 = GameObjectsFactory::Instance().CreateAccessory(2, 1, EquipmentCategory::RING, { bs }, ItemPrefix::UNCURSED);
  InsertGameObject(ring2);

  auto scroll = GameObjectsFactory::Instance().CreateScroll(1, 1, SpellType::MAGIC_MAPPING);
  InsertGameObject(scroll);

  auto lwand = GameObjectsFactory::Instance().CreateWand(1, 1, WandMaterials::IVORY_2, SpellType::LIGHT, ItemPrefix::BLESSED);
  InsertGameObject(lwand);

  auto wand = GameObjectsFactory::Instance().CreateWand(1, 1, WandMaterials::GOLDEN, SpellType::FIREBALL, ItemPrefix::UNCURSED);
  InsertGameObject(wand);

  auto bow = GameObjectsFactory::Instance().CreateRangedWeapon(2, 1, RangedWeaponType::SHORT_BOW, ItemPrefix::UNCURSED);
  InsertGameObject(bow);

  auto arrows = GameObjectsFactory::Instance().CreateArrows(3, 1, ArrowType::ARROWS, ItemPrefix::UNCURSED);
  InsertGameObject(arrows);

  auto door = GameObjectsFactory::Instance().CreateDoor(4, 1, false, "Test Door", 1);
  InsertStaticObject(door);

  auto pickaxe = GameObjectsFactory::Instance().CreateWeapon(5, 1, WeaponType::PICKAXE, ItemPrefix::UNCURSED);
  InsertGameObject(pickaxe);

  auto xbow = GameObjectsFactory::Instance().CreateRangedWeapon(2, 1, RangedWeaponType::HEAVY_XBOW, ItemPrefix::BLESSED);
  InsertGameObject(xbow);

  auto bolts = GameObjectsFactory::Instance().CreateArrows(1, 3, ArrowType::BOLTS, ItemPrefix::UNCURSED, 50);
  InsertGameObject(bolts);
  */
  // ***
}

void MapLevelTown::BuildRoads()
{
  /*
  std::vector<Position> roadMarks;

  // Build roads

  for (int x = 1; x < MapSize.X - 1; x++)
  {
    for (int y = 1; y < MapSize.Y - 1; y++)
    {
      // Skip near town gates
      if ((x == 1 && y == 13) || (x == 1 && y == 14))
      {
        continue;
      }

      Position left  = { x, y - 1 };
      Position right = { x, y + 1 };
      Position up    = { x - 1, y };
      Position down  = { x + 1, y };

      std::vector<Position> tilesToCheck =
      {
        left, right, up, down
      };

      if (StaticMapObjects[x][y] == nullptr && MapArray[x][y]->Image == '.')
      {
        for (auto& c : tilesToCheck)
        {
          if (StaticMapObjects[c.X][c.Y] != nullptr
           && StaticMapObjects[c.X][c.Y]->Image == '+')
          {
            roadMarks.push_back({ x, y });
            break;
          }
        }
      }
    }
  }

  roadMarks.push_back({ 62, 25 });

  std::sort(roadMarks.begin(),
            roadMarks.end(),
            [](const Position& a, const Position& b)
            {
              return (a.X <= b.X && a.Y <= b.Y);
            });

  // Draw roads

  for (auto& c : roadMarks)
  {
    GameObjectInfo gi;
    gi.Set(false, false, '.', Colors::ShadesOfGrey::Ten, Colors::ShadesOfGrey::Eight, "Path");

    MapArray[c.X][c.Y]->MakeTile(gi);
  }

  for (size_t i = 0; i < roadMarks.size() - 1; i++)
  {
    BuildAndDrawRoad(roadMarks[i], roadMarks[i + 1]);
  }
  */

  // Path from gates
  BuildAndDrawRoad({ 1, 13 }, { 62, 13 });
  BuildAndDrawRoad({ 1, 14 }, { 62, 14 });

  // From player's house
  BuildAndDrawRoad({ 5, 8 }, { 5, 13 });

  // Other houses
  BuildAndDrawRoad({ 20, 8 }, { 20, 13 });
  BuildAndDrawRoad({ 35, 8 }, { 35, 13 });
  BuildAndDrawRoad({ 50, 8 }, { 50, 13 });
  BuildAndDrawRoad({ 65, 8 }, { 62, 13 });

  // Blacksmith
  BuildAndDrawRoad({ 77, 7 }, { 65, 8 });

  // Villa
  BuildAndDrawRoad({ 24, 37 }, { 24, 29 });
  BuildAndDrawRoad({ 24, 39 }, { 24, 29 });

  // Hotel
  BuildAndDrawRoad({ 14, 36 }, { 14, 29 });

  // Cook
  BuildAndDrawRoad({ 9, 19 }, { 9, 14 });

  // Church Way
  BuildAndDrawRoad({ 14, 29 }, { 62, 29 });
  BuildAndDrawRoad({ 62, 25 }, { 62, 29 });

  // Town Hall
  BuildAndDrawRoad({ 49, 32 }, { 49, 29 });

  // Church
  BuildAndDrawRoad({ 62, 23 }, { 62, 14 });
  BuildAndDrawRoad({ 62, 25 }, { 62, 14 });
}

void MapLevelTown::BuildAndDrawRoad(const Position& start,
                                    const Position& end)
{
  Pathfinder pf;
  auto path = pf.BuildRoad(this, start, end, { '~' });

  GameObjectInfo gi;
  gi.Set(false, false, '.', Colors::ShadesOfGrey::Ten, Colors::ShadesOfGrey::Eight, "Path");

  MapArray[start.X][start.Y]->MakeTile(gi);

  DrawRoad(path);
}

void MapLevelTown::DrawRoad(const std::stack<Position>& path)
{
  auto pathCopy = path;

  while (!pathCopy.empty())
  {
    Position c = pathCopy.top();

    GameObjectInfo gi;
    gi.Set(false, false, '.', Colors::ShadesOfGrey::Ten, Colors::ShadesOfGrey::Eight, "Path");

    MapArray[c.X][c.Y]->MakeTile(gi);

    pathCopy.pop();
  }
}

void MapLevelTown::ReplaceGroundWithGrass()
{
  for (int x = 1; x < MapSize.X - 1; x++)
  {
    for (int y = 1; y < MapSize.Y - 1; y++)
    {
      if (MapArray[x][y]->Image == '.')
      {
        PlaceGrassTile(x, y, FlowersFrequency);
      }
    }
  }
}

void MapLevelTown::FillArea(int ax, int ay, int aw, int ah, const GameObjectInfo& tileToFill)
{
  for (int x = ax; x <= ax + aw; x++)
  {
    for (int y = ay; y <= ay + ah; y++)
    {
      MapArray[x][y]->MakeTile(tileToFill);
    }
  }
}

void MapLevelTown::CreateBlacksmith(int x, int y, const std::vector<std::string>& layout, bool randomizeOrientation)
{
  GameObjectInfo t;

  int posX = x;
  int posY = y;

  std::vector<std::string> newLayout = layout;

  std::vector<RoomLayoutRotation> rotations =
  {
    RoomLayoutRotation::NONE,
    RoomLayoutRotation::CCW_90,
    RoomLayoutRotation::CCW_180,
    RoomLayoutRotation::CCW_270
  };

  if (randomizeOrientation)
  {
    int index = RNG::Instance().Random() % rotations.size();
    newLayout = Util::RotateRoomLayout(layout, rotations[index]);
  }

  for (auto& row : newLayout)
  {
    for (auto& c : row)
    {
      switch (c)
      {
        case '#':
          PlaceWall(posX, posY, c, Colors::ShadesOfGrey::Eight, Colors::BlackColor, "Stone Wall");
          break;

        case 'T':
          t.Set(true, false, c, Colors::IronColor, Colors::BlackColor, "Workbench");
          InsertStaticObject(posX, posY, t);
          break;

        case 'B':
          t.Set(true, false, c, Colors::WhiteColor, Colors::BlackColor, "Bed");
          InsertStaticObject(posX, posY, t);
          break;

        case '.':
          PlaceGroundTile(posX, posY, ' ', Colors::BlackColor, Colors::RoomFloorColor, "Wooden Floor");
          break;

        case '\\':
        case '/':
        case '_':
        case '|':
        case '-':
        case ' ':
        {
          t.Set(true, true, c, Colors::WhiteColor, Colors::BlackColor, "Forge");
          InsertStaticObject(posX, posY, t);
        }
        break;

        case '+':
          PlaceDoor(posX, posY);
          break;
      }

      posX++;
    }

    posX = x;
    posY++;
  }
}

void MapLevelTown::CreateRoom(int x, int y, const std::vector<std::string>& layout, bool randomizeOrientation)
{
  int posX = x;
  int posY = y;

  std::vector<std::string> newLayout = layout;

  std::vector<RoomLayoutRotation> rotations =
  {
    RoomLayoutRotation::NONE,
    RoomLayoutRotation::CCW_90,
    RoomLayoutRotation::CCW_180,
    RoomLayoutRotation::CCW_270
  };

  if (randomizeOrientation)
  {
    int index = RNG::Instance().Random() % rotations.size();
    newLayout = Util::RotateRoomLayout(layout, rotations[index]);
  }

  for (auto& row : newLayout)
  {
    for (auto& c : row)
    {
      GameObjectInfo t;

      switch (c)
      {
        case '#':
          PlaceWall(posX, posY, c, Colors::ShadesOfGrey::Eight, Colors::BlackColor, "Stone Wall");
          break;

        case 'g':
          PlaceGrassTile(posX, posY);
          break;

        case 'F':
          t.Set(true, false, c, Colors::WhiteColor, Colors::DeepWaterColor, "Fountain");
          InsertStaticObject(posX, posY, t);
          break;

        case 'T':
          PlaceTree(posX, posY);
          break;

        case 'B':
          t.Set(true, false, c, Colors::WhiteColor, Colors::BlackColor, "Bed");
          InsertStaticObject(posX, posY, t);
          break;

        case '.':
          PlaceGroundTile(posX, posY, '-', "#490E11", Colors::RoomFloorColor, "Wooden Floor");
          break;

        // NOTE: since ' ' (i.e. 'Space', 32 ASCII) is a transparent tile in the tileset,
        // you must use bg color to color it, because colored tile for background
        // is made from tile 219, which is a white block. So basically for ' ' tile
        // foreground color is ignored.
        //
        // To allow fog of war to cover floor made of
        // background colored ' ', set FgColor to black.
        case ' ':
          PlaceGroundTile(posX, posY, c, Colors::BlackColor, Colors::ShadesOfGrey::Four, "Stone Tiles");
          break;

        case 'm':
          t.Set(true, false, '#', Colors::MarbleColor, Colors::GrassColor, "Marble Fence");
          InsertStaticObject(posX, posY, t);
          break;

        case '~':
          PlaceShallowWaterTile(posX, posY);
          break;

        case '|':
        case '-':
          t.Set(true, false, c, Colors::WhiteColor, Colors::BlackColor, "Window");
          InsertStaticObject(posX, posY, t);
          break;

        case '+':
          PlaceDoor(posX, posY);
          break;
      }

      posX++;
    }

    posX = x;
    posY++;
  }
}

void MapLevelTown::CreateChurch(int x, int y)
{
  int posX = x;
  int posY = y;

  GameObjectInfo t;

  for (auto& row : _layoutsForLevel[7])
  {
    for (auto& c : row)
    {
      switch (c)
      {
        case '#':
          PlaceWall(posX, posY, c, Colors::ShadesOfGrey::Eight, Colors::BlackColor, "Stone Wall");
          break;

        case 'P':
          PlaceWall(posX, posY, '#', Colors::ShadesOfGrey::Eight, Colors::BlackColor, "Stone Pillar");
          break;

        case '|':
        case '-':
          t.Set(true, true, c, Colors::WhiteColor, Colors::BlackColor, "Stained Glass");
          InsertStaticObject(posX, posY, t);
          break;

        // Check out important comments in CreateRoom()
        case ' ':
          PlaceGroundTile(posX, posY, c, Colors::BlackColor, Colors::ShadesOfGrey::Four, "Stone Tiles");
          break;

        case '+':
          PlaceDoor(posX, posY);
          break;

        case 'A':
        {
          auto pc = _playerRef->GetClass();
          // Only members of the clergy are allowed into the altar room
          size_t openedBy = (pc != PlayerClass::ARCANIST) ? GlobalConstants::OpenedByNobody : GlobalConstants::OpenedByAnyone;
          PlaceDoor(posX, posY, false, openedBy, "Royal Gate");
        }
        break;

        case 'h':
          t.Set(false, false, c, Colors::WoodColor, Colors::BlackColor, "Wooden Bench", "?Bench?");
          InsertStaticObject(posX, posY, t);
          break;

        case '/':
        {
          // Globally updated game objects are not shown under
          // fog of war by default,
          // so sometimes we must "adjust" tiles if we want
          // certain objects to be shown, like in this case.
          ShrineType shrineType = ShrineType::KNOWLEDGE;
          std::string description = GlobalConstants::ShrineNameByType.at(shrineType);
          t.Set(true, false, '/', Colors::ShadesOfGrey::Four, Colors::BlackColor, description, "?Shrine?");
          InsertStaticObject(posX, posY, t);

          // Tiles are updated only around player.
          // Shrine has some logic (buff and timeout count), thus
          // we must make it a global game object so it could be updated
          // every turn no matter where the player is.
          auto go = GameObjectsFactory::Instance().CreateShrine(posX, posY, shrineType, 100);
          InsertGameObject(go);
        }
        break;
      }

      posX++;
    }

    posX = x;
    posY++;
  }
}

void MapLevelTown::CreatePlayerHouse()
{
  auto rot = Util::RotateRoomLayout(_layoutsForLevel[0], RoomLayoutRotation::CCW_180);
  CreateRoom(3, 3, rot);

  GameObjectInfo t;
  t.Set(true, false, 'C', Colors::WhiteColor, Colors::ChestColor, "Stash");

  Position cp(6, 6);
  InsertStaticObject(6, 6, t);

  auto stash = GameObjectsFactory::Instance().CreateContainer("Stash", Colors::ChestColor, 'C', cp.X, cp.Y);
  InsertGameObject(stash);
}

void MapLevelTown::CreateNPCs()
{
  Rect playerHome   = {  2,  2,  8,  8 };
  Rect mineEntrance = { 87, 40, 95, 48 };
  Rect altarRoom    = { 87, 21, 95, 27 };

  std::vector<NPCType> npcs =
  {
    NPCType::CLAIRE,
    NPCType::CLOUD,
    NPCType::IARSPIDER,
    NPCType::MILES,
    NPCType::PHOENIX,
    NPCType::STEVE,
    NPCType::GIMLEY
  };

  std::vector<Position> visited;

  for (auto& npc : npcs)
  {
    std::vector<Position> emptyCells;

    for (int x = 1; x <= MapSize.X - 1; x++)
    {
      for (int y = 1; y <= MapSize.Y - 1; y++)
      {
        // Skip area around player house, altar room and mine entrance
        if (SkipArea({ x, y }, playerHome)
         || SkipArea({ x, y }, mineEntrance)
         || SkipArea({ x, y }, altarRoom))
        {
          continue;
        }

        bool alreadyAdded = false;

        for (auto& c : visited)
        {
          if (c.X == x && c.Y == y)
          {
            alreadyAdded = true;
            break;
          }
        }

        bool isBlocking = IsCellBlocking({ x, y });
        bool isSpecial = MapArray[x][y]->Special;

        // Also avoid shallow water tiles
        // or NPC may spawn inside walled fountain.
        if (!alreadyAdded && !isBlocking
         && !isSpecial && MapArray[x][y]->Image != '~')
        {
          emptyCells.push_back(Position(x, y));
        }
      }
    }

    int index = RNG::Instance().RandomRange(0, emptyCells.size());

    auto go = GameObjectsFactory::Instance().CreateNPC(emptyCells[index].X, emptyCells[index].Y, npc);
    InsertActor(go);

    visited.push_back(Position(emptyCells[index].X, emptyCells[index].Y));
  }

  GameObject* go = nullptr;

  go = GameObjectsFactory::Instance().CreateNPC(73, 24, NPCType::TIGRA);
  InsertActor(go);

  // Traders

  go = GameObjectsFactory::Instance().CreateNPC(83, 24, NPCType::MARTIN, true, ServiceType::BLESS);
  InsertActor(go);

  go = GameObjectsFactory::Instance().CreateNPC(9, 22, NPCType::CASEY, true);
  InsertActor(go);

  go = GameObjectsFactory::Instance().CreateNPC(9, 43, NPCType::MAYA, true, ServiceType::IDENTIFY);
  InsertActor(go);

  go = GameObjectsFactory::Instance().CreateNPC(81, 7, NPCType::GRISWOLD, true, ServiceType::REPAIR);
  InsertActor(go);
}

void MapLevelTown::PlaceMineEntrance(int x, int y)
{
  int posX = x;
  int posY = y;

  GameObjectInfo t;

  for (auto& row : _layoutsForLevel[10])
  {
    for (auto& c : row)
    {
      switch (c)
      {
        case '#':
          PlaceWall(posX, posY, ' ', Colors::BlackColor, Colors::BrickColor, "Brick Wall");
          break;

        case '.':
          PlaceGroundTile(posX, posY, ' ', Colors::BlackColor, Colors::RoomFloorColor, "Wooden Floor");
          break;

        case '+':
          PlaceDoor(posX, posY);
          break;
      }

      posX++;
    }

    posX = x;
    posY++;
  }
}

void MapLevelTown::PlaceGarden(int x, int y)
{
  int posX = x;
  int posY = y;

  GameObjectInfo t;

  for (auto& row : _layoutsForLevel[8])
  {
    for (auto& c : row)
    {
      switch (c)
      {
        case '#':
          t.Set(true, false, '#', Colors::MarbleColor, Colors::GrassColor, "Marble Fence");
          InsertStaticObject(posX, posY, t);
          break;

        case '.':
          PlaceGrassTile(posX, posY, FlowersFrequency);
          break;

        case 'T':
          PlaceTree(posX, posY);
          break;

        case '~':
          PlaceShallowWaterTile(posX, posY);
          break;

        case 'W':
          PlaceDeepWaterTile(posX, posY);
          break;

        case 'F':
          t.Set(true, false, c, Colors::WhiteColor, Colors::DeepWaterColor, "Fountain");
          InsertStaticObject(posX, posY, t);
          break;
      }

      posX++;
    }

    posX = x;
    posY++;
  }
}

void MapLevelTown::PlacePortalSquare(int x, int y)
{
  int posX = x;
  int posY = y;

  GameObjectInfo t;

  for (auto& row : _layoutsForLevel[9])
  {
    for (auto& c : row)
    {
      switch (c)
      {
        case '#':
          PlaceWall(posX, posY, '#', Colors::MarbleColor, Colors::GrassColor, "Marble Column");
          break;

        case '~':
          PlaceDeepWaterTile(posX, posY);
          break;

        case '.':
          PlaceGrassTile(posX, posY, FlowersFrequency);
          break;

        case 'P':
        {
          _townPortalPos.Set(posX, posY);
          t.Set(false, false, ' ', Colors::BlackColor, Colors::ShadesOfGrey::Ten, "Stone Tiles");
          MapArray[posX][posY]->MakeTile(t);
          MapArray[posX][posY]->Special = true;
        }
        break;

        case 't':
        {
          t.Set(false, false, ' ', Colors::BlackColor, Colors::ShadesOfGrey::Ten, "Stone Tiles");
          MapArray[posX][posY]->MakeTile(t);
          MapArray[posX][posY]->Special = true;
        }
        break;
      }

      posX++;
    }

    posX = x;
    posY++;
  }
}

void MapLevelTown::CreateTownGates()
{
  GameObject* gate1 = ItemsFactory::Instance().CreateDummyItem("Village Gates", '+', Colors::WhiteColor, Colors::BlackColor, std::vector<std::string>());
  GameObject* gate2 = ItemsFactory::Instance().CreateDummyItem("Village Gates", '+', Colors::WhiteColor, Colors::BlackColor, std::vector<std::string>());

  std::function<void()> fn = []()
  {
    Printer::Instance().AddMessage("The gates are closed");
  };

  gate1->Blocking = true;
  gate1->BlocksSight = true;

  gate1->PosX = 0;
  gate1->PosY = 13;

  gate2->Blocking = true;
  gate2->BlocksSight = true;

  gate2->PosX = 0;
  gate2->PosY = 14;

  gate1->InteractionCallback = fn;
  gate2->InteractionCallback = fn;

  InsertStaticObject(gate1);
  InsertStaticObject(gate2);
}

bool MapLevelTown::SkipArea(const Position& pos, const Rect& area)
{
  return (pos.X >= area.X1
       && pos.X <= area.X2
       && pos.Y >= area.Y1
       && pos.Y <= area.Y2);
}

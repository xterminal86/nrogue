#include "map-level-town.h"

#include "player.h"
#include "rect.h"
#include "game-objects-factory.h"
#include "monsters-inc.h"
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

// =============================================================================

const Position& MapLevelTown::TownPortalPos()
{
  return _townPortalPos;
}

// =============================================================================

void MapLevelTown::PrepareMap()
{
  MapLevelBase::PrepareMap();

  CreateLevel();
}

// =============================================================================

void MapLevelTown::CreateLevel()
{
  Peaceful = true;

  VisibilityRadius = GlobalConstants::MaxVisibilityRadius;

  GameObjectInfo t;
  t.Set(false,
        false,
        '.',
        Colors::ShadesOfGrey::Four,
        Colors::BlackColor,
        Strings::TileNames::GroundText);

  FillArea(0, 0, MapSize.X - 1, MapSize.Y - 1, t);

  ReplaceGroundWithGrass();

  LevelStart.X = 1;
  LevelStart.Y = 13;

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

  BuildRoads();

  CreateBorders('#',
                Colors::ShadesOfGrey::Four,
                Colors::ShadesOfGrey::Two,
                Strings::TileNames::WallsText);

  CreateTownGates();

  RecordEmptyCells();

  LevelExit.X = 91;
  LevelExit.Y = 44;

  GameObjectsFactory::Instance().CreateStairs(this, LevelExit.X, LevelExit.Y, '>', MapType::MINES_1);

  // *** FIXME: debug

  // ========================= CREATE 20 RANDOM MELEE WEAPONS ==================

  /*
  for (int i = 0; i < 20; i++)
  {
    GameObject* go = ItemsFactory::Instance().CreateRandomMeleeWeapon();
    go->PosX = 1 + i;
    go->PosY = 1;
    PlaceGameObject(go);
  }
  */

  // ============================ CREATE ITEM WITH BONUS =======================

  /*
  ItemBonusStruct bs;
  bs.BonusValue = 2;
  bs.Type = ItemBonusType::KNOCKBACK;
  bs.FromItem = true;

  GameObject* go = ItemsFactory::Instance().CreateRangedWeapon(1, 1, RangedWeaponType::XBOW, ItemPrefix::UNCURSED, ItemQuality::NORMAL, { bs });
  ItemComponent* ic = go->GetComponent<ItemComponent>();
  ic->Data.Damage.SetMin(1);
  ic->Data.Damage.SetMax(1);
  PlaceGameObject(go);

  GameObject* bolts = ItemsFactory::Instance().CreateArrows(1, 1, ArrowType::BOLTS, ItemPrefix::UNCURSED, 10);
  PlaceGameObject(bolts);

  GameObject* dag = ItemsFactory::Instance().CreateWeapon(1, 1, WeaponType::DAGGER, ItemPrefix::UNCURSED, ItemQuality::NORMAL, { bs });
  PlaceGameObject(dag);
  */

  /*
  ItemBonusStruct b;
  b.BonusValue = 1;
  b.Period = -1;
  b.Type = ItemBonusType::LEVITATION;
  b.FromItem = true;

  GameObject* go = ItemsFactory::Instance().CreateAccessory(1, 1, EquipmentCategory::RING, { b }, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
  PlaceGameObject(go);
  */

  // =================================== RANDOM WANDS ==========================

  /*
  for (size_t x = 1; x < 16; x++)
  {
    auto wand = ItemsFactory::Instance().CreateRandomWand();
    ItemComponent* ic = wand->GetComponent<ItemComponent>();
    ic->Data.IsIdentified = true;
    wand->PosX = x;
    wand->PosY = 1;
    PlaceGameObject(wand);
  }
  */

  // =================================== RINGS =================================

  /*
  ItemQuality q = ItemQuality::EXCEPTIONAL;

  std::unordered_map<ItemQuality, int> multByQ =
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
    PlaceGameObject(ring);
  }
  */

  /*
  ItemBonusStruct bs;
  bs.Type = ItemBonusType::REGEN;
  bs.BonusValue = 1;
  bs.Period = 5;

  auto ring1 = GameObjectsFactory::Instance().CreateAccessory(1, 1, EquipmentCategory::RING, { bs }, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
  auto ring2 = GameObjectsFactory::Instance().CreateAccessory(1, 1, EquipmentCategory::RING, { bs }, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
  PlaceGameObject(ring1);
  PlaceGameObject(ring2);
  */
}

// =============================================================================

void MapLevelTown::BuildRoads()
{
  // ---------------------------------------------------------------------------
  // Path from gates.
  // ---------------------------------------------------------------------------
  BuildAndDrawRoad({ 1, 13 }, { 62, 13 });
  BuildAndDrawRoad({ 1, 14 }, { 62, 14 });


  // ---------------------------------------------------------------------------
  // From player's house.
  // ---------------------------------------------------------------------------
  BuildAndDrawRoad({ 5, 8 }, { 5, 13 });


  // ---------------------------------------------------------------------------
  // Other houses.
  // ---------------------------------------------------------------------------
  BuildAndDrawRoad({ 20, 8 }, { 20, 13 });
  BuildAndDrawRoad({ 35, 8 }, { 35, 13 });
  BuildAndDrawRoad({ 50, 8 }, { 50, 13 });
  BuildAndDrawRoad({ 65, 8 }, { 62, 13 });


  // ---------------------------------------------------------------------------
  // Blacksmith.
  // ---------------------------------------------------------------------------
  BuildAndDrawRoad({ 77, 7 }, { 65, 8 });


  // ---------------------------------------------------------------------------
  // Villa.
  // ---------------------------------------------------------------------------
  BuildAndDrawRoad({ 24, 37 }, { 24, 29 });
  BuildAndDrawRoad({ 24, 39 }, { 24, 29 });


  // ---------------------------------------------------------------------------
  // Hotel.
  // ---------------------------------------------------------------------------
  BuildAndDrawRoad({ 14, 36 }, { 14, 29 });


  // ---------------------------------------------------------------------------
  // Cook.
  // ---------------------------------------------------------------------------
  BuildAndDrawRoad({ 9, 19 }, { 9, 14 });


  // ---------------------------------------------------------------------------
  // Church Way.
  // ---------------------------------------------------------------------------
  BuildAndDrawRoad({ 14, 29 }, { 62, 29 });
  BuildAndDrawRoad({ 62, 25 }, { 62, 29 });


  // ---------------------------------------------------------------------------
  // Town Hall.
  // ---------------------------------------------------------------------------
  BuildAndDrawRoad({ 49, 32 }, { 49, 29 });


  // ---------------------------------------------------------------------------
  // Church.
  // ---------------------------------------------------------------------------
  BuildAndDrawRoad({ 62, 23 }, { 62, 14 });
  BuildAndDrawRoad({ 62, 25 }, { 62, 14 });
}

// =============================================================================

void MapLevelTown::BuildAndDrawRoad(const Position& start,
                                    const Position& end)
{
  Pathfinder pf;
  auto path = pf.BuildRoad(this, start, end, { '~' }, true, false, 0);

  PlaceGroundTile(start.X,
                  start.Y,
                  '.',
                  Colors::ShadesOfGrey::Ten,
                  Colors::ShadesOfGrey::Eight,
                  Strings::TileNames::FlagstoneText);

  DrawRoad(path);
}

// =============================================================================

void MapLevelTown::DrawRoad(const std::stack<Position>& path)
{
  auto pathCopy = path;

  while (!pathCopy.empty())
  {
    Position c = pathCopy.top();

    PlaceGroundTile(c.X,
                    c.Y,
                    '.',
                    Colors::ShadesOfGrey::Ten,
                    Colors::ShadesOfGrey::Eight,
                    Strings::TileNames::FlagstoneText);

    pathCopy.pop();
  }
}

// =============================================================================

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

// =============================================================================

void MapLevelTown::CreateBlacksmith(int x, int y, const std::vector<std::string>& layout, bool randomizeOrientation)
{
  GameObjectInfo t;

  int posX = x;
  int posY = y;

  std::vector<std::string> newLayout = layout;

  if (randomizeOrientation)
  {
    int index = RNG::Instance().Random() % _rotations.size();
    newLayout = Util::RotateRoomLayout(layout, _rotations[index]);
  }

  for (auto& row : newLayout)
  {
    for (auto& c : row)
    {
      switch (c)
      {
        case '#':
          PlaceWall(posX,
                    posY,
                    c,
                    Colors::ShadesOfGrey::Eight,
                    Colors::BlackColor,
                    Strings::TileNames::StoneWallText);
          break;

        case 'T':
          t.Set(true,
                false,
                c,
                Colors::IronColor,
                Colors::BlackColor,
                Strings::TileNames::WorkbenchText);
          PlaceStaticObject(posX, posY, t);
          break;

        case 'B':
          t.Set(true,
                false,
                c,
                Colors::WhiteColor,
                Colors::BlackColor,
                Strings::TileNames::BedText);
          PlaceStaticObject(posX, posY, t);
          break;

        case '.':
          PlaceGroundTile(posX,
                          posY,
                          ' ',
                          Colors::BlackColor,
                          Colors::RoomFloorColor,
                          Strings::TileNames::WoodenFloorText);
          break;

        case '\\':
        case '/':
        case '_':
        case '|':
        case '-':
        case ' ':
        {
          t.Set(true,
                true,
                c,
                Colors::WhiteColor,
                Colors::BlackColor,
                Strings::TileNames::ForgeText);
          PlaceStaticObject(posX, posY, t);
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

// =============================================================================

void MapLevelTown::CreateRoom(int x, int y, const std::vector<std::string>& layout, bool randomizeOrientation)
{
  int posX = x;
  int posY = y;

  std::vector<std::string> newLayout = layout;

  if (randomizeOrientation)
  {
    int index = RNG::Instance().Random() % _rotations.size();
    newLayout = Util::RotateRoomLayout(layout, _rotations[index]);
  }

  for (auto& row : newLayout)
  {
    for (auto& c : row)
    {
      GameObjectInfo t;

      switch (c)
      {
        case '#':
          PlaceWall(posX,
                    posY,
                    c,
                    Colors::ShadesOfGrey::Eight,
                    Colors::BlackColor,
                    Strings::TileNames::StoneWallText);
          break;

        case 'g':
          PlaceGrassTile(posX, posY);
          break;

        case 'F':
          t.Set(true,
                false,
                'T',
                Colors::WhiteColor,
                Colors::DeepWaterColor,
                Strings::TileNames::FountainText);
          PlaceStaticObject(posX, posY, t);
          break;

        case 'T':
          PlaceTree(posX, posY);
          break;

        case 'B':
          t.Set(true,
                false,
                c,
                Colors::WhiteColor,
                Colors::BlackColor,
                Strings::TileNames::BedText);
          PlaceStaticObject(posX, posY, t);
          break;

        case '.':
          PlaceGroundTile(posX,
                          posY,
                          '-',
                          0x490E11,
                          Colors::RoomFloorColor,
                          Strings::TileNames::WoodenFloorText);
          break;

        //
        // NOTE: since ' ' (i.e. 'Space', 32 ASCII) is a transparent tile in the tileset,
        // you must use bg color to color it, because colored tile for background
        // is made from tile 219, which is a white block. So basically for ' ' tile
        // foreground color is ignored.
        //
        // To allow fog of war to cover floor made of
        // background colored ' ', set FgColor to black.
        //
        case ' ':
          PlaceGroundTile(posX,
                          posY,
                          c,
                          Colors::BlackColor,
                          Colors::ShadesOfGrey::Four,
                          Strings::TileNames::StoneTilesText);
          break;

        case 'm':
          t.Set(true,
                false,
                '#',
                Colors::MarbleColor,
                Colors::GrassColor,
                Strings::TileNames::MarbleFenceText);
          PlaceStaticObject(posX, posY, t);
          break;

        case '~':
          PlaceShallowWaterTile(posX, posY);
          break;

        case '|':
        case '-':
          t.Set(true,
                false,
                c,
                Colors::WhiteColor,
                Colors::BlackColor,
                Strings::TileNames::WindowText);
          PlaceStaticObject(posX, posY, t);
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

// =============================================================================

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
          PlaceWall(posX,
                    posY,
                    c,
                    Colors::ShadesOfGrey::Eight,
                    Colors::BlackColor,
                    Strings::TileNames::StoneWallText);
          break;

        case 'P':
          PlaceWall(posX,
                    posY,
                    '#',
                    Colors::ShadesOfGrey::Eight,
                    Colors::BlackColor,
                    Strings::TileNames::StonePillarText);
          break;

        case '|':
        case '-':
          t.Set(true,
                true,
                c,
                Colors::WhiteColor,
                Colors::BlackColor,
                Strings::TileNames::StainedGlassText);
          PlaceStaticObject(posX, posY, t);
          break;

        // Check out important comments in CreateRoom()
        case ' ':
          PlaceGroundTile(posX,
                          posY,
                          c,
                          Colors::BlackColor,
                          Colors::ShadesOfGrey::Four,
                          Strings::TileNames::StoneTilesText);
          break;

        case '+':
          PlaceDoor(posX, posY);
          break;

        case 'A':
        {
          auto pc = _playerRef->GetClass();

          //
          // Only members of the clergy are allowed into the altar room.
          //
          size_t openedBy = (pc != PlayerClass::ARCANIST)
                          ? GlobalConstants::OpenedByNobody
                          : GlobalConstants::OpenedByAnyone;

          PlaceDoor(posX,
                    posY,
                    false,
                    openedBy,
                    Strings::TileNames::RoyalGatesText);
        }
        break;

        case 'h':
          t.Set(false,
                false,
                c,
                Colors::WoodColor,
                Colors::BlackColor,
                Strings::TileNames::WoodenBenchText,
                "?" + Strings::TileNames::WoodenBenchText + "?");
          PlaceStaticObject(posX, posY, t);
          break;

        case '/':
        {
          //
          // Globally updated game objects are not shown under
          // fog of war by default,
          // so sometimes we must "adjust" tiles if we want
          // certain objects to be shown, like in this case.
          //
          ShrineType shrineType = ShrineType::KNOWLEDGE;
          std::string description = GlobalConstants::ShrineNameByType.at(shrineType);
          t.Set(true,
                false,
                '/',
                Colors::ShadesOfGrey::Four,
                Colors::BlackColor, description,
                "?Shrine?");
          PlaceStaticObject(posX, posY, t);

          //
          // Tiles are updated only around player.
          // Shrine has some logic (buff and timeout count), thus
          // we must make it a global game object so it could be updated
          // every turn no matter where the player is.
          //
          auto go = GameObjectsFactory::Instance().CreateShrine(posX, posY, shrineType, 100);
          PlaceGameObject(go);
        }
        break;
      }

      posX++;
    }

    posX = x;
    posY++;
  }
}

// =============================================================================

void MapLevelTown::CreatePlayerHouse()
{
  auto rot = Util::RotateRoomLayout(_layoutsForLevel[0], RoomLayoutRotation::CCW_180);
  CreateRoom(3, 3, rot);

  GameObjectInfo t;
  t.Set(true,
        false,
        'C',
        Colors::WhiteColor,
        Colors::ChestColor,
        Strings::TileNames::StashText);

  Position cp(6, 6);
  PlaceStaticObject(6, 6, t);

  auto stash = GameObjectsFactory::Instance().CreateContainer(cp.X,
                                                              cp.Y,
                                                              'C',
                                                              Strings::TileNames::StashText,
                                                              Colors::ChestColor);
  PlaceGameObject(stash);
}

// =============================================================================

void MapLevelTown::CreateNPCs()
{
  Rect playerHome   = {  2,  2,  8,  8 };
  Rect mineEntrance = { 87, 40, 95, 48 };
  Rect altarRoom    = { 87, 21, 95, 27 };

  std::vector<Position> visited;

  for (auto& npc : _npcs)
  {
    std::vector<Position> emptyCells;

    for (int x = 1; x <= MapSize.X - 1; x++)
    {
      for (int y = 1; y <= MapSize.Y - 1; y++)
      {
        //
        // Skip area around player house, altar room and mine entrance.
        //
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

        //
        // Also avoid shallow water tiles
        // or NPC may spawn inside walled fountain.
        //
        if (!alreadyAdded && !isBlocking
         && !isSpecial && MapArray[x][y]->Image != '~')
        {
          emptyCells.push_back(Position(x, y));
        }
      }
    }

    int index = RNG::Instance().RandomRange(0, emptyCells.size());

    auto go = MonstersInc::Instance().CreateNPC(emptyCells[index].X, emptyCells[index].Y, npc);
    PlaceActor(go);

    visited.push_back(Position(emptyCells[index].X, emptyCells[index].Y));
  }

  GameObject* go = nullptr;

  go = MonstersInc::Instance().CreateNPC(73, 24, NPCType::TIGRA);
  PlaceActor(go);

  // Traders

  go = MonstersInc::Instance().CreateNPC(83, 24, NPCType::MARTIN, true, ServiceType::BLESS);
  PlaceActor(go);

  go = MonstersInc::Instance().CreateNPC(9, 22, NPCType::CASEY, true);
  PlaceActor(go);

  go = MonstersInc::Instance().CreateNPC(9, 43, NPCType::MAYA, true, ServiceType::IDENTIFY);
  PlaceActor(go);

  go = MonstersInc::Instance().CreateNPC(81, 7, NPCType::GRISWOLD, true, ServiceType::REPAIR);
  PlaceActor(go);
}

// =============================================================================

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
          PlaceWall(posX,
                    posY,
                    ' ',
                    Colors::BlackColor,
                    Colors::BrickColor,
                    Strings::TileNames::BrickWallText);
          break;

        case '.':
          PlaceGroundTile(posX,
                          posY,
                          ' ',
                          Colors::BlackColor,
                          Colors::RoomFloorColor,
                          Strings::TileNames::WoodenFloorText);
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

// =============================================================================

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
          t.Set(true,
                false,
                '#',
                Colors::MarbleColor,
                Colors::GrassColor,
                Strings::TileNames::MarbleFenceText);
          PlaceStaticObject(posX, posY, t);
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
          t.Set(true,
                false,
                'T',
                Colors::WhiteColor,
                Colors::DeepWaterColor,
                Strings::TileNames::FountainText);
          PlaceStaticObject(posX, posY, t);
          break;
      }

      posX++;
    }

    posX = x;
    posY++;
  }
}

// =============================================================================

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
          PlaceWall(posX,
                    posY,
                    '#',
                    Colors::MarbleColor,
                    Colors::GrassColor,
                    Strings::TileNames::MarbleColumnText);
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
          t.Set(false,
                false,
                ' ',
                Colors::BlackColor,
                Colors::ShadesOfGrey::Ten,
                Strings::TileNames::StoneTilesText);
          MapArray[posX][posY]->MakeTile(t);
          MapArray[posX][posY]->Special = true;
        }
        break;

        case 't':
        {
          t.Set(false,
                false,
                ' ',
                Colors::BlackColor,
                Colors::ShadesOfGrey::Ten,
                Strings::TileNames::StoneTilesText);
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

// =============================================================================

void MapLevelTown::CreateTownGates()
{
  GameObject* gate1 = ItemsFactory::Instance().CreateDummyItem(Strings::TileNames::GatesText, '+', Colors::WhiteColor, Colors::BlackColor, std::vector<std::string>());
  GameObject* gate2 = ItemsFactory::Instance().CreateDummyItem(Strings::TileNames::GatesText, '+', Colors::WhiteColor, Colors::BlackColor, std::vector<std::string>());

  //
  // Have to explicitly specify trailing return type
  // to use braced initialized list.
  // I don't understand why, so fuck it.
  //
  // Quote from StackOverflow:
  //
  // "Lambda return type deduction uses the auto rules,
  // which normally would have deduced std::initializer_list just fine.
  // However, the language designers banned deduction from
  // a braced initializer list in a return statement ([dcl.spec.auto]/7)"
  //
  std::function<IR()> fn = []() -> IR
  {
    return { InteractionResult::SUCCESS, GameStates::EXITING_STATE };
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

  PlaceStaticObject(gate1);
  PlaceStaticObject(gate2);
}

// =============================================================================

bool MapLevelTown::SkipArea(const Position& pos, const Rect& area)
{
  return (pos.X >= area.X1
       && pos.X <= area.X2
       && pos.Y >= area.Y1
       && pos.Y <= area.Y2);
}

// =============================================================================

void MapLevelTown::CreateCommonObjects(int x, int y, char image)
{
}

#ifdef MSVC_COMPILER
#include "game-object.h"
#endif

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

MapLevelTown::MapLevelTown(int sizeX,
                           int sizeY,
                           MapType type,
                           int dungeonLevel) :
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
        Colors::Black,
        Strings::TileNames::Ground,
        Strings::Empty);

  FillArea(0, 0, MapSize.X - 1, MapSize.Y - 1, t);

  ReplaceGroundWithGrass();

  LevelStart.X = 1;
  LevelStart.Y = 13;

  CreatePlayerHouse();

  // Bydlo (that includes you, btw) ;-)

  int numHouses = 4;

  auto rotatedRoom = Util::RotateRoomLayout(_layoutsForLevel[0],
                                            RoomLayoutRotation::CCW_180);

  int offset = 15;
  for (int i = 0; i < numHouses; i++)
  {
    CreateRoom(18 + offset * i, 3, rotatedRoom);
  }

  CreateBlacksmith(78, 3, _layoutsForLevel[6]);

  // Majors

  CreateRoom(5, 20, _layoutsForLevel[3]);

  auto room = Util::RotateRoomLayout(_layoutsForLevel[3],
                                     RoomLayoutRotation::CCW_270);
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
                Strings::TileNames::Walls,
                GraphicTiles::STONE_BRICK);

  CreateTownGates();

  RecordEmptyCells();

  LevelExit.X = 91;
  LevelExit.Y = 44;

  Game::gGOF.CreateStairs(this,
                          LevelExit.X,
                          LevelExit.Y,
                          '>',
                          MapType::MINES_1);
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
                  Strings::TileNames::Flagstone,
                  GraphicTiles::COBBLESTONE);

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
                    Strings::TileNames::Flagstone,
                    GraphicTiles::COBBLESTONE);

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

void MapLevelTown::CreateBlacksmith(int x,
                                    int y,
                                    const std::vector<std::string>& layout,
                                    bool randomizeOrientation)
{
  int posX = x;
  int posY = y;

  std::vector<std::string> newLayout = layout;

  if (randomizeOrientation)
  {
    int index = Game::gRng.Random() % _rotations.size();
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
                    Colors::Black,
                    Strings::TileNames::StoneWall,
                    false,
                    GraphicTiles::STONE_BRICK_NOISY);
          break;

        case 'T':
          t.Set(true,
                false,
                c,
                Colors::Iron,
                Colors::Black,
                Strings::TileNames::Workbench,
                Strings::Empty);
          PlaceStaticObject(posX, posY, t);
          break;

        case 'B':
        {
          PlaceGroundTile(posX,
                          posY,
                          ' ',
                          Colors::Black,
                          Colors::RoomFloor,
                          Strings::TileNames::Dirt,
                          GraphicTiles::DIRT);

          t.Set(true,
                false,
                c,
                Colors::White,
                Colors::Black,
                Strings::TileNames::Bed,
                Strings::Empty);
          t.SetGraphics(GraphicTiles::BED_MC);
          PlaceStaticObject(posX, posY, t);
        }
        break;

        case '.':
          PlaceGroundTile(posX,
                          posY,
                          ' ',
                          Colors::Black,
                          Colors::RoomFloor,
                          Strings::TileNames::Dirt,
                          GraphicTiles::DIRT);
          break;

        case '\\':
        case '/':
        case '_':
        case '|':
        case '-':
        case ' ':
        {
          PlaceGroundTile(posX,
                          posY,
                          ' ',
                          Colors::Black,
                          Colors::RoomFloor,
                          Strings::TileNames::Dirt,
                          GraphicTiles::DIRT);

          t.Set(true,
                true,
                c,
                Colors::White,
                Colors::Black,
                Strings::TileNames::Forge,
                Strings::Empty);
          PlaceStaticObject(posX, posY, t);
        }
        break;

        case '+':
        {
          PlaceGroundTile(posX,
                          posY,
                          ' ',
                          Colors::Black,
                          Colors::RoomFloor,
                          Strings::TileNames::Dirt,
                          GraphicTiles::DIRT);

          PlaceDoor(posX,
                    posY,
                    false,
                    GlobalConstants::OpenedByAnyone,
                    std::string(),
                    DoorMaterials::WOOD,
                    DoorGraphicType::WOOD);
        }
        break;
      }

      posX++;
    }

    posX = x;
    posY++;
  }

#ifdef USE_SDL
  //
  // Fix blacksmith's bedroom flooring.
  //
  for (int x = 84; x <= 85; x++)
  {
    for (int y = 4; y <= 8; y++)
    {
      PlaceGroundTile(x,
                      y,
                      ' ',
                      Colors::Black,
                      Colors::RoomFloor,
                      Strings::TileNames::Dirt,
                      GraphicTiles::WOODEN_PLANKS_DESAT);
    }
  }

  PlaceGroundTile(83,
                  8,
                  ' ',
                  Colors::Black,
                  Colors::RoomFloor,
                  Strings::TileNames::Dirt,
                  GraphicTiles::WOODEN_PLANKS_DESAT);
#endif
}

// =============================================================================

void MapLevelTown::CreateRoom(int x,
                              int y,
                              const std::vector<std::string>& layout,
                              bool randomizeOrientation)
{
  int posX = x;
  int posY = y;

  std::vector<std::string> newLayout = layout;

  if (randomizeOrientation)
  {
    int index = Game::gRng.Random() % _rotations.size();
    newLayout = Util::RotateRoomLayout(layout, _rotations[index]);
  }

  const std::unordered_map<GraphicTiles, int> houseWallsMap =
  {
    { GraphicTiles::FACHWERK2, 100 },
    { GraphicTiles::FACHWERK1,  20 },
    { GraphicTiles::FACHWERK3,  20 },
    { GraphicTiles::FACHWERK4,  20 },
  };

  for (auto& row : newLayout)
  {
    for (auto& c : row)
    {
      GameObjectInfo t;

      switch (c)
      {
        case '#':
        {
          auto r = Util::WeightedRandom(houseWallsMap);
          PlaceWall(posX,
                    posY,
                    c,
                    Colors::Wood,
                    Colors::Black,
                    Strings::TileNames::Fachwerk,
                    false,
                    r.first);
        }
        break;

        case 'g':
          PlaceGrassTile(posX, posY);
          break;

        case 'F':
          t.Set(true,
                false,
                'T',
                Colors::White,
                Colors::DeepWater,
                Strings::TileNames::Fountain,
                Strings::Empty);
          PlaceStaticObject(posX, posY, t);
          break;

        case 'T':
          PlaceTree(posX, posY);
          break;

        case 'B':
        {
          PlaceGroundTile(posX,
                          posY,
                          '-',
                          Colors::WoodPlankSeam,
                          Colors::RoomFloor,
                          Strings::TileNames::WoodenFloor,
                          GraphicTiles::WOODEN_PLANKS_DESAT);

          t.Set(true,
                false,
                c,
                Colors::White,
                Colors::Black,
                Strings::TileNames::Bed,
                Strings::Empty);
          t.SetGraphics(GraphicTiles::BED_MC);
          PlaceStaticObject(posX, posY, t);
        }
        break;

        case '.':
          PlaceGroundTile(posX,
                          posY,
                          '-',
                          Colors::WoodPlankSeam,
                          Colors::RoomFloor,
                          Strings::TileNames::WoodenFloor,
                          GraphicTiles::WOODEN_PLANKS_DESAT);
          break;

        //
        // NOTE: since ' ' (i.e. 'Space', 32 ASCII) is a transparent tile in the
        // tileset, you must use bg color to color it, because colored tile for
        // background is made from tile 219, which is a white block.
        // So basically for ' ' tile foreground color is ignored.
        //
        // To allow fog of war to cover floor made of background colored ' ',
        // set FgColor to black.
        //
        case ' ':
          PlaceGroundTile(posX,
                          posY,
                          c,
                          Colors::Black,
                          Colors::ShadesOfGrey::Four,
                          Strings::TileNames::StoneTiles,
                          GraphicTiles::STONE_TILES);
          break;

        case 'm':
          t.Set(true,
                false,
                '#',
                Colors::Marble,
                Colors::Grass,
                Strings::TileNames::WoodenFence,
                Strings::Empty);
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
                Colors::White,
                Colors::Black,
                Strings::TileNames::Window,
                Strings::Empty);
          t.SetGraphics(GraphicTiles::WINDOW);
          PlaceStaticObject(posX, posY, t);
          break;

        case '+':
        {
          // FIXME: doesn't look good if next tile from the door is not wooden
          // planks.
          PlaceGroundTile(posX,
                          posY,
                          '-',
                          Colors::WoodPlankSeam,
                          Colors::RoomFloor,
                          Strings::TileNames::WoodenFloor,
                          GraphicTiles::WOODEN_PLANKS_DESAT);

          PlaceDoor(posX,
                    posY,
                    false,
                    GlobalConstants::OpenedByAnyone,
                    std::string(),
                    DoorMaterials::WOOD,
                    DoorGraphicType::WOOD);
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

void MapLevelTown::CreateChurch(int x, int y)
{
  int posX = x;
  int posY = y;

  for (auto& row : _layoutsForLevel[7])
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
                    Colors::Black,
                    Strings::TileNames::StoneWall,
                    false,
                    GraphicTiles::STONE_BRICK_WHITE);
          break;

        case 'P':
        {
          PlaceGroundTile(posX,
                          posY,
                          c,
                          Colors::Black,
                          Colors::ShadesOfGrey::Four,
                          Strings::TileNames::StoneTiles,
                          GraphicTiles::TILES_DIAMOND_WHITE_BROWN);

          PlaceWall(posX,
                    posY,
                    '#',
                    Colors::ShadesOfGrey::Eight,
                    Colors::Black,
                    Strings::TileNames::StonePillar,
                    false,
                    GraphicTiles::STONE_COLUMN);
        }
        break;

        case '|':
        case '-':
        {
          t.Set(true,
                true,
                c,
                Colors::White,
                Colors::Black,
                Strings::TileNames::StainedGlass,
                Strings::Empty);
          t.SetGraphics(GraphicTiles::STONE_BRICK_WINDOW);
          PlaceStaticObject(posX, posY, t);
        }
        break;

        // Check out important comments in CreateRoom()
        case ' ':
          PlaceGroundTile(posX,
                          posY,
                          c,
                          Colors::Black,
                          Colors::ShadesOfGrey::Four,
                          Strings::TileNames::StoneTiles,
                          GraphicTiles::TILES_DIAMOND_WHITE_BROWN);
          break;

        case '+':
        {
          PlaceGroundTile(posX,
                          posY,
                          '-',
                          Colors::WoodPlankSeam,
                          Colors::RoomFloor,
                          Strings::TileNames::WoodenFloor,
                          GraphicTiles::TILES_DIAMOND_WHITE_BROWN);

          PlaceDoor(posX,
                    posY,
                    false,
                    GlobalConstants::OpenedByAnyone,
                    std::string(),
                    DoorMaterials::WOOD,
                    DoorGraphicType::WOOD);
        }
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
                    Strings::TileNames::RoyalGates);
        }
        break;

        case 'h':
        {
          t.Set(false,
                false,
                c,
                Colors::Wood,
                Colors::Black,
                Strings::TileNames::WoodenBench,
                "?" + Strings::TileNames::WoodenBench + "?");
          PlaceStaticObject(posX, posY, t);
        }
        break;

        case '/':
          PlaceShrine({ posX, posY }, ShrineType::KNOWLEDGE);
          break;
      }

      posX++;
    }

    posX = x;
    posY++;
  }

  //
  // Some post-processing for graphics.
  //
#ifdef USE_SDL

#endif
}

// =============================================================================

void MapLevelTown::CreatePlayerHouse()
{
  auto rot = Util::RotateRoomLayout(_layoutsForLevel[0],
                                    RoomLayoutRotation::CCW_180);
  CreateRoom(3, 3, rot);

  //
  // Add blocker dummy object.
  //
  GameObject* go = Game::gGOF.CreateDummyObject(6,
                                                6,
                                                "Blocker",
                                                ' ',
                                                Colors::None,
                                                Colors::None);
  go->Blocking = true;

  PlaceStaticObject(go);

  GameObject* stash = Game::gGOF.CreateContainer(6,
                                                 6,
                                                 'C',
                                                 Strings::TileNames::Stash,
                                                 Colors::Chest,
                                                 GraphicTiles::CHEST_GOLD);

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

    int index = Game::gRng.RandomRange(0, emptyCells.size());

    auto go = Game::gMI.CreateNPC(emptyCells[index].X,
                                  emptyCells[index].Y,
                                  npc);
    PlaceActor(go);

    visited.push_back(Position(emptyCells[index].X, emptyCells[index].Y));
  }

  GameObject* go = nullptr;

  go = Game::gMI.CreateNPC(73, 24, NPCType::TIGRA);
  PlaceActor(go);

  // Traders

  go = Game::gMI.CreateNPC(83, 24, NPCType::MARTIN, true, ServiceType::BLESS);
  PlaceActor(go);

  go = Game::gMI.CreateNPC(9, 22, NPCType::CASEY, true);
  PlaceActor(go);

  go = Game::gMI.CreateNPC(9, 43, NPCType::MAYA, true, ServiceType::IDENTIFY);
  PlaceActor(go);

  go = Game::gMI.CreateNPC(81, 7, NPCType::GRISWOLD, true, ServiceType::REPAIR);
  PlaceActor(go);
}

// =============================================================================

void MapLevelTown::PlaceMineEntrance(int x, int y)
{
  int posX = x;
  int posY = y;

  for (auto& row : _layoutsForLevel[10])
  {
    for (auto& c : row)
    {
      GameObjectInfo t;

      switch (c)
      {
        case '#':
          PlaceWall(posX,
                    posY,
                    ' ',
                    Colors::Black,
                    Colors::Brick,
                    Strings::TileNames::BrickWall,
                    false,
                    GraphicTiles::BRICK_WALL2);
          break;

        case '.':
          PlaceGroundTile(posX,
                          posY,
                          ' ',
                          Colors::Black,
                          Colors::RoomFloor,
                          Strings::TileNames::Dirt,
                          GraphicTiles::DIRT);
          break;

        case '+':
        {
          PlaceGroundTile(posX,
                          posY,
                          ' ',
                          Colors::Black,
                          Colors::RoomFloor,
                          Strings::TileNames::Dirt,
                          GraphicTiles::DIRT);

          PlaceDoor(posX,
                    posY,
                    false,
                    GlobalConstants::OpenedByAnyone,
                    std::string(),
                    DoorMaterials::WOOD,
                    DoorGraphicType::WOOD);
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

void MapLevelTown::PlaceGarden(int x, int y)
{
  int posX = x;
  int posY = y;

  for (auto& row : _layoutsForLevel[8])
  {
    for (auto& c : row)
    {
      GameObjectInfo t;

      switch (c)
      {
        case '#':
        {
          PlaceGrassTile(posX, posY, -1);

          t.Set(true,
                false,
                '#',
                Colors::Wood,
                Colors::Grass,
                Strings::TileNames::WoodenFence,
                Strings::Empty);
          t.SetGraphics(GraphicTiles::FENCE_WOODEN);
          PlaceStaticObject(posX, posY, t);
        }
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
                Colors::White,
                Colors::DeepWater,
                Strings::TileNames::Fountain,
                Strings::Empty);
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

  for (auto& row : _layoutsForLevel[9])
  {
    for (auto& c : row)
    {
      GameObjectInfo t;

      switch (c)
      {
        case '#':
        {
          PlaceGroundTile(posX,
                          posY,
                          ' ',
                          Colors::White,
                          Colors::White,
                          "",
                          GraphicTiles::TILE_BIG_WHITE);
          PlaceWall(posX,
                    posY,
                    '#',
                    Colors::Marble,
                    Colors::Grass,
                    Strings::TileNames::MarbleColumn,
                    false,
                    GraphicTiles::MARBLE_COLUMN);
        }
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
                Colors::Black,
                Colors::ShadesOfGrey::Ten,
                Strings::TileNames::StoneTiles,
                Strings::Empty);
          t.SetGraphics(GraphicTiles::STONE_TILES);
          MapArray[posX][posY]->MakeTile(t);
          MapArray[posX][posY]->Special = true;
        }
        break;

        case 't':
        {
          t.Set(false,
                false,
                ' ',
                Colors::Black,
                Colors::ShadesOfGrey::Ten,
                Strings::TileNames::StoneTiles,
                Strings::Empty);
          t.SetGraphics(GraphicTiles::STONE_TILES);
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
  static ItemsFactory& factory = Game::gIF;

  GameObject* gate1 = factory.CreateDummyItem(Strings::TileNames::Gates,
                                              '+',
                                              Colors::White,
                                              Colors::Black,
                                              std::vector<std::string>());
  gate1->Graphic.Tile = GraphicTiles::GATE_IRON2_CLOSED;

  GameObject* gate2 = factory.CreateDummyItem(Strings::TileNames::Gates,
                                              '+',
                                              Colors::White,
                                              Colors::Black,
                                              std::vector<std::string>());
  gate2->Graphic.Tile = GraphicTiles::GATE_IRON2_CLOSED;

  //
  // Have to explicitly specify trailing return type to use braced initialized
  // list. I don't understand why, so fuck it.
  //
  // Quote from StackOverflow:
  //
  // "Lambda return type deduction uses the auto rules, which normally would
  // have deduced std::initializer_list just fine. However, the language
  // designers banned deduction from a braced initializer list in a return
  // statement ([dcl.spec.auto]/7)"
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

  PlaceGroundTile(gate1->PosX,
                  gate1->PosY,
                  '.',
                  Colors::ShadesOfGrey::Ten,
                  Colors::ShadesOfGrey::Eight,
                  Strings::TileNames::Flagstone,
                  GraphicTiles::COBBLESTONE);

  PlaceGroundTile(gate2->PosX,
                  gate2->PosY,
                  '.',
                  Colors::ShadesOfGrey::Ten,
                  Colors::ShadesOfGrey::Eight,
                  Strings::TileNames::Flagstone,
                  GraphicTiles::COBBLESTONE);

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

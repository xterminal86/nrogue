#include "map-level-town.h"

#include "player.h"
#include "rect.h"
#include "game-objects-factory.h"
#include "door-component.h"

MapLevelTown::MapLevelTown(int sizeX, int sizeY, MapType type) :
  MapLevelBase(sizeX, sizeY, type, 0)
{  
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
      "................#       #........",
      "................#       #........",
      "####-#######-#######+#######-####",
      "#    h h h h h h        #       #",
      "+    h h h h h h        #       |",
      "#                       +   /   |",
      "+    h h h h h h        #       |",
      "#    h h h h h h        #       #",
      "####-#######-#######+#######-####",
      "................#       #........",
      "................#       #........",
      "................|       |........",
      "................#       #........",
      "................#       #........",
      "................####-####........",
    }
  };
}

void MapLevelTown::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

void MapLevelTown::CreateLevel()
{
  Peaceful = true;

  VisibilityRadius = 20;

  GameObjectInfo t;
  t.Set(false, false, '.', GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Ground");

  Rect r(0, 0, MapSize.X - 1, MapSize.Y - 1);

  FillArea(r.X1, r.Y1, r.X2, r.Y2, t);

  t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Mountains");

  auto bounds = r.GetBoundaryElements();
  for (auto& pos : bounds)
  {
    InsertStaticObject(pos.X, pos.Y, t);
  }

  LevelStart.X = 5;
  LevelStart.Y = 2;

  _playerRef->PosX = 5;
  _playerRef->PosY = 2;

  AdjustCamera();

  CreatePlayerHouse();

  // Bydlo (that includes you, btw) ;-)

  int numHouses = 4;

  int offset = 15;
  for (int i = 0; i < numHouses; i++)
  {
    CreateRoom(18 + offset * i, 3, _layoutsForLevel[0], true);
  }

  CreateBlacksmith(78, 3, _layoutsForLevel[6]);

  // Majors

  CreateRoom(5, 20, _layoutsForLevel[3]);

  auto room = Util::RotateRoomLayout(_layoutsForLevel[3], RoomLayoutRotation::CCW_270);
  CreateRoom(5, 32, room);

  CreateRoom(25, 30, _layoutsForLevel[4]);
  CreateRoom(45, 33, _layoutsForLevel[5]);

  // Church

  CreateChurch(63, 15);

  CreateTownGates();

  RecordEmptyCells();

  LevelExit.X = 98;
  LevelExit.Y = 48;

  GameObjectsFactory::Instance().CreateStairs(this, LevelExit.X, LevelExit.Y, '>', MapType::MINES_1);          

  // *** FIXME: debug
  /*
  auto wand = GameObjectsFactory::Instance().CreateWand(1, 1, WandMaterials::GOLDEN, SpellType::FIREBALL, ItemPrefix::UNCURSED);
  InsertGameObject(wand);

  auto bow = GameObjectsFactory::Instance().CreateRangedWeapon(2, 1, RangedWeaponType::LIGHT_BOW, ItemPrefix::UNCURSED);
  InsertGameObject(bow);

  auto arrows = GameObjectsFactory::Instance().CreateArrows(3, 1, ArrowType::ARROWS, ItemPrefix::UNCURSED);
  InsertGameObject(arrows);

  auto door = GameObjectsFactory::Instance().CreateDoor(4, 1, false, "Test Door", 1);
  InsertStaticObject(door);

  auto pickaxe = GameObjectsFactory::Instance().CreateWeapon(5, 1, WeaponType::PICKAXE, ItemPrefix::UNCURSED);
  InsertGameObject(pickaxe);
  */
  // ***
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
          t.Set(true, true, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Stone Wall");
          InsertStaticObject(posX, posY, t);
          break;

        case 'T':
          t.Set(true, false, c, GlobalConstants::IronColor, GlobalConstants::BlackColor, "Workbench");
          InsertStaticObject(posX, posY, t);
          break;

        case 'B':
          t.Set(true, false, c, GlobalConstants::WhiteColor, GlobalConstants::BlackColor, "Bed");
          InsertStaticObject(posX, posY, t);
          break;

        case '.':
          t.Set(false, false, ' ', GlobalConstants::BlackColor, GlobalConstants::RoomFloorColor, "Wooden Floor");
          MapArray[posX][posY]->MakeTile(t);
          break;

        case '\\':
        case '/':
        case '_':
        case '|':
        case '-':
        case ' ':
        {
          t.Set(true, true, c, GlobalConstants::WhiteColor, GlobalConstants::BlackColor, "Forge");
          InsertStaticObject(posX, posY, t);
        }
        break;

        case '+':
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(posX, posY, false, "Door");
          InsertStaticObject(door);
        }
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
          t.Set(true, true, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Stone Wall");          
          InsertStaticObject(posX, posY, t);
          break;

        case 'g':
          t.Set(false, false, ' ', GlobalConstants::BlackColor, GlobalConstants::GrassColor, "Grass");
          InsertStaticObject(posX, posY, t);
          break;

        case 'F':
          t.Set(true, false, c, GlobalConstants::WhiteColor, GlobalConstants::DeepWaterColor, "Fountain");
          InsertStaticObject(posX, posY, t);
          break;

        case 'T':
          t.Set(true, true, c, GlobalConstants::TreeColor, GlobalConstants::BlackColor, "Tree");
          InsertStaticObject(posX, posY, t);
          break;

        case 'B':
          t.Set(true, false, c, GlobalConstants::WhiteColor, GlobalConstants::BlackColor, "Bed");
          InsertStaticObject(posX, posY, t);
          break;

        case '.':
          t.Set(false, false, ' ', GlobalConstants::BlackColor, GlobalConstants::RoomFloorColor, "Wooden Floor");
          MapArray[posX][posY]->MakeTile(t);
          break;

        case ' ':
          t.Set(false, false, c, GlobalConstants::BlackColor, GlobalConstants::GroundColor, "Stone Tiles");
          MapArray[posX][posY]->MakeTile(t);
          break;

        case 'm':
          t.Set(true, false, ' ', GlobalConstants::BlackColor, GlobalConstants::MarbleColor, "Marble Fence");
          InsertStaticObject(posX, posY, t);
          break;

        case '~':
          t.Set(false, false, c, GlobalConstants::WhiteColor, GlobalConstants::ShallowWaterColor, "Shallow Water");
          MapArray[posX][posY]->MakeTile(t);
          break;

        case '|':
        case '-':
          t.Set(true, false, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Window");
          InsertStaticObject(posX, posY, t);
          break;

        case '+':          
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(posX, posY, false, "Door");
          InsertStaticObject(door);
        }
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
          t.Set(true, true, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Stone Wall");
          InsertStaticObject(posX, posY, t);
          break;

        case '|':
        case '-':
          t.Set(true, true, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Stained Glass");
          InsertStaticObject(posX, posY, t);
          break;

        // To allow fog of war to cover floor made of
        // background colored ' ', set FgColor to empty string.
        case ' ':
        {
          t.Set(false, false, c, GlobalConstants::BlackColor, GlobalConstants::GroundColor, "Stone Tiles");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case '+':
        {
          GameObject* door = GameObjectsFactory::Instance().CreateDoor(posX, posY, false, "Door");
          InsertStaticObject(door);
        }
        break;

        case 'h':
          t.Set(false, false, c, GlobalConstants::WoodColor, GlobalConstants::BlackColor, "Wooden Bench", "?Bench?");
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
          t.Set(true, false, '/', GlobalConstants::GroundColor, GlobalConstants::BlackColor, description, "?Shrine?");
          InsertStaticObject(posX, posY, t);

          // Tiles are updated only around player.
          // Shrine has some logic (buff and timeout count), thus
          // we must make it a global game object so it could be updated
          // every turn no matter where the player is.
          auto go = GameObjectsFactory::Instance().CreateShrine(posX, posY, shrineType, 1000);
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
  CreateRoom(3, 3, _layoutsForLevel[0]);

  GameObjectInfo t;
  t.Set(true, false, 'C', GlobalConstants::WhiteColor, GlobalConstants::ChestColor, "Stash");

  Position cp(6, 6);
  InsertStaticObject(6, 6, t);

  auto stash = GameObjectsFactory::Instance().CreateContainer("Stash", GlobalConstants::ChestColor, 'C', cp.X, cp.Y);
  InsertGameObject(stash);
}

void MapLevelTown::CreateNPCs()
{
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
        // skip player house
        if (x >= 4 && x <= 6 && y >= 4 && y <= 6)
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

        if (!alreadyAdded && !isBlocking)
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

  go = GameObjectsFactory::Instance().CreateNPC(83, 24, NPCType::MARTIN, true);
  InsertActor(go);

  go = GameObjectsFactory::Instance().CreateNPC(9, 22, NPCType::CASEY, true);
  InsertActor(go);

  go = GameObjectsFactory::Instance().CreateNPC(9, 43, NPCType::MAYA, true);
  InsertActor(go);

  go = GameObjectsFactory::Instance().CreateNPC(81, 7, NPCType::GRISWOLD, true);
  InsertActor(go);
}

void MapLevelTown::CreateTownGates()
{
  GameObjectInfo t;
  t.Set(true, true, '+', GlobalConstants::WhiteColor, GlobalConstants::BlackColor, "Village Gates");

  InsertStaticObject(0, 13, t);
  InsertStaticObject(0, 14, t);
}

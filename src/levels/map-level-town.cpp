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

  Tile t;
  t.Set(false, false, '.', GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Ground");

  Rect r(0, 0, MapSize.X - 1, MapSize.Y - 1);

  FillArea(r.X1, r.Y1, r.X2, r.Y2, t);

  t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Mountains");

  auto bounds = r.GetBoundaryElements();
  for (auto& pos : bounds)
  {
    MapArray[pos.X][pos.Y]->MakeTile(t);
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

  CreateBlacksmith(78, 3, _layoutsForLevel[6], true);

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

  // FIXME: debug
  auto go = GameObjectsFactory::Instance().CreateReturner(1, 1, ItemPrefix::UNCURSED);
  InsertGameObject(go);

  GameObjectsFactory::Instance().CreateStairs(this, LevelExit.X, LevelExit.Y, '>', MapType::MINES_1);    
}

void MapLevelTown::FillArea(int ax, int ay, int aw, int ah, const Tile& tileToFill)
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
  Tile t;

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
          MapArray[posX][posY]->MakeTile(t);
          break;

        case 'T':
        {
          t.Set(true, false, c, GlobalConstants::IronColor, GlobalConstants::BlackColor, "Workbench");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case 'B':
        {
          t.Set(true, false, c, GlobalConstants::WhiteColor, GlobalConstants::BlackColor, "Bed");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case '.':
        {
          t.Set(false, false, ' ', GlobalConstants::BlackColor, GlobalConstants::RoomFloorColor, "Wooden Floor");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case '\\':
        case '/':
        case '_':
        case '|':
        case '-':
        {
          t.Set(true, true, c, GlobalConstants::WhiteColor, GlobalConstants::BlackColor, "Forge");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case ' ':
        {
          t.Set(true, true, c, GlobalConstants::WhiteColor, GlobalConstants::BlackColor, "Forge");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case '+':
          CreateDoor(posX, posY);
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
  Tile t;

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
          MapArray[posX][posY]->MakeTile(t);
          break;

        case 'g':
        {
          t.Set(false, false, ' ', GlobalConstants::BlackColor, GlobalConstants::GrassColor, "Grass");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case 'F':
        {
          t.Set(true, false, c, GlobalConstants::WhiteColor, GlobalConstants::DeepWaterColor, "Fountain");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case 'T':
        {
          t.Set(true, true, c, GlobalConstants::TreeColor, GlobalConstants::BlackColor, "Tree");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case 'B':
        {
          t.Set(true, false, c, GlobalConstants::WhiteColor, GlobalConstants::BlackColor, "Bed");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case '.':
        {
          t.Set(false, false, ' ', GlobalConstants::BlackColor, GlobalConstants::RoomFloorColor, "Wooden Floor");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case ' ':
        {
          t.Set(false, false, c, GlobalConstants::BlackColor, GlobalConstants::GroundColor, "Stone Tiles");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case 'm':
        {
          t.Set(true, false, ' ', GlobalConstants::BlackColor, GlobalConstants::MarbleColor, "Marble Fence");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case '~':
        {
          t.Set(false, false, c, GlobalConstants::WhiteColor, GlobalConstants::ShallowWaterColor, "Shallow Water");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case '|':
        case '-':
        {
          t.Set(true, false, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Window");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case '+':
          CreateDoor(posX, posY);
          break;
      }

      posX++;
    }

    posX = x;
    posY++;
  }
}

void MapLevelTown::CreateDoor(int x, int y, bool isOpen)
{
  auto c = MapArray[x][y]->AddComponent<DoorComponent>();
  DoorComponent* dc = static_cast<DoorComponent*>(c);
  dc->IsOpen = isOpen;
  dc->UpdateDoorState();

  // https://stackoverflow.com/questions/15264003/using-stdbind-with-member-function-use-object-pointer-or-not-for-this-argumen/15264126#15264126
  //
  // When using std::bind to bind a member function, the first argument is the object's this pointer.

  MapArray[x][y]->InteractionCallback = std::bind(&DoorComponent::Interact, dc);

  MapArray[x][y]->ObjectName = "Door";
}

void MapLevelTown::CreateChurch(int x, int y)
{
  int posX = x;
  int posY = y;

  Tile t;

  for (auto& row : _layoutsForLevel[7])
  {
    for (auto& c : row)
    {
      switch (c)
      {
        case '#':
        {
          t.Set(true, true, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Stone Wall");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case '|':
        case '-':
        {
          t.Set(true, true, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Stained Glass");
          MapArray[posX][posY]->MakeTile(t);
        }
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
          CreateDoor(posX, posY);
          break;

        case 'h':
        {
          t.Set(false, false, c, GlobalConstants::WoodColor, GlobalConstants::BlackColor, "Wooden Bench", "?Bench?");
          MapArray[posX][posY]->MakeTile(t);
        }
        break;

        case '/':
        {
          // Game objects are not shown under fog of war by default,
          // so sometimes we must "adjust" tiles if we want
          // certain objects to be shown, like in this case.
          ShrineType shrineType = ShrineType::KNOWLEDGE;
          std::string description = GlobalConstants::ShrineNameByType.at(shrineType);
          t.Set(true, false, '/', GlobalConstants::GroundColor, GlobalConstants::BlackColor, description, "?Shrine?");
          MapArray[posX][posY]->MakeTile(t);

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

  Tile t;
  t.Set(true, false, 'C', GlobalConstants::WhiteColor, GlobalConstants::ChestColor, "Stash");

  Position cp(6, 6);

  MapArray[cp.X][cp.Y]->MakeTile(t);

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

        if (!alreadyAdded && !MapArray[x][y]->Blocking && !MapArray[x][y]->Occupied)
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
}

void MapLevelTown::CreateTownGates()
{
  Tile t;
  t.Set(true, true, '+', GlobalConstants::WhiteColor, GlobalConstants::BlackColor, "Village Gates");

  MapArray[0][13]->MakeTile(t);
  MapArray[0][14]->MakeTile(t);
}

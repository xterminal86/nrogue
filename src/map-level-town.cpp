#include "map-level-town.h"

#include "player.h"
#include "rect.h"
#include "game-objects-factory.h"
#include "door-component.h"

MapLevelTown::MapLevelTown(int sizeX, int sizeY, MapType type) :
  MapLevelBase(sizeX, sizeY, type, 0)
{  
}

void MapLevelTown::PrepareMap(MapLevelBase* levelOwner)
{
  MapLevelBase::PrepareMap(levelOwner);

  CreateLevel();
}

void MapLevelTown::CreateLevel()
{
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

  // Bydlo (that includes you, btw) ;-)

  CreateRoom(3, 3, GlobalConstants::SpecialRooms[0]);

  int numHouses = 5;

  int offset = 15;
  for (int i = 0; i < numHouses; i++)
  {
    CreateRoom(18 + offset * i, 3, GlobalConstants::SpecialRooms[0], true);
  }

  // Majors

  CreateRoom(5, 20, GlobalConstants::SpecialRooms[3]);

  auto room = Util::RotateRoomLayout(GlobalConstants::SpecialRooms[3], RoomLayoutRotation::CCW_270);
  CreateRoom(5, 32, room);

  CreateRoom(25, 30, GlobalConstants::SpecialRooms[4]);
  CreateRoom(45, 33, GlobalConstants::SpecialRooms[5]);

  // Church

  CreateChurch(63, 15);

  RecordEmptyCells();

  LevelExit.X = 96;
  LevelExit.Y = 47;

  GameObjectsFactory::Instance().CreateStairs(this, LevelExit.X, LevelExit.Y, '>', MapType::MINES_1);

  /*
  // Some rats

  for (int i = 0; i < 30; i++)
  {
    int x = RNG::Instance().Random() % GlobalConstants::MapX;
    int y = RNG::Instance().Random() % GlobalConstants::MapY;

    if (!MapArray[x][y]->Blocking)
    {
      // Special rats
      //auto rat = GameObjectsFactory::Instance().CreateRat(x, y, false);

      // Normal rats
      auto rat = GameObjectsFactory::Instance().CreateRat(x, y);

      InsertActor(rat);
    }
  }
  */
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
          t.Set(true, false, c, GlobalConstants::WhiteColor, GlobalConstants::WaterColor, "Fountain");
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

        case 'w':
        {
          t.Set(true, false, ' ', GlobalConstants::BlackColor, GlobalConstants::WaterColor, "Water");
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

  for (auto& row : GlobalConstants::SpecialRooms[7])
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
          t.Set(false, false, c, GlobalConstants::RoomFloorColor, GlobalConstants::BlackColor, "Wooden Bench", "?Bench?");
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
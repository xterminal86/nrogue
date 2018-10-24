#include "map.h"

#include "application.h"
#include "printer.h"
#include "util.h"
#include "rng.h"
#include "ai-dummy.h"
#include "door-component.h"
#include "game-objects-factory.h"
#include "util.h"

void Map::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;

  for (int x = 0; x < GlobalConstants::MapX; x++)
  {
    for (int y = 0; y < GlobalConstants::MapY; y++)
    {
      MapArray[x][y].Init(x, y, ' ', "#000000", "#000000");
    }
  }
}

void Map::CreateMap(MapType mapType)
{
  switch (mapType)
  {
    case MapType::TOWN:
      CreateTown();
      break;
  }
}

void Map::Draw(int playerX, int playerY)
{
  // Draw map tiles around player

  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;
    
  auto mapCells = Util::GetRectAroundPoint(playerX, playerY, tw / 2, th / 2);
  for (auto& cell : mapCells)
  {
    int x = cell.X;
    int y = cell.Y;

    if (MapArray[x][y].Visible)
    {
      MapArray[x][y].Draw();
    }
    else
    {
      std::string tileColor = MapArray[x][y].Revealed ?
                              GlobalConstants::FogOfWarColor :
                              GlobalConstants::BlackColor;

      if (MapArray[x][y].FgColor == GlobalConstants::BlackColor)
      {
        MapArray[x][y].Draw(GlobalConstants::BlackColor, tileColor);
      }
      else
      {
        MapArray[x][y].Draw(tileColor, GlobalConstants::BlackColor);
      }
    }
  }

  // Draw game objects

  for (auto& go : GameObjects)
  {
    int x = go.get()->PosX;
    int y = go.get()->PosY;

    if (MapArray[x][y].Visible)
    {
      // If game object has black bg color,
      // replace it with current floor color
      bool cond = (go->BgColor == GlobalConstants::BlackColor);
      go.get()->Draw(go.get()->FgColor, cond ? MapArray[x][y].BgColor : go->BgColor);
    }
  }

  // Draw actors

  for (auto& go : ActorGameObjects)
  {
    int x = go.get()->PosX;
    int y = go.get()->PosY;

    if (MapArray[x][y].Visible)
    {
      // If game object has black bg color,
      // replace it with current floor color
      bool cond = (go->BgColor == GlobalConstants::BlackColor);
      go.get()->Draw(go.get()->FgColor, cond ? MapArray[x][y].BgColor : go->BgColor);
    }
  }
}

void Map::UpdateGameObjects()
{
  RemoveDestroyed();

  for (auto& go : GameObjects)
  {
    go.get()->Update();
  }

  for (auto& go : ActorGameObjects)
  {
    go.get()->Update();
  }
}

void Map::InsertActor(GameObject* goToInsert)
{
  ActorGameObjects.push_back(std::unique_ptr<GameObject>(goToInsert));
}

void Map::InsertGameObject(GameObject* goToInsert)
{
  GameObjects.push_back(std::unique_ptr<GameObject>(goToInsert));
}

std::pair<int, GameObject*> Map::GetGameObjectToPickup(int x, int y)
{
  std::pair<int, GameObject*> res;

  res.first = -1;
  res.second = nullptr;

  int index = 0;
  for (auto& go : GameObjects)
  {    
    auto c = go->GetComponent<ItemComponent>();
    if (go.get()->PosX == x && go.get()->PosY == y && c != nullptr)
    {
      res.first = index;
      res.second = go.get();
    }

    index++;
  }

  return res;
}

GameObject* Map::GetActorAtPosition(int x, int y)
{
  for (auto& go : ActorGameObjects)
  {
    if (go.get()->PosX == x && go.get()->PosY == y)
    {
      return go.get();
    }
  }

  return nullptr;
}

std::vector<GameObject*> Map::GetGameObjectsAtPosition(int x, int y)
{
  std::vector<GameObject*> res;

  for (auto& go : GameObjects)
  {
    if (go.get()->PosX == x && go.get()->PosY == y)
    {
      res.push_back(go.get());
    }
  }

  return res;
}

void Map::CreateTown()
{
  _playerRef->VisibilityRadius = 20;

  int mw = GlobalConstants::MapX;
  int mh = GlobalConstants::MapY;

  Tile t;
  t.Set(false, false, '.', GlobalConstants::GroundColor, GlobalConstants::BlackColor, "Ground");

  Rect r(0, 0, mw - 1, mh - 1);

  FillArea(r.X1, r.Y1, r.X2, r.Y2, t);

  t.Set(true, true, ' ', GlobalConstants::BlackColor, GlobalConstants::MountainsColor, "Mountains");

  auto bounds = r.GetBoundaryElements();
  for (auto& pos : bounds)
  {
    MapArray[pos.X][pos.Y].MakeTile(t);
  }

  SetPlayerStartingPosition(5, 2);

  // Bydlo (that includes you, btw) ;-)

  CreateRoom(3, 3, GlobalConstants::RoomLayouts[0]);

  int numHouses = 5;

  int offset = 15;
  for (int i = 0; i < numHouses; i++)
  {
    CreateRoom(18 + offset * i, 3, GlobalConstants::RoomLayouts[0], true);
  }

  // Majors

  CreateRoom(5, 20, GlobalConstants::RoomLayouts[3]);

  auto room = Util::RotateRoomLayout(GlobalConstants::RoomLayouts[3], RoomLayoutRotation::CCW_270);
  CreateRoom(5, 32, room);

  CreateRoom(25, 30, GlobalConstants::RoomLayouts[4]);
  CreateRoom(45, 33, GlobalConstants::RoomLayouts[5]);

  // Church
  CreateChurch(63, 15);

  // Castle
  // CreateCastle(65, 15);

  // Some rats

  for (int i = 0; i < 30; i++)
  {
    int x = RNG::Instance().Random() % GlobalConstants::MapX;
    int y = RNG::Instance().Random() % GlobalConstants::MapY;

    if (!MapArray[x][y].Blocking)
    {
      auto rat = GameObjectsFactory::Instance().CreateRat(x, y);
      InsertActor(rat);
    }
  }
}

void Map::CreateRoom(int x, int y, int w, int h)
{
  Rect room(x, y, w, h);

  Tile t;
  t.Set(false, false, ' ', GlobalConstants::BlackColor, GlobalConstants::RoomFloorColor, "Wooden Floor");

  FillArea(x + 1, y + 1, w - 1, h - 1, t);

  t.Set(true, true, '#', GlobalConstants::WallColor, "#000000", "Stone Wall");

  auto bounds = room.GetBoundaryElements();
  for (auto& pos : bounds)
  {
     MapArray[pos.X][pos.Y].MakeTile(t);
  }

  bounds = room.GetBoundaryElements(true);

  auto wallIndex = RNG::Instance().Random() % bounds.size();
  Position wallPos = bounds[wallIndex];

  CreateDoor(wallPos.X, wallPos.Y);
}

void Map::FillArea(int ax, int ay, int aw, int ah, const Tile& tileToFill)
{
  for (int x = ax; x <= ax + aw; x++)
  {
    for (int y = ay; y <= ay + ah; y++)
    {
      MapArray[x][y].MakeTile(tileToFill);
    }
  }
}

void Map::CreateDoor(int x, int y, bool isOpen)
{  
  auto c = MapArray[x][y].AddComponent<DoorComponent>();  
  DoorComponent* dc = static_cast<DoorComponent*>(c);
  dc->IsOpen = isOpen;
  dc->UpdateDoorState();

  // https://stackoverflow.com/questions/15264003/using-stdbind-with-member-function-use-object-pointer-or-not-for-this-argumen/15264126#15264126
  //
  // When using std::bind to bind a member function, the first argument is the object's this pointer.

  MapArray[x][y].InteractionCallback = std::bind(&DoorComponent::Interact, dc);

  MapArray[x][y].ObjectName = "Door";
}

void Map::DrawGameObjects()
{
  for (auto& item : GameObjects)
  {
    float d = Util::LinearDistance(item.get()->PosX, item.get()->PosY,
                                   _playerRef->PosX, _playerRef->PosY);

    if (d < _playerRef->VisibilityRadius)
    {
      item.get()->Draw();
    }
  }
}

void Map::SetPlayerStartingPosition(int x, int y)
{
  PlayerStartX = x;
  PlayerStartY = y;

  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  MapOffsetX = tw / 2 - PlayerStartX;
  MapOffsetY = th / 2 - PlayerStartY;
}

void Map::CreateRoom(int x, int y, const std::vector<std::string>& layout, bool randomizeOrientation)
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
          MapArray[posX][posY].MakeTile(t);
          break;

        case 'g':
        {
          t.Set(false, false, ' ', GlobalConstants::BlackColor, GlobalConstants::GrassColor, "Grass");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        case 'F':
        {
          t.Set(true, false, c, GlobalConstants::WhiteColor, GlobalConstants::WaterColor, "Fountain");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        case '.':
        {
          t.Set(false, false, ' ', GlobalConstants::BlackColor, GlobalConstants::RoomFloorColor, "Wooden Floor");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        case ' ':
        {
          t.Set(false, false, c, GlobalConstants::BlackColor, GlobalConstants::GroundColor, "Stone Tiles");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        case 'w':
        {
          t.Set(true, false, ' ', GlobalConstants::BlackColor, GlobalConstants::WaterColor, "Water");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        case '|':
        case '-':
        {
          t.Set(true, false, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Window");
          MapArray[posX][posY].MakeTile(t);
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

void Map::CreateChurch(int x, int y)
{
  int posX = x;
  int posY = y;

  Tile t;

  for (auto& row : GlobalConstants::RoomLayouts[7])
  {
    for (auto& c : row)
    {
      switch (c)
      {
        case '#':
        {
          t.Set(true, true, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Stone Wall");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        case '|':
        case '-':
        {
          t.Set(true, true, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Stained Glass");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        // To allow fog of war to cover floor made of
        // background colored ' ', set FgColor to empty string.
        case ' ':
        {
          t.Set(false, false, c, GlobalConstants::BlackColor, GlobalConstants::GroundColor, "Stone Tiles");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        case '+':
          CreateDoor(posX, posY);
          break;

        case 'h':
        {
          t.Set(false, false, c, GlobalConstants::RoomFloorColor, GlobalConstants::BlackColor, "Wooden Bench", "?Bench?");
          MapArray[posX][posY].MakeTile(t);          
        }
        break;

        case '/':
        {
          // Game objects are not shown under fog of war by default,
          // so sometimes we must "adjust" tiles if we want
          // certain objects to be shown, like in this case.
          ShrineType shrineType = ShrineType::SPIRIT;
          std::string description = (shrineType == ShrineType::MIGHT) ? "Shrine of Might" : "Shrine of Spirit";
          t.Set(true, false, '/', GlobalConstants::GroundColor, GlobalConstants::BlackColor, description, "?Shrine?");
          MapArray[posX][posY].MakeTile(t);

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

void Map::CreateCastle(int x, int y)
{
  int posX = x;
  int posY = y;

  Tile t;

  for (auto& row : GlobalConstants::RoomLayouts[6])
  {
    for (auto& c : row)
    {
      switch (c)
      {
        case '#':
        {
          t.Set(true, true, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Stone Wall");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        case '|':
        case '-':
        {
          t.Set(true, false, c, GlobalConstants::WallColor, GlobalConstants::BlackColor, "Window");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        // Castle only, layout should not be rotated.
        case 'S':
        {
          t.Set(true, true, '|', GlobalConstants::WallColor, GlobalConstants::BlackColor, "Stained Glass");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        case ' ':
        {
          t.Set(false, false, c, GlobalConstants::BlackColor, GlobalConstants::GroundColor, "Stone Tiles");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        case '+':
          CreateDoor(posX, posY);
          break;

        case 'h':
        {
          t.Set(false, false, c, GlobalConstants::RoomFloorColor, GlobalConstants::BlackColor, "Wooden Bench");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        case '/':
        {
          // Game objects are not shown under fog of war by default,
          // so sometimes we must "adjust" tiles if we want
          // certain objects to be shown, like in this case.
          ShrineType shrineType = ShrineType::MIGHT;
          std::string description = (shrineType == ShrineType::MIGHT) ? "Shrine of Might" : "Shrine of Spirit";
          t.Set(true, false, '/', GlobalConstants::GroundColor, GlobalConstants::BlackColor, description);
          MapArray[posX][posY].MakeTile(t);

          // Tiles are updated only around player.
          // Shrine has some logic (buff and timeout count), thus
          // we must make it a global game object so it could be updated
          // every turn no matter where the player is.
          auto go = GameObjectsFactory::Instance().CreateShrine(posX, posY, shrineType, 1000);
          InsertGameObject(go);
        }
        break;

        case 'g':
        {
          t.Set(false, false, ' ', GlobalConstants::BlackColor, GlobalConstants::GrassColor, "Grass");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        case 'F':
        {
          t.Set(true, false, c, GlobalConstants::WhiteColor, GlobalConstants::WaterColor, "Fountain");
          MapArray[posX][posY].MakeTile(t);
        }
        break;

        case 'w':
        {
          t.Set(true, false, ' ', GlobalConstants::BlackColor, GlobalConstants::WaterColor, "Water");
          MapArray[posX][posY].MakeTile(t);
        }
        break;
      }

      posX++;
    }

    posX = x;
    posY++;
  }
}

void Map::RemoveDestroyed()
{  
  for (int i = 0; i < GameObjects.size(); i++)
  {
    if (GameObjects[i]->IsDestroyed)
    {
      int x = GameObjects[i]->PosX;
      int y = GameObjects[i]->PosY;

      MapArray[x][y].Occupied = false;

      GameObjects.erase(GameObjects.begin() + i);
    }
  }

  for (int i = 0; i < ActorGameObjects.size(); i++)
  {
    if (ActorGameObjects[i]->IsDestroyed)
    {
      int x = ActorGameObjects[i]->PosX;
      int y = ActorGameObjects[i]->PosY;

      MapArray[x][y].Occupied = false;

      ActorGameObjects.erase(ActorGameObjects.begin() + i);
    }
  }
}

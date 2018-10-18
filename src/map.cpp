#include "map.h"

#include "application.h"
#include "printer.h"
#include "util.h"
#include "rng.h"
#include "ai-dummy.h"
#include "door-component.h"

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

      // Because some tiles can be just spaces with bg color,
      // we have to check them separately
      if (MapArray[x][y].FgColor.length() == 0)
      {
        MapArray[x][y].Draw(GlobalConstants::BlackColor, tileColor);
      }
      else
      {
        MapArray[x][y].Draw(tileColor, "#000000");
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
      go.get()->Draw(go.get()->FgColor, MapArray[x][y].BgColor);
    }
  }
}

void Map::UpdateGameObjects()
{
  for (auto& go : GameObjects)
  {
    go.get()->Update();
  }
}

void Map::InsertGameObject(GameObject* goToInsert)
{
  GameObjects.push_back(std::unique_ptr<GameObject>(goToInsert));
}

std::pair<int, GameObject*> Map::GetTopGameObjectAtPosition(int x, int y)
{
  std::pair<int, GameObject*> res;

  int index = 0;
  for (auto& go : GameObjects)
  {
    if (go.get()->PosX == x && go.get()->PosY == y)
    {
      res.first = index;
      res.second = go.get();
    }

    index++;
  }

  return res;
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
  t.Set(false, false, '.', GlobalConstants::GroundColor, "#000000", "Ground");

  Rect r(0, 0, mw - 1, mh - 1);

  FillArea(r.X1, r.Y1, r.X2, r.Y2, t);

  t.Set(true, true, ' ', "", GlobalConstants::MountainsColor, "Mountains");

  auto bounds = r.GetBoundaryElements();
  for (auto& pos : bounds)
  {
    MapArray[pos.X][pos.Y].MakeTile(t);
  }

  CreateRoom(20, 20, 10, 10);

  PlayerStartX = 10;
  PlayerStartY = 10;

  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  MapOffsetX = tw / 2 - PlayerStartX;
  MapOffsetY = th / 2 - PlayerStartY;

  // create some npcs

  for (int i = 0 ; i < 20; i++)
  {
    int x = 1 + (RNG::Instance().Random() % (GlobalConstants::MapX - 2));
    int y = 1 + (RNG::Instance().Random() % (GlobalConstants::MapY - 2));

    if (MapArray[x][y].Occupied)
    {
      continue;
    }

    GameObject* npc = new GameObject(x, y, '@', GlobalConstants::NpcColor);
    npc->ObjectName = "Dummy AI " + std::to_string(i);
    npc->AddComponent<AIDummy>();

    auto up = std::unique_ptr<GameObject>();
    up.reset(npc);

    GameObjects.push_back(std::move(up));
  }  

  t.Set(false, true, 'T', GlobalConstants::TreeColor, "#000000", "Tree");

  int numTrees = 50;
  for (int i = 0; i < numTrees; i++)
  {
    int x = 1 + (RNG::Instance().Random() % (GlobalConstants::MapX - 2));
    int y = 1 + (RNG::Instance().Random() % (GlobalConstants::MapY - 2));

    if (MapArray[x][y].Blocking)
    {
      continue;
    }

    MapArray[x][y].MakeTile(t);
  }
}

void Map::CreateRoom(int x, int y, int w, int h)
{
  Rect room(x, y, w, h);

  Tile t;
  t.Set(false, false, ' ', "", GlobalConstants::RoomFloorColor, "Wooden Floor");

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
  DoorComponent* dc = dynamic_cast<DoorComponent*>(c);
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

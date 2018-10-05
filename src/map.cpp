#include "map.h"

#include "application.h"
#include "printer.h"
#include "util.h"
#include "rng.h"
#include "ai-dummy.h"

void Map::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void Map::CreateMap(MapType mapType)
{  
  switch (mapType)
  {
    case MapType::TOWN:
      CreateTown();
      break;
  }

  /*
  for (int x = 0; x < GlobalConstants::MapX; x++)
  {
    for (int y = 0; y < GlobalConstants::MapY; y++)
    {
      MapArray[x][y].Init(false);

      MapArray[x][y].SetFloor();      
    }
  }

  for (int x = 0; x < GlobalConstants::MapX; x++)
  {
    MapArray[x][0].SetWall();
    MapArray[x][GlobalConstants::MapY - 1].SetWall();    
  }

  for (int y = 0; y < GlobalConstants::MapY; y++)
  {
    MapArray[0][y].SetWall();
    MapArray[GlobalConstants::MapX - 1][y].SetWall();    
  }

  for (int x = 10; x < 40; x++)
  {
    MapArray[x][10].SetWall();
  }
  */
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
        Printer::Instance().Print(x + MapOffsetX, y + MapOffsetY, 
                                  MapArray[x][y].Image,                                    
                                  MapArray[x][y].Color);      
      }
      else
      {
        auto tileColor = MapArray[x][y].Revealed ? "#222222" : "#000000";

        Printer::Instance().Print(x + MapOffsetX, y + MapOffsetY, 
                                  MapArray[x][y].Image,
                                  tileColor);      
      }
  }

  // Draw game objects

  for (auto& go : _gameObjects)
  {
    int x = go.get()->PosX;
    int y = go.get()->PosY;

    float d = Util::LinearDistance(x, y, _playerRef->PosX, _playerRef->PosY);

    if (d < _playerRef->VisibilityRadius)
    {
      go.get()->Draw();
    }
  }
}

void Map::UpdateGameObjects()
{
  for (auto& go : _gameObjects)
  {
    go.get()->Update();
  }
}

std::vector<GameObject*> Map::GetGameObjectsAtPosition(int x, int y)
{
  std::vector<GameObject*> res;

  for (auto& go : _gameObjects)
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
  int mw = GlobalConstants::MapX;
  int mh = GlobalConstants::MapY;

  Rect r(0, 0, mw - 1, mh - 1);

  ClearArea(r.X1, r.Y1, r.X2, r.Y2);

  auto bounds = r.GetBoundaryElements();
  for (auto& pos : bounds)
  {
    MapArray[pos.X][pos.Y].SetWall();
  }

  PlayerStartX = 10;
  PlayerStartY = 10;

  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  MapOffsetX = tw / 2 - PlayerStartX;
  MapOffsetY = th / 2 - PlayerStartY;

  GameObject* npc = new GameObject(20, 20, '@', "#FFFF00");
  npc->AddComponent<AIDummy>();

  auto up = std::unique_ptr<GameObject>();
  up.reset(npc);

  _gameObjects.push_back(std::move(up));

  _playerRef->VisibilityRadius = 16;
}

void Map::CreateRoom(int x, int y, int w, int h)
{
  Rect room(x, y, w, h);

  ClearArea(x, y, w, h);

  int randomIndex = 0;

  auto bounds = room.GetBoundaryElements();
  for (auto& pos : bounds)
  {
     MapArray[pos.X][pos.Y].SetWall();
  }

  // TODO:
  randomIndex = RNG::Instance().Random() % bounds.size();
}

void Map::ClearArea(int ax, int ay, int aw, int ah)
{
  for (int x = ax; x <= aw; x++)
  {
    for (int y = ay; y <= ah; y++)
    {
      MapArray[x][y].Init(false);
      MapArray[x][y].SetFloor();
    }
  }
}

void Map::DrawGameObjects()
{
  for (auto& item : _gameObjects)
  {
    float d = Util::LinearDistance(item.get()->PosX, item.get()->PosY,
                                   _playerRef->PosX, _playerRef->PosY);

    if (d < _playerRef->VisibilityRadius)
    {
      item.get()->Draw();
    }
  }
}

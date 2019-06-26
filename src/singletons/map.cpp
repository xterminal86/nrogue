#include "map.h"

#include "application.h"
#include "printer.h"
#include "rng.h"
#include "door-component.h"
#include "game-objects-factory.h"
#include "map-level-town.h"
#include "map-level-mines.h"
#include "map-level-caves.h"
#include "map-level-lost-city.h"
#include "map-level-deep-dark.h"
#include "map-level-abyss.h"
#include "map-level-nether.h"
#include "map-level-endgame.h"

void Map::Init()
{
  _levels[MapType::TOWN] = std::unique_ptr<MapLevelBase>(new MapLevelTown(100, 50, MapType::TOWN));

  CurrentLevel = _levels[MapType::TOWN].get();

  _levels[MapType::TOWN]->PrepareMap(_levels[MapType::TOWN].get());

  // Give player reference to current level
  Application::Instance().PlayerInstance.SetLevelOwner(CurrentLevel);

  Application::Instance().PlayerInstance.VisibilityRadius.Set(CurrentLevel->VisibilityRadius);

  _mapVisitFirstTime[MapType::MINES_1] = false;
  _mapVisitFirstTime[MapType::CAVES_1] = false;
  _mapVisitFirstTime[MapType::LOST_CITY] = false;
  _mapVisitFirstTime[MapType::DEEP_DARK_1] = false;
  _mapVisitFirstTime[MapType::ABYSS_1] = false;
  _mapVisitFirstTime[MapType::NETHER_1] = false;
  _mapVisitFirstTime[MapType::THE_END] = false;  

  _playerRef = &Application::Instance().PlayerInstance;
}

void Map::Draw(int playerX, int playerY)
{
  // Draw map tiles around player

  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;
    
  auto mapCells = Util::GetRectAroundPoint(playerX, playerY, tw / 2, th / 2, CurrentLevel->MapSize);
  for (auto& cell : mapCells)
  {
    int x = cell.X;
    int y = cell.Y;

    if (CurrentLevel->MapArray[x][y]->Visible)
    {
      CurrentLevel->MapArray[x][y]->Draw();

      // Draw static object on top if present
      if (CurrentLevel->StaticMapObjects[x][y] != nullptr)
      {
        CurrentLevel->StaticMapObjects[x][y]->Draw();
      }
    }
    else
    {
      DrawNonVisibleMapTile(x, y);
      DrawNonVisibleStaticObject(x, y);
    }
  }

  // Draw game objects

  for (auto& go : CurrentLevel->GameObjects)
  {
    int x = go.get()->PosX;
    int y = go.get()->PosY;

    if (CurrentLevel->MapArray[x][y]->Visible)
    {
      // NOTE: gems should not respect floor color

      // If game object has black bg color,
      // replace it with current floor color
      //bool cond = (go->BgColor == GlobalConstants::BlackColor);
      //go.get()->Draw(go.get()->FgColor, cond ? CurrentLevel->MapArray[x][y]->BgColor : go->BgColor);

      go->Draw(go->FgColor, go->BgColor);
    }
  }

  // Draw actors

  for (auto& go : CurrentLevel->ActorGameObjects)
  {
    int x = go->PosX;
    int y = go->PosY;

    if (_playerRef->HasEffect(EffectType::INFRAVISION)
     || CurrentLevel->MapArray[x][y]->Visible)
    {
      // If game object has black bg color,
      // replace it with current floor color
      bool cond = (go->BgColor == GlobalConstants::BlackColor);
      go->Draw(go->FgColor, cond ? CurrentLevel->MapArray[x][y]->BgColor : go->BgColor);
    }
  }
}

void Map::UpdateGameObjects()
{
  RemoveDestroyed();

  CurrentLevel->TryToSpawnMonsters();

  for (auto& go : CurrentLevel->GameObjects)
  {
    go->Update();
  }

  for (auto& go : CurrentLevel->ActorGameObjects)
  {
    // Update does the action meter increment as well
    // so if object had action meter 0 at start,
    // it needs to increment it in its component logic class
    // (e.g. ai-monster-basic)
    go->Update();

    // If there are extra turns available, perform them
    while (go->Attrs.ActionMeter >= GlobalConstants::TurnReadyValue)
    {
      go->Update();
    }
  }
}

void Map::InsertActor(GameObject* goToInsert)
{  
  CurrentLevel->InsertActor(goToInsert);
}

void Map::InsertGameObject(GameObject* goToInsert)
{  
  CurrentLevel->InsertGameObject(goToInsert);
}

std::pair<int, GameObject*> Map::GetGameObjectToPickup(int x, int y)
{
  std::pair<int, GameObject*> res;

  res.first = -1;
  res.second = nullptr;

  int index = 0;
  for (auto& go : CurrentLevel->GameObjects)
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
  for (auto& go : CurrentLevel->ActorGameObjects)
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

  for (auto& go : CurrentLevel->GameObjects)
  {
    if (go.get()->PosX == x && go.get()->PosY == y)
    {
      res.push_back(go.get());
    }
  }

  if (res.empty())
  {
    if (CurrentLevel->StaticMapObjects[x][y] != nullptr)
    {
      res.push_back(CurrentLevel->StaticMapObjects[x][y].get());
    }
  }

  return res;
}

std::vector<GameObject*> Map::GetActorsInRange(int range)
{
  std::vector<GameObject*> res;

  auto& playerRef = Application::Instance().PlayerInstance;

  int lx = playerRef.PosX - range;
  int ly = playerRef.PosY - range;
  int hx = playerRef.PosX + range;
  int hy = playerRef.PosY + range;

  for (auto& a : CurrentLevel->ActorGameObjects)
  {
    if (a->PosX >= lx &&
        a->PosY >= ly &&
        a->PosX <= hx &&
        a->PosY <= hy)
    {
      res.push_back(a.get());
    }
  }

  return res;
}

MapLevelBase* Map::GetLevelRefByType(MapType type)
{
  if (_levels.count(type) != 0)
  {
    return _levels[type].get();
  }

  return nullptr;
}

void Map::RemoveDestroyed()
{
  // Check if static objects need to be destroyed

  int playerX = Application::Instance().PlayerInstance.PosX;
  int playerY = Application::Instance().PlayerInstance.PosY;

  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  auto mapCells = Util::GetRectAroundPoint(playerX, playerY, tw / 2, th / 2, CurrentLevel->MapSize);
  for (auto& cell : mapCells)
  {
    if (CurrentLevel->StaticMapObjects[cell.X][cell.Y] != nullptr
     && CurrentLevel->StaticMapObjects[cell.X][cell.Y]->IsDestroyed)
    {
      CurrentLevel->StaticMapObjects[cell.X][cell.Y].reset(nullptr);
    }
  }

  // Check if globally updated objects need to be destroyed

  for (int i = 0; i < CurrentLevel->GameObjects.size(); i++)
  {
    if (CurrentLevel->GameObjects[i] != nullptr &&
        CurrentLevel->GameObjects[i]->IsDestroyed)
    {
      int x = CurrentLevel->GameObjects[i]->PosX;
      int y = CurrentLevel->GameObjects[i]->PosY;

      CurrentLevel->MapArray[x][y]->Occupied = false;

      CurrentLevel->GameObjects.erase(CurrentLevel->GameObjects.begin() + i);
    }
  }

  // Check if actors need to be destroyed

  for (int i = 0; i < CurrentLevel->ActorGameObjects.size(); i++)
  {
    if (CurrentLevel->ActorGameObjects[i] != nullptr &&
        CurrentLevel->ActorGameObjects[i]->IsDestroyed)
    {
      int x = CurrentLevel->ActorGameObjects[i]->PosX;
      int y = CurrentLevel->ActorGameObjects[i]->PosY;

      CurrentLevel->MapArray[x][y]->Occupied = false;

      CurrentLevel->ActorGameObjects.erase(CurrentLevel->ActorGameObjects.begin() + i);
    }
  }
}

void Map::ChangeLevel(MapType levelToChange, bool goingDown)
{  
  auto& player = Application::Instance().PlayerInstance;

  // Unblock cell on stairs before going
  CurrentLevel->MapArray[player.PosX][player.PosY]->Occupied = false;

  ChangeOrInstantiateLevel(levelToChange);

  auto pos = goingDown ? CurrentLevel->LevelStart : CurrentLevel->LevelExit;

  player.SetLevelOwner(CurrentLevel);
  player.MoveTo(pos);
  player.VisibilityRadius.Set(CurrentLevel->VisibilityRadius);

  CurrentLevel->AdjustCamera();
}

void Map::TeleportToExistingLevel(MapType levelToChange, const Position& teleportTo)
{
  auto& player = Application::Instance().PlayerInstance;

  // Unblock cell before teleporting.
  //
  // MoveTo() does the unblocking as well, but if we switch places with actor,
  // we must manually unblock player's cell first
  // or MoveTo() for actor won't work.
  CurrentLevel->MapArray[player.PosX][player.PosY]->Occupied = false;

  CurrentLevel = _levels[levelToChange].get();

  auto& mapRef = CurrentLevel->MapArray[teleportTo.X][teleportTo.Y];
  auto& soRef = CurrentLevel->StaticMapObjects[teleportTo.X][teleportTo.Y];

  bool tpToWall = (mapRef->Blocking || (soRef != nullptr && soRef->Blocking));

  auto actor = GetActorAtPosition(teleportTo.X, teleportTo.Y);
  bool tpOccupied = (actor != nullptr);

  std::string tpTo = mapRef->Blocking ?
                     mapRef->ObjectName :
                     (soRef != nullptr ? soRef->ObjectName : "unknown");

  if (tpToWall)
  {
    auto str = Util::StringFormat("You teleported into a %s!", tpTo.data());
    Printer::Instance().AddMessage(str);

    player.Attrs.HP.Set(0);
  }  
  else if (tpOccupied)
  {
    // Assume that if some NPC occupied returner destination,
    // he can be moved at least to his 'previous' position
    // (thus, any empty cell around him).
    auto str = Util::StringFormat("You bump into %s!", actor->ObjectName.data());
    Printer::Instance().AddMessage(str);

    Position tp = teleportTo;

    auto points = Util::GetEightPointsAround(teleportTo, CurrentLevel->MapSize);
    for (auto& p : points)
    {
      if (!CurrentLevel->IsCellBlocking(p))
      {
        tp = p;
        break;
      }
    }

    // Read the comment in the beginning of the method
    actor->MoveTo(tp);
  }

  player.SetLevelOwner(CurrentLevel);
  player.MoveTo(teleportTo);
  player.VisibilityRadius.Set(CurrentLevel->VisibilityRadius);

  CurrentLevel->AdjustCamera();
}

void Map::ChangeOrInstantiateLevel(MapType levelName)
{
  if (_levels.count(levelName) == 0)
  {
    ShowLoadingText();

    switch (levelName)
    {
      case MapType::MINES_1:
      case MapType::MINES_2:
      case MapType::MINES_3:
      case MapType::MINES_4:
      case MapType::MINES_5:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelMines(60, 60, levelName, (int)levelName));
        break;

      case MapType::CAVES_1:
      case MapType::CAVES_2:
      case MapType::CAVES_3:
      case MapType::CAVES_4:
      case MapType::CAVES_5:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelCaves(80, 80, levelName, (int)levelName));
        break;

      case MapType::LOST_CITY:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelLostCity(75, 150, levelName, (int)levelName));
        break;

      case MapType::DEEP_DARK_1:
      case MapType::DEEP_DARK_2:
      case MapType::DEEP_DARK_3:
      case MapType::DEEP_DARK_4:
      case MapType::DEEP_DARK_5:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelDeepDark(100, 100, levelName, (int)levelName));
        break;

      case MapType::ABYSS_1:
      case MapType::ABYSS_2:
      case MapType::ABYSS_3:
      case MapType::ABYSS_4:
      case MapType::ABYSS_5:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelAbyss(150, 150, levelName, (int)levelName));
        break;

      case MapType::NETHER_1:
      case MapType::NETHER_2:
      case MapType::NETHER_3:
      case MapType::NETHER_4:
      case MapType::NETHER_5:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelNether(200, 200, levelName, (int)levelName));
        break;

      case MapType::THE_END:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelEndgame(64, 64, levelName, (int)levelName));
        break;
    }

    // Now GameObjectsFactory inside PrepareMap can reference correct level
    CurrentLevel = _levels[levelName].get();

    _levels[levelName]->PrepareMap(CurrentLevel);
  }
  else
  {
    CurrentLevel = _levels[levelName].get();
  }

  if (_mapVisitFirstTime.count(levelName) == 1
  && !CurrentLevel->WelcomeTextDisplayed)
  {
    Printer::Instance().Clear();
    Printer::Instance().Render();

    CurrentLevel->WelcomeTextDisplayed = true;
    CurrentLevel->DisplayWelcomeText();
  }  
}

std::vector<MapType> Map::GetAllVisitedLevels()
{
  std::vector<MapType> res;

  for (auto& kvp : _levels)
  {
    res.push_back(kvp.first);
  }

  return res;
}

void Map::ShowLoadingText()
{
  std::string text = "Now loading...";

  int tw = Printer::Instance().TerminalWidth / 2;
  int th = Printer::Instance().TerminalHeight / 2;

  int lx = tw - text.length() / 2;
  int hx = tw + text.length() / 2;

  #ifdef USE_SDL
  Printer::Instance().DrawWindow({ lx - 3, th - 3 },
                                 { hx - lx + 5, 6 },
                                 "",
                                 "#FFFFFF",
                                 GlobalConstants::MessageBoxHeaderBgColor,
                                 "#444444");
  #else
  for (int i = lx - 3; i < hx + 3; i++)
  {
    for (int j = th - 3; j <= th + 3; j++)
    {
      Printer::Instance().PrintFB(i, j, ' ', "#000000", "#222222");
    }
  }

  auto res = Util::GetPerimeter(lx - 4, th - 4, text.length() * 2 - 7, 8, true);
  for (auto& p : res)
  {
    Printer::Instance().PrintFB(p.X, p.Y, ' ', "#000000", "#FFFFFF");
  }
  #endif

  Printer::Instance().PrintFB(tw, th, text, Printer::kAlignCenter, "#FFFFFF");

  Printer::Instance().Render();
}

void Map::PrintMapArrayRevealedStatus()
{
  std::vector<std::string> dbg;
  for (int x = 0; x < CurrentLevel->MapSize.X; x++)
  {
    std::string row;
    for (int y = 0; y < CurrentLevel->MapSize.Y; y++)
    {
      auto str = Util::StringFormat("%i", CurrentLevel->MapArray[x][y]->Revealed);
      row += str;
    }
    dbg.push_back(row);
  }

  for (auto& s : dbg)
  {
    Logger::Instance().Print(s);
  }

  Printer::Instance().AddMessage("Current map layout revealed status logged");
}

void Map::PrintMapLayout()
{
  std::ofstream f;

  std::string fname = Util::StringFormat("%s.txt", CurrentLevel->LevelName.data());

  f.open(fname);

  std::string layout;

  for (int x = 0; x < CurrentLevel->MapSize.Y; x++)
  {
    for (int y = 0; y < CurrentLevel->MapSize.X; y++)
    {      
      char ch = CurrentLevel->MapArray[y][x]->Image;

      // Replace 'wall' tiles with '#'
      if (CurrentLevel->MapArray[y][x]->Blocking
       && CurrentLevel->MapArray[y][x]->BlocksSight
       && ch == ' ')
      {
        ch = '#';
      }

      auto staticObjects = GetGameObjectsAtPosition(y, x);
      if (!staticObjects.empty())
      {
        ch = staticObjects.back()->Image;

        // Replace 'wall' objects with '#'
        if (staticObjects.back()->Blocking
         && staticObjects.back()->BlocksSight
         && ch == ' ')
        {
          ch = '#';
        }
      }

      layout += ch;
    }

    layout += "\n";
  }

  f << layout;

  f.close();

  auto dbg = Util::StringFormat("Layout saved to %s", fname.data());
  Printer::Instance().AddMessage(dbg);
}

/// Returns list of cell that can be walked upon
std::vector<Position> Map::GetWalkableCellsAround(const Position& pos)
{
  std::vector<Position> res;

  if (CurrentLevel != nullptr)
  {
    int lx = pos.X - 1;
    int ly = pos.Y - 1;
    int hx = pos.X + 1;
    int hy = pos.Y + 1;

    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        if (x == pos.X && y == pos.Y)
        {
          continue;
        }

        if (Util::IsInsideMap({ x, y }, CurrentLevel->MapSize)
        && (!CurrentLevel->IsCellBlocking({ x, y })
         && !CurrentLevel->MapArray[x][y]->Occupied))
        {
          res.push_back({ x, y });
        }
      }
    }
  }

  return res;
}

void Map::DrawNonVisibleMapTile(int x, int y)
{
  // If map tile has already been seen (revealed),
  // draw it with fog of war color,
  // otherwise use tile's color and black color as a background.
  std::string tileColor = CurrentLevel->MapArray[x][y]->Revealed ?
                          GlobalConstants::FogOfWarColor :
                          GlobalConstants::BlackColor;

  // If tile's fg color is already black
  // ("block" tiles with no symbols like water, floor, walls etc.),
  // replace the background instead.
  if (CurrentLevel->MapArray[x][y]->FgColor == GlobalConstants::BlackColor)
  {
    CurrentLevel->MapArray[x][y]->Draw(GlobalConstants::BlackColor, tileColor);
  }
  else
  {
    CurrentLevel->MapArray[x][y]->Draw(tileColor, GlobalConstants::BlackColor);
  }
}

void Map::DrawNonVisibleStaticObject(int x, int y)
{  
  if (CurrentLevel->StaticMapObjects[x][y] != nullptr)
  {
    // Same as in method above

    std::string tileColor = CurrentLevel->MapArray[x][y]->Revealed ?
                            GlobalConstants::FogOfWarColor :
                            GlobalConstants::BlackColor;

    if (CurrentLevel->StaticMapObjects[x][y]->FgColor == GlobalConstants::BlackColor)
    {
      CurrentLevel->StaticMapObjects[x][y]->Draw(GlobalConstants::BlackColor, tileColor);
    }
    else
    {
      CurrentLevel->StaticMapObjects[x][y]->Draw(tileColor, GlobalConstants::BlackColor);
    }
  }
}

bool Map::IsObjectVisible(const Position &from, const Position &to)
{
  auto line = Util::BresenhamLine(from, to);
  for (auto& c : line)
  {
    if (!Util::IsInsideMap(c, CurrentLevel->MapSize))
    {
      return false;
    }

    // Object can be blocking but not block sight (e.g. lava, chasm)
    // so check against BlocksSight only is needed.

    bool groundBlock = CurrentLevel->MapArray[c.X][c.Y]->BlocksSight;
    bool staticBlock = false;

    if (CurrentLevel->StaticMapObjects[c.X][c.Y] != nullptr)
    {
      staticBlock = CurrentLevel->StaticMapObjects[c.X][c.Y]->BlocksSight;
    }

    if (groundBlock || staticBlock)
    {
      return false;
    }
  }

  return true;
}

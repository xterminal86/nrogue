#include "map.h"

#include "application.h"
#include "printer.h"
#include "util.h"
#include "rng.h"
#include "door-component.h"
#include "game-objects-factory.h"
#include "map-level-town.h"
#include "map-level-mines.h"
#include "map-level-caves.h"
#include "util.h"

void Map::Init()
{
  _levels[MapType::TOWN] = std::unique_ptr<MapLevelBase>(new MapLevelTown(100, 50, MapType::TOWN));

  CurrentLevel = _levels[MapType::TOWN].get();

  _levels[MapType::TOWN]->PrepareMap(_levels[MapType::TOWN].get());

  // Give player reference to current level
  Application::Instance().PlayerInstance.SetLevelOwner(CurrentLevel);

  Application::Instance().PlayerInstance.VisibilityRadius = CurrentLevel->VisibilityRadius;

  _mapVisitFirstTime[MapType::MINES_1] = false;
  _mapVisitFirstTime[MapType::CAVES_1] = false;
  _mapVisitFirstTime[MapType::LOST_CITY] = false;
  _mapVisitFirstTime[MapType::DEEP_DARK_1] = false;
  _mapVisitFirstTime[MapType::ABYSS_1] = false;
  _mapVisitFirstTime[MapType::NETHER_1] = false;
  _mapVisitFirstTime[MapType::THE_END] = false;

  // FIXME: debug
  // ChangeLevel(MapType::MINES_1, true);
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
    }
    else
    {
      std::string tileColor = CurrentLevel->MapArray[x][y]->Revealed ?
                              GlobalConstants::FogOfWarColor :
                              GlobalConstants::BlackColor;

      if (CurrentLevel->MapArray[x][y]->FgColor == GlobalConstants::BlackColor)
      {
        CurrentLevel->MapArray[x][y]->Draw(GlobalConstants::BlackColor, tileColor);
      }
      else
      {
        CurrentLevel->MapArray[x][y]->Draw(tileColor, GlobalConstants::BlackColor);
      }
    }
  }

  // Draw game objects

  for (auto& go : CurrentLevel->GameObjects)
  {
    int x = go.get()->PosX;
    int y = go.get()->PosY;

    if (CurrentLevel->MapArray[x][y]->Visible)
    {
      // If game object has black bg color,
      // replace it with current floor color
      bool cond = (go->BgColor == GlobalConstants::BlackColor);
      go.get()->Draw(go.get()->FgColor, cond ? CurrentLevel->MapArray[x][y]->BgColor : go->BgColor);
    }
  }

  // Draw actors

  for (auto& go : CurrentLevel->ActorGameObjects)
  {
    int x = go.get()->PosX;
    int y = go.get()->PosY;

    if (CurrentLevel->MapArray[x][y]->Visible)
    {
      // If game object has black bg color,
      // replace it with current floor color
      bool cond = (go->BgColor == GlobalConstants::BlackColor);
      go.get()->Draw(go.get()->FgColor, cond ? CurrentLevel->MapArray[x][y]->BgColor : go->BgColor);
    }
  }
}

void Map::UpdateGameObjects()
{
  RemoveDestroyed();

  CurrentLevel->TryToSpawnMonsters();

  for (auto& go : CurrentLevel->GameObjects)
  {
    go.get()->Update();
  }

  for (auto& go : CurrentLevel->ActorGameObjects)
  {
    go.get()->Update();
  }
}

void Map::InsertActor(GameObject* goToInsert)
{
  CurrentLevel->ActorGameObjects.push_back(std::unique_ptr<GameObject>(goToInsert));
}

void Map::InsertGameObject(GameObject* goToInsert)
{
  CurrentLevel->GameObjects.push_back(std::unique_ptr<GameObject>(goToInsert));
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

  return res;
}

void Map::RemoveDestroyed()
{  
  for (int i = 0; i < CurrentLevel->GameObjects.size(); i++)
  {
    if (CurrentLevel->GameObjects[i]->IsDestroyed)
    {
      int x = CurrentLevel->GameObjects[i]->PosX;
      int y = CurrentLevel->GameObjects[i]->PosY;

      CurrentLevel->MapArray[x][y]->Occupied = false;

      CurrentLevel->GameObjects.erase(CurrentLevel->GameObjects.begin() + i);
    }
  }

  for (int i = 0; i < CurrentLevel->ActorGameObjects.size(); i++)
  {
    if (CurrentLevel->ActorGameObjects[i]->IsDestroyed)
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
  player.MoveTo(pos.X, pos.Y);
  player.VisibilityRadius = CurrentLevel->VisibilityRadius;

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
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelMines(108, 108, levelName, (int)levelName));
        break;

      case MapType::MINES_2:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelMines(135, 135, levelName, (int)levelName));
        break;

      case MapType::MINES_3:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelMines(150, 150, levelName, (int)levelName));
        break;

      case MapType::MINES_4:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelMines(175, 175, levelName, (int)levelName));
        break;

      case MapType::MINES_5:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelMines(190, 190, levelName, (int)levelName));
        break;

      case MapType::CAVES_1:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelCaves(200, 200, levelName, (int)levelName));
        break;

      case MapType::CAVES_2:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelCaves(200, 200, levelName, (int)levelName));
        break;

      case MapType::CAVES_3:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelCaves(200, 200, levelName, (int)levelName));
        break;

      case MapType::CAVES_4:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelCaves(200, 200, levelName, (int)levelName));
        break;

      case MapType::CAVES_5:
        _levels[levelName] = std::unique_ptr<MapLevelBase>(new MapLevelCaves(200, 200, levelName, (int)levelName));
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

void Map::ShowLoadingText()
{
  int tw = Printer::Instance().TerminalWidth / 2;
  int th = Printer::Instance().TerminalHeight / 2;

  std::string text = "Now loading...";

  int lx = tw - text.length() / 2;
  int hx = tw + text.length() / 2;

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
      layout += CurrentLevel->MapArray[y][x]->Image;
    }

    layout += "\n";
  }

  f << layout;

  f.close();

  auto dbg = Util::StringFormat("Layout saved to %s", fname.data());
  Printer::Instance().AddMessage(dbg);
}

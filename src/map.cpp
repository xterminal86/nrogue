#include "map.h"

#include "application.h"
#include "printer.h"
#include "util.h"
#include "rng.h"
#include "door-component.h"
#include "game-objects-factory.h"
#include "map-level-town.h"
#include "util.h"

void Map::Init()
{
  _levels[MapType::TOWN] = std::unique_ptr<MapLevelBase>(new MapLevelTown(160, 60, MapType::TOWN));

  CurrentLevel = _levels[MapType::TOWN].get();

  // Assign corresponding level refs to their map arrays
  for (auto& l : _levels)
  {
    l.second->PrepareMap(l.second.get());
  }

  // Give player reference to current level
  Application::Instance().PlayerInstance.SetLevelOwner(CurrentLevel);
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

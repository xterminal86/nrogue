#include "task-move-smart.h"

#include "map.h"
#include "door-component.h"

BTResult TaskMoveSmart::Run()
{
  //DebugLog("[TaskMoveSmart]\n");

  auto cells = Util::GetEightPointsAround(_objectToControl->GetPosition(),
                                          Map::Instance().CurrentLevel->MapSize);
  std::vector<Position> cellsToMove;
  for (auto& c : cells)
  {
    auto& so = Map::Instance().CurrentLevel->StaticMapObjects[c.X][c.Y];

    bool isOk = !Map::Instance().CurrentLevel->MapArray[c.X][c.Y]->Special;
    bool isOccupied = Map::Instance().CurrentLevel->MapArray[c.X][c.Y]->Occupied;
    bool isDoor = (so != nullptr && (so->GetComponent<DoorComponent>() != nullptr));

    if (isOk && (!isOccupied || isDoor))
    {
      cellsToMove.push_back(c);
    }
  }

  if (!cellsToMove.empty())
  {
    int index = RNG::Instance().RandomRange(0, cellsToMove.size());
    auto& p = cellsToMove[index];

    if (!_objectToControl->MoveTo(p.X, p.Y))
    {
      auto& so = Map::Instance().CurrentLevel->StaticMapObjects[p.X][p.Y];
      if (so != nullptr)
      {
        auto dc = so->GetComponent<DoorComponent>();
        if (dc != nullptr)
        {
          if (dc->InteractBySomeone())
          {
            _objectToControl->FinishTurn();
            return BTResult::Success;
          }
        }
      }
    }
    else
    {
      _objectToControl->FinishTurn();
      return BTResult::Success;
    }
  }

  return BTResult::Failure;
}

#include "task-goto-last-player-pos.h"

#include "game-object.h"
#include "application.h"
#include "pathfinder.h"
#include "map.h"
#include "blackboard.h"

BTResult TaskGotoLastPlayerPos::Run()
{
  //printf("[TaskGotoLastPlayerPos]\n");

  auto sX = Blackboard::Instance().Get(_objectToControl->ObjectId(), "pl_x");
  auto sY = Blackboard::Instance().Get(_objectToControl->ObjectId(), "pl_y");

  if (sX.empty() || sY.empty())
  {
    return BTResult::Failure;
  }

  int plX = std::stoi(sX);
  int plY = std::stoi(sY);

  //printf("\tplX: %i plY: %i\n", plX, plY);

  if (_path.empty())
  {
    Pathfinder pf;
    _path = pf.BuildRoad(Map::Instance().CurrentLevel,
                        { _objectToControl->PosX, _objectToControl->PosY },
                        { plX, plY },
                        std::vector<char>(),
                        true);    
  }

  if (!_path.empty())
  {
    auto moveTo = _path.top();
    if (_objectToControl->MoveTo(moveTo))
    {
      _path.pop();
      _objectToControl->FinishTurn();
      return BTResult::Success;
    }
    else
    {
      // If MoveTo() failed, clear possible remaining path
      _path = std::stack<Position>();
    }
  }
  else
  {
    // No path can be built or we arrived to last known player pos
    Blackboard::Instance().Set(_objectToControl->ObjectId(), { "pl_x", "" });
    Blackboard::Instance().Set(_objectToControl->ObjectId(), { "pl_y", "" });
  }

  return BTResult::Failure;
}

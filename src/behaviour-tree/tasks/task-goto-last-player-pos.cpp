#include "task-goto-last-player-pos.h"

#include "game-object.h"
#include "application.h"
#include "pathfinder.h"
#include "map.h"
#include "blackboard.h"

BTResult TaskGotoLastPlayerPos::Run()
{
  //DebugLog("[TaskGotoLastPlayerPos]\n");

  auto sX = Blackboard::Instance().Get(_objectToControl->ObjectId(), GlobalConstants::BlackboardKeyPlayerPosX);
  auto sY = Blackboard::Instance().Get(_objectToControl->ObjectId(), GlobalConstants::BlackboardKeyPlayerPosY);

  if (sX.empty() || sY.empty())
  {
    return BTResult::Failure;
  }

  int plX = std::stoi(sX);
  int plY = std::stoi(sY);

  if (_objectToControl->PosX == plX
   && _objectToControl->PosY == plY)
  {
    // We have arrived at the last known player position

    Blackboard::Instance().Set(_objectToControl->ObjectId(), { GlobalConstants::BlackboardKeyPlayerPosX, "" });
    Blackboard::Instance().Set(_objectToControl->ObjectId(), { GlobalConstants::BlackboardKeyPlayerPosY, "" });

    return BTResult::Success;
  }

  //DebugLog("\tplX: %i plY: %i\n\n", plX, plY);

  Pathfinder pf;
  Position objPos = { _objectToControl->PosX, _objectToControl->PosY };
  Position playerPos = { plX, plY };
  auto path = pf.BuildRoad(Map::Instance().CurrentLevel,
                           objPos,
                           playerPos,
                           std::vector<char>(),
                           false,
                           true);

  if (!path.empty())
  {
    auto moveTo = path.top();
    if (_objectToControl->MoveTo(moveTo))
    {
      path.pop();
      _objectToControl->FinishTurn();
      return BTResult::Success;
    }
  }

  // No path can be built or MoveTo() failed
  Blackboard::Instance().Set(_objectToControl->ObjectId(), { GlobalConstants::BlackboardKeyPlayerPosX, "" });
  Blackboard::Instance().Set(_objectToControl->ObjectId(), { GlobalConstants::BlackboardKeyPlayerPosY, "" });

  return BTResult::Failure;
}

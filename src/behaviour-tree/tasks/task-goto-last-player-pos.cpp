#include "task-goto-last-player-pos.h"

#include "game-object.h"
#include "application.h"
#include "pathfinder.h"
#include "map.h"
#include "blackboard.h"

BTResult TaskGotoLastPlayerPos::Run()
{
  //DebugLog("[TaskGotoLastPlayerPos]\n");

  auto plPos = Blackboard::Instance().Get(_objectToControl->ObjectId(), Strings::BlackboardKeyPlayerPos);

  if (plPos.empty())
  {
    return BTResult::Failure;
  }

  auto split = Util::Instance().StringSplit(plPos, ',');

  int plX = std::stoi(split[0]);
  int plY = std::stoi(split[1]);

  if (_objectToControl->PosX == plX
   && _objectToControl->PosY == plY)
  {
    // We have arrived at the last known player position

    Blackboard::Instance().Set(_objectToControl->ObjectId(), { Strings::BlackboardKeyPlayerPos, std::string() });

    return BTResult::Success;
  }

  //DebugLog("\tplX: %i plY: %i\n\n", plX, plY);

  Pathfinder pf;
  auto path = pf.BuildRoad(Map::Instance().CurrentLevel,
                           _objectToControl->GetPosition(),
                           _playerRef->GetPosition(),
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
  Blackboard::Instance().Set(_objectToControl->ObjectId(), { Strings::BlackboardKeyPlayerPos, std::string() });

  return BTResult::Failure;
}
